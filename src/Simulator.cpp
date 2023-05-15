#include <iostream>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <thread>
//#include <unistd.h>
//#include <fstream>
#include "../include/PopulationDB.h"
#include "../include/TextFileLogger.h"


int main() {
	CINIConfigParser::parse("./data/Simulator.ini");
	CTextFileLogger::GetLogger()->Log("************ Start application ************");
	const unsigned short totalthreads = 4;//
	long popcount = CINIConfigParser::GetIntValue(SECTION_POP, KEY_POP_COUNT);
	long countperthread = static_cast<long>(popcount / (totalthreads));
	CUtil::m_nTotalEvents = 0;
	long totalth = (countperthread*totalthreads);
	long temp = (popcount - totalth);
	//CTextFileLogger::GetLogger()->Log("popcount= %d, totalth=%d, temp=%d ************",popcount,totalth,temp);
	CPopulationDB ppDB_main(countperthread + temp);
	
	try
	{
		CTextFileLogger::GetLogger()->Log("************ Start generating data ************");
		CPopulationDB ppDB_t1(countperthread);
		CPopulationDB ppDB_t2(countperthread);
		CPopulationDB ppDB_t3(countperthread);
		CPopulationDB ppDB_t4(countperthread);
		{
		std::thread t1(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t1);
		std::thread t2(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t2);
		std::thread t3(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t3);
		std::thread t4(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t4);

		//ppDB_main.GenerateRandomPopulationData();
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		}
		CTextFileLogger::GetLogger()->Log("********** Generating population data complete");

		CTextFileLogger::GetLogger()->Log("************ Start making connections");
		std::thread t4(&CPopulationDB::Evolve, &ppDB_t1);
		std::thread t5(&CPopulationDB::Evolve, &ppDB_t2);
		std::thread t6(&CPopulationDB::Evolve, &ppDB_t3);
		std::thread t7(&CPopulationDB::Evolve, &ppDB_t4);
		//ppDB_main.Evolve();
		t4.join();
		t5.join();
		t6.join();
		t7.join();	
		CTextFileLogger::GetLogger()->Log("********** Evolving population data complete");
		

		//Write to file
		CTextFileLogger::GetLogger()->Log("********** Start writing data to csv");
		ppDB_main = ppDB_t1;
		ppDB_main = ppDB_t2;
		ppDB_main = ppDB_t3;
		ppDB_main = ppDB_t4;
		
		ppDB_main.WritePopulationDataToFile();
		CTextFileLogger::GetLogger()->Log("********** writing data to file complete");
		//generate parent,child,sibling edges
		//ppDB_main.generate_family_tree();
		CTextFileLogger::GetLogger()->Log("********** generating family tree complete");		
		//print connections
		//ppDB_main.print_family_edges();
		
	}
	catch (const std::out_of_range& oor)
	{
		std::cout << "Out of Range error: " << oor.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown error: " << std::endl;
	}
	std::cout << "Generation complete. Generated -> " << ppDB_main.GetPopCount() << " Persons data. Check persons.csv file." << std::endl;
	return 0;
}
