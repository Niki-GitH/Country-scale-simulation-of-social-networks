#pragma once
#include "defines.h"
#include <string>
#include <vector>
#include <iostream>
#include <utility>

class CPersonInfoGen
{

private:
	std::vector<std::string> m_vFirstNames_F; //Female first names
	std::vector<std::string> m_vFirstNames_M; //Male first names
	std::vector<std::string> m_vLastName; //last names
	const std::string m_vLetters[26] = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n",
		"o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};
	/*61.01688588478779, 12.253077071404247
		58.88192702304769, 11.582159230958942
		56.59014715040078, 12.926481759863089
		55.75749625342424, 12.926481759863089
		55.35938802570984, 13.278441483095298
		56.21687311190694, 14.744940329896174
		56.7352160432533, 16.211439176697056
		58.07797461555288, 16.710048784609352
		58.89051418593416, 16.47540896912121
		58.784272878010135, 17.296648323329705
		59.08695652390739, 18.17654763141023
		59.7435640951897, 18.997786985618724
		60.125129392197636, 18.7478312143365
		60.68132768329673, 17.28618877622352
		61.412065773635035, 17.094497964667713
		62.23791211811556, 17.45391825847285
		62.73600213471838, 17.885222584473404
		63.365811105356, 19.10725153925452
		64.96296418982145, 21.215850501071156
		65.58433796660411, 22.222227278492202
		66.1428028665398, 23.80367648579406
		66.9532958055039, 23.75575378649505
		67.71954509537312, 23.34841081313563
		68.62809807807844, 21.599232145722453
		69.05190552774822, 20.137589707609468
		68.38226493879358, 19.99382159894262
		67.98152825447922, 17.90918402327328
		67.046919738567, 16.42358021516776
		66.2877571021507, 15.41720344971321
		64.01312345902683, 13.96389200509539*/

private:
	void ReadFirstNamesDBF(std::string filepath);
	void ReadFirstNamesDBM(std::string filepath);
	void ReadLastNamesDB(std::string filepath);
public:
	CPersonInfoGen();
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
	_dob GenerateRandomDOB(int stYear, int endyear, int year=0,int month = 0, int day = 0);

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

