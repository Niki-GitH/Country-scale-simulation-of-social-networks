#include "../include/PersonInfoGen.h"
#include <fstream>
#include <iostream>
#include <random>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <string>  // for string

CPersonInfoGen::CPersonInfoGen()
{
	ReadFirstNamesDBF("./data/FirstName-Female.txt");
	ReadFirstNamesDBM("./data/FirstName-Male.txt");
	ReadLastNamesDB("./data/surnames.txt");
}

CPersonInfoGen::~CPersonInfoGen()
{
	m_vFirstNames_M.clear();
	m_vFirstNames_F.clear();
	m_vLastName.clear();
}


/*
Generates a random name
In     : gender -> gender type
return : pair<FirstName, Last Name>
*/
std::pair<std::string, std::string> CPersonInfoGen::GenerateRandomName(GenderInfo_ gender) const
{
	std::pair<std::string, std::string> strname{ "AAAA","BBBB" };
	do {
		
		int min = 0;
		int max = m_vFirstNames_F.size() - 1;
		if (gender != GENDER_TYPE_FEMALE)
		{
			max = m_vFirstNames_M.size() - 1;
		}
		if (max > 1)
		{
			long randNo = generateRandomNumber((long)min, (long)max);
			//int randNo = min + rand() % ((max + 1) - min);

			std::string strFname = "";
			if (gender != GENDER_TYPE_FEMALE)
			{
				strFname = m_vFirstNames_M[randNo];
			}
			else
			{
				strFname = m_vFirstNames_F[randNo];
			}
		
			strFname.erase(std::remove(strFname.begin(), strFname.end(), '\r'), strFname.cend());
			strFname.erase(std::remove(strFname.begin(), strFname.end(), '\n'), strFname.cend());
			strname.first = strFname;
		}
		//std::cout<<"Gen :"<<strFname<<std::endl;
		max = m_vLastName.size() - 1;
		if (max > 1)
		{
			long randNo = generateRandomNumber((long)min, (long)max);
			//int randNo = min + rand() % ((max + 1) - min);
			std::string strLname = m_vLastName[randNo];
			//std::cout<<"Gen :"<<strLname<<std::endl;
			strLname.erase(std::remove(strLname.begin(), strLname.end(), '\r'), strLname.cend());
			strLname.erase(std::remove(strLname.begin(), strLname.end(), '\n'), strLname.cend());
			strname.second = strLname;
		}
	} while (false);
	return strname;
}

_dob CPersonInfoGen::GenerateRandomDOB(int stYear, int endyear, int year, int month, int day)
{
	_dob dob;
	int min = 1;
	int max = 12;
	
	if(0 == month)
	{
		dob.m_nMonth = generateRandomNumber((long)min, (long)max);
	  //dob.m_nMonth = min + rand() % ((max + 1) - min);
	}
	else
	{
	  dob.m_nMonth = month;
	}
	
	if(0 == day)
	{
		dob.m_nDay = generateRandomNumber((long)min, (long)max);
	  //dob.m_nDay = min + rand() % ((max + 1) - min);
	}
	else
	{
	  dob.m_nDay = day;
	}
	
	if(0 == year)
	{
	  if( ((stYear>0) && (endyear>0)) && (endyear>stYear))
	  {
	    min = stYear;
  	    max = endyear;
	  }
	  else
	  {
	    min = 1900;
	    max = 2023;
	  }
	  dob.m_nYear = generateRandomNumber((long)min, (long)max);
	  //dob.m_nYear = min + rand() % ((max + 1) - min);
	}
	else
	{
	  dob.m_nYear = year;
	}
	
	return dob;
}

std::string CPersonInfoGen::GenerateRandomSSN()
{
	const int maxdigit = 10;
	std::string outSSN = "";
	int i = 0;
	const int min = 0;
	const int max = 9;
	std::string tempstr;
	for (int k = 1; k < maxdigit; ++k) {
		i = generateRandomNumber((long)min, (long)max);
		//i = min + rand() % ((max + 1) - min);	
		tempstr = std::to_string(i);
		outSSN = outSSN + tempstr;
	}
	//std::cout<<outSSN<<",";
	return outSSN;
}

std::string CPersonInfoGen::GenerateRandomEmail(std::string Fname, std::string Lname)
{
	const int min = 0;
	const int max = 25;
	std::string outEmail = "aaa.bbb@gmail.com";
	long rindx = generateRandomNumber((long)min, (long)max);
	//int rindx = min + rand() % ((max + 1) - min);
	std::string tempstr = m_vLetters[rindx] + std::to_string(rindx+2);
	const std::string emailPrvds[8] = { "Gmail.com","Outlook.com","ProtonMail.com","AOL.com","zohomail.com","icloud.com","yahoo.com","GMX.com" };
	const int min1 = 0;
	const int max1 = 7;

	long rindx1 = generateRandomNumber((long)min, (long)max);
	//int rindx1 = min1 + rand() % ((max1 + 1) - min1);
	std::string temprvdr = emailPrvds[rindx1];
	
	outEmail = Fname + "." + Lname + "." + tempstr + "@" + temprvdr;

	return outEmail;
}



std::string CPersonInfoGen::GenerateRandomAddress(long areainHector, double longi, double lati)
{
	const double approx_lat_to_kilometer = 111;//each deg = 111 km
	double dSqKmradius = areainHector / 100.0;
	int randn = generateRandomNumber(1, 7632);
	const double fMin = 0.0;
	const double fMax = approx_lat_to_kilometer/ dSqKmradius;

	double f = (double)randn / RAND_MAX;
	double frand = fMin + f * (fMax - fMin);
	double random_distance = frand * dSqKmradius;
	double random_bearing = frand * 360;
	
	double flong = longi + (random_distance / approx_lat_to_kilometer);
	double flong1 = lati + (random_distance / approx_lat_to_kilometer);
	std::string ret = "Long: " + std::to_string(flong) + " Lat: " + std::to_string(flong1);
	return ret;
}



Graduation_ CPersonInfoGen::GenerateRandomEducation()
{
	const int min = (int)GRAD_TYPE_NONE;
	const int max = (int)GRAD_TYPE_DOCTORAL;
	long i = generateRandomNumber((long)min, (long)max);
	//int i = min + rand() % ((max + 1) - min);

	return (Graduation_)i;
}

MaritalSatus_ CPersonInfoGen::GenerateRandomMSts()
{
	const int min = (int)MARITAL_STATUS_NEVER_MARRIED;
	const int max = (int)MARITAL_STATUS_SEPERATED;
	long i = generateRandomNumber((long)min, (long)max);
	//int i = min + rand() % ((max + 1) - min);
	return (MaritalSatus_)i;
}


void CPersonInfoGen::ReadFirstNamesDBF(std::string filepath)
{
	do {
		std::fstream myfile;

		// open file
		myfile.open(filepath);

		if (myfile.is_open()) {
			// checking if the file is open
			std::string str;

			// read data from file object
			// and put it into string.
			while (getline(myfile, str)) {
				m_vFirstNames_F.push_back(str);
			}
			// close the file object.
			myfile.close();
		}
	} while (false);
}

void CPersonInfoGen::ReadFirstNamesDBM(std::string filepath)
{
	std::fstream myfile;

	// open file
	myfile.open(filepath);

	if (myfile.is_open()) {
		// checking if the file is open
		std::string str;

		// read data from file object
		// and put it into string.
		while (getline(myfile, str)) {
			m_vFirstNames_M.push_back(str);
		}
		// close the file object.
		myfile.close();
	}
}

void CPersonInfoGen::ReadLastNamesDB(std::string filepath)
{
	std::fstream myfile;

	// open file
	myfile.open(filepath);

	if (myfile.is_open()) {
		// checking if the file is open
		std::string str;

		// read data from file object
		// and put it into string.
		while (getline(myfile, str)) {
			m_vLastName.push_back(str);
		}
		// close the file object.
		myfile.close();
	}
	else
	{
	   std::cout << "error opening file :"<<filepath<<std::endl;
	}
}

// Function to generate a random number within the given range [min, max]
long CPersonInfoGen::generateRandomNumber(long min, long max) const {
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
	} while (randomNumber == lastRandomNumber && currentTime == lastTimestamp);

	// Store the last generated number and timestamp
	lastRandomNumber = randomNumber;
	lastTimestamp = currentTime;

	return randomNumber;
}