#include <iostream>
#include <ctime>
#include <iomanip>
#include <cstdlib>
//#include <unistd.h>
//#include <fstream>
#include "../include/PopulationDB.h"
#include "../include/TextFileLogger.h"

int main() {
	CTextFileLogger::GetLogger()->Log("************ Start application ************");
	CPopulationDB ppDB;
	CTextFileLogger::GetLogger()->Log("Generating population for Sweden");
	std::cout << "Generating population for Sweden"<<std::endl;
	std::string county = "uppsala";
	std::cout << "Type a County name, below are options "<<std::endl;
	CTextFileLogger::GetLogger()->Log("Type a County name, below are options");
	for(auto x: ppDB.m_countyNames)
	{
	   std::cout << x << std::endl;
	   CTextFileLogger::GetLogger()->Log(x);
	}
	std::cout << std::endl;
	std::getline(std::cin, county);// 
	
	std::cout <<"selected -> "<<county<< std::endl;
	CTextFileLogger::GetLogger()->Log("selected -> "+county);
	try
	{
		//make random population
		//ppDB.MakeRandomPopulationData("sweden", county);
		//make random marriages and child
		//ppDB.makeRandomEventMarriage(0,0,0);
		//make random deaths
		//ppDB.makeRandomEventDeaths();
		
		ppDB.MakePopulationData("sweden", 0, county);
		ppDB.Evolve(50);
		
		//Write to file
		ppDB.WritePopulationDataToFile();
		
		//generate parent,child,sibling edges
		//ppDB.generate_family_tree();
		
		//print connections
		//ppDB.print_family_edges();
		
		
	}
	catch (const std::out_of_range& oor)
	{
		std::cout << "Out of Range error: " << oor.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown error: " << std::endl;
	}
	std::cout << "Generation complete. Generated -> " << ppDB.GetPopCount() << " Persons data. Check persons.csv file." << std::endl;
	return 0;
}
