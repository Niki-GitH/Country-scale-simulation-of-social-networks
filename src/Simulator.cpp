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
	CINIConfigParser::parse("./data/Simulator.ini");
	try
	{
		
		ppDB.GenerateRandomPopulationData();
		ppDB.Evolve();
		
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
