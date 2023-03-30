#pragma once
#include <string>
#include <vector>

typedef enum enEvent
{
	EVENT_TYPE_START = -1,
	EVENT_TYPE_GRADUATION=0,
	EVENT_TYPE_NEW_CHILD,
	EVENT_TYPE_MARRIAGE,
	EVENT_TYPE_HOME_PURCHASE,
	EVENT_TYPE_NEW_JOB,
	EVENT_TYPE_DEATH,
	EVENT_TYPE_END,
}EventType_;


typedef enum enMaritalSts
{
	MARITAL_STATUS_SINGLE = 0,
	MARITAL_STATUS_MARRIED,
	MARITAL_STATUS_WIDOWED,
	MARITAL_STATUS_DIVORCED,
	MARITAL_STATUS_SEPERATED,
}MaritalSatus_;


typedef enum enGraduation
{
	GRAD_TYPE_NONE = 0,
	GRAD_TYPE_SENIOR_HIGH,
	GRAD_TYPE_DIPLOMA,
	GRAD_TYPE_ADVANCED_DIPLOMA,
	GRAD_TYPE_BACHELOR,
	GRAD_TYPE_GRADUATE,
	GRAD_TYPE_MASTER,
	GRAD_TYPE_DOCTORAL,
}Graduation_;



typedef enum enFinancialSts
{
	FINANCE_STATUS_POOR = 0,
	FINANCE_STATUS_LOWER_MIDDLE,
	FINANCE_STATUS_MIDDLE,
	FINANCE_STATUS_UPPER_MIDDLE,
	FINANCE_STATUS_RICH,
	FINANCE_STATUS_ULTRA_RICH,
	FINANCE_STATUS_END
}FinancialSatus_;

typedef enum CareerState {
	STUDYING = 0,
	WORKING,
	CAREER_STATE_END
}CareerState_;


enum FinacialObsSymbol {
	LOW,
	MEDIUM,
	HIGH,
	FINCE_OBSV_END
};

enum AgeObsSymbol {
	CHILD,
	YOUNG,
	WORKINGAGE,
	ELDER,
	AGE_OBSV_END
};


typedef enum enGenderInfo
{
	GENDER_TYPE_MALE = 0,
	GENDER_TYPE_FEMALE,
	GENDER_TYPE_TRANS,
	EVENT_INFO_END,
}GenderInfo_;

typedef enum enEMailProvider
{
	EMAIL_SITE_GMAIL = 0,
	EMAIL_SITE_OUTLOOK,
	EMAIL_SITE_PROTONMAIL,
	EMAIL_SITE_AOL,
	EMAIL_SITE_ZOHOMAIL,
	EMAIL_SITE_ICLOUD,
	EMAIL_SITE_YAHOO,
	EMAIL_SITE_END,
}EmailProvider_;



typedef enum enEdgeInfo
{
	EDGE_TYPE_PARTNER = 0,
	EDGE_TYPE_PARENT,
	EDGE_TYPE_GRANDPARENT,
	EDGE_TYPE_CHILD,
	EDGE_TYPE_GRANDCHILD,
	EDGE_TYPE_SIBLING,
	EDGE_TYPE_AUNT,
	EDGE_TYPE_UNCLE,
}EdgeInfo_;


struct PersonEvent
{
	EventType_ m_nEventType;
	std::string m_strEventName;
	std::string m_strLocation;
	std::string m_strDate;
	std::string m_strEventComments;
	PersonEvent() :m_nEventType(EVENT_TYPE_START),
		m_strEventName(""), m_strLocation(""), m_strDate(""),
		m_strEventComments("")
	{
	}

	inline PersonEvent& operator=(const PersonEvent& copyObj) {
		if (&copyObj != this) {
			m_nEventType = copyObj.m_nEventType;
			m_strEventName = copyObj.m_strEventName;
			m_strLocation = copyObj.m_strLocation;
			m_strDate = copyObj.m_strDate;
			m_strEventComments = copyObj.m_strEventComments;
		}
		return *this;
	}
};


struct _dob
{
	std::int8_t	m_nDay;
	std::int8_t	m_nMonth;
	int		m_nYear;
	_dob():
	m_nDay(1), m_nMonth(1),m_nYear(1900)
	{
	}
	inline _dob& operator=(const _dob& copyObj) {
		if (&copyObj != this) {
			m_nDay = copyObj.m_nDay;
			m_nMonth = copyObj.m_nMonth;
			m_nYear = copyObj.m_nYear;
		}
		return *this;
	}
};



struct _PopulationData
{
	std::string	m_strCountry;
	std::string	m_strPrefec;
	std::string	m_strSubPrefec;
	std::string	m_strLocalitycode;
	std::string	m_strLocalityName;
	long m_nPopulation;
	long m_nArea; // in hectors
	double m_longitude;
	double m_latitude;
	const size_t nColms = 9; //number of columns
	_PopulationData() :
		m_strCountry("sweden"), 
		m_strPrefec("stockholm"), 
		m_strSubPrefec("Varmdo"),
		m_strLocalitycode("000"),
		m_strLocalityName("akersberga"),
		m_nPopulation(0),
		m_nArea(0),
		m_longitude(0.0),
		m_latitude(0.0)
	{
	}
	inline _PopulationData& operator=(const _PopulationData& copyObj) {
		if (&copyObj != this) {		
			m_strCountry = copyObj.m_strCountry;
			m_strPrefec = copyObj.m_strPrefec;
			m_strSubPrefec = copyObj.m_strSubPrefec;
			m_strLocalitycode = copyObj.m_strLocalitycode;
			m_strLocalityName = copyObj.m_strLocalityName;
			m_nPopulation = copyObj.m_nPopulation;
			m_nArea = copyObj.m_nArea;
			m_longitude = copyObj.m_longitude;
			m_latitude = copyObj.m_latitude;
		}
		return *this;
	}
};
