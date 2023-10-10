#include <iostream>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <thread>
// #include <unistd.h>
// #include <fstream>
#include "../include/PopulationDB.h"
#include "../include/TextFileLogger.h"

#define MAX_DATA_PER_THREAD 10000

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		std::cout << "*****  Usage  *****" << std::endl;
		std::cout<< "./COUNTRY_SCALE_SIM arg1 arg2"<<std::endl;
		std::cout<<"arg1 : 'data' folder path where all ini file and other inputs are present.\n";
		std::cout<<"arg2 : 1 or 0. 1: Generate family tree. 0: Do not generate family tree.\n";		
		return 1;
	}

	std::string data_folder_path(argv[1]);
	
	int generate_family_tree = 0;
	generate_family_tree = std::stoi(argv[2]);

	static CINIConfigParser m_parser(data_folder_path + "/Simulator.ini");
	std::vector<iniAddress> laddress;
	long scount = m_parser.GetNumOfSchools();
	m_parser.setFolderPath(data_folder_path);
	for (long s = 0; s < scount; s++)
	{
		laddress.push_back(m_parser.GetSchoolNameNAddress_(s));
	}

	long ucount = m_parser.GetNumOfUniversities();
	for (long u = 0; u < ucount; u++)
	{
		laddress.push_back(m_parser.GetUnivstyNameNAddress_(u));
	}

	long ocount = m_parser.GetNumOfOffices();
	for (long o = 0; o < ocount; o++)
	{
		laddress.push_back(m_parser.GetOfficeAddress_(o));
	}

	CTextFileLogger::GetLogger()->Log("************ Start application ************");
	const unsigned short totalthreads = 6; // 4
	long popcount = m_parser.GetIntValue(SECTION_POP, KEY_POP_COUNT);

	if (popcount == 0)
	{
		popcount = 5000;
	}
	CTextFileLogger::GetLogger()->Log("Population count %d", popcount);
	long countperthread = static_cast<long>(popcount / (totalthreads));
	CUtil::m_nTotalEvents = 0;
	long totalth = (countperthread * totalthreads);
	long temp = (popcount - totalth);
	int totalthreads_n = popcount / MAX_DATA_PER_THREAD;
	countperthread = MAX_DATA_PER_THREAD;
	if (totalthreads_n < 1)
	{
		totalthreads_n = 1;
		countperthread = popcount;
	}

	CTextFileLogger::GetLogger()->Log("************ Start generating data ************");
	countperthread = static_cast<long>(popcount / (totalthreads));

	CPopulationDB ppDB_main(m_parser, laddress, 0);
	std::vector<std::thread> threadPool;
	{
		CPopulationDB ppDB_t1(m_parser, laddress, countperthread);
		CPopulationDB ppDB_t2(m_parser, laddress, countperthread);
		CPopulationDB ppDB_t3(m_parser, laddress, countperthread);
		CPopulationDB ppDB_t4(m_parser, laddress, countperthread);
		CPopulationDB ppDB_t5(m_parser, laddress, countperthread);
		CPopulationDB ppDB_t6(m_parser, laddress, countperthread);
		// Function to be executed by each thread
		auto threadFunction = [&ppDB_main](CPopulationDB &worker)
		{
			// Execute the DBProc function for the worker
			worker.GenerateRandomPopulationData();
		};

		// Submit worker instances to the thread pool
		threadPool.emplace_back(threadFunction, std::ref(ppDB_t1));
		threadPool.emplace_back(threadFunction, std::ref(ppDB_t2));
		threadPool.emplace_back(threadFunction, std::ref(ppDB_t3));
		threadPool.emplace_back(threadFunction, std::ref(ppDB_t4));
		threadPool.emplace_back(threadFunction, std::ref(ppDB_t5));
		threadPool.emplace_back(threadFunction, std::ref(ppDB_t6));
		// ... Submit other worker instances

		// Wait for all threads to finish
		std::for_each(threadPool.begin(), threadPool.end(), [](std::thread &t)
					  { t.join(); });

		ppDB_main.mergeCPopulationDB(ppDB_t1);
		ppDB_t1.Dispose();
		ppDB_main.mergeCPopulationDB(ppDB_t2);
		ppDB_t2.Dispose();
		ppDB_main.mergeCPopulationDB(ppDB_t3);
		ppDB_t3.Dispose();
		ppDB_main.mergeCPopulationDB(ppDB_t4);
		ppDB_t4.Dispose();
		ppDB_main.mergeCPopulationDB(ppDB_t5);
		ppDB_t5.Dispose();
		ppDB_main.mergeCPopulationDB(ppDB_t6);
		ppDB_t6.Dispose();
	}
	CTextFileLogger::GetLogger()->Log("************ Start making connections");
	ppDB_main.Evolve();
	CTextFileLogger::GetLogger()->Log("Size of the main class after %f", ppDB_main.GetTotalSizeinMb());
	CTextFileLogger::GetLogger()->Log("********** Evolving population data complete");
	CTextFileLogger::GetLogger()->Log("********** Start writing data to csv");
	ppDB_main.WritePopulationDataToFile("population_database.csv");
	if(generate_family_tree>0)
	{
		ppDB_main.generate_family_tree_new();	
	}
	CTextFileLogger::GetLogger()->Log("********** writing data to file complete");
	return 0;
}
