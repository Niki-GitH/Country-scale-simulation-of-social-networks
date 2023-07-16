#include "../include/PopulationDB.h"
#include "../include/TextFileLogger.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

//long CPopulationDB::m_nTotalEvents = 0;

CPopulationDB::CPopulationDB()
{

	
}

CPopulationDB::CPopulationDB(int popCount)
{
	//CTextFileLogger::GetLogger()->Log("************ CPopulationDB::CPopulationDB >>>>>>>");
	pInoGen = std::make_unique<CPersonInfoGen>();
	ReadPopulationDBFile("./data/sweden_population_ref_scb_2020.csv");

	// Define the states
	std::vector<CareerState_> states = { WORKING, STUDYING };

	// Define the observations
	std::vector<FinacialObsSymbol> observations = { LOW, MEDIUM, HIGH };

	// Define the transition matrix
	std::vector<std::vector<double>> transition_matrix = {
	  {0.9, 0.1},
	  {0.5, 0.5}
	};

	// Define the emission matrix
	std::vector<std::vector<double>> emission_matrix = {
	  {0.1, 0.4, 0.5},
	  {0.3, 0.4, 0.3}
	};

	std::vector<double> pi = { 0.4,0.6 };

	m_csCareer = std::make_unique <CHMM_Career>(states, observations, transition_matrix, emission_matrix, pi);


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

	m_cslifeChoice = std::make_unique <CHMMEvents>(hstates, ages, fstatus);
	//m_cslifeChoice = std::make_unique <CHMMEvents>("./data/HMMEvents.model");

	m_lBasePopulationCnt = 0;
	m_lEvolvePopulationCnt = 0;
	m_lTotalPopulationCnt = popCount;
	m_lBasePopulationCnt = m_lTotalPopulationCnt;
	m_lEvolvePopulationCnt = m_lTotalPopulationCnt - m_lBasePopulationCnt;
	m_lChildrnPopulation = static_cast<long>(m_lBasePopulationCnt * CINIConfigParser::GetDblValue(SECTION_AD, KEY_AGE_14));
	m_lYoungPopulation = static_cast<long>(m_lBasePopulationCnt * CINIConfigParser::GetDblValue(SECTION_AD, KEY_AGE_24));
	double dwrkng = CINIConfigParser::GetDblValue(SECTION_AD, KEY_AGE_54) + CINIConfigParser::GetDblValue(SECTION_AD, KEY_AGE_64);
	m_lWorkingPopulation = static_cast<long>(m_lBasePopulationCnt * dwrkng);
	m_lElderPopulation = static_cast<long>(m_lBasePopulationCnt * CINIConfigParser::GetDblValue(SECTION_AD, KEY_AGE_MAX));	
	m_lTotalMarriages = static_cast<long>(m_lBasePopulationCnt * CINIConfigParser::GetDblValue(SECTION_MARRIAGES, KEY_AVG_MARRIAGE_RATE));


	//CTextFileLogger::GetLogger()->Log("************ CPopulationDB::CPopulationDB <<<<<<<<");
}

CPopulationDB::~CPopulationDB()
{
	
}


void CPopulationDB::ReadPopulationDBFile(std::string filepath)
{
	std::fstream myfile;

	// open file
	myfile.open(filepath);

	if (myfile.is_open()) {
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



CPopulationDB& CPopulationDB::operator=(const CPopulationDB& copyObj)
{
	if (&copyObj != this) {
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
		//vectors
		m_lmalePopln = copyObj.m_lmalePopln;		
		m_lfemalePopln = copyObj.m_lfemalePopln;		
		m_lChildPopln = copyObj.m_lChildPopln;	
		m_parentChild = copyObj.m_parentChild;		
		m_edges = copyObj.m_edges;		
		m_Siblings = copyObj.m_Siblings;
	}
	return *this;
}

void CPopulationDB::mergeCPopulationDB(const CPopulationDB& copyObj)
{
	if (&copyObj != this) {	
		//m_countyNames = copyObj.m_countyNames;
		//m_populationDB = copyObj.m_populationDB;
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

		for (auto x : copyObj.m_lmalePopln) //(size_t x =0; x< copyObj.m_lmalePopln.size();++x)// (auto x : copyObj.m_lmalePopln)
		{
			x.m_nListIndex = (long)tempSizeM + x.m_nListIndex;
			x.m_nListFatherIndex = (long)tempSizeM + x.m_nListFatherIndex;
			x.m_nListMotherIndex = (long)tempSizeF + x.m_nListMotherIndex;
			for (auto i = x.spouse_begin(); i < x.spouse_end(); i++)
			{
				*i = tempSizeF + *i;
			}
			for (auto i = x.children_begin(); i < x.children_end(); i++)
			{
				size_t temp = std::get<2>(*i);
				if (false == std::get<0>(*i))
				{
					std::get<2>(*i) = temp + tempSizeC;
				}
				else
				{
					if (true == std::get<1>(*i))
					{
						std::get<2>(*i) = temp + tempSizeM;
					}
					else
					{
						std::get<2>(*i) = temp + tempSizeF;
					}
				}
			}
			m_lmalePopln.push_back(x);
		}
		
		for (auto x : copyObj.m_lfemalePopln)
		{
			x.m_nListIndex = tempSizeF + x.m_nListIndex;
			x.m_nListFatherIndex = (long)tempSizeM + x.m_nListFatherIndex;
			x.m_nListMotherIndex = (long)tempSizeF + x.m_nListMotherIndex;
			for (auto i = x.spouse_begin(); i < x.spouse_end(); i++)
			{
				*i = tempSizeM + *i;
			}
			for (auto i = x.children_begin(); i < x.children_end(); i++)
			{
				size_t temp = std::get<2>(*i);
				if (false == std::get<0>(*i))
				{
					std::get<2>(*i) = temp + tempSizeC;
				}
				else
				{
					if (true == std::get<1>(*i))
					{
						std::get<2>(*i) = temp + tempSizeM;
					}
					else
					{
						std::get<2>(*i) = temp + tempSizeF;
					}
				}
			}
			m_lfemalePopln.push_back(x);
		}
		
		for (auto x : copyObj.m_lChildPopln)
		{
			x.m_nListIndex = tempSizeC + x.m_nListIndex;
			x.m_nListFatherIndex = (long)tempSizeM + x.m_nListFatherIndex;
			x.m_nListMotherIndex = (long)tempSizeF + x.m_nListMotherIndex;
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
				size_t temp = std::get<2>(*i);
				if (false == std::get<0>(*i))
				{
					std::get<2>(*i) = temp + tempSizeC;
				}
				else
				{
					if (true == std::get<1>(*i))
					{
						std::get<2>(*i) = temp + tempSizeM;
					}
					else
					{
						std::get<2>(*i) = temp + tempSizeF;
					}
				}
			}
			m_lChildPopln.push_back(x);
		}


		const size_t tempEdgParent = m_parentChild.size();
		const size_t tempEdgeSiblings = m_Siblings.size();
		const size_t tempEdges = m_edges.size();
		
		for (auto x : copyObj.m_parentChild)
		{
			if (true == std::get<0>(x))
			{
				std::get<1>(x) = std::get<1>(x) + tempSizeM;
			}
			else
			{
				std::get<1>(x) = std::get<1>(x) + tempSizeF;
			}
			if (true == std::get<2>(x))
			{
				if (true == std::get<3>(x))
				{
					std::get<4>(x) = std::get<4>(x) + tempSizeM;
				}
				else
				{
					std::get<4>(x) = std::get<4>(x) + tempSizeF;
				}
			}
			else
			{
				std::get<4>(x) = std::get<4>(x) + tempSizeC;
			}
			m_parentChild.push_back(x);
		}

		for (auto x : copyObj.m_Siblings)
		{	
			if (true == std::get<0>(x))
			{
				if (true == std::get<1>(x))
				{
					std::get<2>(x) = std::get<2>(x) + tempSizeM;
				}
				else
				{
					std::get<2>(x) = std::get<2>(x) + tempSizeF;
				}

				if (true == std::get<3>(x))
				{
					std::get<4>(x) = std::get<4>(x) + tempSizeM;
				}
				else
				{
					std::get<4>(x) = std::get<4>(x) + tempSizeF;
				}
			}
			else
			{
				std::get<2>(x) = std::get<2>(x) + tempSizeC;
				std::get<4>(x) = std::get<4>(x) + tempSizeC;
			}
			m_Siblings.push_back(x);
		}

		for (auto x : copyObj.m_edges)
		{
			if (true == std::get<0>(x))
			{
				std::get<1>(x) = std::get<1>(x) + tempSizeM;
			}
			else
			{
				std::get<1>(x) = std::get<1>(x) + tempSizeF;
			}

			if (true == std::get<2>(x))
			{
				std::get<3>(x) = std::get<3>(x) + tempSizeM;
			}
			else
			{
				std::get<3>(x) = std::get<3>(x) + tempSizeF;
			}
			m_edges.push_back(x);
		}

	}
}


void CPopulationDB::GenerateRandomPopulationData()
{
	double dmalepercentage = CINIConfigParser::GetDblValue(SECTION_GNDRD, KEY_MALE);
	double dfemalepercentage = CINIConfigParser::GetDblValue(SECTION_GNDRD, KEY_FEMALE);
	double dotherpercentage = 1.0 - (dmalepercentage + dfemalepercentage);
	const int maxage = CINIConfigParser::GetIntValue(SECTION_POP, KEY_MAX_AGE);
	const int minage = CINIConfigParser::GetIntValue(SECTION_POP, KEY_MIN_AGE);

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Generate elder population
	/////////////////////////////////////////////////////////////////////////////////////////////////
	long totalmalepop_elder = static_cast<long>(m_lElderPopulation*dmalepercentage);
	//long totalfemalepop_elder = (m_lElderPopulation*dfemalepercentage);
	long elderTotalUnmarried = 0;
	const long maxUnmarriedElder = static_cast<long>(m_lElderPopulation*0.05);
	for (long ep = 0; ep < m_lElderPopulation; ++ep)
	{
		int randCity = generateRandomNumber_(0, (long)m_populationDB.size());
		//int randCity = 0 + rand() % ((m_populationDB.size()) - 0);
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
		if (msts = MARITAL_STATUS_NEVER_MARRIED)
		{
			elderTotalUnmarried++;
			if (elderTotalUnmarried > maxUnmarriedElder)
			{
				//msts = MARITAL_STATUS_MARRIED;
			}
		}
		//std::string randAdd = pInoGen->GenerateRandomAddress(db.m_nArea, db.m_longitude, db.m_latitude);
		std::string randAdd = db.m_strLocalityName + "-" + db.m_strSubPrefec + "-" + db.m_strPrefec;
		CPersonInfo pMInfo(fullname.first, fullname.second, stdob, ginfo, randAdd);
		pMInfo.setSSN(pInoGen->GenerateRandomSSN());
		pMInfo.setGraduation(grad);
		pMInfo.setMaritalStatus(msts);
		pMInfo.setHasChildren(false);
		pMInfo.setFatherName(fathername.first + " " + fathername.second);
		pMInfo.setMotherName(mothername.first + " " + mothername.second);
		if (pMInfo.getAge() > maxage)
		{
			int year = pMInfo.getDOB_Year();
			int diff = pMInfo.getAge() - maxage;
			pMInfo.setAge(maxage);
			stdob.m_nYear = year - diff;
		}
		int randFSts = generateRandomNumber_(0, (long)FINANCE_STATUS_ULTRA_RICH);
		//int randFSts = FINANCE_STATUS_POOR + rand() % ((FINANCE_STATUS_ULTRA_RICH + 1) - FINANCE_STATUS_POOR);
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

		pMInfo.m_nListFatherIndex = -1;
		pMInfo.m_nListMotherIndex = -1;

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

	long total_sibling_con = static_cast<long>(totalmalepop_elder * 0.40);
	int bmmffmf = 0; //0:random brother connections, 1: sister-sister, 2: bother - sister
	for (int i = 0; i < total_sibling_con; i++)
	{
		std::random_device rd,rd1; // obtain a random number from hardware newrandom
		std::mt19937 gen(rd()); // seed the generator
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
			m_Siblings.push_back(std::make_tuple(true, true, sibcon_1, true, sibcon_2));
			m_Siblings.push_back(std::make_tuple(true, true, sibcon_2, true, sibcon_1));
			
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
			m_Siblings.push_back(std::make_tuple(true, false, sibcon_1, false, sibcon_2));
			m_Siblings.push_back(std::make_tuple(true, false, sibcon_2, false, sibcon_1));
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
			m_Siblings.push_back(std::make_tuple(true, true, sibcon_1, false, sibcon_2));
			m_Siblings.push_back(std::make_tuple(true, false, sibcon_2, true, sibcon_1));
		}
		bmmffmf++;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Generate children population
	/////////////////////////////////////////////////////////////////////////////////////////////////
	long totalmalepop_chld = static_cast<long>(m_lChildrnPopulation*dmalepercentage);
	for (long cp = 0; cp < m_lChildrnPopulation; ++cp)
	{
		int randSchool = generateRandomNumber_(0, (long)CINIConfigParser::GetNumOfSchools()-1);
		//int randSchool = 0 + rand() % ((CINIConfigParser::GetNumOfSchools()) - 0);

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
		CPersonInfo pMInfo(fullname.first, fullname.second, stdob, ginfo, randAdd);
		pMInfo.setSSN(pInoGen->GenerateRandomSSN());
		pMInfo.setGraduation(grad);
		pMInfo.setMaritalStatus(MARITAL_STATUS_NEVER_MARRIED);
		pMInfo.setHasChildren(false);
		pMInfo.setSchoolName(CINIConfigParser::GetSchoolNameNAddress(randSchool));
		pMInfo.setListIndex(m_lChildPopln.size());
		pMInfo.m_nGeneration = 1;
		pMInfo.m_nListIndex = static_cast<long>(m_lChildPopln.size());
		pMInfo.m_nListFatherIndex = -1;
		pMInfo.m_nListMotherIndex = -1;
		m_lChildPopln.push_back(pMInfo);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Generate working age population
	/////////////////////////////////////////////////////////////////////////////////////////////////
	long totalmalepop_wrkng = static_cast<long>(m_lWorkingPopulation*dmalepercentage);
	long totalfemalepop_wrkng = static_cast<long>(m_lWorkingPopulation*dfemalepercentage);
	size_t stElderMaleLastIndex = m_lmalePopln.size();
	size_t stElderFeMaleLastIndex = m_lfemalePopln.size();
	long otherpop = 0;
	const long totalotherpop = static_cast<long>(dotherpercentage * m_lTotalPopulationCnt);
	for (long p = 0; p < m_lWorkingPopulation; ++p)
	{
		int randCity = generateRandomNumber_(0, (long)(m_populationDB.size() - 1));
		//int randCity = 0 + rand() % ((m_populationDB.size()) - 0);
		std::string randAdd = "";
		if (randCity < m_populationDB.size())
		{
			//randAdd = pInoGen->GenerateRandomAddress(m_populationDB[randCity].m_nArea, m_populationDB[randCity].m_longitude, m_populationDB[randCity].m_latitude);
			randAdd = m_populationDB[randCity].m_strLocalityName + "-" + m_populationDB[randCity].m_strSubPrefec + "-" + m_populationDB[randCity].m_strPrefec;
		}
		GenderInfo_ ginfo = GENDER_TYPE_MALE;
		if (p >= totalmalepop_wrkng)
		{
			ginfo = GENDER_TYPE_FEMALE;
			if (otherpop < totalotherpop)
			{
				otherpop++;
				//ginfo = GENDER_TYPE_OTHER;
			}
		}

		int randFSts = generateRandomNumber_(0, (long)(FINANCE_STATUS_ULTRA_RICH));
		//int randFSts = FINANCE_STATUS_POOR + rand() % ((FINANCE_STATUS_ULTRA_RICH + 1) - FINANCE_STATUS_POOR);
		int randEmplmt = rand() % 2; //get 0 or 1
		std::pair<std::string, std::string> malename = pInoGen->GenerateRandomName(ginfo);
		_dob stdob = pInoGen->GenerateRandomDOB(WORKING_DOB_ST, WORKING_DOB_EN);
		Graduation_ grad = pInoGen->GenerateRandomEducation();
		MaritalSatus_ msts = MARITAL_STATUS_NEVER_MARRIED;
		msts = pInoGen->GenerateRandomMSts();
		
		CPersonInfo pMInfo(malename.first, malename.second, stdob, ginfo, randAdd);
		pMInfo.setSSN(pInoGen->GenerateRandomSSN());
		pMInfo.setGraduation(grad);
		pMInfo.setMaritalStatus(msts);
		pMInfo.setHasChildren(false);
		
		if (1 == randEmplmt)
		{
			pMInfo.setIsEmployed(true);			
			int randOfficeAdd = generateRandomNumber_(0, (long)(CINIConfigParser::GetNumOfOffices()-1));
			//int randOfficeAdd =  rand() % ((CINIConfigParser::GetNumOfOffices()));
			pMInfo.setOfficeName(CINIConfigParser::GetOfficeAddress(randOfficeAdd));
		}

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


		int randParentMale = -1;
		int randParentFeMale = -1;
		if (getRandomMaleParent(stElderMaleLastIndex, randParentMale))
		{
			pMInfo.setFatherName(m_lmalePopln[randParentMale].getFullName());		
			pMInfo.setLastName(m_lmalePopln[randParentMale].getLastName());
		}
		if (getRandomFeMaleParent(stElderFeMaleLastIndex, randParentFeMale))
		{
			pMInfo.setMotherName(m_lfemalePopln[randParentFeMale].getFullName());	
		}

		pMInfo.m_nListFatherIndex = static_cast<long>(randParentMale);
		pMInfo.m_nListMotherIndex = static_cast<long>(randParentFeMale);
		bool pbgen = true;
		bool pbmature = true;
		if (pMInfo.getAge() < 18)
		{
			pbmature = false;
		}

		if ((randParentMale != -1) && (randParentFeMale != -1))
		{
			m_lmalePopln[randParentMale].setMaritalStatus(MARITAL_STATUS_MARRIED);
			m_lmalePopln[randParentMale].addSpouse(randParentFeMale);
			size_t tempt = m_lmalePopln.size();
			if (ginfo != GENDER_TYPE_MALE)
			{
				pbgen = false;
				tempt = m_lfemalePopln.size();
			}
			m_lmalePopln[randParentMale].addChild(pMInfo, tempt);
			m_lfemalePopln[randParentFeMale].setMaritalStatus(MARITAL_STATUS_MARRIED);
			m_lfemalePopln[randParentFeMale].addSpouse(randParentMale);
			m_lfemalePopln[randParentFeMale].addChild(pMInfo, tempt);
			m_edges.push_back(std::make_tuple(true, randParentMale, false, randParentFeMale, EDGE_TYPE_PARTNER));
			m_edges.push_back(std::make_tuple(false, randParentFeMale, true, randParentMale, EDGE_TYPE_PARTNER));			
			pMInfo.m_nListIndex = tempt;
			m_parentChild.push_back(std::make_tuple(true, randParentMale, pbmature, pbgen, tempt));
			m_parentChild.push_back(std::make_tuple(false, randParentFeMale, pbmature, pbgen, tempt));
		}
		else
		{
			pMInfo.m_nListFatherIndex = -1;
			pMInfo.m_nListMotherIndex = -1;
		}

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
	}//for


	/////////////////////////////////////////////////////////////////////////////////////////////////
	//Generate young adults population
	/////////////////////////////////////////////////////////////////////////////////////////////////
	long totalmalepop_yng = static_cast<long>(m_lYoungPopulation*dmalepercentage);
	long totalfemalepop_yng = static_cast<long>(m_lYoungPopulation*dfemalepercentage);
	for (long yp = 0; yp < m_lYoungPopulation; ++yp)
	{
		int randSchool = generateRandomNumber_(0, (long)(CINIConfigParser::GetNumOfSchools() - 1));
		//int randSchool = 0 + rand() % ((CINIConfigParser::GetNumOfSchools()) - 0);
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
				//ginfo = GENDER_TYPE_OTHER;
			}
		}
		int randFSts = generateRandomNumber_(0, (long)(FINANCE_STATUS_ULTRA_RICH));
		//int randFSts = FINANCE_STATUS_POOR + rand() % ((FINANCE_STATUS_ULTRA_RICH + 1) - FINANCE_STATUS_POOR);
		std::pair<std::string, std::string> fullname = pInoGen->GenerateRandomName(ginfo);
		_dob stdob = pInoGen->GenerateRandomDOB(CHILD_DOB_ST - 11, CHILD_DOB_ST);
		Graduation_ grad = GRAD_TYPE_NONE;
#if PLATFORM_TYPE_LINUX
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
		pMInfo.setSchoolName(CINIConfigParser::GetSchoolNameNAddress(randSchool));
		pMInfo.m_nListFatherIndex = -1;
		pMInfo.m_nListMotherIndex = -1;
		if (18 > pMInfo.getAge())
		{
			int randSchool = generateRandomNumber_(0, (long)(CINIConfigParser::GetNumOfUniversities()-1));
			//randSchool = rand() % (CINIConfigParser::GetNumOfUniversities());
			pMInfo.setSchoolName(CINIConfigParser::GetUnivstyNameNAddress(randSchool));	
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

	//Make random connections and create marriage event
	makeRandomEventMarriage(m_lTotalMarriages);

	//assign children to random parents
	//std::vector<size_t> unassignedlist;
	for (size_t child = 0; child < m_lChildPopln.size();++child) //(auto child : m_lChildPopln)
	{
		/*if (false == assignChildtoRandomFamily(m_lChildPopln[child]))
		{
			unassignedlist.push_back(m_lChildPopln[child].m_nListIndex);
		}*/
		assignChildtoRandomFamily(m_lChildPopln[child]);
	}

	//Check father and mother names 
	/*for (size_t f = 0; f < m_lfemalePopln.size(); ++f)
	{
		if (-1 != m_lfemalePopln[f].m_nListFatherIndex)
		{
			if ("" == m_lfemalePopln[f].getFatherName())
			{
				m_lfemalePopln[f].setFatherName(m_lmalePopln[m_lfemalePopln[f].m_nListFatherIndex].getFullName());
			}
		}

		if (-1 != m_lfemalePopln[f].m_nListMotherIndex)
		{
			if ("" == m_lfemalePopln[f].getMotherName())
			{
				m_lfemalePopln[f].setMotherName(m_lfemalePopln[m_lfemalePopln[f].m_nListMotherIndex].getFullName());
			}
		}
	}

	for (size_t m = 0; m < m_lmalePopln.size(); ++m)
	{
		if (-1 != m_lmalePopln[m].m_nListFatherIndex)
		{
			if ("" == m_lmalePopln[m].getFatherName())
			{
				m_lmalePopln[m].setFatherName(m_lmalePopln[m_lmalePopln[m].m_nListFatherIndex].getFullName());
			}
		}

		if (-1 != m_lmalePopln[m].m_nListMotherIndex)
		{
			if ("" == m_lmalePopln[m].getMotherName())
			{
				m_lmalePopln[m].setMotherName(m_lfemalePopln[m_lmalePopln[m].m_nListMotherIndex].getFullName());
			}
		}
	}*/

	
	std::cout << "GenerateRandomPopulationData() -> Generated 1st generation random data :" << GetPopCount() << std::endl;
	CTextFileLogger::GetLogger()->Log("GenerateRandomPopulationData() -> Generated 1st generation random data : %d", GetPopCount());
}


void CPopulationDB::Evolve()
{
	//	std::cout << "Evolve population"<<std::endl;
	CTextFileLogger::GetLogger()->Log("Evolve() called...");
	struct timespec ts;
	for (int i = 0; i < 1; ++i)
	{
		CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d]", i+1);
		
		/////////////////////////////////////////////////////////////////////////////////////////////////
		//Predict life events for female population and take necessary action
		/////////////////////////////////////////////////////////////////////////////////////////////////
		for (size_t persn = 0; persn < m_lfemalePopln.size(); ++persn) //(auto persn : m_lfemalePopln)
		{
			//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d] name = %s",i,persn.getFullName().c_str());
			int age = m_lfemalePopln[persn].getAge();
			m_lfemalePopln[persn].setAge(age + 1);
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
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: New event GRADUATION", i);
				}
				else if (ev == EVENT_TYPE_NEW_CHILD)
				{
					if (m_lfemalePopln[persn].getChildCount() > MAX_CHILD_PER_PERSON)
					{
						continue;
					}
					int randgen = generateRandomNumber_(0, (long)(EVENT_INFO_END));
					//int randgen = 0 + rand() % (EVENT_INFO_END);		
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
					m_parentChild.push_back(std::make_tuple(false, m_lfemalePopln[persn].m_nListIndex,false, bchildGen, lindex));
					if (-1 != spusIndx)
					{
						m_lmalePopln[m_lfemalePopln[persn].getSpouseIndex()].AddEvent(pev, pChild);
						m_parentChild.push_back(std::make_tuple(true, m_lfemalePopln[persn].getSpouseIndex(), false, bchildGen, lindex));
					}
					bool childgen = (pChild.getGender() == GENDER_TYPE_MALE) ? true : false;
					for (auto i = m_lfemalePopln[persn].children_begin(); i < m_lfemalePopln[persn].children_end() - 1; i++)
					{
						bool siblinggen = std::get<1>(*i);
						long sibindex = std::get<2>(*i);
						m_Siblings.push_back(std::make_tuple(false,childgen, pChild.m_nListIndex, siblinggen, std::get<2>(*i)));
						m_Siblings.push_back(std::make_tuple(false, siblinggen, sibindex, childgen, pChild.m_nListIndex));
					}
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event NEW_CHILD", i, persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_MARRIAGE)
				{
					makeRandomEventMarriage(persn);
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event MARRIAGE", i, persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_HOME_PURCHASE)
				{
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event HOME_PURCHASE", i, persn.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_HOME_PURCHASE;
					pev.m_strEventComments = "New home address=***";
					m_lfemalePopln[persn].AddEvent(pev, m_lfemalePopln[persn]);
				}
				else if (ev == EVENT_TYPE_NEW_JOB)
				{
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event NEW_JOB", i, persn.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_NEW_JOB;
					
					int randFSts = generateRandomNumber_(0, (long)(CINIConfigParser::GetNumOfOffices()-1));
					//int randFSts = 0 + rand() % ((CINIConfigParser::GetNumOfOffices()) - 0);
					pev.m_strEventComments = CINIConfigParser::GetOfficeAddress(randFSts).second;
					m_lfemalePopln[persn].setOfficeName(CINIConfigParser::GetOfficeAddress(randFSts));
					m_lfemalePopln[persn].AddEvent(pev, m_lfemalePopln[persn]);
				}
			}
		}//for female population

		 /////////////////////////////////////////////////////////////////////////////////////////////////
		 //Predict life events for male population and take necessary action
		 /////////////////////////////////////////////////////////////////////////////////////////////////
		for (size_t persn = 0; persn < m_lmalePopln.size(); ++persn) //(auto persn : m_lmalePopln)
		{
			AgeObsSymbol ageObs = CHILD;
			int age = m_lmalePopln[persn].getAge();
			m_lmalePopln[persn].setAge(age + 1);
			std::vector<Observation> observations;
			Observation ob;
			ob.age = age;
			ob.financial_status = m_lmalePopln[persn].getFinancialSts_();
			observations.push_back(ob);
			std::vector<std::string>  events = m_cslifeChoice->predict(observations);
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
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: New event GRADUATION", i);
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
					//int randgen = 0 + rand() % (EVENT_INFO_END);
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
					m_parentChild.push_back(std::make_tuple(true, m_lmalePopln[persn].m_nListIndex, false, bchildGen, lindex));
					if (-1 != m_lmalePopln[persn].getSpouseIndex())
					{
						m_lfemalePopln[m_lmalePopln[persn].getSpouseIndex()].AddEvent(pev, pChild);
						m_parentChild.push_back(std::make_tuple(false, m_lmalePopln[persn].getSpouseIndex(), false, bchildGen, lindex));
					}

					bool childgen = (pChild.getGender() == GENDER_TYPE_MALE) ? true : false;
					for (auto i = m_lmalePopln[persn].children_begin(); i < m_lmalePopln[persn].children_end() -1 ; i++)
					{
						bool siblinggen = std::get<1>(*i); // (i->getGender() == GENDER_TYPE_MALE) ? true : false;
						long sibindex = std::get<2>(*i);
						m_Siblings.push_back(std::make_tuple(false,childgen, pChild.m_nListIndex, siblinggen, std::get<2>(*i)));
						m_Siblings.push_back(std::make_tuple(false, siblinggen, sibindex, childgen, pChild.m_nListIndex));
					}
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event NEW_CHILD", i, persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_MARRIAGE)
				{
					makeRandomEventMarriage(persn);
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event MARRIAGE", i, persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_HOME_PURCHASE)
				{
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event HOME_PURCHASE", i, persn.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_HOME_PURCHASE;
					pev.m_strEventComments = "New home address=***";
					m_lmalePopln[persn].AddEvent(pev, m_lmalePopln[persn]);
				}
				else if (ev == EVENT_TYPE_NEW_JOB)
				{
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event NEW_JOB", i, persn.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_NEW_JOB;
					int randFSts = generateRandomNumber_(0, (long)(CINIConfigParser::GetNumOfOffices()-1));
					//int randFSts = 0 + rand() % ((CINIConfigParser::GetNumOfOffices()) - 0);
					pev.m_strEventComments = CINIConfigParser::GetOfficeAddress(randFSts).second;
					m_lmalePopln[persn].AddEvent(pev, m_lmalePopln[persn]);
				}
			}
		}////for female population

		 /////////////////////////////////////////////////////////////////////////////////////////////////
		 //Predict life events for child population and take necessary action
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
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][child population]: New event GRADUATION", i);
				}
				else if (ev == EVENT_TYPE_NEW_JOB)
				{
					//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][child population]: name = %s New event NEW_JOB", i, child.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_NEW_JOB;
					int randFSts = generateRandomNumber_(0, (long)(CINIConfigParser::GetNumOfOffices() - 1));
					//int randFSts = 0 + rand() % ((CINIConfigParser::GetNumOfOffices()) - 0);
					pev.m_strEventComments = CINIConfigParser::GetOfficeAddress(randFSts).second;
					m_lChildPopln[child].AddEvent(pev, m_lChildPopln[child]);
				}
			}
		}//for child
	}
}

bool CPopulationDB::getRandomParent(int& retMale, int& retFemale, bool bskipCond)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(m_lfemalePopln.size());
	//int randNoParentM = -1;
	int randNoParentF = -1;
	do
	{
		randNoParentF = generateRandomNumber_(nMin, (long)(nMax - 1));
		//randNoM = nMin + rand() % ((nMax)-nMin);
		if (-1 == randNoParentF)
		{
			break;
		}

		bool bFound = true;
		if (-1 == m_lfemalePopln[randNoParentF].getSpouseIndex())
		{
			//If no partner then same person data is returned. Check if its same
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
		}//if not found

	} while (bRet == false);

	if (bRet)
	{
		retFemale = randNoParentF;
		retMale = m_lfemalePopln[randNoParentF].getSpouseIndex();
		if (-1 == retMale)
		{
			//bRet = false;
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
				if ((m_lfemalePopln[f].getChildCount() == 0)
					&& (m_lfemalePopln[f].getAge() > 18))
				{
					retFemale = f;
					retMale = m_lfemalePopln[f].getSpouseIndex();
					bRet = true;
					break;
				}
			}
		}
	}
	return bRet;
}

bool CPopulationDB::getRandomMaleParent(size_t maxsize, int & retMale)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(maxsize);
	int randNoM = generateRandomNumber_(nMin, (long)(nMax - 1));
	//int randNoM = nMin + rand() % ((nMax)-nMin);
	do
	{
		bool bFound = true;
		if (-1 == m_lmalePopln[randNoM].getSpouseIndex())
		{
			//If no partner then same person data is returned. Check if its same
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
				if ((m_lmalePopln[m].getChildCount() < MAX_CHILD_PER_PERSON)
					&& (m_lmalePopln[m].getAge() > 18))
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
					if ((m_lmalePopln[m].getChildCount() < MAX_CHILD_PER_PERSON)
						&& (m_lmalePopln[m].getAge() > 18))
					{
						randNoM = m;
						bRet = true;
						break;
					}
				}
			}
		}//if not found

	} while (false);
	if (bRet)
	{
		retMale = randNoM;
	}
	return bRet;
}

bool CPopulationDB::getRandomFeMaleParent(size_t maxsize, int & retFeMale)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(maxsize);
	int randNoF = generateRandomNumber_(nMin, (long)(nMax - 1));
	//int randNoF = nMin + rand() % ((nMax)-nMin);
	do
	{
		bool bFound = true;
		if (-1 == m_lfemalePopln[randNoF].getSpouseIndex())
		{
			//If no partner then same person data is returned. Check if its same
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
				if ((m_lfemalePopln[f].getChildCount() < MAX_CHILD_PER_PERSON)
					&& (m_lfemalePopln[f].getAge() > 18))
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
					if ((m_lfemalePopln[f].getChildCount() < MAX_CHILD_PER_PERSON)
						&& (m_lfemalePopln[f].getAge() > 18))
					{
						randNoF = f;
						bRet = true;
						break;
					}
				}
			}
		}//if not found

	} while (false);
	if (bRet)
	{
		retFeMale = randNoF;
	}
	return bRet;
}

bool CPopulationDB::getRandomMaleForMarriage(int& retIndex)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(m_lmalePopln.size());
	int randNoM = generateRandomNumber_(nMin, (long)(nMax - 1));
	//int randNoM = nMin + rand() % ((nMax) - nMin);
	std::random_device rd1; // obtain a random number from hardware
	std::mt19937 gen1(rd1()); // seed the generator
	std::uniform_int_distribution<> distr1(0, m_lmalePopln.size()-1);
	randNoM = distr1(gen1);
	do {	
		if ((m_lmalePopln[randNoM].getMaritalStatus() == MARITAL_STATUS_MARRIED) 
			|| (m_lmalePopln[randNoM].getMaritalStatus() == MARITAL_STATUS_SEPERATED)
			|| (m_lmalePopln[randNoM].getNumberofMarriages() > MAX_MARRIAGES_PER_PERSON))
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

bool CPopulationDB::getRandomFemaleForMarriage(int& retIndex)
{
	bool bRet = false;
	const int nMin = 0;
	const int nMax = static_cast<int>(m_lfemalePopln.size());

	int randNoF = generateRandomNumber_(nMin, (long)(nMax - 1));
	//int randNoF = nMin + rand() % ((nMax) - nMin);
	std::random_device rd1; // obtain a random number from hardware
	std::mt19937 gen1(rd1()); // seed the generator
	std::uniform_int_distribution<> distr1(0, m_lfemalePopln.size()-1);
	randNoF = distr1(gen1);
	do
	{
		if ((m_lfemalePopln[randNoF].getMaritalStatus() == MARITAL_STATUS_MARRIED) 
			|| (m_lfemalePopln[randNoF].getMaritalStatus() == MARITAL_STATUS_SEPERATED)
			|| (m_lfemalePopln[randNoF].getGender() == GENDER_TYPE_OTHER)
			|| (m_lfemalePopln[randNoF].getNumberofMarriages() > MAX_MARRIAGES_PER_PERSON))
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
		}//female
		else
		{
			bRet = true;
		}
	} while (false);
	retIndex = randNoF;
	return bRet;
}

void CPopulationDB::makeRandomEventMarriage(CPersonInfo & partner)
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
			
			m_edges.push_back(std::make_tuple(true, partner.m_nListIndex,false, m_lfemalePopln[index].m_nListIndex, EDGE_TYPE_PARTNER));
			m_edges.push_back(std::make_tuple(false, m_lfemalePopln[index].m_nListIndex, true, partner.m_nListIndex, EDGE_TYPE_PARTNER));
			CTextFileLogger::GetLogger()->Log("Marriage -> %s & %s", malePrtnr.c_str(), femalePrtnr.c_str());
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
			
			m_edges.push_back(std::make_tuple(false, partner.m_nListIndex, true, m_lmalePopln[randNoM].m_nListIndex, EDGE_TYPE_PARTNER));
			m_edges.push_back(std::make_tuple(true, m_lmalePopln[randNoM].m_nListIndex, false, partner.m_nListIndex, EDGE_TYPE_PARTNER));
			CTextFileLogger::GetLogger()->Log("Marriage -> %s & %s", malePrtnr.c_str(), femalePrtnr.c_str());
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
			//CTextFileLogger::GetLogger()->Log("makeRandomEventMarriage -> bfemalefound = %d,bmalefound = %d, randNoM = %d, randNoF = %d, MaleSize=%d, FemaleSize=%d", logf, logm, randNoM, randNoF, m_lmalePopln.size(), m_lfemalePopln.size());
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
		
		m_edges.push_back(std::make_tuple(false, m_lfemalePopln[randNoF].m_nListIndex, true, m_lmalePopln[randNoM].m_nListIndex, EDGE_TYPE_PARTNER));
		m_edges.push_back(std::make_tuple(true, m_lmalePopln[randNoM].m_nListIndex, false, m_lfemalePopln[randNoF].m_nListIndex, EDGE_TYPE_PARTNER));
		//CTextFileLogger::GetLogger()->Log("Marriage -> %s & %s", malePrtnr.c_str(), femalePrtnr.c_str());
	}//for count
	CTextFileLogger::GetLogger()->Log("makeRandomEventMarriage -> Total created = %d, Needed = %d", totalmarriages, m_lTotalMarriages);
}

void CPopulationDB::makeRandomEventDeaths()
{
	long totaldeathsadult = static_cast<long>((m_lmalePopln.size() + m_lfemalePopln.size()) * CINIConfigParser::GetDblValue(SECTION_POP, KEY_DEATH_RATE));
	long totaldeathsinfants = 0;
	
	std::cout << "makeRandomEventDeaths()  -> average adults death per year" <<totaldeathsadult<< std::endl;
	//std::cout << "makeRandomEventDeaths()  -> average infant death per year" <<totaldeathsadult<< std::endl;

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
	struct timespec ts;

	for (int i = 0; i < totaldeathsadult/2; ++i)
	{
		int randNoM = generateRandomNumber_(0, (long)(nMax_M - 1));
		//int randNoM = nMinM + rand() % ((nMax_M + 1) - nMinM);

		bool bMaleFound = false;
		bool bFemaleFound = false;
		
		if ((randNoM < 0) || (randNoM >= m_lmalePopln.size()))
		{
			randNoM = static_cast<int>(m_lmalePopln.size()-1);
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
		//int randNoF = nMinF + rand() % ((nMax_F + 1) - nMinF);
		
		if ((randNoF < 0) || (randNoF >= m_lfemalePopln.size()))
		{
			randNoF = static_cast<int>(m_lfemalePopln.size()-1);
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
		

		//make death events
		PersonEvent ev;
		ev.m_nEventType = EVENT_TYPE_DEATH;
		_dob tempdd = pInoGen->GenerateRandomDOB(2022, 2022, 2022);
		std::cout << "makeRandomEventDeaths()  -> " <<i<<bFemaleFound<<bMaleFound<< std::endl;
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

bool CPopulationDB::assignChildtoRandomFamily(CPersonInfo & child)
{
	int ParentIndex_M = -1;
	int ParentIndex_F = -1;
	bool bRetry = false;
	bool bReset = false;
	int retrycount = 0;
	const int retrycountmax = 3;
	bool bRet = false;
	do {
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
				m_parentChild.push_back(std::make_tuple(true, ParentIndex_M, bMaturechild, childgen, child.m_nListIndex));
			}
			m_lfemalePopln[ParentIndex_F].AddEvent(ev, child);		
			m_lfemalePopln[ParentIndex_F].setHasChildren(true);
			m_parentChild.push_back(std::make_tuple(false, ParentIndex_F, bMaturechild, childgen, child.m_nListIndex));

			for (auto i = m_lfemalePopln[ParentIndex_F].children_begin(); i < m_lfemalePopln[ParentIndex_F].children_end() - 1; i++)
			{
				bool siblinggen = std::get<1>(*i); //(i->getGender() == GENDER_TYPE_MALE) ? true : false;
				long sibindex = std::get<2>(*i);
				m_Siblings.push_back(std::make_tuple(false, childgen, child.m_nListIndex, siblinggen, sibindex));
				m_Siblings.push_back(std::make_tuple(false, siblinggen, sibindex, childgen, child.m_nListIndex));
			}
			bRet = true;
			break;
			//CTextFileLogger::GetLogger()->Log("assignChildtoRandomFamily -> Child[%s]. Parents : %s, %s", childname.c_str(), father.c_str(), mother.c_str());
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
			//CTextFileLogger::GetLogger()->Log("assignChildtoRandomFamily -> Could not find parents");
		}
	} while (true == bRetry);
	return bRet;
}

void CPopulationDB::makeRandomEventChild(int count, CPersonInfo & parentM, CPersonInfo & parentF)
{
	for (int chld = 0; chld < count; chld++)
	{
		int cindx = static_cast<int>(m_lChildPopln.size() - 1);
		bool bChildFound = false;
		int nMinC = 0;
		int nMaxC = static_cast<int>(m_lChildPopln.size());
		
		int randNoM = generateRandomNumber_(0, (long)(nMaxC - 1));
		//int randNoM = nMinC + rand() % ((nMaxC + 1) - nMinC);

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

		if (bChildFound)//if found
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

			//m_parentChild.push_back(std::make_tuple(parentM, m_lChildPopln[cindx]));
			//m_parentChild.push_back(std::make_tuple(parentF, m_lChildPopln[cindx]));

			bool childgen = (m_lChildPopln[cindx].getGender() == GENDER_TYPE_MALE) ? true : false;
			bool bMaturechild =  false;
			m_parentChild.push_back(std::make_tuple(true, parentM.m_nListIndex, bMaturechild, childgen, cindx));
			m_parentChild.push_back(std::make_tuple(false,parentF.m_nListIndex, bMaturechild, childgen, cindx));
		}

	}//for
	//std::cout << "makeRandomEventChild()  -> " <<count<< std::endl;
}

/*
generate edges info in below format.
conn_from: Row index in population_database.csv file
conn_to : Row index in population_database.csv file
edge_type : Any one of these values['grand child', 'aunt', 'uncle', 'grand father', 'grand mother', 'sister', 'brother', 'nephew', 'niece', 'husband', 'wife', 'father', 'mother']
*/
void CPopulationDB::generate_family_tree()
{
    //std::cout << "generate_family_tree()  -> " << std::endl;
	CTextFileLogger::GetLogger()->Log("generate_family_tree()  -> start");
	const long totalmales = static_cast<long>(m_lmalePopln.size());
	const long totalfemales = static_cast<long>(m_lfemalePopln.size());
	std::ofstream outFile("family_edges.csv");
	outFile << "conn_from,conn_to,edge_type\n";

	std::set<std::string> relation_edges;

	for (auto parent : m_parentChild)
	{
		std::string strEdge = "NA";
		size_t sztchildindex = std::get<4>(parent);
		bool bMatureChild = std::get<2>(parent);
		bool bMaleChild = std::get<3>(parent);

		if (false == bMatureChild)
		{
			if (sztchildindex > m_lChildPopln.size())
			{
				continue;
			}

			if (true == std::get<0>(parent))
			{
				long childindxlong = static_cast<long>(m_lChildPopln[sztchildindex].m_nListIndex) + totalmales + totalfemales;
				std::string childindex = to_string_(childindxlong);
				strEdge = to_string_((long)m_lmalePopln[std::get<1>(parent)].m_nListIndex) + ", " + childindex + ", father\n";
			}
			else
			{
				std::string childindex = to_string_(static_cast<long>(m_lChildPopln[sztchildindex].m_nListIndex) + totalmales + totalfemales);
				strEdge = to_string_((long)m_lfemalePopln[std::get<1>(parent)].m_nListIndex) + ", " + childindex + ", mother\n";
			}
			//outFile << strEdge;
			relation_edges.insert(strEdge);
		}
		else
		{
			std::string childindex = "NA";
			if (true == bMaleChild)
			{
				childindex = to_string_(static_cast<long>(m_lmalePopln[sztchildindex].m_nListIndex) + totalfemales);
			}
			else
			{
				childindex = to_string_(static_cast<long>(m_lfemalePopln[sztchildindex].m_nListIndex));
			}

			if (true == std::get<0>(parent))
			{
				strEdge = to_string_((long)m_lmalePopln[std::get<1>(parent)].m_nListIndex + totalfemales) + ", " + childindex + ", father\n";
			}
			else
			{
				strEdge = to_string_((long)m_lfemalePopln[std::get<1>(parent)].m_nListIndex) + ", " + childindex + ", mother\n";
			}
			//outFile << strEdge;
			relation_edges.insert(strEdge);
		}
	}

	for (auto siblings : m_Siblings)
	{
		bool bmature = std::get<0>(siblings);
		bool gender1 = std::get<1>(siblings);
		size_t inds1 = std::get<2>(siblings);
		bool gender2 = std::get<3>(siblings);
		size_t inds2 = std::get<4>(siblings);
		std::string strEdge = "NA";

		if (inds1 == inds2)
		{
			continue;
		}


		if (bmature)
		{
			long s1long = -1;
			long s2long = -1;
			if (true == gender1)
			{
				s1long = static_cast<long>(inds1) + totalfemales;
				s2long = static_cast<long>(inds2);
				if (true == gender2)
				{
					s2long = static_cast<long>(inds2) + totalfemales;
				}
				strEdge = to_string_(s1long) + ", " + to_string_(s2long) + ", brother\n";
			}
			else
			{
				s1long = static_cast<long>(inds1) + totalfemales;
				s2long = static_cast<long>(inds2);
				if (true == gender2)
				{
					s2long = static_cast<long>(inds2) + totalfemales;
				}
				strEdge = to_string_(s1long) + ", " + to_string_(s2long) + ", sister\n";
			}
		}
		else
		{
			long s1long = static_cast<long>(inds1) + totalfemales + totalmales;
			long s2long = static_cast<long>(inds2) + totalfemales + totalmales;
			if (true == gender1)
			{
				strEdge = to_string_(s1long) + ", " + to_string_(s2long) + ", brother\n";
			}
			else
			{
				strEdge = to_string_(s1long) + ", " + to_string_(s2long) + ", sister\n";
			}
		}
		//outFile << strEdge;
		relation_edges.insert(strEdge);

		if (true == bmature)
		{
			if (true == gender1)
			{
				if (m_lmalePopln[inds1].getChildCount() > 0)
				{
					for (auto i = m_lmalePopln[inds1].children_begin(); i < m_lmalePopln[inds1].children_end(); i++)
					{
						std::string untuncle = "NA";
						std::string nephewneice = "NA";
						long sibindex = -1;
						long uncauntindex = -1;
						if (true == gender2)
						{
							untuncle = ", uncle\n";
							uncauntindex = static_cast<long>(inds2) + totalfemales;
						}
						else
						{
							untuncle = ", aunt\n";
							uncauntindex = static_cast<long>(inds2);
						}

						if (true == std::get<1>(*i))
						{
							if (true == std::get<0>(*i))
							{
								sibindex = std::get<2>(*i) + totalfemales;
							}
							else
							{
								sibindex = std::get<2>(*i) + totalfemales + totalmales;
							}
							nephewneice = ", nephew\n";
						}
						else
						{
							sibindex = std::get<2>(*i);
							if (false == std::get<0>(*i))
							{
								sibindex = std::get<2>(*i) + totalfemales + totalmales;
							}
							nephewneice = ", niece\n";
						}
						strEdge = to_string_(uncauntindex) + ", " + to_string_(sibindex) + untuncle;
						//outFile << strEdge;
						relation_edges.insert(strEdge);
						strEdge = to_string_(sibindex) + ", " + to_string_(uncauntindex) + nephewneice;
						//outFile << strEdge;
						relation_edges.insert(strEdge);
					}//for all childrens
				}
			}
			else
			{
				if (m_lfemalePopln[inds1].getChildCount() > 0)
				{
					for (auto i = m_lfemalePopln[inds1].children_begin(); i < m_lfemalePopln[inds1].children_end(); i++)
					{
						std::string untuncle = "NA";
						std::string nephewneice = "NA";
						long sibindex = -1;
						long uncauntindex = -1;
						if (true == gender2)
						{
							untuncle = ", uncle\n";
							uncauntindex = static_cast<long>(inds2) + totalfemales;
						}
						else
						{
							untuncle = ", aunt\n";
							uncauntindex = static_cast<long>(inds2);
						}

						if (true == std::get<1>(*i))
						{
							if (true == std::get<0>(*i))
							{
								sibindex = std::get<2>(*i) + totalfemales;
							}
							else
							{
								sibindex = std::get<2>(*i) + totalfemales + totalmales;
							}
							nephewneice = ", nephew\n";
						}
						else
						{
							sibindex = std::get<2>(*i);
							if (false == std::get<0>(*i))
							{
								sibindex = std::get<2>(*i) + totalfemales + totalmales;
							}
							nephewneice = ", niece\n";
						}


						strEdge = to_string_(uncauntindex) + ", " + to_string_(sibindex) + untuncle;
						//outFile << strEdge;
						relation_edges.insert(strEdge);
						strEdge = to_string_(sibindex) + ", " + to_string_(uncauntindex) + nephewneice;
						//outFile << strEdge;
						relation_edges.insert(strEdge);
					}
				}
			}
		}
	}

	for (auto partner : m_edges)
	{
		bool gender1 = std::get<0>(partner);
		size_t inds1 = std::get<1>(partner);
		bool gender2 = std::get<2>(partner);
		size_t inds2 = std::get<3>(partner);
		std::string strEdge = "NA";
		if (inds1 == inds2)
		{
			continue;
		}

		if (true == gender1)
		{
			strEdge = to_string_(static_cast<long>(inds1) + totalfemales) + ", " + to_string_(static_cast<long>(inds2)) + ", husband\n";
		}
		else
		{
			strEdge = to_string_(static_cast<long>(inds1)) + ", " + to_string_(static_cast<long>(inds2) + totalfemales) + ", wife\n";
		}
		//outFile << strEdge;
		relation_edges.insert(strEdge);
		
		if (true == gender1)
		{
			if (m_lmalePopln[inds1].getChildCount() > 0)
			{
				for (auto i = m_lmalePopln[inds1].children_begin(); i < m_lmalePopln[inds1].children_end(); i++)
				{
					long cldindex = std::get<2>(*i);
					std::string childindex = to_string_(static_cast<long>(cldindex + totalmales + totalfemales));
					
					if (m_lmalePopln[inds1].m_nListFatherIndex != -1)
					{
						long grandfatherindex = static_cast<long>(m_lmalePopln[inds1].m_nListFatherIndex) + totalfemales;
						strEdge = to_string_(grandfatherindex) + ", " + childindex + ", grand father\n";
						//outFile << strEdge;
						relation_edges.insert(strEdge);
						strEdge = childindex + ", " + to_string_(static_cast<long>(m_lmalePopln[inds1].m_nListFatherIndex)) + ", grand child\n";
						//outFile << strEdge;
						relation_edges.insert(strEdge);
					}
					
					if (m_lmalePopln[inds1].m_nListMotherIndex != -1)
					{
						strEdge = to_string_(static_cast<long>(m_lmalePopln[inds1].m_nListMotherIndex)) + ", " + childindex + ", grand mother\n";
						//outFile << strEdge;
						relation_edges.insert(strEdge);
						strEdge = childindex + ", " + to_string_(static_cast<long>(m_lmalePopln[inds1].m_nListMotherIndex)) + ", grand child\n";
						//outFile << strEdge;
						relation_edges.insert(strEdge);
					}
				}
			}//link grandparent

		}
		else
		{
			if (m_lfemalePopln[inds1].getChildCount() > 0)
			{
				for (auto i = m_lfemalePopln[inds1].children_begin(); i < m_lfemalePopln[inds1].children_end(); i++)
				{
					long cldindex = std::get<2>(*i);
					std::string childindex = to_string_(static_cast<long>(cldindex + totalmales + totalfemales));
					if (m_lfemalePopln[inds1].m_nListFatherIndex != -1)
					{
						long grandfatherindex = static_cast<long>(m_lfemalePopln[inds1].m_nListFatherIndex) + totalfemales;
						strEdge = to_string_(grandfatherindex) + ", " + childindex + ", grand father\n";
						//outFile << strEdge;
						relation_edges.insert(strEdge);
						strEdge = childindex + ", " + to_string_(static_cast<long>(m_lfemalePopln[inds1].m_nListFatherIndex)) + ", grand child\n";
						//outFile << strEdge;
						relation_edges.insert(strEdge);
					}
					if (m_lfemalePopln[inds1].m_nListMotherIndex != -1)
					{
						strEdge = to_string_(static_cast<long>(m_lfemalePopln[inds1].m_nListMotherIndex)) + ", " + childindex + ", grand mother\n";
						//outFile << strEdge;
						relation_edges.insert(strEdge);
						strEdge = childindex + ", " + to_string_(static_cast<long>(m_lfemalePopln[inds1].m_nListMotherIndex)) + ", grand child\n";
						//outFile << strEdge;
						relation_edges.insert(strEdge);
					}
				}
			}//link grandparent
		}
				
	}

	size_t totalcnt = relation_edges.size();
	for (std::set<std::string>::iterator itr = relation_edges.begin(); itr != relation_edges.end(); itr++) //(auto str : relation_edges)
	{
		outFile << *itr;
	}

	outFile.close();
	CTextFileLogger::GetLogger()->Log("generate_family_tree()  -> end");
}


/*
generate edges info in below format.
conn_from: UniqueID of person 1
conn_to : UniqueID of person 2
edge_type : Any one of these values['grand child', 'aunt', 'uncle', 'grand father', 'grand mother', 'sister', 'brother', 'nephew', 'niece', 'husband', 'wife', 'father', 'mother']
*/
void CPopulationDB::generate_family_tree_new()
{
	//std::cout << "generate_family_tree()  -> " << std::endl;
	CTextFileLogger::GetLogger()->Log("generate_family_tree()  -> start");
	const long totalmales = static_cast<long>(m_lmalePopln.size());
	const long totalfemales = static_cast<long>(m_lfemalePopln.size());
	std::ofstream outFile("family_edges.csv");
	outFile << "conn_from,conn_to,edge_type\n";

	std::set<std::string> relation_edges;

	for (auto parent : m_parentChild)
	{
		std::string strEdge = "NA";
		size_t sztchildindex = std::get<4>(parent);
		bool bMatureChild = std::get<2>(parent);
		bool bMaleChild = std::get<3>(parent);

		if (false == bMatureChild)
		{
			if (sztchildindex > m_lChildPopln.size())
			{
				continue;
			}
			if (true == std::get<0>(parent))
			{
				strEdge = m_lmalePopln[std::get<1>(parent)].getUniqueID() + ", " + m_lChildPopln[sztchildindex].getUniqueID() + ", father\n";
			}
			else
			{
				strEdge = m_lfemalePopln[std::get<1>(parent)].getUniqueID() + ", " + m_lChildPopln[sztchildindex].getUniqueID() + ", mother\n";
			}
			//outFile << strEdge;
			relation_edges.insert(strEdge);
		}
		else
		{
			std::string childindex = "NA";
			if (true == bMaleChild)
			{
				childindex = m_lmalePopln[sztchildindex].getUniqueID();
			}
			else
			{
				childindex = m_lfemalePopln[sztchildindex].getUniqueID();
			}

			if (true == std::get<0>(parent))
			{
				strEdge = m_lmalePopln[std::get<1>(parent)].getUniqueID() + ", " + childindex + ", father\n";
			}
			else
			{
				strEdge = m_lfemalePopln[std::get<1>(parent)].getUniqueID() + ", " + childindex + ", mother\n";
			}
			//outFile << strEdge;
			relation_edges.insert(strEdge);
		}
	}

	for (auto siblings : m_Siblings)
	{
		bool bmature = std::get<0>(siblings);
		bool gender1 = std::get<1>(siblings);
		size_t inds1 = std::get<2>(siblings);
		bool gender2 = std::get<3>(siblings);
		size_t inds2 = std::get<4>(siblings);
		std::string strEdge = "NA";

		if (inds1 == inds2)
		{
			continue;
		}


		if (bmature)
		{
			if (true == gender1)
			{
				if (true == gender2)
				{
					strEdge = m_lmalePopln[inds1].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", brother\n";
				}
				else
				{ 
					strEdge = m_lmalePopln[inds1].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", brother\n";
				}
				
			}
			else
			{
				if (true == gender2)
				{
					strEdge = m_lfemalePopln[inds1].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", sister\n";
				}
				else
				{
					strEdge = m_lfemalePopln[inds1].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", sister\n";
				}
			}
		}
		else
		{
			if ((inds1 >= m_lChildPopln.size()) || (inds2 >= m_lChildPopln.size()))
			{
				continue;
			}
			if (true == gender1)
			{
				strEdge = m_lChildPopln[inds1].getUniqueID() + ", " + m_lChildPopln[inds2].getUniqueID() + ", brother\n";
			}
			else
			{
				strEdge = m_lChildPopln[inds1].getUniqueID() + ", " + m_lChildPopln[inds2].getUniqueID() + ", sister\n";
			}
		}
		//outFile << strEdge;
		relation_edges.insert(strEdge);

		if (true == bmature)
		{
			if (true == gender1)
			{
				if (m_lmalePopln[inds1].getChildCount() > 0)
				{
					for (auto i = m_lmalePopln[inds1].children_begin(); i < m_lmalePopln[inds1].children_end(); i++)
					{
						if (true == gender2)
						{
							if (true == std::get<0>(*i))//mature
							{
								if (true == std::get<1>(*i))//male
								{
									strEdge = m_lmalePopln[inds2].getUniqueID() + ", " + m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", uncle\n";
									relation_edges.insert(strEdge);
									strEdge = m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", nephew\n";
									relation_edges.insert(strEdge);
								}
								else
								{
									strEdge = m_lmalePopln[inds2].getUniqueID() + ", " + m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", uncle\n";
									relation_edges.insert(strEdge);
									strEdge = m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", niece\n";
									relation_edges.insert(strEdge);
								}
							}
							else
							{
								if (false == std::get<1>(*i))
								{
									strEdge = m_lmalePopln[inds2].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", uncle\n";
									relation_edges.insert(strEdge);
									strEdge = m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", niece\n";
									relation_edges.insert(strEdge);
								}
								else
								{
									strEdge = m_lmalePopln[inds2].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", uncle\n";
									relation_edges.insert(strEdge);
									strEdge = m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", nephew\n";
									relation_edges.insert(strEdge);
								}
							}
						}
						else
						{
							if (true == std::get<0>(*i))//mature
							{
								if (true == std::get<1>(*i))//male
								{
									strEdge = m_lfemalePopln[inds2].getUniqueID() + ", " + m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", aunt\n";
									relation_edges.insert(strEdge);
									strEdge = m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", nephew\n";
									relation_edges.insert(strEdge);
								}
								else
								{
									strEdge = m_lfemalePopln[inds2].getUniqueID() + ", " + m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", aunt\n";
									relation_edges.insert(strEdge);
									strEdge = m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", niece\n";
									relation_edges.insert(strEdge);
								}
							}
							else
							{
								if (false == std::get<1>(*i))
								{
									strEdge = m_lfemalePopln[inds2].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", aunt\n";
									relation_edges.insert(strEdge);
									strEdge = m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", niece\n";
									relation_edges.insert(strEdge);
								}
								else
								{
									strEdge = m_lfemalePopln[inds2].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", aunt\n";
									relation_edges.insert(strEdge);
									strEdge = m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", nephew\n";
									relation_edges.insert(strEdge);
								}
							}
						}
					}//for all childrens
				}
			}
			else
			{
				if (m_lfemalePopln[inds1].getChildCount() > 0)
				{
					for (auto i = m_lfemalePopln[inds1].children_begin(); i < m_lfemalePopln[inds1].children_end(); i++)
					{
						if (true == gender2)
						{
							if (true == std::get<0>(*i))//mature
							{
								if (true == std::get<1>(*i))//male
								{
									strEdge = m_lmalePopln[inds2].getUniqueID() + ", " + m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", uncle\n";
									relation_edges.insert(strEdge);
									strEdge = m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", nephew\n";
									relation_edges.insert(strEdge);
								}
								else
								{
									strEdge = m_lmalePopln[inds2].getUniqueID() + ", " + m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", uncle\n";
									relation_edges.insert(strEdge);
									strEdge = m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", niece\n";
									relation_edges.insert(strEdge);
								}
							}
							else
							{
								if (false == std::get<1>(*i))
								{
									strEdge = m_lmalePopln[inds2].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", uncle\n";
									relation_edges.insert(strEdge);
									strEdge = m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", niece\n";
									relation_edges.insert(strEdge);
								}
								else
								{
									strEdge = m_lmalePopln[inds2].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", uncle\n";
									relation_edges.insert(strEdge);
									strEdge = m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[inds2].getUniqueID() + ", nephew\n";
									relation_edges.insert(strEdge);
								}
							}
						}
						else
						{
							if (true == std::get<0>(*i))//mature
							{
								if (true == std::get<1>(*i))//male
								{
									strEdge = m_lfemalePopln[inds2].getUniqueID() + ", " + m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", aunt\n";
									relation_edges.insert(strEdge);
									strEdge = m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", nephew\n";
									relation_edges.insert(strEdge);
								}
								else
								{
									strEdge = m_lfemalePopln[inds2].getUniqueID() + ", " + m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", aunt\n";
									relation_edges.insert(strEdge);
									strEdge = m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", niece\n";
									relation_edges.insert(strEdge);
								}
							}
							else
							{
								if (false == std::get<1>(*i))
								{
									strEdge = m_lfemalePopln[inds2].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", aunt\n";
									relation_edges.insert(strEdge);
									strEdge = m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", niece\n";
									relation_edges.insert(strEdge);
								}
								else
								{
									strEdge = m_lfemalePopln[inds2].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", aunt\n";
									relation_edges.insert(strEdge);
									strEdge = m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[inds2].getUniqueID() + ", nephew\n";
									relation_edges.insert(strEdge);
								}
							}
						}
					}//for all children
				}
			}//if female
		}//if mature siblings
	}//for all siblings

	for (auto partner : m_edges)
	{
		bool gender1 = std::get<0>(partner);
		size_t inds1 = std::get<1>(partner);
		bool gender2 = std::get<2>(partner);
		size_t inds2 = std::get<3>(partner);
		std::string strEdge = "NA";
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
		//outFile << strEdge;
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
						if (true == std::get<0>(*i))//mature
						{
							if (true == std::get<1>(*i))//male
							{
								strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", grand father\n";
								relation_edges.insert(strEdge);
								strEdge = m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[fatherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
							else
							{
								strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", grand father\n";
								relation_edges.insert(strEdge);
								strEdge = m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[fatherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
						}
						else
						{
							strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", grand father\n";
							relation_edges.insert(strEdge);
						}//if not mature
					}

					if ((motherindx > 0) && (motherindx < m_lfemalePopln.size()))
					{
						if (true == std::get<0>(*i))//mature
						{
							if (true == std::get<1>(*i))//male
							{
								strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", grand mother\n";
								relation_edges.insert(strEdge);
								strEdge = m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[motherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
							else
							{
								strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", grand mother\n";
								relation_edges.insert(strEdge);
								strEdge = m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[motherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
						}
						else
						{
							strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", grand mother\n";
							relation_edges.insert(strEdge);
						}//if not mature
					}//if grand mother
				}//for children
			}//link grandparent
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
						if (true == std::get<0>(*i))//mature
						{
							if (true == std::get<1>(*i))//male
							{
								strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", grand father\n";
								relation_edges.insert(strEdge);
								strEdge = m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[fatherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
							else
							{
								strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", grand father\n";
								relation_edges.insert(strEdge);
								strEdge = m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lmalePopln[fatherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
						}
						else
						{
							strEdge = m_lmalePopln[fatherindx].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", grand father\n";
							relation_edges.insert(strEdge);
						}//if not mature
					}//if grand father

					if ((motherindx > 0) && (motherindx < m_lfemalePopln.size()))
					{
						if (true == std::get<0>(*i))//mature
						{
							if (true == std::get<1>(*i))//male
							{
								strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", grand mother\n";
								relation_edges.insert(strEdge);
								strEdge = m_lmalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[motherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
							else
							{
								strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", grand mother\n";
								relation_edges.insert(strEdge);
								strEdge = m_lfemalePopln[std::get<2>(*i)].getUniqueID() + ", " + m_lfemalePopln[motherindx].getUniqueID() + ", grand child\n";
								relation_edges.insert(strEdge);
							}
						}
						else
						{
							strEdge = m_lfemalePopln[motherindx].getUniqueID() + ", " + m_lChildPopln[std::get<2>(*i)].getUniqueID() + ", grand mother\n";
							relation_edges.insert(strEdge);
						}//if not mature
					}//if grand mother
				}//for children
			}//link grandparent
		}//if female
	}//for partners

	size_t totalcnt = relation_edges.size();
	for (std::set<std::string>::iterator itr = relation_edges.begin(); itr != relation_edges.end(); itr++) //(auto str : relation_edges)
	{
		outFile << *itr;
	}

	outFile.close();
	CTextFileLogger::GetLogger()->Log("generate_family_tree()  -> end");
}

void CPopulationDB::split(std::string str, char del, std::vector < std::string >& out ) 
{
	std::string temp = "";
	for (int i = 0; i < (int)str.size(); i++) {
		if ( (str[i] != del) )
		{
			if ((str[i] == '\r') || (str[i] == '\n'))
				continue;
			temp += str[i];
		}
		else {
			out.push_back(temp);
			temp = "";
		}
	}//for
	out.push_back(temp);
}

void CPopulationDB::WritePopulationDataToFile(std::string filename)
{
	if (filename == "")
	{
		filename = "db.csv";
	}
	std::ofstream outFile(filename);
	outFile << "UniqueID,FName,Lname,DOB,Age,Gender,FatherName,MotherName,SSN,HomeAddress,EducationLevel,MaritalStatus,SpouseName,HasChildren,NumOfChild,ChildrenNames,IsEmployed,OfficeAddress,FatherIndexinDB,MotherIndexinDB";
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
}


long CPopulationDB::generateRandomNumber_(long min, long max) const {
	std::random_device rd;
	std::mt19937_64 gen(rd());

	// Get the current system time in milliseconds
	auto currentTime = std::chrono::system_clock::now();
	auto currentTimeMillis = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch());

	// Generate a random number within the range
	std::uniform_int_distribution<long> distribution(min, max);
	long randomNumber;
	do {
		randomNumber = distribution(gen);
	} while (randomNumber == lastRandomNumber_ && currentTime == lastTimestamp_);

	// Store the last generated number and timestamp
	lastRandomNumber_ = randomNumber;
	lastTimestamp_ = currentTime;

	return randomNumber;
}

template <class T>
inline std::string CPopulationDB::to_string_(const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}