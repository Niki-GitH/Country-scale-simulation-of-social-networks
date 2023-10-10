#include "../include/PopulationDB.h"
#include "../include/TextFileLogger.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <ctime>

// long CPopulationDB::m_nTotalEvents = 0;

CPopulationDB::CPopulationDB(CINIConfigParser &parser, std::vector<iniAddress> &totaladdrs, int popCount)
	: parser_(parser)
{
	Dispose();
	// CTextFileLogger::GetLogger()->Log("************ CPopulationDB::CPopulationDB >>>>>>>");
	std::string path_to_data = parser.getFolderPath();

	pInoGen = std::make_unique<CPersonInfoGen>(path_to_data);
	ReadPopulationDBFile(path_to_data + "/sweden_population_ref_scb_2020.csv");

	// Define the states
	std::vector<CareerState_> states = {WORKING, STUDYING};

	// Define the observations
	std::vector<FinacialObsSymbol> observations = {LOW, MEDIUM, HIGH};

	// Define the transition matrix
	std::vector<std::vector<double>> transition_matrix = {
		{0.9, 0.1},
		{0.5, 0.5}};

	// Define the emission matrix
	std::vector<std::vector<double>> emission_matrix = {
		{0.1, 0.4, 0.5},
		{0.3, 0.4, 0.3}};

	std::vector<double> pi = {0.4, 0.6};

	m_csCareer = std::make_unique<CHMM_Career>(states, observations, transition_matrix, emission_matrix, pi);

	std::vector<std::string> hstates;
	hstates.push_back("GRADUATION");
	hstates.push_back("NEW_CHILD");
	hstates.push_back("MARRIAGE");
	hstates.push_back("HOME_PURCHASE");
	hstates.push_back("NEW_JOB");
	hstates.push_back("DEATH");
	std::vector<int> ages;
	for (int i = 1; i < 100; ++i)
	{
		ages.push_back(i);
	}
	std::vector<std::string> fstatus;
	fstatus.push_back("POOR");
	fstatus.push_back("LOWER_MIDDLE");
	fstatus.push_back("MIDDLE");
	fstatus.push_back("UPPER_MIDDLE");
	fstatus.push_back("RICH");
	fstatus.push_back("ULTRA_RICH");

	m_cslifeChoice = std::make_unique<CHMMEvents>(hstates, ages, fstatus);
	// m_cslifeChoice = std::make_unique <CHMMEvents>("./data/HMMEvents.model");

	m_lBasePopulationCnt = 0;
	m_lEvolvePopulationCnt = 0;
	m_lTotalPopulationCnt = popCount;
	m_lBasePopulationCnt = m_lTotalPopulationCnt;
	m_lEvolvePopulationCnt = m_lTotalPopulationCnt - m_lBasePopulationCnt;
	m_lChildrnPopulation = static_cast<long>(m_lBasePopulationCnt * parser_.GetDblValue(SECTION_AD, KEY_AGE_14));
	m_lYoungPopulation = static_cast<long>(m_lBasePopulationCnt * parser_.GetDblValue(SECTION_AD, KEY_AGE_24));
	double dwrkng = parser_.GetDblValue(SECTION_AD, KEY_AGE_54) + parser_.GetDblValue(SECTION_AD, KEY_AGE_64);
	m_lWorkingPopulation = static_cast<long>(m_lBasePopulationCnt * dwrkng);
	m_lElderPopulation = static_cast<long>(m_lBasePopulationCnt * parser_.GetDblValue(SECTION_AD, KEY_AGE_MAX));
	m_lTotalMarriages = static_cast<long>(m_lBasePopulationCnt * parser_.GetDblValue(SECTION_MARRIAGES, KEY_AVG_MARRIAGE_RATE));

	if (0 != popCount)
	{
		m_lmalePopln.reserve(popCount);
		m_lfemalePopln.reserve(popCount);
		m_lChildPopln.reserve(popCount);
	}

	const long scount = parser_.GetNumOfSchools();
	const long ucount = scount + parser_.GetNumOfUniversities();
	const long ocount = ucount + parser_.GetNumOfOffices();
	long schls = 0;
	long ofcs = 0;
	long unis = 0;
	for (size_t t = 0; t < totaladdrs.size(); t++)
	{
		if (t < scount)
		{
			m_lSchools.push_back(totaladdrs[t]);
		}
		else if ((t >= scount) && (t < ucount))
		{
			m_lUniversities.push_back(totaladdrs[t]);
		}
		else
		{
			m_lOffices.push_back(totaladdrs[t]);
		}
	}

	// CTextFileLogger::GetLogger()->Log("************ CPopulationDB::CPopulationDB <<<<<<<<");
}

CPopulationDB::~CPopulationDB()
{
	Dispose();
}

void CPopulationDB::ReadPopulationDBFile(std::string filepath)
{
	std::fstream myfile;

	// open file
	myfile.open(filepath);

	if (myfile.is_open())
	{
		// checking if the file is open
		std::string str;
		// read data from file object
		// and put it into string.
		getline(myfile, str);
		str = "";
		while (getline(myfile, str))
		{
			std::vector<std::string> tokens;
			split(str, ',', tokens);
			size_t tkns = tokens.size();
			_PopulationData dTemp;
			if (dTemp.nColms == tkns)
			{
				dTemp.m_strCountry = tokens[0];
				std::transform(dTemp.m_strCountry.begin(), dTemp.m_strCountry.end(), dTemp.m_strCountry.begin(), ::tolower);
				dTemp.m_strPrefec = tokens[1];
				std::transform(dTemp.m_strPrefec.begin(), dTemp.m_strPrefec.end(), dTemp.m_strPrefec.begin(), ::tolower);
				m_countyNames.insert(dTemp.m_strPrefec);
				dTemp.m_strSubPrefec = tokens[2];
				dTemp.m_strLocalitycode = tokens[3];
				dTemp.m_strLocalityName = tokens[4];
				dTemp.m_nPopulation = std::stol(tokens[5]);
				dTemp.m_nArea = std::stol(tokens[6]);
				dTemp.m_longitude = std::stod(tokens[7]);
				dTemp.m_latitude = std::stod(tokens[8]);
				m_populationDB.push_back(dTemp);
			}
		}
		// close the file object.
		myfile.close();
	}
	else
	{
		std::cout << "error opening file :" << filepath << std::endl;
	}
}

CPopulationDB &CPopulationDB::operator=(const CPopulationDB &copyObj)
{
	if (&copyObj != this)
	{
		m_countyNames = copyObj.m_countyNames;
		m_populationDB = copyObj.m_populationDB;
		m_lTotalPopulationCnt = copyObj.m_lTotalPopulationCnt;
		m_lBasePopulationCnt = copyObj.m_lBasePopulationCnt;
		m_lEvolvePopulationCnt = copyObj.m_lEvolvePopulationCnt;
		m_lChildrnPopulation = copyObj.m_lChildrnPopulation;
		m_lYoungPopulation = copyObj.m_lYoungPopulation;
		m_lWorkingPopulation = copyObj.m_lWorkingPopulation;
		m_lElderPopulation = copyObj.m_lElderPopulation;
		m_lTotalMarriages = copyObj.m_lTotalMarriages;
		// vectors
		m_lmalePopln = copyObj.m_lmalePopln;
		m_lfemalePopln = copyObj.m_lfemalePopln;
		m_lChildPopln = copyObj.m_lChildPopln;
		m_parentChild = copyObj.m_parentChild;
		m_Marriages = copyObj.m_Marriages;
		m_Siblings = copyObj.m_Siblings;
	}
	return *this;
}

void CPopulationDB::mergeCPopulationDB(const CPopulationDB &copyObj)
{
	if (&copyObj != this)
	{
		// m_countyNames = copyObj.m_countyNames;
		// m_populationDB = copyObj.m_populationDB;
		m_lTotalPopulationCnt = m_lTotalPopulationCnt + copyObj.m_lTotalPopulationCnt;
		m_lBasePopulationCnt = m_lBasePopulationCnt + copyObj.m_lBasePopulationCnt;
		m_lEvolvePopulationCnt = m_lEvolvePopulationCnt + copyObj.m_lEvolvePopulationCnt;
		m_lChildrnPopulation = m_lChildrnPopulation + copyObj.m_lChildrnPopulation;
		m_lYoungPopulation = m_lYoungPopulation + copyObj.m_lYoungPopulation;
		m_lWorkingPopulation = m_lWorkingPopulation + copyObj.m_lWorkingPopulation;
		m_lElderPopulation = m_lElderPopulation + copyObj.m_lElderPopulation;
		m_lTotalMarriages = m_lTotalMarriages + copyObj.m_lTotalMarriages;

		const size_t tempSizeM = m_lmalePopln.size();
		const size_t tempSizeF = m_lfemalePopln.size();
		const size_t tempSizeC = m_lChildPopln.size();

		// m_lmalePopln.reserve(tempSizeM + copyObj.m_lmalePopln.size());
		// m_lfemalePopln.reserve(tempSizeF + copyObj.m_lfemalePopln.size());
		// m_lChildPopln.reserve(tempSizeC + copyObj.m_lChildPopln.size());

		for (auto x : copyObj.m_lmalePopln) //(size_t x =0; x< copyObj.m_lmalePopln.size();++x)// (auto x : copyObj.m_lmalePopln)
		{
			x.m_nListIndex = (long)tempSizeM + x.m_nListIndex;
			if (x.m_nListFatherIndex != -1)
			{
				x.m_nListFatherIndex = (long)tempSizeM + x.m_nListFatherIndex;
			}
			if (x.m_nListMotherIndex != -1)
			{
				x.m_nListMotherIndex = (long)tempSizeF + x.m_nListMotherIndex;
			}
			for (auto i = x.spouse_begin(); i < x.spouse_end(); i++)
			{
				*i = tempSizeF + *i;
			}
			for (auto i = x.children_begin(); i < x.children_end(); i++)
			{
				size_t temp = i->m_nIndex;
				if (false == i->m_bMature)
				{
					i->m_nIndex = temp + tempSizeC;
				}
				else
				{
					if (true == i->m_bMale)
					{
						i->m_nIndex = temp + tempSizeM;
					}
					else
					{
						i->m_nIndex = temp + tempSizeF;
					}
				}
			}
			m_lmalePopln.push_back(x);
		}

		for (auto x : copyObj.m_lfemalePopln)
		{
			x.m_nListIndex = tempSizeF + x.m_nListIndex;
			if (x.m_nListFatherIndex != -1)
			{
				x.m_nListFatherIndex = (long)tempSizeM + x.m_nListFatherIndex;
			}
			if (x.m_nListMotherIndex != -1)
			{
				x.m_nListMotherIndex = (long)tempSizeF + x.m_nListMotherIndex;
			}
			for (auto i = x.spouse_begin(); i < x.spouse_end(); i++)
			{
				*i = tempSizeM + *i;
			}
			for (auto i = x.children_begin(); i < x.children_end(); i++)
			{
				size_t temp = i->m_nIndex;
				if (false == i->m_bMature)
				{
					i->m_nIndex = temp + tempSizeC;
				}
				else
				{
					if (true == i->m_bMale)
					{
						i->m_nIndex = temp + tempSizeM;
					}
					else
					{
						i->m_nIndex = temp + tempSizeF;
					}
				}
			}
			m_lfemalePopln.push_back(x);
		}

		for (auto x : copyObj.m_lChildPopln)
		{
			x.m_nListIndex = tempSizeC + x.m_nListIndex;
			if (x.m_nListFatherIndex != -1)
			{
				x.m_nListFatherIndex = (long)tempSizeM + x.m_nListFatherIndex;
			}
			if (x.m_nListMotherIndex != -1)
			{
				x.m_nListMotherIndex = (long)tempSizeF + x.m_nListMotherIndex;
			}
			long maxcnt = tempSizeF;
			if (GENDER_TYPE_MALE == x.getGender())
			{
				maxcnt = tempSizeM;
			}
			for (auto i = x.spouse_begin(); i < x.spouse_end(); i++)
			{
				*i = maxcnt + *i;
			}
			for (auto i = x.children_begin(); i < x.children_end(); i++)
			{
				size_t temp = i->m_nIndex;
				if (false == i->m_bMature)
				{
					i->m_nIndex = temp + tempSizeC;
				}
				else
				{
					if (true == i->m_bMale)
					{
						i->m_nIndex = temp + tempSizeM;
					}
					else
					{
						i->m_nIndex = temp + tempSizeF;
					}
				}
			}
			m_lChildPopln.push_back(x);
		}

		const size_t tempEdgParent = m_parentChild.size();
		const size_t tempEdgeSiblings = m_Siblings.size();
		const size_t tempEdges = m_Marriages.size();

		for (auto x : copyObj.m_parentChild)
		{
			if (true == x.m_bParentGender)
			{
				x.m_nParentIndex = x.m_nParentIndex + tempSizeM;
			}
			else
			{
				x.m_nParentIndex = x.m_nParentIndex + tempSizeF;
			}
			if (true == x.m_bMatureChild)
			{
				if (true == x.m_bChildGender)
				{
					x.m_nChildIndex = x.m_nChildIndex + tempSizeM;
				}
				else
				{
					x.m_nChildIndex = x.m_nChildIndex + tempSizeF;
				}
			}
			else
			{
				x.m_nChildIndex = x.m_nChildIndex + tempSizeC;
			}
			m_parentChild.push_back(x);
		}

		// for (size_t c = 0; c < copyObj.m_Siblings.size(); c++)
		for (auto x : copyObj.m_Siblings)
		{
			if (true == x.m_bMatureP1)
			{
				if (true == x.m_bGenderP1)
				{
					x.m_nIndexP1 = x.m_nIndexP1 + tempSizeM;
				}
				else
				{
					x.m_nIndexP1 = x.m_nIndexP1 + tempSizeF;
				}
			}
			else
			{
				x.m_nIndexP1 = x.m_nIndexP1 + tempSizeC;
			}

			if (true == x.m_bMatureP2) //
			{
				if (true == x.m_bGenderP2)
				{
					x.m_nIndexP2 = x.m_nIndexP2 + tempSizeM;
				}
				else
				{
					x.m_nIndexP2 = x.m_nIndexP2 + tempSizeF;
				}
			}
			else
			{
				x.m_nIndexP2 = x.m_nIndexP2 + tempSizeC;
			}
			m_Siblings.push_back(x);
		}

		for (auto x : copyObj.m_Marriages)
		{
			if (true == x.m_bGenderP1)
			{
				x.m_nIndexP1 = x.m_nIndexP1 + tempSizeM;
			}
			else
			{
				x.m_nIndexP1 = x.m_nIndexP1 + tempSizeF;
			}

			if (true == x.m_bGenderP2)
			{
				x.m_nIndexP2 = x.m_nIndexP2 + tempSizeM;
			}
			else
			{
				x.m_nIndexP2 = x.m_nIndexP2 + tempSizeF;
			}
			m_Marriages.push_back(x);
		}
	}
}

void CPopulationDB::Dispose()
{
	m_populationDB.clear();
	std::vector<_PopulationData>(m_populationDB).swap(m_populationDB);

	m_lmalePopln.clear();
	std::vector<CPersonInfo>(m_lmalePopln).swap(m_lmalePopln);

	m_lfemalePopln.clear();
	std::vector<CPersonInfo>(m_lfemalePopln).swap(m_lfemalePopln);

	m_lChildPopln.clear();
	std::vector<CPersonInfo>(m_lChildPopln).swap(m_lChildPopln);

	m_parentChild.clear();
	// std::vector<std::tuple<bool, size_t, bool, bool, size_t>>(m_parentChild).swap(m_parentChild);
	std::vector<ParentChildInfo>(m_parentChild).swap(m_parentChild);

	m_Siblings.clear();
	std::vector<SiblingsInfo>(m_Siblings).swap(m_Siblings);

	m_Marriages.clear();
	std::vector<MarriagesInfo>(m_Marriages).swap(m_Marriages);

	m_lUniversities.clear();
	std::vector<iniAddress>(m_lUniversities).swap(m_lUniversities);
	m_lSchools.clear();
	std::vector<iniAddress>(m_lSchools).swap(m_lSchools);
	m_lOffices.clear();
	std::vector<iniAddress>(m_lOffices).swap(m_lOffices);
}

void CPopulationDB::GenerateRandomPopulationData()
{
	auto start = std::chrono::high_resolution_clock::now();
	double dmalepercentage = parser_.GetDblValue(SECTION_GNDRD, KEY_MALE);
	double dfemalepercentage = parser_.GetDblValue(SECTION_GNDRD, KEY_FEMALE);
	double dotherpercentage = 1.0 - (dmalepercentage + dfemalepercentage);
	const int maxage = parser_.GetIntValue(SECTION_POP, KEY_MAX_AGE);
	const int minage = parser_.GetIntValue(SECTION_POP, KEY_MIN_AGE);
	if (0 == m_lSchools.size())
	{
		updateSchoolsFromIni();
	}
	if (0 == m_lOffices.size())
	{
		updateOfficesFromIni();
	}
	if (0 == m_lUniversities.size())
	{
		updateUniversitiesFromIni();
	}

	const long numOfSchools = static_cast<long>(m_lSchools.size());
	const long numOfOffices = static_cast<long>(m_lOffices.size());

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate elder population
	/////////////////////////////////////////////////////////////////////////////////////////////////
	long totalmalepop_elder = static_cast<long>(m_lElderPopulation * dmalepercentage);
	// long totalfemalepop_elder = (m_lElderPopulation*dfemalepercentage);
	long elderTotalUnmarried = 0;
	const long maxUnmarriedElder = static_cast<long>(m_lElderPopulation * 0.05);
	for (long ep = 0; ep < m_lElderPopulation; ++ep)
	{
		int randCity = generateRandomNumber_(0, (long)m_populationDB.size());
		// int randCity = 0 + rand() % ((m_populationDB.size()) - 0);
		_PopulationData db;
		if (randCity < m_populationDB.size())
		{
			db = m_populationDB[randCity];
		}
		GenderInfo_ ginfo = GENDER_TYPE_MALE;
		if (ep >= totalmalepop_elder)
		{
			ginfo = GENDER_TYPE_FEMALE;
		}
		std::pair<std::string, std::string> fathername = pInoGen->GenerateRandomName(GENDER_TYPE_MALE);
		std::pair<std::string, std::string> mothername = pInoGen->GenerateRandomName(GENDER_TYPE_FEMALE);
		std::pair<std::string, std::string> fullname = pInoGen->GenerateRandomName(ginfo);
		fullname.second = fathername.second;
		_dob stdob = pInoGen->GenerateRandomDOB(ELDERLY_DOB_ST, ELDERLY_DOB_EN);
		Graduation_ grad = pInoGen->GenerateRandomEducation();
		MaritalSatus_ msts = MARITAL_STATUS_NEVER_MARRIED;
		msts = pInoGen->GenerateRandomMSts();
		if (msts == MARITAL_STATUS_NEVER_MARRIED)
		{
			elderTotalUnmarried++;
			if (elderTotalUnmarried > maxUnmarriedElder)
			{
				// msts = MARITAL_STATUS_MARRIED;
			}
		}
		// std::string randAdd = pInoGen->GenerateRandomAddress(db.m_nArea, db.m_longitude, db.m_latitude);
		std::string randAdd = db.m_strLocalityName + "-" + db.m_strSubPrefec + "-" + db.m_strPrefec;
		size_t lastindx = 0;
		if (ginfo == GENDER_TYPE_MALE)
		{
			m_lmalePopln.push_back(CPersonInfo(fullname.first, fullname.second, stdob, ginfo, randAdd));
			lastindx = m_lmalePopln.size() - 1;
			CPersonInfo *pInfo = &m_lmalePopln[lastindx];
			pInfo->setSSN(pInoGen->GenerateRandomSSN());
			pInfo->setGraduation(grad);
			if (GRAD_TYPE_NONE != grad)
			{
				int randSchool = generateRandomNumber_(0, numOfSchools - 1);
				if (randSchool < 0)
					randSchool = 0;
				pInfo->setSchoolName(m_lSchools[randSchool].name, m_lSchools[randSchool].address);
			}
			pInfo->setMaritalStatus(msts);
			pInfo->setHasChildren(false);
			pInfo->setFatherName(fathername.first + " " + fathername.second);
			pInfo->setMotherName(mothername.first + " " + mothername.second);
			if (pInfo->getAge() > maxage)
			{
				int year = pInfo->getDOB_Year();
				int diff = pInfo->getAge() - maxage;
				pInfo->setAge(maxage);
				stdob.m_nYear = year - diff;
			}
			int randFSts = generateRandomNumber_(0, (long)FINANCE_STATUS_ULTRA_RICH);
			// int randFSts = FINANCE_STATUS_POOR + rand() % ((FINANCE_STATUS_ULTRA_RICH + 1) - FINANCE_STATUS_POOR);
			if (1 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_LOWER_MIDDLE);
			}
			else if (2 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_MIDDLE);
			}
			else if (3 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_UPPER_MIDDLE);
			}
			else if (4 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_RICH);
			}
			else if (5 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_ULTRA_RICH);
			}
			else
			{
				pInfo->setFinancialSts(FINANCE_STATUS_POOR);
			}
			pInfo->m_nListFatherIndex = -1;
			pInfo->m_nListMotherIndex = -1;
			pInfo->setListIndex(lastindx);
		}
		else
		{
			m_lfemalePopln.push_back(CPersonInfo(fullname.first, fullname.second, stdob, ginfo, randAdd));
			lastindx = m_lfemalePopln.size() - 1;
			CPersonInfo *pInfo = &m_lfemalePopln[lastindx];
			pInfo->setSSN(pInoGen->GenerateRandomSSN());
			pInfo->setGraduation(grad);
			if (GRAD_TYPE_NONE != grad)
			{
				int randSchool = generateRandomNumber_(0, numOfSchools - 1);
				pInfo->setSchoolName(m_lSchools[randSchool].name, m_lSchools[randSchool].address);
			}
			pInfo->setMaritalStatus(msts);
			pInfo->setHasChildren(false);
			pInfo->setFatherName(fathername.first + " " + fathername.second);
			pInfo->setMotherName(mothername.first + " " + mothername.second);
			if (pInfo->getAge() > maxage)
			{
				int year = pInfo->getDOB_Year();
				int diff = pInfo->getAge() - maxage;
				pInfo->setAge(maxage);
				stdob.m_nYear = year - diff;
			}
			int randFSts = generateRandomNumber_(0, (long)FINANCE_STATUS_ULTRA_RICH);
			// int randFSts = FINANCE_STATUS_POOR + rand() % ((FINANCE_STATUS_ULTRA_RICH + 1) - FINANCE_STATUS_POOR);
			if (1 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_LOWER_MIDDLE);
			}
			else if (2 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_MIDDLE);
			}
			else if (3 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_UPPER_MIDDLE);
			}
			else if (4 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_RICH);
			}
			else if (5 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_ULTRA_RICH);
			}
			else
			{
				pInfo->setFinancialSts(FINANCE_STATUS_POOR);
			}
			pInfo->m_nListFatherIndex = -1;
			pInfo->m_nListMotherIndex = -1;
			pInfo->setListIndex(lastindx);
		}
	}

	long total_sibling_con = static_cast<long>(totalmalepop_elder * 0.40);
	int bmmffmf = 0; // 0:random brother connections, 1: sister-sister, 2: bother - sister
	for (int i = 0; i < total_sibling_con; i++)
	{
		std::random_device rd, rd1; // obtain a random number from hardware newrandom
		std::mt19937 gen(rd());		// seed the generator
		std::mt19937 gen1(rd1());
		if (0 == bmmffmf)
		{
			std::uniform_int_distribution<> distr(0, m_lmalePopln.size() - 1);
			std::uniform_int_distribution<> distr1(0, m_lmalePopln.size() - 1);
			long sibcon_1 = distr(gen);
			long sibcon_2 = distr1(gen1);
			if (sibcon_1 == sibcon_2)
			{
				sibcon_2 = distr1(gen1);
			}
			m_lmalePopln[sibcon_1].setLastName(m_lmalePopln[sibcon_2].getLastName());
			m_lmalePopln[sibcon_1].setFatherName(m_lmalePopln[sibcon_2].getFatherName());
			m_lmalePopln[sibcon_1].setMotherName(m_lmalePopln[sibcon_2].getMotherName());
			m_Siblings.push_back(SiblingsInfo(true, true, sibcon_1, true, true, sibcon_2));
			m_Siblings.push_back(SiblingsInfo(true, true, sibcon_2, true, true, sibcon_1));
		}
		else if (1 == bmmffmf)
		{
			std::uniform_int_distribution<> distr(0, m_lfemalePopln.size() - 1);
			std::uniform_int_distribution<> distr1(0, m_lfemalePopln.size() - 1);
			long sibcon_1 = distr(gen);
			long sibcon_2 = distr1(gen1);
			if (sibcon_1 == sibcon_2)
			{
				sibcon_2 = distr1(gen1);
			}
			m_lfemalePopln[sibcon_1].setLastName(m_lfemalePopln[sibcon_2].getLastName());
			m_lfemalePopln[sibcon_1].setFatherName(m_lfemalePopln[sibcon_2].getFatherName());
			m_lfemalePopln[sibcon_1].setMotherName(m_lfemalePopln[sibcon_2].getMotherName());
			m_Siblings.push_back(SiblingsInfo(true, false, sibcon_1, true, false, sibcon_2));
			m_Siblings.push_back(SiblingsInfo(true, false, sibcon_2, true, false, sibcon_1));
		}
		else
		{
			bmmffmf = 0;
			std::uniform_int_distribution<> distr(0, m_lmalePopln.size() - 1);
			std::uniform_int_distribution<> distr1(0, m_lfemalePopln.size() - 1);
			long sibcon_1 = distr(gen);
			long sibcon_2 = distr1(gen1);
			m_lfemalePopln[sibcon_2].setLastName(m_lmalePopln[sibcon_1].getLastName());
			m_lfemalePopln[sibcon_2].setFatherName(m_lmalePopln[sibcon_1].getFatherName());
			m_lfemalePopln[sibcon_2].setMotherName(m_lmalePopln[sibcon_1].getMotherName());
			m_Siblings.push_back(SiblingsInfo(true, true, sibcon_1, true, false, sibcon_2));
			m_Siblings.push_back(SiblingsInfo(true, false, sibcon_2, true, true, sibcon_1));
		}
		bmmffmf++;
	}

	// random marriage for elder population
	for (size_t m = 0; m < m_lmalePopln.size(); m++)
	{
		makeRandomEventMarriage(m_lmalePopln[m]);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate children population
	/////////////////////////////////////////////////////////////////////////////////////////////////
	long totalmalepop_chld = static_cast<long>(m_lChildrnPopulation * dmalepercentage);
	for (long cp = 0; cp < m_lChildrnPopulation; ++cp)
	{
		int randSchool = generateRandomNumber_(0, numOfSchools - 1);

		GenderInfo_ ginfo = GENDER_TYPE_MALE;
		if (cp >= totalmalepop_chld)
		{
			ginfo = GENDER_TYPE_FEMALE;
		}
		std::pair<std::string, std::string> fullname = pInoGen->GenerateRandomName(ginfo);
		_dob stdob = pInoGen->GenerateRandomDOB(CHILD_DOB_ST, CHILD_DOB_EN);
		Graduation_ grad = GRAD_TYPE_NONE;
		if (stdob.m_nYear < 2010)
		{
			grad = GRAD_TYPE_SENIOR_HIGH;
		}
		std::string randAdd = "";
		m_lChildPopln.push_back(CPersonInfo(fullname.first, fullname.second, stdob, ginfo, randAdd));
		size_t lastindx = m_lChildPopln.size() - 1;
		m_lChildPopln[lastindx].setSSN(pInoGen->GenerateRandomSSN());
		m_lChildPopln[lastindx].setGraduation(grad);
		m_lChildPopln[lastindx].setMaritalStatus(MARITAL_STATUS_NEVER_MARRIED);
		m_lChildPopln[lastindx].setHasChildren(false);
		if (randSchool < 0)
			randSchool = 0;
		m_lChildPopln[lastindx].setSchoolName(m_lSchools[randSchool].name, m_lSchools[randSchool].address);
		m_lChildPopln[lastindx].setListIndex(lastindx);
		m_lChildPopln[lastindx].m_nGeneration = 1;
		m_lChildPopln[lastindx].m_nListIndex = static_cast<long>(lastindx);
		m_lChildPopln[lastindx].m_nListFatherIndex = -1;
		m_lChildPopln[lastindx].m_nListMotherIndex = -1;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate working age population
	/////////////////////////////////////////////////////////////////////////////////////////////////
	long totalmalepop_wrkng = static_cast<long>(m_lWorkingPopulation * dmalepercentage);
	long totalfemalepop_wrkng = static_cast<long>(m_lWorkingPopulation * dfemalepercentage);
	size_t stElderMaleLastIndex = m_lmalePopln.size();
	size_t stElderFeMaleLastIndex = m_lfemalePopln.size();
	long otherpop = 0;
	const long totalotherpop = static_cast<long>(dotherpercentage * m_lTotalPopulationCnt);
	for (long p = 0; p < m_lWorkingPopulation; ++p)
	{
		int randCity = generateRandomNumber_(0, (long)(m_populationDB.size() - 1));
		// int randCity = 0 + rand() % ((m_populationDB.size()) - 0);
		std::string randAdd = "";
		if (randCity < m_populationDB.size())
		{
			// randAdd = pInoGen->GenerateRandomAddress(m_populationDB[randCity].m_nArea, m_populationDB[randCity].m_longitude, m_populationDB[randCity].m_latitude);
			randAdd = m_populationDB[randCity].m_strLocalityName + "-" + m_populationDB[randCity].m_strSubPrefec + "-" + m_populationDB[randCity].m_strPrefec;
		}
		GenderInfo_ ginfo = GENDER_TYPE_MALE;
		if (p >= totalmalepop_wrkng)
		{
			ginfo = GENDER_TYPE_FEMALE;
			if (otherpop < totalotherpop)
			{
				otherpop++;
				ginfo = GENDER_TYPE_OTHER;
			}
		}

		int randFSts = generateRandomNumber_(0, (long)(FINANCE_STATUS_ULTRA_RICH));
		// int randFSts = FINANCE_STATUS_POOR + rand() % ((FINANCE_STATUS_ULTRA_RICH + 1) - FINANCE_STATUS_POOR);
		int randEmplmt = rand() % 2; // get 0 or 1
		std::pair<std::string, std::string> malename = pInoGen->GenerateRandomName(ginfo);
		_dob stdob = pInoGen->GenerateRandomDOB(WORKING_DOB_ST, WORKING_DOB_EN);
		Graduation_ grad = pInoGen->GenerateRandomEducation();
		MaritalSatus_ msts = MARITAL_STATUS_NEVER_MARRIED;
		msts = pInoGen->GenerateRandomMSts();

		if (ginfo == GENDER_TYPE_MALE)
		{
			m_lmalePopln.push_back(CPersonInfo(malename.first, malename.second, stdob, ginfo, randAdd));
			size_t lastindx = m_lmalePopln.size() - 1;
			CPersonInfo *pInfo = &m_lmalePopln[lastindx];
			pInfo->setListIndex(lastindx);
			pInfo->setSSN(pInoGen->GenerateRandomSSN());
			pInfo->setGraduation(grad);
			if (GRAD_TYPE_NONE != grad)
			{
				int randSchool = generateRandomNumber_(0, numOfSchools - 1);
				if (randSchool < 0)
					randSchool = 0;
				pInfo->setSchoolName(m_lSchools[randSchool].name, m_lSchools[randSchool].address);
			}
			pInfo->setMaritalStatus(msts);
			pInfo->setHasChildren(false);

			if (1 == randEmplmt)
			{
				pInfo->setIsEmployed(true);
				int randOfficeAdd = generateRandomNumber_(0, (numOfOffices - 1));
				if (randOfficeAdd < 0)
					randOfficeAdd = 0;
				pInfo->setOfficeName(m_lOffices[randOfficeAdd].name, m_lOffices[randOfficeAdd].address);
			}

			if (1 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_LOWER_MIDDLE);
			}
			else if (2 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_MIDDLE);
			}
			else if (3 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_UPPER_MIDDLE);
			}
			else if (4 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_RICH);
			}
			else if (5 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_ULTRA_RICH);
			}
			else
			{
				pInfo->setFinancialSts(FINANCE_STATUS_POOR);
			}

			int randParentMale = -1;
			int randParentFeMale = -1;
			if (getRandomMaleParent(stElderMaleLastIndex, randParentMale))
			{
				pInfo->setFatherName(m_lmalePopln[randParentMale].getFullName());
				pInfo->setLastName(m_lmalePopln[randParentMale].getLastName());
			}
			if (getRandomFeMaleParent(stElderFeMaleLastIndex, randParentFeMale))
			{
				pInfo->setMotherName(m_lfemalePopln[randParentFeMale].getFullName());
				if (randParentMale == -1)
				{
					randParentMale = m_lfemalePopln[randParentFeMale].getSpouseIndex();
				}
			}

			pInfo->m_nListFatherIndex = static_cast<long>(randParentMale);
			pInfo->m_nListMotherIndex = static_cast<long>(randParentFeMale);
			bool pbgen = true;
			bool pbmature = true;
			if (pInfo->getAge() < 18)
			{
				pbmature = false;
			}

			if ((randParentMale != -1) && (randParentFeMale != -1))
			{
				m_lmalePopln[randParentMale].setMaritalStatus(MARITAL_STATUS_MARRIED);
				m_lmalePopln[randParentMale].addSpouse(randParentFeMale);
				m_lmalePopln[randParentMale].addChild(m_lmalePopln[lastindx], lastindx);
				m_lfemalePopln[randParentFeMale].setMaritalStatus(MARITAL_STATUS_MARRIED);
				m_lfemalePopln[randParentFeMale].addSpouse(randParentMale);
				m_lfemalePopln[randParentFeMale].addChild(m_lmalePopln[lastindx], lastindx);
				m_Marriages.push_back(MarriagesInfo(true, randParentMale, false, randParentFeMale));
				m_Marriages.push_back(MarriagesInfo(false, randParentFeMale, true, randParentMale));
				m_lmalePopln[lastindx].m_nListIndex = lastindx;
				m_parentChild.push_back(ParentChildInfo(true, randParentMale, pbmature, pbgen, lastindx));
				m_parentChild.push_back(ParentChildInfo(false, randParentFeMale, pbmature, pbgen, lastindx));
			}
			else
			{
				pInfo->m_nListFatherIndex = -1;
				pInfo->m_nListMotherIndex = -1;
			}
		}
		else
		{
			m_lfemalePopln.push_back(CPersonInfo(malename.first, malename.second, stdob, ginfo, randAdd));
			size_t lastindx = m_lfemalePopln.size() - 1;
			CPersonInfo *pInfo = &m_lfemalePopln[lastindx];
			pInfo->setListIndex(lastindx);

			pInfo->setSSN(pInoGen->GenerateRandomSSN());
			pInfo->setGraduation(grad);
			if (GRAD_TYPE_NONE != grad)
			{
				int randSchool = generateRandomNumber_(0, numOfSchools - 1);
				if (randSchool < 0)
					randSchool = 0;
				pInfo->setSchoolName(m_lSchools[randSchool].name, m_lSchools[randSchool].address);
			}
			pInfo->setMaritalStatus(msts);
			pInfo->setHasChildren(false);

			if (1 == randEmplmt)
			{
				pInfo->setIsEmployed(true);
				int randOfficeAdd = generateRandomNumber_(0, (numOfOffices - 1));
				if (randOfficeAdd < 0)
					randOfficeAdd = 0;
				pInfo->setOfficeName(m_lOffices[randOfficeAdd].name, m_lOffices[randOfficeAdd].address);
			}

			if (1 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_LOWER_MIDDLE);
			}
			else if (2 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_MIDDLE);
			}
			else if (3 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_UPPER_MIDDLE);
			}
			else if (4 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_RICH);
			}
			else if (5 == randFSts)
			{
				pInfo->setFinancialSts(FINANCE_STATUS_ULTRA_RICH);
			}
			else
			{
				pInfo->setFinancialSts(FINANCE_STATUS_POOR);
			}

			int randParentMale = -1;
			int randParentFeMale = -1;
			if (getRandomMaleParent(stElderMaleLastIndex, randParentMale))
			{
				pInfo->setFatherName(m_lmalePopln[randParentMale].getFullName());
				pInfo->setLastName(m_lmalePopln[randParentMale].getLastName());
			}
			if (getRandomFeMaleParent(stElderFeMaleLastIndex, randParentFeMale))
			{
				pInfo->setMotherName(m_lfemalePopln[randParentFeMale].getFullName());
			}

			pInfo->m_nListFatherIndex = static_cast<long>(randParentMale);
			pInfo->m_nListMotherIndex = static_cast<long>(randParentFeMale);
			bool pbgen = false;
			bool pbmature = true;
			if (pInfo->getAge() < 18)
			{
				pbmature = false;
			}

			if ((randParentMale != -1) && (randParentFeMale != -1))
			{
				m_lmalePopln[randParentMale].setMaritalStatus(MARITAL_STATUS_MARRIED);
				m_lmalePopln[randParentMale].addSpouse(randParentFeMale);
				m_lmalePopln[randParentMale].addChild(m_lfemalePopln[lastindx], lastindx);
				m_lfemalePopln[randParentFeMale].setMaritalStatus(MARITAL_STATUS_MARRIED);
				m_lfemalePopln[randParentFeMale].addSpouse(randParentMale);
				m_lfemalePopln[randParentFeMale].addChild(m_lfemalePopln[lastindx], lastindx);
				m_Marriages.push_back(MarriagesInfo(true, randParentMale, false, randParentFeMale));
				m_Marriages.push_back(MarriagesInfo(false, randParentFeMale, true, randParentMale));
				pInfo->m_nListIndex = lastindx;
				m_parentChild.push_back(ParentChildInfo(true, randParentMale, pbmature, pbgen, lastindx));
				m_parentChild.push_back(ParentChildInfo(false, randParentFeMale, pbmature, pbgen, lastindx));
			}
			else
			{
				pInfo->m_nListFatherIndex = -1;
				pInfo->m_nListMotherIndex = -1;
			}
		}
	} // for

	/////////////////////////////////////////////////////////////////////////////////////////////////
	// Generate young adults population
	/////////////////////////////////////////////////////////////////////////////////////////////////
	long totalmalepop_yng = static_cast<long>(m_lYoungPopulation * dmalepercentage);
	long totalfemalepop_yng = static_cast<long>(m_lYoungPopulation * dfemalepercentage);
	for (long yp = 0; yp < m_lYoungPopulation; ++yp)
	{
		int randSchool = generateRandomNumber_(0, (numOfSchools - 1));
		// int randSchool = 0 + rand() % ((CINIConfigParser::GetNumOfSchools()) - 0);
		/*int randCity = 0 + rand() % ((m_populationDB.size()) - 0);
		_PopulationData db;
		if (randCity < m_populationDB.size())
		{
		db = m_populationDB[randCity];
		}*/
		GenderInfo_ ginfo = GENDER_TYPE_MALE;
		if (yp >= totalmalepop_yng)
		{
			ginfo = GENDER_TYPE_FEMALE;
			if (yp >= totalfemalepop_yng)
			{
				// ginfo = GENDER_TYPE_OTHER;
			}
		}
		int randFSts = generateRandomNumber_(0, (long)(FINANCE_STATUS_ULTRA_RICH));
		// int randFSts = FINANCE_STATUS_POOR + rand() % ((FINANCE_STATUS_ULTRA_RICH + 1) - FINANCE_STATUS_POOR);
		std::pair<std::string, std::string> fullname = pInoGen->GenerateRandomName(ginfo);
		_dob stdob = pInoGen->GenerateRandomDOB(CHILD_DOB_ST - 11, CHILD_DOB_ST);
		Graduation_ grad = GRAD_TYPE_NONE;
#if PLATFORM_TYPE_LINUX
		struct timespec ts;
		clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
#else
		srand(time(NULL));
#endif
		grad = (Graduation_)(GRAD_TYPE_SENIOR_HIGH + rand() % ((GRAD_TYPE_BACHELOR + 1) - GRAD_TYPE_SENIOR_HIGH));
		std::string randAdd = "";
		CPersonInfo pMInfo(fullname.first, fullname.second, stdob, ginfo, randAdd);
		pMInfo.setSSN(pInoGen->GenerateRandomSSN());
		pMInfo.setGraduation(grad);
		pMInfo.setMaritalStatus(MARITAL_STATUS_NEVER_MARRIED);
		pMInfo.setHasChildren(false);
		if (1 == randFSts)
		{
			pMInfo.setFinancialSts(FINANCE_STATUS_LOWER_MIDDLE);
		}
		else if (2 == randFSts)
		{
			pMInfo.setFinancialSts(FINANCE_STATUS_MIDDLE);
		}
		else if (3 == randFSts)
		{
			pMInfo.setFinancialSts(FINANCE_STATUS_UPPER_MIDDLE);
		}
		else if (4 == randFSts)
		{
			pMInfo.setFinancialSts(FINANCE_STATUS_RICH);
		}
		else if (5 == randFSts)
		{
			pMInfo.setFinancialSts(FINANCE_STATUS_ULTRA_RICH);
		}
		else
		{
			pMInfo.setFinancialSts(FINANCE_STATUS_POOR);
		}
		if (randSchool < 0)
			randSchool = 0;
		pMInfo.setSchoolName(m_lSchools[randSchool].name, m_lSchools[randSchool].address);
		pMInfo.m_nListFatherIndex = -1;
		pMInfo.m_nListMotherIndex = -1;
		if (18 > pMInfo.getAge())
		{
			int randSchool = generateRandomNumber_(0, (long)(m_lUniversities.size() - 1));
			if (randSchool < 0)
				randSchool = 0;
			pMInfo.setSchoolName(m_lUniversities[randSchool].name, m_lUniversities[randSchool].address);
			pMInfo.setListIndex(m_lChildPopln.size());
			pMInfo.m_nListIndex = m_lChildPopln.size();
			m_lChildPopln.push_back(pMInfo);
		}
		else
		{
			if (ginfo == GENDER_TYPE_MALE)
			{
				pMInfo.setListIndex(m_lmalePopln.size());
				m_lmalePopln.push_back(pMInfo);
			}
			else
			{
				pMInfo.setListIndex(m_lfemalePopln.size());
				m_lfemalePopln.push_back(pMInfo);
			}
		}
	}

	// Make random connections and create marriage event
	makeRandomEventMarriage(m_lTotalMarriages);

	// assign children to random parents
	// std::vector<size_t> unassignedlist;
	for (size_t child = 0; child < m_lChildPopln.size(); ++child) //(auto child : m_lChildPopln)
	{
		/*if (false == assignChildtoRandomFamily(m_lChildPopln[child]))
		{
			unassignedlist.push_back(m_lChildPopln[child].m_nListIndex);
		}*/
		assignChildtoRandomFamily(m_lChildPopln[child]);
	}

	std::cout << "GenerateRandomPopulationData() -> Generated 1st generation random data :" << GetPopCount() << std::endl;
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = finish - start;
	std::cout << "Elapsed Time: " << elapsed.count() << " seconds" << std::endl;

	CTextFileLogger::GetLogger()->Log("GenerateRandomPopulationData() -> Generated 1st generation random data : %d", GetPopCount());
}

void CPopulationDB::Evolve()
{
	//	std::cout << "Evolve population"<<std::endl;
	CTextFileLogger::GetLogger()->Log("Evolve() called...");
	const long numOfSchools = static_cast<long>(m_lSchools.size());
	const long numOfOffices = static_cast<long>(m_lOffices.size());

	for (int i = 0; i < 1; ++i)
	{
		CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d]", i + 1);

		/////////////////////////////////////////////////////////////////////////////////////////////////
		// Predict life events for female population and take necessary action
		/////////////////////////////////////////////////////////////////////////////////////////////////
		for (size_t persn = 0; persn < m_lfemalePopln.size(); ++persn) //(auto persn : m_lfemalePopln)
		{
			// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d] name = %s",i,persn.getFullName().c_str());
			int age = m_lfemalePopln[persn].getAge();
			m_lfemalePopln[persn].setAge(age + 1);
			int randTotalEvents = generateRandomNumber_(1, 5);
			if (age < 20)
			{
				randTotalEvents = 1;
			}
			for (int te = 0; te < randTotalEvents; te++)
			{
				std::vector<Observation> observations;
				Observation ob;
				ob.age = age;
				ob.financial_status = m_lfemalePopln[persn].getFinancialSts_();
				observations.push_back(ob);
				std::vector<std::string> events = m_cslifeChoice->predict(observations);
				for (auto evst : events)
				{
					EventType_ ev = CUtil::getLifeEventFromString(evst);
					PersonEvent pev;
					if (ev == EVENT_TYPE_GRADUATION)
					{
						pev.m_nEventType = EVENT_TYPE_GRADUATION;
						pev.m_strEventComments = "SENIOR";
						if ((age > 15) && (age < 70))
						{
							if (m_lfemalePopln[persn].getGraduation() == GRAD_TYPE_NONE)
							{
								pev.m_strEventComments = "SENIOR";
							}
							else if (m_lfemalePopln[persn].getGraduation() == GRAD_TYPE_SENIOR_HIGH)
							{
								pev.m_strEventComments = "DIPLOMA";
							}
							else if (m_lfemalePopln[persn].getGraduation() == GRAD_TYPE_DIPLOMA)
							{
								pev.m_strEventComments = "ADVANCED";
							}
							else if (m_lfemalePopln[persn].getGraduation() == GRAD_TYPE_ADVANCED_DIPLOMA)
							{
								pev.m_strEventComments = "BACHELOR";
							}
							else if (m_lfemalePopln[persn].getGraduation() == GRAD_TYPE_BACHELOR)
							{
								pev.m_strEventComments = "MASTER";
							}
							else if (m_lfemalePopln[persn].getGraduation() == GRAD_TYPE_MASTER)
							{
								pev.m_strEventComments = "DOCTORAL";
							}
						}
						m_lfemalePopln[persn].AddEvent(pev, m_lfemalePopln[persn]);
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: New event GRADUATION", i);
					}
					else if (ev == EVENT_TYPE_NEW_CHILD)
					{
						if (m_lfemalePopln[persn].getChildCount() > MAX_CHILD_PER_PERSON)
						{
							continue;
						}
						int randgen = generateRandomNumber_(0, (long)(EVENT_INFO_END));
						// int randgen = 0 + rand() % (EVENT_INFO_END);
						GenderInfo_ ginfo = GENDER_TYPE_MALE;
						bool bchildGen = true;
						if (randgen > GENDER_TYPE_MALE)
						{
							bchildGen = false;
							ginfo = GENDER_TYPE_FEMALE;
						}
						std::pair<std::string, std::string> fullname = pInoGen->GenerateRandomName(ginfo);
						_dob stdob = pInoGen->GenerateRandomDOB(ELDERLY_DOB_ST, ELDERLY_DOB_EN);
						Graduation_ grad = GRAD_TYPE_NONE;
						MaritalSatus_ msts = MARITAL_STATUS_NEVER_MARRIED;
						CPersonInfo pChild(fullname.first, fullname.second, stdob, ginfo, m_lfemalePopln[persn].getHomeAddress());
						size_t spusIndx = m_lfemalePopln[persn].getSpouseIndex();
						pChild.setMotherName(m_lfemalePopln[persn].getFullName());
						pChild.setSSN(pInoGen->GenerateRandomSSN());
						pChild.setGraduation(grad);
						pChild.setMaritalStatus(msts);
						pChild.setHasChildren(false);
						size_t lindex = m_lChildPopln.size();
						pChild.m_nGeneration = m_lfemalePopln[persn].m_nGeneration + 1;
						pChild.setListIndex(lindex);
						if (-1 == spusIndx)
						{
							pChild.setLastName(m_lfemalePopln[persn].getLastName());
							pChild.setFatherName("Not declared");
							pChild.m_nListFatherIndex = -1;
						}
						else
						{
							pChild.setLastName(m_lmalePopln[spusIndx].getLastName());
							pChild.setFatherName(m_lfemalePopln[persn].getSpouseName());
							pChild.m_nListFatherIndex = static_cast<long>(spusIndx);
						}
						pChild.m_nListMotherIndex = static_cast<long>(m_lfemalePopln[persn].m_nListIndex);
						m_lChildPopln.push_back(pChild);
						pev.m_nEventType = EVENT_TYPE_NEW_CHILD;
						pev.m_strEventComments = "NEW CHILD";
						m_lfemalePopln[persn].AddEvent(pev, pChild);
						m_parentChild.push_back(ParentChildInfo(false, m_lfemalePopln[persn].m_nListIndex, false, bchildGen, lindex));
						if (-1 != spusIndx)
						{
							m_lmalePopln[m_lfemalePopln[persn].getSpouseIndex()].AddEvent(pev, pChild);
							m_parentChild.push_back(ParentChildInfo(true, m_lfemalePopln[persn].getSpouseIndex(), false, bchildGen, lindex));
						}
						bool childgen = (pChild.getGender() == GENDER_TYPE_MALE) ? true : false;
						for (auto i = m_lfemalePopln[persn].children_begin(); i < m_lfemalePopln[persn].children_end() - 1; i++)
						{
							m_Siblings.push_back(SiblingsInfo(false, childgen, pChild.m_nListIndex, i->m_bMature, i->m_bMale, i->m_nIndex));
							m_Siblings.push_back(SiblingsInfo(i->m_bMature, i->m_bMale, i->m_nIndex, false, childgen, pChild.m_nListIndex));
						}
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event NEW_CHILD", i, persn.getFullName().c_str());
					}
					else if (ev == EVENT_TYPE_MARRIAGE)
					{
						makeRandomEventMarriage(persn);
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event MARRIAGE", i, persn.getFullName().c_str());
					}
					else if (ev == EVENT_TYPE_HOME_PURCHASE)
					{
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event HOME_PURCHASE", i, persn.getFullName().c_str());
						pev.m_nEventType = EVENT_TYPE_HOME_PURCHASE;
						pev.m_strEventComments = "New home address=***";
						m_lfemalePopln[persn].AddEvent(pev, m_lfemalePopln[persn]);
					}
					else if (ev == EVENT_TYPE_NEW_JOB)
					{
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event NEW_JOB", i, persn.getFullName().c_str());
						pev.m_nEventType = EVENT_TYPE_NEW_JOB;

						int randFSts = generateRandomNumber_(0, (numOfOffices - 1));
						if (randFSts < 0)
							randFSts = 0;
						iniAddress addrs(m_lOffices[randFSts]);
						pev.m_strEventComments = addrs.address;
						m_lfemalePopln[persn].setOfficeName(addrs.name, addrs.address);
						m_lfemalePopln[persn].AddEvent(pev, m_lfemalePopln[persn]);
					}
				} // for events
			}	  // for total events
		}		  // for female population

		/////////////////////////////////////////////////////////////////////////////////////////////////
		// Predict life events for male population and take necessary action
		/////////////////////////////////////////////////////////////////////////////////////////////////
		for (size_t persn = 0; persn < m_lmalePopln.size(); ++persn) //(auto persn : m_lmalePopln)
		{
			// AgeObsSymbol ageObs = CHILD;

			int age = m_lmalePopln[persn].getAge();
			m_lmalePopln[persn].setAge(age + 1);

			int randTotalEvents = generateRandomNumber_(1, 5);
			if (age < 20)
			{
				randTotalEvents = 1;
			}
			for (int te = 0; te < randTotalEvents; te++)
			{

				std::vector<Observation> observations;
				Observation ob;
				ob.age = age;
				ob.financial_status = m_lmalePopln[persn].getFinancialSts_();
				observations.push_back(ob);
				std::vector<std::string> events = m_cslifeChoice->predict(observations);
				for (auto evst : events)
				{
					EventType_ ev = CUtil::getLifeEventFromString(evst);
					PersonEvent pev;
					if (ev == EVENT_TYPE_GRADUATION)
					{
						pev.m_nEventType = EVENT_TYPE_GRADUATION;
						pev.m_strEventComments = "SENIOR";
						if ((age > 15) && (age < 70))
						{
							if (m_lmalePopln[persn].getGraduation() == GRAD_TYPE_NONE)
							{
								pev.m_strEventComments = "SENIOR";
							}
							else if (m_lmalePopln[persn].getGraduation() == GRAD_TYPE_SENIOR_HIGH)
							{
								pev.m_strEventComments = "DIPLOMA";
							}
							else if (m_lmalePopln[persn].getGraduation() == GRAD_TYPE_DIPLOMA)
							{
								pev.m_strEventComments = "ADVANCED";
							}
							else if (m_lmalePopln[persn].getGraduation() == GRAD_TYPE_ADVANCED_DIPLOMA)
							{
								pev.m_strEventComments = "BACHELOR";
							}
							else if (m_lmalePopln[persn].getGraduation() == GRAD_TYPE_BACHELOR)
							{
								pev.m_strEventComments = "MASTER";
							}
							else if (m_lmalePopln[persn].getGraduation() == GRAD_TYPE_MASTER)
							{
								pev.m_strEventComments = "DOCTORAL";
							}
						}
						m_lmalePopln[persn].AddEvent(pev, m_lmalePopln[persn]);
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: New event GRADUATION", i);
					}
					else if (ev == EVENT_TYPE_NEW_CHILD)
					{
						if (m_lmalePopln[persn].getChildCount() > MAX_CHILD_PER_PERSON)
						{
							continue;
						}

						if (m_lmalePopln[persn].getSpouseIndex() == -1)
						{
							continue;
						}
						int randgen = generateRandomNumber_(0, (long)(EVENT_INFO_END));
						// int randgen = 0 + rand() % (EVENT_INFO_END);
						GenderInfo_ ginfo = GENDER_TYPE_MALE;
						bool bchildGen = true;
						if (randgen > GENDER_TYPE_MALE)
						{
							bchildGen = false;
							ginfo = GENDER_TYPE_FEMALE;
						}
						std::pair<std::string, std::string> fullname = pInoGen->GenerateRandomName(ginfo);
						_dob stdob = pInoGen->GenerateRandomDOB(ELDERLY_DOB_ST, ELDERLY_DOB_EN);
						Graduation_ grad = GRAD_TYPE_NONE;
						MaritalSatus_ msts = MARITAL_STATUS_NEVER_MARRIED;
						CPersonInfo pChild(fullname.first, fullname.second, stdob, ginfo, m_lmalePopln[persn].getHomeAddress());
						pChild.setLastName(m_lmalePopln[persn].getLastName());
						pChild.setFatherName(m_lmalePopln[persn].getFullName());
						pChild.setMotherName(m_lmalePopln[persn].getSpouseName());
						pChild.setSSN(pInoGen->GenerateRandomSSN());
						pChild.setGraduation(grad);
						pChild.setMaritalStatus(msts);
						pChild.setHasChildren(false);
						pChild.m_nGeneration = m_lmalePopln[persn].m_nGeneration + 1;
						size_t lindex = m_lChildPopln.size();
						pChild.setListIndex(lindex);
						pChild.m_nListFatherIndex = static_cast<long>(m_lmalePopln[persn].m_nListIndex);
						pChild.m_nListMotherIndex = static_cast<long>(m_lmalePopln[persn].getSpouseIndex());
						m_lChildPopln.push_back(pChild);
						pev.m_nEventType = EVENT_TYPE_NEW_CHILD;
						pev.m_strEventComments = "NEW CHILD";
						m_lmalePopln[persn].AddEvent(pev, pChild);
						m_parentChild.push_back(ParentChildInfo(true, m_lmalePopln[persn].m_nListIndex, false, bchildGen, lindex));
						if (-1 != m_lmalePopln[persn].getSpouseIndex())
						{
							m_lfemalePopln[m_lmalePopln[persn].getSpouseIndex()].AddEvent(pev, pChild);
							m_parentChild.push_back(ParentChildInfo(false, m_lmalePopln[persn].getSpouseIndex(), false, bchildGen, lindex));
						}

						bool childgen = (pChild.getGender() == GENDER_TYPE_MALE) ? true : false;
						for (auto i = m_lmalePopln[persn].children_begin(); i < m_lmalePopln[persn].children_end() - 1; i++)
						{
							m_Siblings.push_back(SiblingsInfo(false, childgen, pChild.m_nListIndex, i->m_bMature, i->m_bMale, i->m_nIndex));
							m_Siblings.push_back(SiblingsInfo(i->m_bMature, i->m_bMale, i->m_nIndex, false, childgen, pChild.m_nListIndex));
						}
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event NEW_CHILD", i, persn.getFullName().c_str());
					}
					else if (ev == EVENT_TYPE_MARRIAGE)
					{
						makeRandomEventMarriage(persn);
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event MARRIAGE", i, persn.getFullName().c_str());
					}
					else if (ev == EVENT_TYPE_HOME_PURCHASE)
					{
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event HOME_PURCHASE", i, persn.getFullName().c_str());
						pev.m_nEventType = EVENT_TYPE_HOME_PURCHASE;
						pev.m_strEventComments = "New home address=***";
						m_lmalePopln[persn].AddEvent(pev, m_lmalePopln[persn]);
					}
					else if (ev == EVENT_TYPE_NEW_JOB)
					{
						// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event NEW_JOB", i, persn.getFullName().c_str());
						pev.m_nEventType = EVENT_TYPE_NEW_JOB;
						int randFSts = generateRandomNumber_(0, (numOfOffices - 1));
						if (randFSts < 0)
							randFSts = 0;
						iniAddress addrs(m_lOffices[randFSts]);
						pev.m_strEventComments = addrs.address;
						m_lmalePopln[persn].AddEvent(pev, m_lmalePopln[persn]);
						m_lmalePopln[persn].setOfficeName(addrs.name, addrs.address);
					}
				} // for events
			}	  // for total events
		}		  ////for male population

		/////////////////////////////////////////////////////////////////////////////////////////////////
		// Predict life events for child population and take necessary action
		/////////////////////////////////////////////////////////////////////////////////////////////////
		for (size_t child = 0; child < m_lChildPopln.size(); ++child) //(auto child : m_lChildPopln)
		{
			int age = m_lChildPopln[child].getAge();
			m_lChildPopln[child].setAge(age + 1);
			std::vector<Observation> observations;
			Observation ob;
			ob.age = age;
			ob.financial_status = m_lChildPopln[child].getFinancialSts_();
			observations.push_back(ob);
			std::vector<std::string> events = m_cslifeChoice->predict(observations);
			for (auto evst : events)
			{
				EventType_ ev = CUtil::getLifeEventFromString(evst);
				PersonEvent pev;
				if (ev == EVENT_TYPE_GRADUATION)
				{
					pev.m_nEventType = EVENT_TYPE_GRADUATION;
					pev.m_strEventComments = "SENIOR";
					if ((age > 15) && (age < 70))
					{
						if (m_lChildPopln[child].getGraduation() == GRAD_TYPE_NONE)
						{
							pev.m_strEventComments = "SENIOR";
						}
						else if (m_lChildPopln[child].getGraduation() == GRAD_TYPE_SENIOR_HIGH)
						{
							pev.m_strEventComments = "DIPLOMA";
						}
						else if (m_lChildPopln[child].getGraduation() == GRAD_TYPE_DIPLOMA)
						{
							pev.m_strEventComments = "ADVANCED";
						}
						else if (m_lChildPopln[child].getGraduation() == GRAD_TYPE_ADVANCED_DIPLOMA)
						{
							pev.m_strEventComments = "BACHELOR";
						}
						else if (m_lChildPopln[child].getGraduation() == GRAD_TYPE_BACHELOR)
						{
							pev.m_strEventComments = "MASTER";
						}
						else if (m_lChildPopln[child].getGraduation() == GRAD_TYPE_MASTER)
						{
							pev.m_strEventComments = "DOCTORAL";
						}
					}
					m_lChildPopln[child].AddEvent(pev, m_lChildPopln[child]);
					// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][child population]: New event GRADUATION", i);
				}
				else if (ev == EVENT_TYPE_NEW_JOB)
				{
					// CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][child population]: name = %s New event NEW_JOB", i, child.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_NEW_JOB;
					int randFSts = generateRandomNumber_(0, (long)(numOfOffices - 1));
					if (randFSts < 0)
						randFSts = 0;
					iniAddress addrs(m_lOffices[randFSts]);
					pev.m_strEventComments = addrs.address;
					m_lChildPopln[child].setOfficeName(addrs.name, addrs.address);
					m_lChildPopln[child].AddEvent(pev, m_lChildPopln[child]);
				}
			}
		} // for child
	}
}

bool CPopulationDB::getRandomParent(int &retMale, int &retFemale, bool bskipCond)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(m_lfemalePopln.size());
	// int randNoParentM = -1;
	int randNoParentF = -1;
	do
	{
		randNoParentF = generateRandomNumber_(nMin, (long)(nMax - 1));
		// randNoM = nMin + rand() % ((nMax)-nMin);
		if (-1 == randNoParentF)
		{
			break;
		}

		bool bFound = true;
		if (-1 == m_lfemalePopln[randNoParentF].getSpouseIndex())
		{
			// If no partner then same person data is returned. Check if its same
			bFound = false;
		}
		else
		{
			if (m_lmalePopln[m_lfemalePopln[randNoParentF].getSpouseIndex()].getChildCount() > MAX_CHILD_PER_PERSON)
			{
				bFound = false;
			}
		}

		if (m_lfemalePopln[randNoParentF].getChildCount() >= MAX_CHILD_PER_PERSON)
		{
			bFound = false;
		}

		if (m_lfemalePopln[randNoParentF].getMaritalStatus() == MARITAL_STATUS_NEVER_MARRIED)
		{
			bFound = false;
		}

		if (m_lfemalePopln[randNoParentF].getAge() >= 50)
		{
			bFound = false;
		}

		if (false == bFound)
		{
			for (int m = randNoParentF + 1; m < nMax; ++m)
			{
				if (m_lfemalePopln[m].getMaritalStatus() != MARITAL_STATUS_NEVER_MARRIED)
				{
					randNoParentF = m;
					bRet = true;
					break;
				}
			}
			if (false == bRet)
			{
				for (int m = nMin; m < randNoParentF; ++m)
				{
					if (m_lfemalePopln[m].getMaritalStatus() != MARITAL_STATUS_NEVER_MARRIED)
					{
						randNoParentF = m;
						bRet = true;
						break;
					}
				}
			}
		} // if not found

	} while (bRet == false);

	if (bRet)
	{
		retFemale = randNoParentF;
		retMale = (int)m_lfemalePopln[randNoParentF].getSpouseIndex();
		if (-1 == retMale)
		{
			// bRet = false;
		}
		else
		{
			if (m_lmalePopln[retMale].getChildCount() > MAX_CHILD_PER_PERSON)
			{
				bRet = false;
			}
		}
	}
	else
	{
		if (bskipCond)
		{
			for (int f = 0; f < nMax; ++f)
			{
				if ((m_lfemalePopln[f].getChildCount() == 0) && (m_lfemalePopln[f].getAge() > 18))
				{
					retFemale = f;
					retMale = (int)m_lfemalePopln[f].getSpouseIndex();
					bRet = true;
					break;
				}
			}
		}
	}
	return bRet;
}

bool CPopulationDB::getRandomMaleParent(size_t maxsize, int &retMale)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(maxsize);
	int randNoM = generateRandomNumber_(nMin, (nMax - 1));
	// int randNoM = nMin + rand() % ((nMax)-nMin);
	do
	{
		bool bFound = true;
		if (-1 == m_lmalePopln[randNoM].getSpouseIndex())
		{
			// If no partner then same person data is returned. Check if its same
			bFound = false;
		}
		else
		{
			if (m_lfemalePopln[m_lmalePopln[randNoM].getSpouseIndex()].getChildCount() > MAX_CHILD_PER_PERSON)
			{
				bFound = false;
			}
		}
		if (m_lmalePopln[randNoM].getChildCount() > MAX_CHILD_PER_PERSON)
		{
			bFound = false;
		}

		if (m_lmalePopln[randNoM].getMaritalStatus() != MARITAL_STATUS_MARRIED)
		{
			bFound = false;
		}

		if (false == bFound)
		{
			for (int m = nMin; m < randNoM; ++m)
			{
				if ((m_lmalePopln[m].getChildCount() < MAX_CHILD_PER_PERSON) && (m_lmalePopln[m].getAge() > 18))
				{
					randNoM = m;
					bRet = true;
					break;
				}
			}

			if (false == bRet)
			{
				for (int m = randNoM + 1; m < nMax; ++m)
				{
					if ((m_lmalePopln[m].getChildCount() < MAX_CHILD_PER_PERSON) && (m_lmalePopln[m].getAge() > 18))
					{
						randNoM = m;
						bRet = true;
						break;
					}
				}
			}
		} // if not found
		else
		{
			bRet = true;
		}

	} while (false);
	if (bRet)
	{
		retMale = randNoM;
	}
	return bRet;
}

bool CPopulationDB::getRandomFeMaleParent(size_t maxsize, int &retFeMale)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(maxsize);
	int randNoF = generateRandomNumber_(nMin, (long)(nMax - 1));
	do
	{
		bool bFound = true;
		if (-1 == m_lfemalePopln[randNoF].getSpouseIndex())
		{
			// If no partner then same person data is returned. Check if its same
			bFound = false;
		}
		else
		{
			if (m_lmalePopln[m_lfemalePopln[randNoF].getSpouseIndex()].getChildCount() > MAX_CHILD_PER_PERSON)
			{
				bFound = false;
			}
		}

		if (m_lfemalePopln[randNoF].getChildCount() > MAX_CHILD_PER_PERSON)
		{
			bFound = false;
		}

		if (m_lfemalePopln[randNoF].getMaritalStatus() != MARITAL_STATUS_MARRIED)
		{
			bFound = false;
		}

		if (false == bFound)
		{
			for (int f = randNoF + 1; f < nMax; ++f)
			{
				if ((m_lfemalePopln[f].getChildCount() < MAX_CHILD_PER_PERSON) && (m_lfemalePopln[f].getAge() > 18))
				{
					randNoF = f;
					bRet = true;
					break;
				}
			}
			if (false == bRet)
			{
				for (int f = nMin; f < randNoF; ++f)
				{
					if ((m_lfemalePopln[f].getChildCount() < MAX_CHILD_PER_PERSON) && (m_lfemalePopln[f].getAge() > 18))
					{
						randNoF = f;
						bRet = true;
						break;
					}
				}
			}
		} // if not found
		else
		{
			bRet = true;
		}

	} while (false);
	if (bRet)
	{
		retFeMale = randNoF;
	}
	return bRet;
}

bool CPopulationDB::getRandomMaleForMarriage(int &retIndex)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(m_lmalePopln.size());
	int randNoM = generateRandomNumber_(nMin, (long)(nMax - 1));
	// int randNoM = nMin + rand() % ((nMax) - nMin);
	std::random_device rd1;	  // obtain a random number from hardware
	std::mt19937 gen1(rd1()); // seed the generator
	std::uniform_int_distribution<> distr1(0, (int)m_lmalePopln.size() - 1);
	randNoM = distr1(gen1);
	do
	{
		if ((m_lmalePopln[randNoM].getMaritalStatus() == MARITAL_STATUS_MARRIED) || (m_lmalePopln[randNoM].getMaritalStatus() == MARITAL_STATUS_SEPERATED) || (m_lmalePopln[randNoM].getNumberofMarriages() > MAX_MARRIAGES_PER_PERSON))
		{
			for (int m = nMin; m < randNoM; ++m)
			{
				if (m_lmalePopln[m].getMaritalStatus() != MARITAL_STATUS_MARRIED)
				{
					randNoM = m;
					bRet = true;
					break;
				}
			}

			if (false == bRet)
			{
				for (int m = randNoM + 1; m < nMax; ++m)
				{
					if (m_lmalePopln[m].getMaritalStatus() != MARITAL_STATUS_MARRIED)
					{
						randNoM = m;
						bRet = true;
						break;
					}
				}
			}
		}
		else
		{
			bRet = true;
		}
	} while (false);
	retIndex = randNoM;
	return bRet;
}

bool CPopulationDB::getRandomFemaleForMarriage(int &retIndex)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(m_lfemalePopln.size());

	int randNoF = generateRandomNumber_(nMin, (long)(nMax - 1));
	// int randNoF = nMin + rand() % ((nMax) - nMin);
	std::random_device rd1;	  // obtain a random number from hardware
	std::mt19937 gen1(rd1()); // seed the generator
	std::uniform_int_distribution<> distr1(0, (int)m_lfemalePopln.size() - 1);
	randNoF = distr1(gen1);
	do
	{
		if ((m_lfemalePopln[randNoF].getMaritalStatus() == MARITAL_STATUS_MARRIED) || (m_lfemalePopln[randNoF].getMaritalStatus() == MARITAL_STATUS_SEPERATED) || (m_lfemalePopln[randNoF].getGender() == GENDER_TYPE_OTHER) || (m_lfemalePopln[randNoF].getNumberofMarriages() > MAX_MARRIAGES_PER_PERSON))
		{
			for (int f = nMin; f < randNoF; ++f)
			{
				if (m_lfemalePopln[f].getMaritalStatus() != MARITAL_STATUS_MARRIED)
				{
					randNoF = f;
					bRet = true;
					break;
				}
			}

			if (false == bRet)
			{
				for (int f = randNoF + 1; f < nMax; ++f)
				{
					if (m_lfemalePopln[f].getMaritalStatus() != MARITAL_STATUS_MARRIED)
					{
						randNoF = f;
						bRet = true;
						break;
					}
				}
			}
		} // female
		else
		{
			bRet = true;
		}
	} while (false);
	retIndex = randNoF;
	return bRet;
}

void CPopulationDB::makeRandomEventMarriage(CPersonInfo &partner)
{
	if (partner.getGender() == GENDER_TYPE_MALE)
	{
		int index = 0;
		if (getRandomFemaleForMarriage(index))
		{
			m_lfemalePopln[index].setMaritalStatus(MARITAL_STATUS_MARRIED);
			partner.setMaritalStatus(MARITAL_STATUS_MARRIED);
			std::string malePrtnr = partner.getFirstName() + " " + partner.getLastName();
			std::string femalePrtnr = m_lfemalePopln[index].getFirstName() + " " + m_lfemalePopln[index].getLastName();
			PersonEvent tempEv;
			tempEv.m_nEventType = EVENT_TYPE_MARRIAGE;
			tempEv.m_strLocation = m_lfemalePopln[index].getHomeAddress();
			tempEv.m_strEventComments = malePrtnr;
			m_lfemalePopln[index].AddEvent(tempEv, partner);
			tempEv.m_strEventComments = femalePrtnr;
			partner.AddEvent(tempEv, m_lfemalePopln[index]);
			m_lfemalePopln[index].setHomeAddress(partner.getHomeAddress());

			m_Marriages.push_back(MarriagesInfo(true, partner.m_nListIndex, false, m_lfemalePopln[index].m_nListIndex));
			m_Marriages.push_back(MarriagesInfo(false, m_lfemalePopln[index].m_nListIndex, true, partner.m_nListIndex));
			// CTextFileLogger::GetLogger()->Log("Marriage -> %s & %s", malePrtnr.c_str(), femalePrtnr.c_str());
		}
	}
	else
	{
		int randNoM = 0;
		if (getRandomMaleForMarriage(randNoM))
		{
			partner.setMaritalStatus(MARITAL_STATUS_MARRIED);
			m_lmalePopln[randNoM].setMaritalStatus(MARITAL_STATUS_MARRIED);
			std::string malePrtnr = m_lmalePopln[randNoM].getFirstName() + " " + m_lmalePopln[randNoM].getLastName();
			std::string femalePrtnr = partner.getFirstName() + " " + partner.getLastName();
			PersonEvent tempEv;
			tempEv.m_nEventType = EVENT_TYPE_MARRIAGE;
			tempEv.m_strLocation = partner.getHomeAddress();
			tempEv.m_strEventComments = malePrtnr;
			partner.AddEvent(tempEv, m_lmalePopln[randNoM]);
			tempEv.m_strEventComments = femalePrtnr;
			m_lmalePopln[randNoM].AddEvent(tempEv, partner);
			partner.setHomeAddress(m_lmalePopln[randNoM].getHomeAddress());

			m_Marriages.push_back(MarriagesInfo(false, partner.m_nListIndex, true, m_lmalePopln[randNoM].m_nListIndex));
			m_Marriages.push_back(MarriagesInfo(true, m_lmalePopln[randNoM].m_nListIndex, false, partner.m_nListIndex));
			// CTextFileLogger::GetLogger()->Log("Marriage -> %s & %s", malePrtnr.c_str(), femalePrtnr.c_str());
		}
	}
}

void CPopulationDB::makeRandomEventMarriage(int ncount)
{
	long totalmarriages = 0;
	for (int cnt = 0; cnt < ncount; cnt++)
	{
		bool bmalefound = false;
		bool bfemalefound = false;
		int randNoM = 0;
		int randNoF = 0;
		bmalefound = getRandomMaleForMarriage(randNoM);
		bfemalefound = getRandomFemaleForMarriage(randNoF);
		if ((false == bfemalefound) || (false == bmalefound))
		{
			int logm = (bmalefound) ? 1 : 0;
			int logf = (bfemalefound) ? 1 : 0;
			// CTextFileLogger::GetLogger()->Log("makeRandomEventMarriage -> bfemalefound = %d,bmalefound = %d, randNoM = %d, randNoF = %d, MaleSize=%d, FemaleSize=%d", logf, logm, randNoM, randNoF, m_lmalePopln.size(), m_lfemalePopln.size());
			break;
		}
		totalmarriages++;
		m_lfemalePopln[randNoF].setMaritalStatus(MARITAL_STATUS_MARRIED);
		m_lmalePopln[randNoM].setMaritalStatus(MARITAL_STATUS_MARRIED);
		std::string malePrtnr = m_lmalePopln[randNoM].getFirstName() + " " + m_lmalePopln[randNoM].getLastName();
		std::string femalePrtnr = m_lfemalePopln[randNoF].getFirstName() + " " + m_lfemalePopln[randNoF].getLastName();
		PersonEvent tempEv;
		tempEv.m_nEventType = EVENT_TYPE_MARRIAGE;
		tempEv.m_strLocation = m_lfemalePopln[randNoF].getHomeAddress();
		tempEv.m_strEventComments = malePrtnr;
		m_lfemalePopln[randNoF].AddEvent(tempEv, m_lmalePopln[randNoM]);
		tempEv.m_strEventComments = femalePrtnr;
		m_lmalePopln[randNoM].AddEvent(tempEv, m_lfemalePopln[randNoF]);
		m_lfemalePopln[randNoF].setHomeAddress(m_lmalePopln[randNoM].getHomeAddress());

		m_Marriages.push_back(MarriagesInfo(false, m_lfemalePopln[randNoF].m_nListIndex, true, m_lmalePopln[randNoM].m_nListIndex));
		m_Marriages.push_back(MarriagesInfo(true, m_lmalePopln[randNoM].m_nListIndex, false, m_lfemalePopln[randNoF].m_nListIndex));
		// CTextFileLogger::GetLogger()->Log("Marriage -> %s & %s", malePrtnr.c_str(), femalePrtnr.c_str());
	} // for count
	CTextFileLogger::GetLogger()->Log("makeRandomEventMarriage -> Total created = %d, Needed = %d", totalmarriages, m_lTotalMarriages);
}

void CPopulationDB::makeRandomEventDeaths()
{
	long totaldeathsadult = static_cast<long>((m_lmalePopln.size() + m_lfemalePopln.size()) * parser_.GetDblValue(SECTION_POP, KEY_DEATH_RATE));
	long totaldeathsinfants = 0;

	std::cout << "makeRandomEventDeaths()  -> average adults death per year" << totaldeathsadult << std::endl;
	// std::cout << "makeRandomEventDeaths()  -> average infant death per year" <<totaldeathsadult<< std::endl;

	int nMinM = 0;
	int nMax_M = static_cast<int>(m_lmalePopln.size());
	if (nMinM >= nMax_M)
	{
		nMinM = 0;
	}
	int nMinF = 0;
	int nMax_F = static_cast<int>(m_lfemalePopln.size());
	if (nMinF >= nMax_F)
	{
		nMinF = 0;
	}
	// struct timespec ts;

	for (int i = 0; i < totaldeathsadult / 2; ++i)
	{
		int randNoM = generateRandomNumber_(0, (long)(nMax_M - 1));
		// int randNoM = nMinM + rand() % ((nMax_M + 1) - nMinM);

		bool bMaleFound = false;
		bool bFemaleFound = false;

		if ((randNoM < 0) || (randNoM >= m_lmalePopln.size()))
		{
			randNoM = static_cast<int>(m_lmalePopln.size() - 1);
		}

		if (m_lmalePopln[randNoM].getDOB_Year() > WORKING_DOB_ST)
		{
			for (int m = randNoM + 1; m < nMax_M; ++m)
			{
				if (m_lmalePopln[m].getDOB_Year() < WORKING_DOB_ST)
				{
					randNoM = m;
					bMaleFound = true;
					break;
				}
			}
			if (false == bMaleFound)
			{
				for (int m = nMinM; m < randNoM; ++m)
				{
					if (m_lmalePopln[m].getDOB_Year() < WORKING_DOB_ST)
					{
						randNoM = m;
						bMaleFound = true;
						break;
					}
				}
				if (false == bMaleFound)
				{
					break;
				}
			}
		}
		else
		{
			bMaleFound = true;
		}

		int randNoF = generateRandomNumber_(0, (long)(nMax_F - 1));
		// int randNoF = nMinF + rand() % ((nMax_F + 1) - nMinF);

		if ((randNoF < 0) || (randNoF >= m_lfemalePopln.size()))
		{
			randNoF = static_cast<int>(m_lfemalePopln.size() - 1);
		}

		if (m_lfemalePopln[randNoF].getDOB_Year() > WORKING_DOB_ST)
		{
			for (int f = randNoF + 1; f < nMax_M; ++f)
			{
				if (m_lfemalePopln[f].getDOB_Year() < WORKING_DOB_ST)
				{
					randNoF = f;
					bFemaleFound = true;
					break;
				}
			}
			if (false == bFemaleFound)
			{
				for (int f = nMinF; f < randNoF; ++f)
				{
					if (m_lfemalePopln[f].getDOB_Year() < WORKING_DOB_ST)
					{
						randNoF = f;
						bFemaleFound = true;
						break;
					}
				}
				if (false == bFemaleFound)
				{
					break;
				}
			}
		}
		else
		{
			bFemaleFound = true;
		}

		// make death events
		PersonEvent ev;
		ev.m_nEventType = EVENT_TYPE_DEATH;
		_dob tempdd = pInoGen->GenerateRandomDOB(2022, 2022, 2022);
		std::cout << "makeRandomEventDeaths()  -> " << i << bFemaleFound << bMaleFound << std::endl;
		if (bMaleFound)
		{
			m_lmalePopln[randNoM].setDOD(tempdd);
			m_lmalePopln[randNoM].AddEvent(ev, m_lmalePopln[randNoM]);
		}
		if (bFemaleFound)
		{
			m_lfemalePopln[randNoF].setDOD(tempdd);
			m_lfemalePopln[randNoF].AddEvent(ev, m_lfemalePopln[randNoF]);
		}
	}
}

bool CPopulationDB::assignChildtoRandomFamily(CPersonInfo &child)
{
	int ParentIndex_M = -1;
	int ParentIndex_F = -1;
	bool bRetry = false;
	bool bReset = false;
	int retrycount = 0;
	const int retrycountmax = 3;
	bool bRet = false;
	do
	{
		if (getRandomParent(ParentIndex_M, ParentIndex_F, bReset))
		{
			std::string father = "NA";
			std::string lname = m_lfemalePopln[ParentIndex_F].getLastName();
			std::string address = m_lfemalePopln[ParentIndex_F].getHomeAddress();
			long findex = -1;
			if (ParentIndex_M >= 0)
			{
				father = m_lmalePopln[ParentIndex_M].getFirstName() + " " + m_lmalePopln[ParentIndex_M].getLastName();
				m_lmalePopln[ParentIndex_M].setChildCount((int)m_lmalePopln[ParentIndex_M].getChildCount() + 1);
				lname = m_lmalePopln[ParentIndex_M].getLastName();
				address = m_lmalePopln[ParentIndex_M].getHomeAddress();
				findex = ParentIndex_M;
			}
			std::string mother = m_lfemalePopln[ParentIndex_F].getFirstName() + " " + m_lfemalePopln[ParentIndex_F].getLastName();
			m_lfemalePopln[ParentIndex_F].setChildCount((int)m_lfemalePopln[ParentIndex_F].getChildCount() + 1);
			std::string childname = "";
			child.setFatherName(father);
			child.setMotherName(mother);
			child.setLastName(lname);
			childname = child.getFirstName() + " " + child.getLastName();
			child.setHomeAddress(address);
			child.m_nListFatherIndex = findex;
			child.m_nListMotherIndex = ParentIndex_F;
			PersonEvent ev;
			ev.m_nEventType = EVENT_TYPE_NEW_CHILD;
			ev.m_strDate = child.getDOBstr();
			ev.m_strEventComments = childname;
			bool childgen = (child.getGender() == GENDER_TYPE_MALE) ? true : false;
			bool bMaturechild = (child.getAge() > 18) ? true : false;
			if (ParentIndex_M >= 0)
			{
				m_lmalePopln[ParentIndex_M].AddEvent(ev, child);
				m_lmalePopln[ParentIndex_M].setHasChildren(true);
				m_parentChild.push_back(ParentChildInfo(true, ParentIndex_M, bMaturechild, childgen, child.m_nListIndex));
			}
			m_lfemalePopln[ParentIndex_F].AddEvent(ev, child);
			m_lfemalePopln[ParentIndex_F].setHasChildren(true);
			m_parentChild.push_back(ParentChildInfo(false, ParentIndex_F, bMaturechild, childgen, child.m_nListIndex));

			for (auto i = m_lfemalePopln[ParentIndex_F].children_begin(); i < m_lfemalePopln[ParentIndex_F].children_end() - 1; i++)
			{
				m_Siblings.push_back(SiblingsInfo(bMaturechild, childgen, child.m_nListIndex, i->m_bMature, i->m_bMale, i->m_nIndex));
				m_Siblings.push_back(SiblingsInfo(i->m_bMature, i->m_bMale, i->m_nIndex, bMaturechild, childgen, child.m_nListIndex));
			}
			bRet = true;
			break;
			// CTextFileLogger::GetLogger()->Log("assignChildtoRandomFamily -> Child[%s]. Parents : %s, %s", childname.c_str(), father.c_str(), mother.c_str());
		}
		else
		{
			retrycount++;
			bRetry = true;
			if (retrycount >= retrycountmax)
			{
				bRetry = false;
				if (false == bReset)
				{
					bReset = true;
					bRetry = true;
				}
			}
			// CTextFileLogger::GetLogger()->Log("assignChildtoRandomFamily -> Could not find parents");
		}
	} while (true == bRetry);
	return bRet;
}

void CPopulationDB::makeRandomEventChild(int count, CPersonInfo &parentM, CPersonInfo &parentF)
{
	for (int chld = 0; chld < count; chld++)
	{
		int cindx = static_cast<int>(m_lChildPopln.size() - 1);
		bool bChildFound = false;
		int nMinC = 0;
		int nMaxC = static_cast<int>(m_lChildPopln.size());

		int randNoM = generateRandomNumber_(0, (long)(nMaxC - 1));
		// int randNoM = nMinC + rand() % ((nMaxC + 1) - nMinC);

		if ((randNoM < nMaxC) && (m_lChildPopln[randNoM].getFatherName() == ""))
		{
			cindx = randNoM;
			bChildFound = true;
		}
		else
		{
			for (auto it = m_lChildPopln.rbegin(); it != m_lChildPopln.rend(); it++)
			{
				if ((it->getDOB_Year() > 2005) && (it->getFatherName() == ""))
				{
					bChildFound = true;
					break;
				}
				cindx--;
			}
		}

		if ((cindx < 0) || (cindx >= m_lChildPopln.size()))
		{
			return;
		}

		if (bChildFound) // if found
		{
			std::string father = parentM.getFirstName() + " " + parentM.getLastName();
			std::string mother = parentF.getFirstName() + " " + parentF.getLastName();
			parentM.setChildCount((int)parentM.getChildCount() + 1);
			parentF.setChildCount((int)parentF.getChildCount() + 1);
			std::string childname = "";

			m_lChildPopln[cindx].setFatherName(father);
			m_lChildPopln[cindx].setMotherName(mother);
			m_lChildPopln[cindx].setLastName(parentM.getLastName());
			childname = m_lChildPopln[cindx].getFirstName() + " " + m_lChildPopln[cindx].getLastName();
			m_lChildPopln[cindx].setHomeAddress(parentM.getHomeAddress());
			PersonEvent ev;
			ev.m_nEventType = EVENT_TYPE_NEW_CHILD;
			ev.m_strDate = m_lChildPopln[cindx].getDOBstr();
			ev.m_strEventComments = childname;
			parentM.AddEvent(ev, m_lChildPopln[cindx]);
			parentF.AddEvent(ev, m_lChildPopln[cindx]);
			parentM.setHasChildren(true);
			parentF.setHasChildren(true);

			// m_parentChild.push_back(std::make_tuple(parentM, m_lChildPopln[cindx]));
			// m_parentChild.push_back(std::make_tuple(parentF, m_lChildPopln[cindx]));

			bool childgen = (m_lChildPopln[cindx].getGender() == GENDER_TYPE_MALE) ? true : false;
			bool bMaturechild = false;
			m_parentChild.push_back(ParentChildInfo(true, parentM.m_nListIndex, bMaturechild, childgen, cindx));
			m_parentChild.push_back(ParentChildInfo(false, parentF.m_nListIndex, bMaturechild, childgen, cindx));
		}

	} // for
	// std::cout << "makeRandomEventChild()  -> " <<count<< std::endl;
}

void CPopulationDB::linkUncleAuntEdges(std::string p1uniqueid, std::string p1relation, CPersonInfo &p2, std::set<std::string> &vecedges)
{
	std::string strEdge;
	for (auto i = p2.children_begin(); i < p2.children_end(); i++)
	{
		if (i->m_bMature)
		{
			if (i->m_bMale)
			{
				strEdge = p1uniqueid + ", " + m_lmalePopln[i->m_nIndex].getUniqueID() + p1relation;
				vecedges.insert(strEdge);
				strEdge = m_lmalePopln[i->m_nIndex].getUniqueID() + ", " + p1uniqueid + ",nephew\n";
				vecedges.insert(strEdge);
			}
			else
			{
				strEdge = p1uniqueid + ", " + m_lfemalePopln[i->m_nIndex].getUniqueID() + p1relation;
				vecedges.insert(strEdge);
				strEdge = m_lfemalePopln[i->m_nIndex].getUniqueID() + ", " + p1uniqueid + ",niece\n";
				vecedges.insert(strEdge);
			}
		}
		else
		{
			strEdge = p1uniqueid + ", " + m_lChildPopln[i->m_nIndex].getUniqueID() + p1relation;
			vecedges.insert(strEdge);
			if (i->m_bMale)
			{
				strEdge = m_lChildPopln[i->m_nIndex].getUniqueID() + ", " + p1uniqueid + ",nephew\n";
				vecedges.insert(strEdge);
			}
			else
			{
				strEdge = m_lChildPopln[i->m_nIndex].getUniqueID() + ", " + p1uniqueid + ",niece\n";
				vecedges.insert(strEdge);
			}
		}
	} // for
}

void CPopulationDB::updateSchoolsFromIni()
{
	long scount = parser_.GetNumOfSchools();
	m_lSchools.reserve(scount);
	for (long s = 0; s < scount; s++)
	{
		m_lSchools.push_back(parser_.GetSchoolNameNAddress_(s));
	}
	if (scount == 0)
	{
		m_lSchools.push_back(iniAddress());
	}
}

void CPopulationDB::updateOfficesFromIni()
{
	long ocount = parser_.GetNumOfOffices();
	m_lOffices.reserve(ocount);
	for (long o = 0; o < ocount; o++)
	{
		m_lOffices.push_back(parser_.GetOfficeAddress_(o));
	}
	if (ocount == 0)
	{
		m_lSchools.push_back(iniAddress());
	}
}

void CPopulationDB::updateUniversitiesFromIni()
{
	long ucount = parser_.GetNumOfUniversities();
	m_lUniversities.reserve(ucount);
	for (long u = 0; u < ucount; u++)
	{
		m_lUniversities.push_back(parser_.GetUnivstyNameNAddress_(u));
	}
	if (ucount == 0)
	{
		m_lSchools.push_back(iniAddress());
	}
}

/*
generate edges info in below format.
conn_from: UniqueID of person 1
conn_to : UniqueID of person 2
edge_type : Any one of these values['grand child', 'aunt', 'uncle', 'grand father', 'grand mother', 'sister', 'brother', 'nephew', 'niece', 'husband', 'wife', 'father', 'mother']
*/
void CPopulationDB::generate_family_tree_new()
{
	// std::cout << "generate_family_tree()  -> " << std::endl;
	CTextFileLogger::GetLogger()->Log("generate_family_tree()  -> start");
	std::ofstream outFile("family_edges.csv");
	outFile << "conn_from,conn_to,edge_type\n";

	std::set<std::string> relation_edges;

	for (auto parent : m_parentChild)
	{
		std::string strEdge = "NA";
		if (false == parent.m_bMatureChild)
		{
			if (parent.m_nChildIndex > m_lChildPopln.size())
			{
				continue;
			}
			if (true == parent.m_bParentGender)
			{
				strEdge = m_lmalePopln[parent.m_nParentIndex].getUniqueID() + ", " + m_lChildPopln[parent.m_nChildIndex].getUniqueID() + ", father\n";
			}
			else
			{
				strEdge = m_lfemalePopln[parent.m_nParentIndex].getUniqueID() + ", " + m_lChildPopln[parent.m_nChildIndex].getUniqueID() + ", mother\n";
			}
			// outFile << strEdge;
			relation_edges.insert(strEdge);
		}
		else
		{
			std::string childindex = "NA";
			if (true == parent.m_bChildGender)
			{
				childindex = m_lmalePopln[parent.m_nChildIndex].getUniqueID();
			}
			else
			{
				childindex = m_lfemalePopln[parent.m_nChildIndex].getUniqueID();
			}

			if (true == parent.m_bParentGender)
			{
				strEdge = m_lmalePopln[parent.m_nParentIndex].getUniqueID() + ", " + childindex + ", father\n";
			}
			else
			{
				strEdge = m_lfemalePopln[parent.m_nParentIndex].getUniqueID() + ", " + childindex + ", mother\n";
			}
			// outFile << strEdge;
			relation_edges.insert(strEdge);
		}
	}

	for (auto siblings : m_Siblings)
	{
		std::string strEdge = "NA";
		if (siblings.m_nIndexP1 == siblings.m_nIndexP2)
		{
			continue;
		}

		if ((true == siblings.m_bMatureP1) && (true == siblings.m_bMatureP2))
		{
			if (true == siblings.m_bGenderP1)
			{
				if (true == siblings.m_bGenderP2)
				{
					strEdge = m_lmalePopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lmalePopln[siblings.m_nIndexP2].getUniqueID() + ",brother\n";
					linkUncleAuntEdges(m_lmalePopln[siblings.m_nIndexP1].getUniqueID(), ",uncle\n", m_lmalePopln[siblings.m_nIndexP2], relation_edges);
				}
				else
				{
					strEdge = m_lmalePopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lfemalePopln[siblings.m_nIndexP2].getUniqueID() + ",brother\n";
					linkUncleAuntEdges(m_lmalePopln[siblings.m_nIndexP1].getUniqueID(), ",uncle\n", m_lfemalePopln[siblings.m_nIndexP2], relation_edges);
				}
			}
			else
			{
				if (true == siblings.m_bGenderP2)
				{
					strEdge = m_lfemalePopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lmalePopln[siblings.m_nIndexP2].getUniqueID() + ",sister\n";
					linkUncleAuntEdges(m_lfemalePopln[siblings.m_nIndexP1].getUniqueID(), ",aunt\n", m_lmalePopln[siblings.m_nIndexP2], relation_edges);
				}
				else
				{
					strEdge = m_lfemalePopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lfemalePopln[siblings.m_nIndexP2].getUniqueID() + ",sister\n";
					linkUncleAuntEdges(m_lfemalePopln[siblings.m_nIndexP1].getUniqueID(), ",aunt\n", m_lfemalePopln[siblings.m_nIndexP2], relation_edges);
				}
			}
		}
		else if ((true == siblings.m_bMatureP1) && (false == siblings.m_bMatureP2))
		{
			if (true == siblings.m_bGenderP1)
			{
				strEdge = m_lmalePopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lChildPopln[siblings.m_nIndexP2].getUniqueID() + ", brother\n";
			}
			else
			{
				strEdge = m_lfemalePopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lChildPopln[siblings.m_nIndexP2].getUniqueID() + ", sister\n";
			}
		}
		else if ((false == siblings.m_bMatureP1) && (true == siblings.m_bMatureP2))
		{
			if (true == siblings.m_bGenderP1)
			{
				if (true == siblings.m_bGenderP2)
				{
					strEdge = m_lChildPopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lmalePopln[siblings.m_nIndexP2].getUniqueID() + ", brother\n";
				}
				else
				{
					strEdge = m_lChildPopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lfemalePopln[siblings.m_nIndexP2].getUniqueID() + ", brother\n";
				}
			}
			else
			{
				if (true == siblings.m_bGenderP2)
				{
					strEdge = m_lChildPopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lmalePopln[siblings.m_nIndexP2].getUniqueID() + ", sister\n";
				}
				else
				{
					strEdge = m_lChildPopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lfemalePopln[siblings.m_nIndexP2].getUniqueID() + ", sister\n";
				}
			}
		}
		else
		{
			if (true == siblings.m_bGenderP1)
			{
				strEdge = m_lChildPopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lChildPopln[siblings.m_nIndexP2].getUniqueID() + ", brother\n";
			}
			else
			{
				strEdge = m_lChildPopln[siblings.m_nIndexP1].getUniqueID() + ", " + m_lChildPopln[siblings.m_nIndexP2].getUniqueID() + ", sister\n";
			}
		}
		// outFile << strEdge;
		relation_edges.insert(strEdge);
	} // for all siblings

	for (auto p = 0; p < m_Marriages.size(); ++p)
	{
		std::string strEdge = "NA";
		long inds1 = m_Marriages[p].m_nIndexP1;
		long inds2 = m_Marriages[p].m_nIndexP2;
		bool gender1 = m_Marriages[p].m_bGenderP1;
		bool gender2 = m_Marriages[p].m_bGenderP2;
		if (inds1 == inds2)
		{
			continue;
		}

		if (true == gender1)
		{
			if (false == gender2)
			{
				strEdge = m_lmalePopln[inds1].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", husband\n";
			}
			else
			{
				strEdge = m_lmalePopln[inds1].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", husband\n";
			}
		}
		else
		{
			if (false == gender2)
			{
				strEdge = m_lfemalePopln[inds1].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", wife\n";
			}
			else
			{
				strEdge = m_lfemalePopln[inds1].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", wife\n";
			}
		}
		// outFile << strEdge;
		relation_edges.insert(strEdge);

		if (true == gender1)
		{
			if (m_lmalePopln[inds1].getChildCount() > 0)
			{
				for (auto i = m_lmalePopln[inds1].children_begin(); i < m_lmalePopln[inds1].children_end(); i++)
				{
					long fatherindx = m_lmalePopln[inds1].m_nListFatherIndex;
					long motherindx = m_lmalePopln[inds1].m_nListMotherIndex;
					if ((fatherindx > 0) && (fatherindx < m_lmalePopln.size()))
					{
						if (true == i->m_bMature) // mature
						{
							if (true == i->m_bMale) // male
							{
								strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lmalePopln[i->m_nIndex].getUniqueID() + ", grand father\n";
								relation_edges.insert(strEdge);
								strEdge = m_lmalePopln[i->m_nIndex].getUniqueID() + ", " + m_lmalePopln[fatherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
							else
							{
								strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lfemalePopln[i->m_nIndex].getUniqueID() + ", grand father\n";
								relation_edges.insert(strEdge);
								strEdge = m_lfemalePopln[i->m_nIndex].getUniqueID() + ", " + m_lmalePopln[fatherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
						}
						else
						{
							strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lChildPopln[i->m_nIndex].getUniqueID() + ", grand father\n";
							relation_edges.insert(strEdge);
						} // if not mature
					}

					if ((motherindx > 0) && (motherindx < m_lfemalePopln.size()))
					{
						if (true == i->m_bMature) // mature
						{
							if (true == i->m_bMale) // male
							{
								strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lmalePopln[i->m_nIndex].getUniqueID() + ", grand mother\n";
								relation_edges.insert(strEdge);
								strEdge = m_lmalePopln[i->m_nIndex].getUniqueID() + ", " + m_lfemalePopln[motherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
							else
							{
								strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lfemalePopln[i->m_nIndex].getUniqueID() + ", grand mother\n";
								relation_edges.insert(strEdge);
								strEdge = m_lfemalePopln[i->m_nIndex].getUniqueID() + ", " + m_lfemalePopln[motherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
						}
						else
						{
							strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lChildPopln[i->m_nIndex].getUniqueID() + ", grand mother\n";
							relation_edges.insert(strEdge);
						} // if not mature
					}	  // if grand mother
				}		  // for children
			}			  // link grandparent
		}
		else
		{
			if (m_lfemalePopln[inds1].getChildCount() > 0)
			{
				for (auto i = m_lfemalePopln[inds1].children_begin(); i < m_lfemalePopln[inds1].children_end(); i++)
				{
					long fatherindx = m_lfemalePopln[inds1].m_nListFatherIndex;
					long motherindx = m_lfemalePopln[inds1].m_nListMotherIndex;
					if ((fatherindx > 0) && (fatherindx < m_lmalePopln.size()))
					{
						if (true == i->m_bMature) // mature
						{
							if (true == i->m_bMale) // male
							{
								strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lmalePopln[i->m_nIndex].getUniqueID() + ", grand father\n";
								relation_edges.insert(strEdge);
								strEdge = m_lmalePopln[i->m_nIndex].getUniqueID() + ", " + m_lmalePopln[fatherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
							else
							{
								strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lfemalePopln[i->m_nIndex].getUniqueID() + ", grand father\n";
								relation_edges.insert(strEdge);
								strEdge = m_lfemalePopln[i->m_nIndex].getUniqueID() + ", " + m_lmalePopln[fatherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
						}
						else
						{
							strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lChildPopln[i->m_nIndex].getUniqueID() + ", grand father\n";
							relation_edges.insert(strEdge);
						} // if not mature
					}	  // if grand father

					if ((motherindx > 0) && (motherindx < m_lfemalePopln.size()))
					{
						if (true == i->m_bMature) // mature
						{
							if (true == i->m_bMale) // male
							{
								strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lmalePopln[i->m_nIndex].getUniqueID() + ", grand mother\n";
								relation_edges.insert(strEdge);
								strEdge = m_lmalePopln[i->m_nIndex].getUniqueID() + ", " + m_lfemalePopln[motherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
							else
							{
								strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lfemalePopln[i->m_nIndex].getUniqueID() + ", grand mother\n";
								relation_edges.insert(strEdge);
								strEdge = m_lfemalePopln[i->m_nIndex].getUniqueID() + ", " + m_lfemalePopln[motherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
						}
						else
						{
							strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lChildPopln[i->m_nIndex].getUniqueID() + ", grand mother\n";
							relation_edges.insert(strEdge);
						} // if not mature
					}	  // if grand mother
				}		  // for children
			}			  // link grandparent
		}				  // if female
	}					  // for partners

	size_t totalcnt = relation_edges.size();
	for (std::set<std::string>::iterator itr = relation_edges.begin(); itr != relation_edges.end(); itr++) //(auto str : relation_edges)
	{
		outFile << *itr;
	}

	outFile.close();
	// CTextFileLogger::GetLogger()->Log("generate_family_tree()  -> end");
}

void CPopulationDB::split(std::string str, char del, std::vector<std::string> &out)
{
	std::string temp = "";
	for (int i = 0; i < (int)str.size(); i++)
	{
		if ((str[i] != del))
		{
			if ((str[i] == '\r') || (str[i] == '\n'))
				continue;
			temp += str[i];
		}
		else
		{
			out.push_back(temp);
			temp = "";
		}
	} // for
	out.push_back(temp);
}

void CPopulationDB::WritePopulationDataToFile(std::string filename)
{
	auto start = std::chrono::high_resolution_clock::now();
	if (filename == "")
	{
		filename = "db.csv";
	}
	std::ofstream outFile(filename);
	outFile << "UniqueID,FName,Lname,DOB,Age,Gender,FatherName,MotherName,HomeAddress,EducationLevel,MaritalStatus,SpouseName,HasChildren,NumOfChild,ChildrenNames,SchoolsAttended,IsEmployed,OfficeAddress,FatherIndexinDB,MotherIndexinDB";
	for (int evs = 0; evs < CUtil::m_nTotalEvents; ++evs)
	{
		std::string ev = std::to_string(evs + 1);
		std::string evens = ", Event" + ev + ", Event" + ev + " Info";
		outFile << evens;
	}
	outFile << "\n";

	for (size_t c = 0; c < m_lChildPopln.size(); ++c)
	{
		if (-1 != m_lChildPopln[c].m_nListFatherIndex)
		{
			if ("" == m_lChildPopln[c].getFatherName())
			{
				m_lChildPopln[c].setFatherName(m_lmalePopln[m_lChildPopln[c].m_nListFatherIndex].getFullName());
			}
		}

		if (-1 != m_lChildPopln[c].m_nListMotherIndex)
		{
			if ("" == m_lChildPopln[c].getMotherName())
			{
				m_lChildPopln[c].setMotherName(m_lfemalePopln[m_lChildPopln[c].m_nListMotherIndex].getFullName());
			}
		}
	}

	long totalFemalecount = 0;
	const size_t femalessize = m_lfemalePopln.size();
	for (size_t persn = 0; persn < femalessize; ++persn) //(auto persn : m_lfemalePopln)
	{
		totalFemalecount++;
		if (0 < m_lfemalePopln[persn].m_nListFatherIndex)
		{
			m_lfemalePopln[persn].m_nListFatherIndex = m_lfemalePopln[persn].m_nListFatherIndex + static_cast<long>(femalessize);
		}
		outFile << m_lfemalePopln[persn].GetFormatedString();
		outFile << "\n";
	}

	const size_t malessize = m_lmalePopln.size();
	for (size_t persn = 0; persn < malessize; ++persn) //(auto persn : m_lmalePopln)
	{
		if (0 < m_lmalePopln[persn].m_nListFatherIndex)
		{
			m_lmalePopln[persn].m_nListFatherIndex = m_lmalePopln[persn].m_nListFatherIndex + totalFemalecount;
		}
		outFile << m_lmalePopln[persn].GetFormatedString();
		outFile << "\n";
	}

	const size_t childsize = m_lChildPopln.size();
	for (size_t persn = 0; persn < childsize; ++persn) //(auto persn : m_lChildPopln)
	{
		if (0 < m_lChildPopln[persn].m_nListFatherIndex)
		{
			m_lChildPopln[persn].m_nListFatherIndex = m_lChildPopln[persn].m_nListFatherIndex + totalFemalecount;
		}
		outFile << m_lChildPopln[persn].GetFormatedString();
		outFile << "\n";
	}
	outFile.close();

	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = finish - start;
	std::cout << "Elapsed Time: " << elapsed.count() << " seconds" << std::endl;
}

long CPopulationDB::generateRandomNumber_(long min, long max) const
{
	std::random_device rd;
	std::mt19937_64 gen(rd());

	// Get the current system time in milliseconds
	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeMillis = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());

	// Generate a random number within the range
	std::uniform_int_distribution<long> distribution(min, max);
	long randomNumber;
	do
	{
		randomNumber = distribution(gen);
	} while (randomNumber == lastRandomNumber_ && currentTime == lastTimestamp_);

	// Store the last generated number and timestamp
	lastRandomNumber_ = randomNumber;
	lastTimestamp_ = currentTime;

	return randomNumber;
}

template <class T>
inline std::string CPopulationDB::to_string_(const T &t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}