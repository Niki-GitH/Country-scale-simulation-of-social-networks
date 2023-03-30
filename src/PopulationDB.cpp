#include "../include/PopulationDB.h"
#include "../include/TextFileLogger.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define ELDERLY_DOB_ST   1960
#define ELDERLY_DOB_EN   1965
#define WORKING_DOB_ST	 1966
#define WORKING_DOB_EN   2004
#define CHILD_DOB_ST	 2005
#define CHILD_DOB_EN     2022

long CPopulationDB::m_nTotalEvents = 0;

CPopulationDB::CPopulationDB()
{
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

	m_csCareer = std::make_unique < CHMM_Career>(states, observations, transition_matrix, emission_matrix, pi);


	std::vector<std::string> hstates;
	hstates.push_back("GRADUATION");
	hstates.push_back("NEW_CHILD");
	hstates.push_back("MARRIAGE");
	hstates.push_back("HOME_PURCHASE");
	hstates.push_back("NEW_JOB");
	hstates.push_back("DEATH");
	std::vector<int> ages;
	for (int i = 1; i < 90; ++i)
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
	//m_cslifeChoice->savemodel();

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


void CPopulationDB::MakeRandomPopulationData(std::string country, std::string prefectr)
{
	std::transform(country.begin(), country.end(), country.begin(), ::tolower);
	std::transform(prefectr.begin(), prefectr.end(), prefectr.begin(), ::tolower);
	struct timespec ts;
	//long prvspopm = 0;
	//long prvspopf = 0;
	for (auto db : m_populationDB)
	{
		clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int minG = 0;
		int maxG = 2;
		int randFSts = minG + rand() % ((maxG + 1) - minG);
		if ((db.m_strCountry == country) && (db.m_strPrefec == prefectr))
		{
			long population = db.m_nPopulation;
			long ealderlyPopln = population * (m_dAvgPercentageElders/100.0); //gen1
			long eldermales = (ealderlyPopln * (m_dAvgPercentageMale / 100.0));
			for (long ep = 0; ep < ealderlyPopln; ++ep)
			{
				GenderInfo_ ginfo = GENDER_TYPE_MALE;
				if (ep >= eldermales)
				{
					ginfo = GENDER_TYPE_FEMALE;
				}
				std::pair<std::string, std::string> fullname = pInoGen->GenerateRandomName(ginfo);
				_dob stdob = pInoGen->GenerateRandomDOB(ELDERLY_DOB_ST, ELDERLY_DOB_EN);
				Graduation_ grad = pInoGen->GenerateRandomEducation();
				MaritalSatus_ msts = MARITAL_STATUS_SINGLE;
				msts = pInoGen->GenerateRandomMSts();
				std::string randAdd = pInoGen->GenerateRandomAddress(db.m_nArea, db.m_longitude, db.m_latitude);
				randAdd = randAdd + db.m_strLocalityName + db.m_strSubPrefec + db.m_strPrefec + db.m_strCountry;
				CPersonInfo pMInfo(fullname.first, fullname.second, stdob, ginfo, randAdd);
				pMInfo.setSSN(pInoGen->GenerateRandomSSN());
				pMInfo.setEmail(pInoGen->GenerateRandomEmail(pMInfo.getFirstName(), pMInfo.getLastName()));
				pMInfo.setGraduation(grad);
				pMInfo.setMaritalStatus(msts);
				pMInfo.setHasChildren(false);
				
				if(1 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_LOWER_MIDDLE);
				}
				else if(2 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_MIDDLE);
				}
				else if(3 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_UPPER_MIDDLE);
				}
				else if(4 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_RICH);
				}
				else if(5 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_ULTRA_RICH);
				}
				else
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_POOR);
				}
				
				if (ginfo == GENDER_TYPE_MALE)
				{
					m_lmalePopln.push_back(pMInfo);
				}
				else
				{
					m_lfemalePopln.push_back(pMInfo);
				}
			}

			long childPopln = population * (m_dAvgPercentageYoung/100.0);
			long childmales = (childPopln * (m_dAvgPercentageMale / 100.0));
			for (long cp = 0; cp < childPopln; ++cp)
			{
				GenderInfo_ ginfo = GENDER_TYPE_MALE;
				if (cp >= childmales)
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
				pMInfo.setEmail(pInoGen->GenerateRandomEmail(pMInfo.getFirstName(), pMInfo.getLastName()));
				pMInfo.setGraduation(grad);
				pMInfo.setMaritalStatus(MARITAL_STATUS_SINGLE);
				pMInfo.setHasChildren(false);
				m_lChildPopln.push_back(pMInfo);
			}

			long workingPopln = population - (childPopln + ealderlyPopln);
			long workingmales = (workingPopln * (m_dAvgPercentageMale / 100.0));
			for (long p = 0; p < workingPopln; ++p)
			{
				GenderInfo_ ginfo = GENDER_TYPE_MALE;
				if (p >= workingmales)
				{
					ginfo = GENDER_TYPE_FEMALE;
				}
				std::pair<std::string, std::string> malename = pInoGen->GenerateRandomName(ginfo);
				_dob stdob = pInoGen->GenerateRandomDOB(WORKING_DOB_ST, WORKING_DOB_EN);
				Graduation_ grad = pInoGen->GenerateRandomEducation();
				MaritalSatus_ msts = MARITAL_STATUS_SINGLE;
				msts = pInoGen->GenerateRandomMSts();	
				std::string randAdd = pInoGen->GenerateRandomAddress(db.m_nArea, db.m_longitude, db.m_latitude);
				randAdd = randAdd + db.m_strLocalityName + db.m_strSubPrefec + db.m_strPrefec + db.m_strCountry;
				CPersonInfo pMInfo(malename.first, malename.second, stdob, ginfo, randAdd);
				pMInfo.setSSN(pInoGen->GenerateRandomSSN());
				pMInfo.setEmail(pInoGen->GenerateRandomEmail(pMInfo.getFirstName(), pMInfo.getLastName()));
				pMInfo.setGraduation(grad);
				pMInfo.setMaritalStatus(msts);
				pMInfo.setHasChildren(false);
				if (ginfo == GENDER_TYPE_MALE)
				{
					m_lmalePopln.push_back(pMInfo);
				}
				else
				{
					m_lfemalePopln.push_back(pMInfo);
				}
			}//for
			
			//prvspopm = m_lmalePopln.size();
			//prvspopf = m_lfemalePopln.size();
			std::cout<<"MakeRandomPopulationData() -> Population Count :"<< GetPopCount() << std::endl;
			
		}//if
	}//for

}

void CPopulationDB::MakePopulationData(std::string country, int gen, std::string prefectr)
{
	CTextFileLogger::GetLogger()->Log("MakePopulationData(), generation = %d",gen);
	std::transform(country.begin(), country.end(), country.begin(), ::tolower);
	std::transform(prefectr.begin(), prefectr.end(), prefectr.begin(), ::tolower);
	struct timespec ts;
	//long prvspopm = 0;
	//long prvspopf = 0;
	int popcount = 0;
	for (auto db : m_populationDB)
	{
		clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int minG = 0;
		int maxG = (int)FINANCE_STATUS_END;
		int randFSts = minG + rand() % ((maxG + 1) - minG);
		

		if ((db.m_strCountry == country) && (db.m_strPrefec == prefectr))
		{
			long population = db.m_nPopulation;
			long ealderlyPopln = population * (m_dAvgPercentageElders / 100.0); //gen1
			long eldermales = (ealderlyPopln * (m_dAvgPercentageMale / 100.0));
			for (long ep = 0; ep < ealderlyPopln; ++ep)
			{
				GenderInfo_ ginfo = GENDER_TYPE_MALE;
				if (ep >= eldermales)
				{
					ginfo = GENDER_TYPE_FEMALE;
				}
				std::pair<std::string, std::string> fullname = pInoGen->GenerateRandomName(ginfo);
				_dob stdob = pInoGen->GenerateRandomDOB(ELDERLY_DOB_ST, ELDERLY_DOB_EN);
				Graduation_ grad = pInoGen->GenerateRandomEducation();
				MaritalSatus_ msts = MARITAL_STATUS_SINGLE;
				msts = pInoGen->GenerateRandomMSts();
				std::string randAdd = pInoGen->GenerateRandomAddress(db.m_nArea, db.m_longitude, db.m_latitude);
				randAdd = randAdd + db.m_strLocalityName + db.m_strSubPrefec + db.m_strPrefec + db.m_strCountry;
				CPersonInfo pMInfo(fullname.first, fullname.second, stdob, ginfo, randAdd);
				pMInfo.setSSN(pInoGen->GenerateRandomSSN());
				pMInfo.setEmail(pInoGen->GenerateRandomEmail(pMInfo.getFirstName(), pMInfo.getLastName()));
				pMInfo.setGraduation(grad);
				pMInfo.setMaritalStatus(msts);
				pMInfo.setHasChildren(false);
				
				if(1 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_LOWER_MIDDLE);
				}
				else if(2 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_MIDDLE);
				}
				else if(3 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_UPPER_MIDDLE);
				}
				else if(4 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_RICH);
				}
				else if(5 == randFSts)
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_ULTRA_RICH);
				}
				else
				{
					pMInfo.setFinancialSts(FINANCE_STATUS_POOR);
				}
				
				clock_gettime(CLOCK_MONOTONIC, &ts);
				srand((time_t)ts.tv_nsec);
				int minAge = 18;
				int maxAge = 50;
				int randAge = minAge + rand() % ((maxAge + 1) - minAge);
				pMInfo.setAge(randAge);
				if (ginfo == GENDER_TYPE_MALE)
				{
					m_lmalePopln.push_back(pMInfo);
				}
				else
				{
					m_lfemalePopln.push_back(pMInfo);
				}
				popcount++;
			}

			std::cout << "Generated random first generation population. Count :" << GetPopCount() << std::endl;
			CTextFileLogger::GetLogger()->Log("MakePopulationData(), Generated random first generation population. Count : %d",GetPopCount());
			if(popcount>500)
			{
			  break;
			}
		}//if
	}//for

}


void CPopulationDB::Evolve(int years)
{
//	std::cout << "Evolve population"<<std::endl;
	for (int i = 0; i < years; ++i)
	{
	CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d]",i);
	//std::cout << "Evolve population"<<std::endl;
		int childCount = 0;
		for (auto persn : m_lfemalePopln)
		{
			//CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d] name = %s",i,persn.getFullName().c_str());
			int age = persn.getAge();
			persn.setAge(age+1);
			std::vector<Observation> observations;
			Observation ob;
			ob.age = age;
			ob.financial_status = persn.getFinancialSts_();
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
					  if(persn.getGraduation() == GRAD_TYPE_NONE)
					  {
					  pev.m_strEventComments ="SENIOR";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_SENIOR_HIGH)
					  {
					  pev.m_strEventComments ="DIPLOMA";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_DIPLOMA)
					  {
					  pev.m_strEventComments ="ADVANCED";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_ADVANCED_DIPLOMA)
					  {
					  pev.m_strEventComments ="BACHELOR";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_BACHELOR)
					  {
					  pev.m_strEventComments ="MASTER";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_MASTER)
					  {
					  pev.m_strEventComments ="DOCTORAL";
					  }
					  
					}
					persn.AddEvent(pev);
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event GRADUATION",i,persn.getFullName().c_str());
					//m_csCareer
				}
				else if (ev == EVENT_TYPE_NEW_CHILD)
				{
				   CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event NEW_CHILD",i,persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_MARRIAGE)
				{
					//std::cout << "New event MARRIAGE"<<std::endl;
					makeRandomEventMarriage(persn);
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event MARRIAGE",i,persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_HOME_PURCHASE)
				{
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event HOME_PURCHASE",i,persn.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_HOME_PURCHASE;
					pev.m_strEventComments = "New home address=***";
					persn.AddEvent(pev);
				}
				else if (ev == EVENT_TYPE_NEW_JOB)
				{
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][female population]: name = %s New event NEW_JOB",i,persn.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_NEW_JOB;
					pev.m_strEventComments = "Office address=***";
					persn.AddEvent(pev);
				}
			}
			
			
		}

		for (auto persn : m_lmalePopln)
		{
			AgeObsSymbol ageObs = CHILD;
			
			int age = persn.getAge();
			persn.setAge(age+1);
			std::vector<Observation> observations;
			Observation ob;
			ob.age = age;
			ob.financial_status = persn.getFinancialSts_();
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
					  if(persn.getGraduation() == GRAD_TYPE_NONE)
					  {
					  pev.m_strEventComments ="SENIOR";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_SENIOR_HIGH)
					  {
					  pev.m_strEventComments ="DIPLOMA";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_DIPLOMA)
					  {
					  pev.m_strEventComments ="ADVANCED";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_ADVANCED_DIPLOMA)
					  {
					  pev.m_strEventComments ="BACHELOR";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_BACHELOR)
					  {
					  pev.m_strEventComments ="MASTER";
					  }
					  else if(persn.getGraduation() == GRAD_TYPE_MASTER)
					  {
					  pev.m_strEventComments ="DOCTORAL";
					  }
					  
					}
					persn.AddEvent(pev);
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event GRADUATION",i,persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_NEW_CHILD)
				{
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event NEW_CHILD",i,persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_MARRIAGE)
				{
					//std::cout << "New event MARRIAGE"<<std::endl;
					makeRandomEventMarriage(persn);
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event MARRIAGE",i,persn.getFullName().c_str());
				}
				else if (ev == EVENT_TYPE_HOME_PURCHASE)
				{
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event HOME_PURCHASE",i,persn.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_HOME_PURCHASE;
					pev.m_strEventComments = "New home address=***";
					persn.AddEvent(pev);
				}
				else if (ev == EVENT_TYPE_NEW_JOB)
				{
					CTextFileLogger::GetLogger()->Log("Evolve() [Evolve year = %d][Male population]: name = %s New event NEW_JOB",i,persn.getFullName().c_str());
					pev.m_nEventType = EVENT_TYPE_NEW_JOB;
					pev.m_strEventComments = "Office address=***";
					persn.AddEvent(pev);
				}
			}
		}
	}
}


void CPopulationDB::makeRandomEventMarriage(CPersonInfo & parentM)
{

	int nMin = 0;
	int nMax = m_lmalePopln.size();
	if (nMin >= nMax)
	{
		nMin = 0;
	}
	struct timespec ts;
	if (parentM.getGender() != GENDER_TYPE_MALE)
	{
		nMax = m_lfemalePopln.size();
		//clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int randNoF = nMin + rand() % ((nMax + 1) - nMin);
		bool bFemaleFound = false;
		if ((m_lfemalePopln[randNoF].getMaritalStatus() != MARITAL_STATUS_SINGLE) && (m_lfemalePopln[randNoF].getMaritalStatus() != MARITAL_STATUS_DIVORCED))
		{
			for (int f = randNoF + 1; f < nMax; ++f)
			{
				if (m_lfemalePopln[f].getMaritalStatus() == MARITAL_STATUS_SINGLE)
				{
					if (m_lfemalePopln[f].getDOB_Year() < (CHILD_DOB_ST - 4))
					{
						randNoF = f;
						bFemaleFound = true;
						break;
					}
				}
			}
			if (false == bFemaleFound)
			{
				for (int f = nMin; f < randNoF; ++f)
				{
					if (m_lfemalePopln[f].getMaritalStatus() == MARITAL_STATUS_SINGLE)
					{
						if (m_lfemalePopln[f].getDOB_Year() < (CHILD_DOB_ST - 4))
						{
							randNoF = f;
							bFemaleFound = true;
							break;
						}
					}
				}
				if (false == bFemaleFound)
				{
					return;
				}
			}
		}

		m_lfemalePopln[randNoF].setMaritalStatus(MARITAL_STATUS_MARRIED);
		parentM.setMaritalStatus(MARITAL_STATUS_MARRIED);
		std::string malePrtnr = parentM.getFirstName() + " " + parentM.getLastName();
		std::string femalePrtnr = m_lfemalePopln[randNoF].getFirstName() + " " + m_lfemalePopln[randNoF].getLastName();
		PersonEvent tempEv;
		tempEv.m_nEventType = EVENT_TYPE_MARRIAGE;
		tempEv.m_strLocation = m_lfemalePopln[randNoF].getHomeAddress();
		tempEv.m_strEventComments = malePrtnr;
		m_lfemalePopln[randNoF].AddEvent(tempEv);
		tempEv.m_strEventComments = femalePrtnr;
		parentM.AddEvent(tempEv);
		m_lfemalePopln[randNoF].setHomeAddress(parentM.getHomeAddress());
		//std::cout << " -> " << malePrtnr << " & " << femalePrtnr << std::endl;
		m_marriages.push_back(std::make_tuple(parentM, m_lfemalePopln[randNoF]));

	}
	else
	{
		//clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int randNoM = nMin + rand() % ((nMax + 1) - nMin);

		bool bMaleFound = false;
		if ((m_lmalePopln[randNoM].getMaritalStatus() != MARITAL_STATUS_SINGLE) && (m_lmalePopln[randNoM].getMaritalStatus() != MARITAL_STATUS_DIVORCED))
		{
			for (int m = randNoM + 1; m < nMax; ++m)
			{
				if (m_lmalePopln[m].getMaritalStatus() == MARITAL_STATUS_SINGLE)
				{
					if (m_lmalePopln[m].getDOB_Year() < (CHILD_DOB_ST - 4))
					{
						randNoM = m;
						bMaleFound = true;
						break;
					}
				}
			}
			if (false == bMaleFound)
			{
				for (int m = nMin; m < randNoM; ++m)
				{
					if (m_lmalePopln[m].getMaritalStatus() == MARITAL_STATUS_SINGLE)
					{
						if (m_lmalePopln[m].getDOB_Year() < (CHILD_DOB_ST - 4))
						{
							randNoM = m;
							bMaleFound = true;
							break;
						}
					}
				}
				if (false == bMaleFound)
				{
					return;
				}
			}
		}

		parentM.setMaritalStatus(MARITAL_STATUS_MARRIED);
		m_lmalePopln[randNoM].setMaritalStatus(MARITAL_STATUS_MARRIED);
		std::string malePrtnr = m_lmalePopln[randNoM].getFirstName() + " " + m_lmalePopln[randNoM].getLastName();
		std::string femalePrtnr = parentM.getFirstName() + " " + parentM.getLastName();
		PersonEvent tempEv;
		tempEv.m_nEventType = EVENT_TYPE_MARRIAGE;
		tempEv.m_strLocation = parentM.getHomeAddress();
		tempEv.m_strEventComments = malePrtnr;
		parentM.AddEvent(tempEv);
		tempEv.m_strEventComments = femalePrtnr;
		m_lmalePopln[randNoM].AddEvent(tempEv);
		parentM.setHomeAddress(m_lmalePopln[randNoM].getHomeAddress());
		//std::cout << " -> " << malePrtnr << " & " << femalePrtnr << std::endl;

		m_marriages.push_back(std::make_tuple(m_lmalePopln[randNoM], parentM));


	}
	

}


void CPopulationDB::makeRandomEventMarriage(int count,int startIndexM, int startIndexF)
{
	std::cout << "makeRandomEventMarriage()  -> Create marriages and Childs " << std::endl;
	int nMinM = startIndexM;
	int nMax_M = m_lmalePopln.size();
	if (nMinM >= nMax_M)
	{
		nMinM = 0;
	}
	int nMinF = startIndexF;
	int nMax_F = m_lfemalePopln.size();
	if (nMinF >= nMax_F)
	{
		nMinF = 0;
	}
	struct timespec ts;

	if (count <= 0)
	{
		count = (m_lmalePopln.size() + m_lfemalePopln.size() ) * (m_dAvgMarriages/100.0);
	}
	//std::cout << "makeRandomEventMarriage()  :" << std::endl;
	long totalChildConnections = 0;
	for (int i = 0; i < count; ++i)
	{
		clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int randNoM = nMinM + rand() % ((nMax_M + 1) - nMinM);
		
		bool bMaleFound = false;
		bool bFemaleFound = false;
		if ( (m_lmalePopln[randNoM].getMaritalStatus() != MARITAL_STATUS_SINGLE) && (m_lmalePopln[randNoM].getMaritalStatus() != MARITAL_STATUS_DIVORCED))
		{
			for (int m = randNoM + 1; m < nMax_M; ++m)
			{
				if (m_lmalePopln[m].getMaritalStatus() == MARITAL_STATUS_SINGLE)
				{
					if (m_lmalePopln[m].getDOB_Year() < (CHILD_DOB_ST-4))
					{
						randNoM = m;
						bMaleFound = true;
						break;
					}
				}
			}
			if (false == bMaleFound)
			{
				for (int m = nMinM; m < randNoM; ++m)
				{
					if (m_lmalePopln[m].getMaritalStatus() == MARITAL_STATUS_SINGLE)
					{
						if (m_lmalePopln[m].getDOB_Year() < (CHILD_DOB_ST - 4))
						{
							randNoM = m;
							bMaleFound = true;
							break;
						}
					}
				}
				if (false == bMaleFound)
				{
					i = count;
					break;
					//std::cout << "makeRandomEventMarriage() -> failed to find male partner" << std::endl;
				}
			}
		}
		else
		{
			bMaleFound = true;
		}
		
		clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int randNoF = nMinF + rand() % ((nMax_F + 1) - nMinF);

		if ((m_lfemalePopln[randNoF].getMaritalStatus() != MARITAL_STATUS_SINGLE)&&(m_lfemalePopln[randNoF].getMaritalStatus() != MARITAL_STATUS_DIVORCED))
		{
			for (int f = randNoF + 1; f < nMax_M; ++f)
			{
				if (m_lfemalePopln[f].getMaritalStatus() == MARITAL_STATUS_SINGLE)
				{
					if (m_lfemalePopln[f].getDOB_Year() < (CHILD_DOB_ST - 4))
					{
						randNoF = f;
						bFemaleFound = true;
						break;
					}
				}
			}
			if (false == bFemaleFound)
			{
				for (int f = nMinF; f < randNoF; ++f)
				{
					if (m_lfemalePopln[f].getMaritalStatus() == MARITAL_STATUS_SINGLE)
					{
						if (m_lfemalePopln[f].getDOB_Year() < (CHILD_DOB_ST - 4))
						{
							randNoF = f;
							bFemaleFound = true;
							break;
						}
					}
				}
				if (false == bFemaleFound)
				{
					i = count;
					break;
					//std::cout << "makeRandomEventMarriage() -> failed to find Female partner" << std::endl;
				}
			}
		}
		else
		{
			bFemaleFound = true;
		}


		if ((randNoM < 0) || (randNoM >= m_lmalePopln.size()))
		{
			continue;
		}

		if ((randNoF < 0) || (randNoF >= m_lfemalePopln.size()))
		{
			continue;
		}
		
		if (bFemaleFound && bMaleFound)
		{
			m_lfemalePopln[randNoF].setMaritalStatus(MARITAL_STATUS_MARRIED);
			m_lmalePopln[randNoM].setMaritalStatus(MARITAL_STATUS_MARRIED);
			std::string malePrtnr = m_lmalePopln[randNoM].getFirstName() + " " + m_lmalePopln[randNoM].getLastName();
			std::string femalePrtnr = m_lfemalePopln[randNoF].getFirstName() + " " + m_lfemalePopln[randNoF].getLastName();
			PersonEvent tempEv;
			tempEv.m_nEventType = EVENT_TYPE_MARRIAGE;
			tempEv.m_strLocation = m_lfemalePopln[randNoF].getHomeAddress();
			tempEv.m_strEventComments = malePrtnr;
			m_lfemalePopln[randNoF].AddEvent(tempEv);
			tempEv.m_strEventComments = femalePrtnr;
			m_lmalePopln[randNoM].AddEvent(tempEv);
			m_lfemalePopln[randNoF].setHomeAddress(m_lmalePopln[randNoM].getHomeAddress());
			//std::cout << " -> " << malePrtnr << " & " << femalePrtnr << std::endl;
			

			m_marriages.push_back(std::make_tuple(m_lmalePopln[randNoM], m_lfemalePopln[randNoF]));

			clock_gettime(CLOCK_MONOTONIC, &ts);
			srand((time_t)ts.tv_nsec);
			const int childmin = 0;
			const int childmax = 3;
			int childs = childmin + rand() % ((childmax + 1) - childmin);
			
			totalChildConnections = totalChildConnections + childs;

			//make random child connections
			makeRandomEventChild(childs,m_lmalePopln[randNoM], m_lfemalePopln[randNoF]);
		}
		else
		{
		  continue;
		}
	}
	std::cout << "makeRandomEventMarriage()  -> Total marriages = " <<count<< std::endl;
	std::cout << "makeRandomEventChild()  -> Total child connections = " << totalChildConnections << std::endl;
}

void CPopulationDB::makeRandomEventDeaths()
{

	long totaldeathsadult = (m_lmalePopln.size() + m_lfemalePopln.size()) * (m_dCrudeDeathRate / 100.0);
	long totaldeathsinfants = m_dInfantMortalityRate;
	
	std::cout << "makeRandomEventDeaths()  -> average adults death per year" <<totaldeathsadult<< std::endl;
	//std::cout << "makeRandomEventDeaths()  -> average infant death per year" <<totaldeathsadult<< std::endl;

	int nMinM = 0;
	int nMax_M = m_lmalePopln.size();
	if (nMinM >= nMax_M)
	{
		nMinM = 0;
	}
	int nMinF = 0;
	int nMax_F = m_lfemalePopln.size();
	if (nMinF >= nMax_F)
	{
		nMinF = 0;
	}
	struct timespec ts;

	for (int i = 0; i < totaldeathsadult/2; ++i)
	{
		clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int randNoM = nMinM + rand() % ((nMax_M + 1) - nMinM);

		bool bMaleFound = false;
		bool bFemaleFound = false;
		
		if ((randNoM < 0) || (randNoM >= m_lmalePopln.size()))
		{
			randNoM = m_lmalePopln.size()-1;
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

		clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int randNoF = nMinF + rand() % ((nMax_F + 1) - nMinF);
		
		if ((randNoF < 0) || (randNoF >= m_lfemalePopln.size()))
		{
			randNoF = m_lfemalePopln.size()-1;
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
			m_lmalePopln[randNoM].AddEvent(ev);
		}
		if (bFemaleFound)
		{
			m_lfemalePopln[randNoF].setDOD(tempdd);
			m_lfemalePopln[randNoF].AddEvent(ev);
		}

	}
}


void CPopulationDB::makeRandomEventChild(int count, CPersonInfo & parentM, CPersonInfo & parentF)
{
	for (int chld = 0; chld < count; chld++)
	{
		int cindx = m_lChildPopln.size() - 1;
		bool bChildFound = false;
		int nMinC = 0;
		int nMaxC = m_lChildPopln.size();
		struct timespec ts;
		//clock_gettime(CLOCK_MONOTONIC, &ts);
		srand((time_t)ts.tv_nsec);
		int randNoM = nMinC + rand() % ((nMaxC + 1) - nMinC);

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
			parentM.setChildCount(parentM.getChildCount() + 1);
			parentF.setChildCount(parentF.getChildCount() + 1);
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
			parentM.AddEvent(ev);
			parentF.AddEvent(ev);
			parentM.setHasChildren(true);
			parentF.setHasChildren(true);

			m_parentChild.push_back(std::make_tuple(parentM, m_lChildPopln[cindx]));
			m_parentChild.push_back(std::make_tuple(parentF, m_lChildPopln[cindx]));
		}

	}//for
	//std::cout << "makeRandomEventChild()  -> " <<count<< std::endl;
}

void CPopulationDB::generate_family_tree()
{
        std::cout << "generate_family_tree()  -> " << std::endl;
	for (auto marriage : m_marriages)
	{
		m_edges.push_back(std::make_tuple(std::get<0>(marriage), std::get<1>(marriage), EDGE_TYPE_PARTNER));
		m_edges.push_back(std::make_tuple(std::get<1>(marriage), std::get<0>(marriage), EDGE_TYPE_PARTNER));
		std::string strEdge;
		strEdge = std::get<0>(marriage).getFullName() + ", " + std::get<1>(marriage).getFullName() + ", Partner";
		m_strEdges.push_back(strEdge);
		strEdge = std::get<1>(marriage).getFullName() + ", " + std::get<0>(marriage).getFullName() + ", Partner";
		m_strEdges.push_back(strEdge);
	}

	CPersonInfo prvsparent = std::get<0>(m_parentChild.front());
	CPersonInfo prvschild = std::get<1>(m_parentChild.front());
	int indx = 0;
	for (auto parentchild : m_parentChild)
	{
		std::string strEdge;
		m_edges.push_back(std::make_tuple(std::get<0>(parentchild), std::get<1>(parentchild), EDGE_TYPE_PARENT));
		m_edges.push_back(std::make_tuple(std::get<1>(parentchild), std::get<0>(parentchild), EDGE_TYPE_CHILD));		
		strEdge = std::get<0>(parentchild).getFullName() + ", " + std::get<1>(parentchild).getFullName() + ", Parent";
		m_strEdges.push_back(strEdge);
		strEdge = std::get<1>(parentchild).getFullName() + ", " + std::get<0>(parentchild).getFullName() + ", Child";
		m_strEdges.push_back(strEdge);
		if (0 == (indx % 2))
		{
			if (prvsparent != std::get<0>(parentchild))
			{
				prvsparent = std::get<0>(parentchild);
				prvschild = std::get<0>(parentchild);
			}
			else
			{
				m_edges.push_back(std::make_tuple(prvschild, std::get<1>(parentchild), EDGE_TYPE_SIBLING));
				m_edges.push_back(std::make_tuple(std::get<1>(parentchild), prvschild,EDGE_TYPE_SIBLING));
				strEdge = prvschild.getFullName() + ", " + std::get<1>(parentchild).getFullName() + ", Sibling";
				m_strEdges.push_back(strEdge);
				strEdge = std::get<1>(parentchild).getFullName() + ", " + prvschild.getFullName() + ", Sibling";
				m_strEdges.push_back(strEdge);
			}
		}
		indx++;
	}
	
}


void CPopulationDB::print_family_edges()
{
	std::ofstream outFile("family_edges.csv");
	outFile << "From,To,EdgeType\n";
	int cnt = 0;
	for (auto str : m_strEdges)
	{
		outFile << str;
		outFile << "\n";
		if(0 == (cnt%100))
		{
		  std::cout << str << std::endl;
		  cnt =0;
		}
		cnt++;
	}
	std::cout << "***  Total of "<<m_strEdges.size()<<" connections were established." << std::endl;
	outFile.close();
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

void CPopulationDB::WritePopulationDataToFile()
{
	std::ofstream outFile("persons.csv");
	outFile << "FName,Mname,Lname,DOB,Age,Gender,BirthPlace,FatherName,MotherName,SSN,Email,HomeAddress,EducationLevel,MaritalStatus,SpouseName,HasChildren,NumOfChild,ChildrenNames,IsEmployed,OfficeAddress";
	for (int evs = 0; evs < m_nTotalEvents; ++evs)
	{
		std::string ev = std::to_string(evs + 1);
		std::string evens = ", Event" + ev + ", Event" + ev + " Info";
		outFile << evens;
	}
	outFile << "\n";

	for (auto persn : m_lfemalePopln)
	{
		outFile << persn.GetFormatedString();
		outFile << "\n";
	}
	
	for (auto persn : m_lmalePopln)
	{
		outFile << persn.GetFormatedString();
		outFile << "\n";
	}
	
	for (auto persn : m_lChildPopln)
	{
		outFile << persn.GetFormatedString();
		outFile << "\n";
	}
	outFile.close();
}
