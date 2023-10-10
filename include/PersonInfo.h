#pragma once
#include "defines.h"
#include "INIConfigParser.h"
#include <string>
#include <vector>
#include <stdarg.h>

class CPersonInfo
{
private:
	// attributes
	std::string m_uniqueID;
	bool m_bHasChildren;
	bool m_bIsEmployed;
	std::int8_t m_nDOB_Day;
	std::int8_t m_nDOB_Month;
	int m_nDOB_Year;
	int m_nChildCount;
	GenderInfo_ m_nGender;
	MaritalSatus_ m_nMaritalStatus;
	Graduation_ m_nGraduation;
	std::string m_nSSN;
	std::string m_strFirstName;
	std::string m_strMiddleName;
	std::string m_strLastName;
	std::string m_strFatherName;
	std::string m_strMotherName;
	std::string m_strSpouseName;
	std::string m_strHomeAddress;
	std::string m_strWorkAddress;
	std::vector<std::string> m_listChildNames;
	std::vector<PersonEvent> m_listEvents;
	std::string m_strDateofDeath;
	_dob m_stDeathDate;
	FinancialSatus_ m_stFinancialStatus;
	int m_nAge;
	double m_dIncome;
	std::vector<iniAddress> m_schoolsattended;
	std::vector<iniAddress> m_OfficePlaces;
	// childerns tuple format: mature[>18], gendertype true=male, index of respective list
	// std::vector<std::tuple<bool,bool, long>> m_vChildren;
	std::vector<ChildInfo> m_vChildren;
	std::vector<long> m_vSpouses;

public:
	unsigned short m_nGeneration;
	size_t m_nListIndex;	 // store the index for faster search;
	long m_nListFatherIndex; // store the index of one parent for faster search;
	long m_nListMotherIndex; // store the index of one parent for faster search;
private:
	// Constructor
	CPersonInfo();

	//
	void UpdateGraduationDetatils(std::string GraduationType);

	//
	void UpdateFamilyDetatils(std::string childName, CPersonInfo &othrPrsn);

	//
	void UpdateMaritalDetatils(std::string spouseName, CPersonInfo &othrPrsn);

	// format string
	const std::string vformat(const char *const zcFormat, ...);

public:
	// Functions

	// Parameter constructor
	CPersonInfo(std::string FirstName, std::string Lastname, _dob stdob, GenderInfo_ gender, std::string Address);

	// Destructor
	~CPersonInfo(void);

	// Copy Constructor
	CPersonInfo(const CPersonInfo &copyObj);

	// Operator = overload
	CPersonInfo &operator=(const CPersonInfo &copyObj);

	// comparision operator overload
	inline bool operator!=(const CPersonInfo &copyObj)
	{
		bool bRet = false;
		if ((m_strFirstName != copyObj.m_strFirstName) && (m_nSSN != copyObj.m_nSSN))
		{
			bRet = true;
		}
		return bRet;
	}

	// comparision operator overload
	inline bool operator==(const CPersonInfo &copyObj)
	{
		bool bRet = false;
		if ((m_strFirstName == copyObj.m_strFirstName) && (m_nSSN == copyObj.m_nSSN))
		{
			bRet = true;
		}
		return bRet;
	}

	void Dispose();

	// add new event to person's life
	void AddEvent(const PersonEvent &newEvent, CPersonInfo &othrPrsn);

	// Return a formated string to write in CSV
	const std::string GetFormatedString();

	inline std::string getUniqueID()
	{
		return m_uniqueID;
	}

	inline void setUniqueID(std::string m_strID)
	{
		m_uniqueID = m_strID;
	}

	inline bool getHasChildren()
	{
		return m_bHasChildren;
	}

	inline void setHasChildren(bool m_bHasChildren_)
	{
		m_bHasChildren = m_bHasChildren_;
	}

	inline bool getIsEmployed()
	{
		return m_bIsEmployed;
	}

	inline void setIsEmployed(bool m_bIsEmployed_)
	{
		m_bIsEmployed = m_bIsEmployed_;
	}

	inline std::int8_t getDOB_Day()
	{
		return m_nDOB_Day;
	}

	inline void setDOB_Day(std::int8_t m_nDOB_Day_)
	{
		m_nDOB_Day = m_nDOB_Day_;
	}

	inline std::int8_t getDOB_Month()
	{
		return m_nDOB_Month;
	}

	inline void setDOB_Month(std::int8_t m_nDOB_Month_)
	{
		m_nDOB_Month = m_nDOB_Month_;
	}

	inline int getDOB_Year()
	{
		return m_nDOB_Year;
	}

	inline void setDOB_Year(int m_nDOB_Year_)
	{
		m_nDOB_Year = m_nDOB_Year_;
	}

	inline _dob getDOB()
	{
		_dob dob;
		dob.m_nDay = m_nDOB_Day;
		dob.m_nMonth = m_nDOB_Month;
		dob.m_nYear = m_nDOB_Year;
		return dob;
	}

	inline void setDOB_Year(_dob &mDOB)
	{
		m_nDOB_Day = mDOB.m_nDay;
		m_nDOB_Month = mDOB.m_nMonth;
		m_nDOB_Year = mDOB.m_nYear;
	}

	std::string getDOBstr();

	inline void setDOD(_dob &mdod)
	{
		m_stDeathDate = mdod;
	}
	std::string getDODstr();

	inline std::string getSSN()
	{
		return m_nSSN;
	}

	inline void setSSN(std::string m_nSSN_)
	{
		m_nSSN = m_nSSN_;
		m_uniqueID = m_nSSN + m_strFirstName;
	}

	inline int getChildCount()
	{
		// return m_nChildCount;
		return static_cast<int>(m_vChildren.size());
	}

	inline void setChildCount(int m_nChildCount_)
	{
		m_nChildCount = m_nChildCount_;
	}

	inline GenderInfo_ getGender()
	{
		return m_nGender;
	}

	const std::string getEducationStr();

	const std::string getMStsStr();

	const std::string getGenderStr();

	inline void setGender(GenderInfo_ m_nGender_)
	{
		m_nGender = m_nGender_;
	}

	inline MaritalSatus_ getMaritalStatus()
	{
		return m_nMaritalStatus;
	}

	inline void setMaritalStatus(MaritalSatus_ m_nMaritalStatus_)
	{
		m_nMaritalStatus = m_nMaritalStatus_;
	}

	inline Graduation_ getGraduation()
	{
		return m_nGraduation;
	}

	inline void setGraduation(Graduation_ m_nGraduation_)
	{
		m_nGraduation = m_nGraduation_;
	}

	inline std::string getFirstName()
	{
		return m_strFirstName;
	}

	inline void setFirstName(std::string m_strFirstName_)
	{
		m_strFirstName = m_strFirstName_;
	}

	inline std::string getMiddleName()
	{
		return m_strMiddleName;
	}

	inline void setMiddleName(std::string m_strMiddleName_)
	{
		m_strMiddleName = m_strMiddleName_;
	}

	inline std::string getLastName()
	{
		return m_strLastName;
	}

	inline void setLastName(std::string m_strLastName_)
	{
		m_strLastName = m_strLastName_;
	}

	inline std::string getFullName()
	{
		return (m_strFirstName + " " + m_strMiddleName + " " + m_strLastName);
	}

	inline std::string getFatherName()
	{
		return m_strFatherName;
	}

	inline void setFatherName(std::string m_strFatherName_)
	{
		m_strFatherName = m_strFatherName_;
	}

	inline std::string getMotherName()
	{
		return m_strMotherName;
	}

	inline void setMotherName(std::string m_strMotherName_)
	{
		m_strMotherName = m_strMotherName_;
	}

	inline std::string getSpouseName()
	{
		return m_strSpouseName;
	}

	inline std::string getHomeAddress()
	{
		return m_strHomeAddress;
	}

	inline void setHomeAddress(std::string m_strHomeAddress_)
	{
		m_strHomeAddress = m_strHomeAddress_;
	}

	inline std::string getWorkAddress()
	{
		return m_strWorkAddress;
	}

	inline void setWorkAddress(std::string m_strWorkAddress_)
	{
		m_strWorkAddress = m_strWorkAddress_;
	}

	inline void setFinancialSts(FinancialSatus_ stVal_)
	{
		m_stFinancialStatus = stVal_;
		if (FINANCE_STATUS_LOWER_MIDDLE == stVal_)
		{
			m_dIncome = 244000.0;
		}
		else if (FINANCE_STATUS_MIDDLE == stVal_)
		{
			m_dIncome = 290000.0;
		}
		else if (FINANCE_STATUS_UPPER_MIDDLE == stVal_)
		{
			m_dIncome = 344000.0;
		}
		else if (FINANCE_STATUS_RICH == stVal_)
		{
			m_dIncome = 1004000.0;
		}
		else if (FINANCE_STATUS_ULTRA_RICH == stVal_)
		{
			m_dIncome = 5004000.0;
		}
		else
		{
			m_dIncome = 144000.0;
		}
	}

	inline FinancialSatus_ getFinancialSts()
	{
		return m_stFinancialStatus;
	}

	inline std::string getFinancialSts_()
	{
		if (FINANCE_STATUS_LOWER_MIDDLE == m_stFinancialStatus)
		{
			return "LOWER_MIDDLE";
		}
		else if (FINANCE_STATUS_MIDDLE == m_stFinancialStatus)
		{
			return "MIDDLE";
		}
		else if (FINANCE_STATUS_UPPER_MIDDLE == m_stFinancialStatus)
		{
			return "UPPER_MIDDLE";
		}
		else if (FINANCE_STATUS_RICH == m_stFinancialStatus)
		{
			return "RICH";
		}
		else if (FINANCE_STATUS_ULTRA_RICH == m_stFinancialStatus)
		{
			return "ULTRA_RICH";
		}
		else
		{
			return "POOR";
		}
	}

	inline int getAge()
	{
		return m_nAge;
	}

	inline void setAge(int nage)
	{
		m_nAge = nage;
	}

	inline double getIncome()
	{
		return m_dIncome;
	}

	inline void setIncome(double dval)
	{
		m_dIncome = dval;
	}

	size_t getSpouseIndex()
	{
		if (m_vSpouses.size() == 0)
		{
			return -1;
		}
		return m_vSpouses[m_vSpouses.size() - 1];
	}

	int getNumberofMarriages()
	{
		return static_cast<int>(m_vSpouses.size());
	}

	void addSpouse(long spouseIndex)
	{
		m_vSpouses.push_back(spouseIndex);
	}

	void addChild(CPersonInfo &child, long index)
	{
		bool bmature = (child.getAge() > 18) ? true : false;
		bool bGender = (child.getGender() == GENDER_TYPE_MALE) ? true : false;
		// m_vChildren.push_back(std::make_tuple(bmature, bGender, index));
		m_vChildren.push_back(ChildInfo(bmature, bGender, index));
		m_listChildNames.push_back(child.getFullName());
	}

	inline size_t getListIndex()
	{
		return m_nListIndex;
	}

	inline void setListIndex(size_t index)
	{
		m_nListIndex = index;
	}

	inline void setSchoolName(std::string schoolName, std::string address)
	{
		m_schoolsattended.push_back(iniAddress(schoolName, address));
	}

	inline iniAddress getSchoolName()
	{
		iniAddress addr;
		size_t size1 = m_schoolsattended.size();
		if (size1 != 0)
		{
			addr.name = m_schoolsattended[size1 - 1].name;
			addr.address = m_schoolsattended[size1 - 1].address;
		}
		return addr;
	}

	inline void setOfficeName(std::string officeName, std::string address)
	{
		m_OfficePlaces.push_back(iniAddress(officeName, address));
		m_strWorkAddress = address;
	}

	inline iniAddress getOfficeName()
	{
		iniAddress addr;
		size_t size1 = m_OfficePlaces.size();
		if (size1 != 0)
		{
			addr.name = m_OfficePlaces[size1 - 1].name;
			addr.address = m_OfficePlaces[size1 - 1].address;
		}
		return addr;
	}
	auto children_begin() { return m_vChildren.begin(); }
	auto children_end() { return m_vChildren.end(); }

	auto spouse_begin() { return m_vSpouses.begin(); }
	auto spouse_end() { return m_vSpouses.end(); }
};
