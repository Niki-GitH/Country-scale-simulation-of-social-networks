#pragma once
#include "PersonInfo.h"
#include "PersonInfoGen.h"
#include "HMM_Career.h"
#include "HMMEvents.h"
#include <set>
#include <memory>
#include <list>
#include <tuple>


class CPopulationDB
{
public:
	std::set<std::string> m_countyNames;
	static long		m_nTotalEvents;
private:
	//const double	m_dAvgPercentageMale = 50.32; //From SCB Summary of Population Statistics 1960–2022
	//const double	m_dAvgPercentageFeMale = 49.68; //From SCB Summary of Population Statistics 1960–2022
	//const double	m_dAvgPercentageYoung = 21.8; //average young [0-17]From SCB Summary of Population Statistics 1960–2022
	//const double	m_dAvgPercentageElders = 17.9; //average  65 and above From SCB Summary of Population Statistics 1960–2022
	//const double	m_dCrudeDeathRate = 10.2; //average deaths per 1000 From SCB Summary of Population Statistics 1960–2022
	//const double	m_dInfantMortalityRate = 4.5; //average deaths per 1000 From SCB Summary of Population Statistics 1960–2022
	//const long		m_dAvgMarriages = 33.5; //% rate From SCB Population in Sweden 31 December 2022
	//const long		m_dAvgDivorces = 9.5; //% rate  From SCB Population in Sweden 31 December 2022

	long m_lTotalPopulationCnt;
	long m_lBasePopulationCnt;
	long m_lEvolvePopulationCnt;
	std::vector< _PopulationData> m_populationDB;	
	std::vector<CPersonInfo> m_lmalePopln;
	std::vector<CPersonInfo> m_lfemalePopln;
	std::vector<CPersonInfo> m_lChildPopln;
	std::unique_ptr<CPersonInfoGen> pInoGen;
	std::list<std::tuple<CPersonInfo, CPersonInfo>> m_marriages;
	std::list<std::tuple<CPersonInfo, CPersonInfo>> m_parentChild;
	std::list<std::tuple<CPersonInfo, CPersonInfo, EdgeInfo_>> m_edges;
	std::list <std::string> m_strEdges;
	std::unique_ptr <CHMM_Career> m_csCareer;
	std::unique_ptr <CHMMEvents> m_cslifeChoice;
private:
	void ReadPopulationDBFile(std::string filepath);
	void split(std::string str, char del, std::vector < std::string >& out);
public:
	CPopulationDB();
	~CPopulationDB();

	///
	void GenerateRandomPopulationData();
	
	//Writes generated population data to csv file
	void WritePopulationDataToFile();
	
	
	/*
	randomly choose male and female whose status is single and change status.
	Change spouse name for both
	In     : count number of changes
	return : void
	*/	
	void makeRandomEventMarriage(CPersonInfo& parentM);

	/*
	randomly choose male and female whose age is above 65 and make event death.
	In     : none
	return : void
	*/
	void makeRandomEventDeaths();

	
	/*
	randomly choose person, check marital status.
	If married add a new child or make child connection to existing person.
	In     : count number of changes
	return : void
	*/	
	void makeRandomEventChild(int count, CPersonInfo& parentM, CPersonInfo& parentF);

	/*
	From existing data create a edges table.
	In     : none
	return : void
	*/
	void generate_family_tree();
	
	/*
	prints the generated data and writes it to file.
	In     : none
	return : void
	*/
	void print_family_edges();

	void Evolve(int years=10);

	inline size_t GetPopCount() { return (m_lmalePopln.size()+m_lfemalePopln.size()+m_lChildPopln.size());}
};


