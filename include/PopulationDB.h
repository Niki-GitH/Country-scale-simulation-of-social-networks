#pragma once
#include "PersonInfo.h"
#include "PersonInfoGen.h"
#include "HMM_Career.h"
#include "HMMEvents.h"
#include <set>
#include <memory>
#include <list>
#include <tuple>
#include <chrono>

class CPopulationDB
{
public:
	std::set<std::string> m_countyNames;
	//static long		m_nTotalEvents;
private:
	long m_lTotalPopulationCnt;					//Total population count
	long m_lBasePopulationCnt;					//base_percentage of population_size is randomly generaed and then HMM is used to evolve
	long m_lEvolvePopulationCnt;				//Evolution population count
	long m_lChildrnPopulation;					//total population of children[<14] taken from ini file
	long m_lYoungPopulation;					//total population of young adults[15<age<24] taken from ini file
	long m_lWorkingPopulation;					//total population of working age[24<age<64] taken from ini file
	long m_lElderPopulation;					//total population of elders[>64] taken from ini file
	long m_lTotalMarriages;						//total population of elders[>64] taken from ini file
	std::unique_ptr<CPersonInfoGen> pInoGen;    //random info generator
	std::unique_ptr <CHMM_Career> m_csCareer;	//Hidden markov model to predict carrier options 
	std::unique_ptr <CHMMEvents> m_cslifeChoice;//Hidden markov model to predict life choices.[Observations:Age,Income]
	std::vector<_PopulationData> m_populationDB;//Population statistics according to city
	std::vector<CPersonInfo> m_lmalePopln;		//Total male population
	std::vector<CPersonInfo> m_lfemalePopln;	//Total female population
	std::vector<CPersonInfo> m_lChildPopln;		//Total childrens population

	//Store parent child tuple<true=male,Male/Female vector index, true=mature, true = child gender male, mature/Child vector index>
	std::vector<std::tuple<bool, size_t, bool, bool, size_t>> m_parentChild;
	//Store parent child tuple<true=mature[>18 years of age], true=male,Male/Female vector index, true=male, Male/Female vector index>
	std::vector<std::tuple<bool, bool, size_t, bool, size_t>> m_Siblings;
	//Store parent child tuple<true=male,Male/Female vector index, true=male, Male/Female vector index,EdgeType>
	std::vector<std::tuple<bool, size_t, bool, size_t, EdgeInfo_>> m_edges; //for now used only for marriages
	
	mutable long lastRandomNumber_ = -1;
	mutable std::chrono::system_clock::time_point lastTimestamp_ = std::chrono::system_clock::now();
private:
	CPopulationDB();
	//
	void ReadPopulationDBFile(std::string filepath);
	//
	void split(std::string str, char del, std::vector < std::string >& out);
	//get index of male partner, returns true if found.
	bool getRandomMaleForMarriage(int& retIndex);
	//get index of female partner, returns true if found.
	bool getRandomFemaleForMarriage(int& retIndex);
	//get random male and female parent, return true if found
	bool getRandomParent(int& retMale, int& retFemale,bool bskipCond = false);
	//get random male, return true if found
	bool getRandomMaleParent(size_t maxsize, int& retMale);
	//get random female, return true if found
	bool getRandomFeMaleParent(size_t maxsize, int& retFeMale);
	//
	long generateRandomNumber_(long min, long max) const;
public:
	//
	CPopulationDB(int popCount);
	//
	~CPopulationDB();
	//
	CPopulationDB& operator=(const CPopulationDB& copyObj);
	//
	void mergeCPopulationDB(const CPopulationDB& copyObj);
	///
	void GenerateRandomPopulationData();
	
	//Writes generated population data to csv file
	void WritePopulationDataToFile(std::string filename);
	
	
	/*
	randomly choose male and female whose status is single and change status.
	Change spouse name for both
	In     : count number of changes
	return : void
	*/	
	void makeRandomEventMarriage(int ncount);
	void makeRandomEventMarriage(CPersonInfo& partner);

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
	bool assignChildtoRandomFamily(CPersonInfo& child);
	void makeRandomEventChild(int count, CPersonInfo& parentM, CPersonInfo& parentF);

	/*
	From existing data create a edges table.
	In     : none
	return : void
	
	generate edges info in below format.
	conn_from: Row index in population_database.csv file
	conn_to : Row index in population_database.csv file
	edge_type : Any one of these values['grand child', 'aunt', 'uncle', 'grand father', 'grand mother', 'sister', 'brother', 'nephew', 'niece', 'husband', 'wife', 'father', 'mother']
	*/
	void generate_family_tree();
	/*
	generate edges info in below format.
	conn_from: UniqueID of person 1
	conn_to : UniqueID of person 2
	edge_type : Any one of these values['grand child', 'aunt', 'uncle', 'grand father', 'grand mother', 'sister', 'brother', 'nephew', 'niece', 'husband', 'wife', 'father', 'mother']
	*/
	void generate_family_tree_new();
	

	void Evolve();

	inline size_t GetPopCount() { return (m_lmalePopln.size()+m_lfemalePopln.size()+m_lChildPopln.size());}

	template<class T>
	std::string to_string_(const T & t);

};

