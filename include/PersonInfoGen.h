#pragma once
#include "defines.h"
#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include <chrono>

class CPersonInfoGen
{

private:
	std::vector<std::string> m_vFirstNames_F; // Female first names
	std::vector<std::string> m_vFirstNames_M; // Male first names
	std::vector<std::string> m_vLastName;	  // last names
	std::vector<std::string> m_vLetters;
	mutable long lastRandomNumber = -1;
	mutable std::chrono::system_clock::time_point lastTimestamp = std::chrono::system_clock::now();
	

private:
	void ReadFirstNamesDBF(std::string filepath);
	void ReadFirstNamesDBM(std::string filepath);
	void ReadLastNamesDB(std::string filepath);
	long generateRandomNumber(long min, long max) const;

public:
	CPersonInfoGen(std::string data_folder_path);
	~CPersonInfoGen();
	/*
	Generates a random name
	In     : gender -> gender type
	return : pair<FirstName, Last Name>
	*/
	std::pair<std::string, std::string> GenerateRandomName(GenderInfo_ gender) const;

	/*
	Generates a random D.O.B
	In     : stYear -> For random generation, start year
	In     : endyear -> For random generation, end year
	In     : year -> Fixed year
	In     : month -> Fixed month
	In     : day -> Fixed day
	return : D.O.B
	*/
	_dob GenerateRandomDOB(int stYear, int endyear, int year = 0, int month = 0, int day = 0);

	/*
	Generates a random SSN
	In     : None
	return : std::string -> a 10 digit format
	*/
	std::string GenerateRandomSSN();

	/*
	Generates a random email address
	In     : First name
	In     : last name
	return : std::string -> email
	*/
	std::string GenerateRandomEmail(std::string Fname, std::string Lname);

	/*
	Generates a random location Lat,Lan in string format
	In     : areainHector
	In     : longi/lati longitude/latitude to be in center
	return : std::string ->
	*/
	std::string GenerateRandomAddress(long areainHector, double longi, double lati);

	/*
	Generates a random education level
	In     : None
	return : Graduation_ ->
	*/
	Graduation_ GenerateRandomEducation();

	/*
	Generates a random marital status
	In     : None
	return : MaritalSatus_ ->
	*/
	MaritalSatus_ GenerateRandomMSts();
};
