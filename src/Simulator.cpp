#include <iostream>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <thread>
//#include <unistd.h>
//#include <fstream>
#include "../include/PopulationDB.h"
#include "../include/TextFileLogger.h"

#define MAX_DATA_PER_THREAD	10000

int main() {
	CINIConfigParser::parse("./data/Simulator.ini");
	CTextFileLogger::GetLogger()->Log("************ Start application ************");
	const unsigned short totalthreads = 6;//4
	long popcount = CINIConfigParser::GetIntValue(SECTION_POP, KEY_POP_COUNT);
	long countperthread = static_cast<long>(popcount / (totalthreads));
	CUtil::m_nTotalEvents = 0;
	long totalth = (countperthread*totalthreads);
	long temp = (popcount - totalth);
	//CTextFileLogger::GetLogger()->Log("popcount= %d, totalth=%d, temp=%d ************",popcount,totalth,temp);
	//CPopulationDB ppDB_main(countperthread + temp);
	
	int totalthreads_n = popcount / MAX_DATA_PER_THREAD;
	countperthread = MAX_DATA_PER_THREAD;
	if (totalthreads_n < 1)
	{
		totalthreads_n = 1;
		countperthread = popcount;
	}
	try
	{
		
#if 1
		CTextFileLogger::GetLogger()->Log("************ Start generating data ************");
		countperthread = static_cast<long>(popcount / (totalthreads));
		CPopulationDB ppDB_t1(countperthread);
		CPopulationDB ppDB_t2(countperthread);
		CPopulationDB ppDB_t3(countperthread);
		CPopulationDB ppDB_t4(countperthread);
		CPopulationDB ppDB_t5(countperthread);
		CPopulationDB ppDB_t6(countperthread);
		{
			std::thread t1(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t1);
			std::thread t2(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t2);
			std::thread t3(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t3);
			std::thread t4(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t4);
			std::thread t5(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t5);
			std::thread t6(&CPopulationDB::GenerateRandomPopulationData, &ppDB_t6);
			
			t1.join();
			t2.join();
			t3.join();
			t4.join();
			t5.join();
			t6.join();
		}

		CTextFileLogger::GetLogger()->Log("********** Generating population data complete");

		//Write to file
		
		CPopulationDB ppDB_main(0);
		ppDB_main.mergeCPopulationDB(ppDB_t1);
		ppDB_main.mergeCPopulationDB(ppDB_t2);
		ppDB_main.mergeCPopulationDB(ppDB_t3);
		ppDB_main.mergeCPopulationDB(ppDB_t4);
		ppDB_main.mergeCPopulationDB(ppDB_t5);
		ppDB_main.mergeCPopulationDB(ppDB_t6);
#if 1
		CTextFileLogger::GetLogger()->Log("************ Start making connections");
		ppDB_main.Evolve();
		CTextFileLogger::GetLogger()->Log("********** Evolving population data complete");
#endif
		CTextFileLogger::GetLogger()->Log("********** Start writing data to csv");
		ppDB_main.WritePopulationDataToFile("population_database.csv");
		CTextFileLogger::GetLogger()->Log("********** writing data to file complete");
		ppDB_main.generate_family_tree_new();
#else
		CTextFileLogger::GetLogger()->Log("************ Start generating data ************");
		std::vector<CPopulationDB*> dbs;
		for (int i = 0; i < totalthreads_n; i++)
		{
			CPopulationDB* ppDB_t1 = new CPopulationDB(countperthread);
			dbs.push_back(ppDB_t1);
		}

		std::vector<std::thread*> threads_v;
		for (int i = 0; i < totalthreads_n; i++)
		{
			std::thread* t1 = new std::thread(&CPopulationDB::GenerateRandomPopulationData, dbs[i]);
			threads_v.push_back(t1);
		}

		for (int i = 0; i < totalthreads_n; i++)
		{
			threads_v[i]->join();
		}
		CTextFileLogger::GetLogger()->Log("********** Generating population data complete");


		//Write to file
		CTextFileLogger::GetLogger()->Log("********** Start writing data to csv");
		CPopulationDB ppDB_main(countperthread);
		for (int i = 0; i < totalthreads_n; i++)
		{
			ppDB_main = *dbs[i];
		}
		ppDB_main.WritePopulationDataToFile();
		CTextFileLogger::GetLogger()->Log("********** writing data to file complete");

#endif
		//generate parent,child,sibling edges
		//ppDB_main.generate_family_tree();
		CTextFileLogger::GetLogger()->Log("********** generating family tree complete");		
		//print connections
		//ppDB_main.print_family_edges();
		CTextFileLogger::GetLogger()->Log("Generation complete. Generated population size -> %ld", ppDB_main.GetPopCount());
		std::cout << "Generation complete. Generated population size -> " << ppDB_main.GetPopCount() << std::endl;
	}
	catch (const std::out_of_range& oor)
	{
		std::cout << "Out of Range error: " << oor.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown error: " << std::endl;
	}
	
	return 0;
}

