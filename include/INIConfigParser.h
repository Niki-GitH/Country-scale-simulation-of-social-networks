#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <regex>
#include <map>
#include <mutex>
#include "defines.h"

#define SECTION_POP "Population"
#define SECTION_LOG "log"
#define SECTION_GNDRD "GenderDistribution"
#define SECTION_AD "AgeDistribution"
#define SECTION_ED "EthnicityDistribution"
#define SECTION_EDUCTND "EducationDistribution"
#define SECTION_EMPD "EmploymentDistribution"
#define SECTION_OD "OccupationDistribution"
#define SECTION_INCOMED "IncomeDistribution"
#define SECTION_SCHOOLS "schools"
#define SECTION_UNIS "universities"
#define SECTION_OFFICES "Workplaces"
#define SECTION_MARRIAGES "Marriages"

#define KEY_LOG_FOLDER "output_folder"
#define KEY_POP_COUNT "population_size"
#define KEY_MIN_AGE "min_age"
#define KEY_MAX_AGE "max_age"
#define KEY_BASE_RATE "base_percentage"
#define KEY_BIRTH_RATE "birth_rate"
#define KEY_DEATH_RATE "crude_death_rate"
#define KEY_FNAME_F "names_db_female"
#define KEY_FNAME_M "names_db_male"
#define KEY_LNAME "names_db_lname"
#define KEY_HMM_EVENT "event_predict_model_name"
#define KEY_MALE "male"
#define KEY_FEMALE "female"
#define KEY_MEDIAN_AGE "median_age"
#define KEY_AGE_14 "age_0_14"
#define KEY_AGE_24 "age_15_24"
#define KEY_AGE_54 "age_25_54"
#define KEY_AGE_64 "age_55_64"
#define KEY_AGE_MAX "age_65_above"
#define KEY_ED_NONE "no_info"
#define KEY_ED_JUNIOR "primary_or_junior"
#define KEY_ED_JUNIOR_HIGH "JUNIOR_HIGH"
#define KEY_ED_SENIOR "SENIOR"
#define KEY_ED_SENIOR_HIGH "SENIOR_HIGH"
#define KEY_ED_DIPLOMA "DIPLOMA"
#define KEY_ED_BACHELOR_MASTER "BACHELOR_MASTER"
#define KEY_ED_PG "PG"

#define KEY_EMPD_FULLTIME "Employed_Full-Time"
#define KEY_EMPD_PARTTIME "Employed_Part-Time"
#define KEY_EMPD_SELF "Self_Employed"
#define KEY_EMPD_UNEMP "Unemployed"
#define KEY_EMPD_STDNT "Student"

#define KEY_OD_AGRICULTURE "AGRICULTURE"
#define KEY_OD_EDUCATION "EDUCATION_SECTOR"
#define KEY_OD_MEDICAL "MEDICAL_SECTOR"
#define KEY_OD_PA "PUBLIC_ADMINSTRATION"
#define KEY_OD_CONSTRUCTION "CONSTRUCTION"
#define KEY_OD_TRANSPORT "TRANSPORT"
#define KEY_OD_FANDH "MANUFACTURING_MINING_ETC"
#define KEY_OD_IT "IORMATION_TECHNOLOGY"
#define KEY_OD_TRADE "TRADE"
#define KEY_OD_FINANCE "FINANCIAL_OPERATIONS_SERVICES"
#define KEY_OD_PCS "PERSONAL_CULTURAL_SERVICES"

#define KEY_NUM_OF_SCHOOLS "num_schools"
#define KEY_NUM_OF_UNI "num_universities"
#define KEY_NUM_OF_OFFICES "num_offices"

#define KEY_AVG_MARRIAGE_RATE "average_marriage_rate"
#define KEY_AVG_DIVORCE_RATE "average_divorece_rate"

class CINIConfigParser
{
	private:
	std::map<std::string, std::map<std::string, std::string>> data;
	mutable std::mutex mutex_;
	std::string m_folder_path;
public:
	CINIConfigParser(const std::string &filename);

	long GetIntValue(const std::string &section, const std::string &key) const;

	double GetDblValue(const std::string &section, const std::string &key) const;

	std::string GetStringValue(const std::string &section, const std::string &key) const;

	iniAddress GetOfficeAddress_(int index);

	iniAddress GetSchoolNameNAddress_(int index);

	iniAddress GetUnivstyNameNAddress_(int index);

	std::string getFolderPath();

	void setFolderPath(std::string path);

	long GetNumOfSchools() { return GetIntValue(SECTION_SCHOOLS, KEY_NUM_OF_SCHOOLS); }
	long GetNumOfUniversities() { return GetIntValue(SECTION_UNIS, KEY_NUM_OF_UNI); }
	long GetNumOfOffices() { return GetIntValue(SECTION_OFFICES, KEY_NUM_OF_OFFICES); }

	// Copy constructor
	CINIConfigParser(const CINIConfigParser &other)
	{
		std::lock_guard<std::mutex> lock(other.mutex_);
		data = other.data;
		m_folder_path = other.m_folder_path;
	}

	// Copy assignment operator
	CINIConfigParser &operator=(const CINIConfigParser &other)
	{
		if (this != &other)
		{
			std::lock_guard<std::mutex> lock(mutex_);
			std::lock_guard<std::mutex> otherLock(other.mutex_);
			data = other.data;
			m_folder_path = other.m_folder_path;
		}
		return *this;
	}

	void printMapData();

private:
	void parseLine(const std::string &line, std::string &current_section);

	// template <typename T>
	// T convertValue(const std::string& str) const;

	iniAddress parseAddress(const std::string &entry) const;

	std::string trim(const std::string &str);

	
};
