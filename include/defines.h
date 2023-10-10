#pragma once
#include <string>
#include <vector>

#if defined(_WIN32)
#define PLATFORM_TYPE_LINUX 0 // Windows
#elif defined(_WIN64)
#define PLATFORM_TYPE_LINUX 0 // Windows
#elif defined(__CYGWIN__) && !defined(_WIN32)
#define PLATFORM_TYPE_LINUX 0 // Windows (Cygwin POSIX under Microsoft Window)
#elif defined(__linux__)
#define PLATFORM_TYPE_LINUX 1
#else
#define PLATFORM_TYPE_LINUX 0
#endif

#define ELDERLY_DOB_ST 1940
#define ELDERLY_DOB_EN 1955
#define WORKING_DOB_ST 1966
#define WORKING_DOB_EN 2004
#define CHILD_DOB_ST 2008
#define CHILD_DOB_EN 2022
#define MAX_CHILD_PER_PERSON 5
#define MAX_MARRIAGES_PER_PERSON 4

typedef enum enEvent
{
	EVENT_TYPE_START = -1,
	EVENT_TYPE_GRADUATION = 0,
	EVENT_TYPE_NEW_CHILD,
	EVENT_TYPE_MARRIAGE,
	EVENT_TYPE_HOME_PURCHASE,
	EVENT_TYPE_NEW_JOB,
	EVENT_TYPE_DEATH,
	EVENT_TYPE_END,
} EventType_;

typedef enum enMaritalSts
{
	MARITAL_STATUS_NEVER_MARRIED = 0,
	MARITAL_STATUS_MARRIED,
	MARITAL_STATUS_WIDOWED,
	MARITAL_STATUS_DIVORCED,
	MARITAL_STATUS_SEPERATED,
} MaritalSatus_;

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
} Graduation_;

typedef enum enFinancialSts
{
	FINANCE_STATUS_POOR = 0,
	FINANCE_STATUS_LOWER_MIDDLE,
	FINANCE_STATUS_MIDDLE,
	FINANCE_STATUS_UPPER_MIDDLE,
	FINANCE_STATUS_RICH,
	FINANCE_STATUS_ULTRA_RICH,
	FINANCE_STATUS_END
} FinancialSatus_;

typedef enum CareerState
{
	STUDYING = 0,
	WORKING,
	CAREER_STATE_END
} CareerState_;

enum FinacialObsSymbol
{
	LOW,
	MEDIUM,
	HIGH,
	FINCE_OBSV_END
};

enum AgeObsSymbol
{
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
	GENDER_TYPE_OTHER,
	EVENT_INFO_END,
} GenderInfo_;

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
} EmailProvider_;

typedef enum enWorkAffiliation
{
	EDUCATION_SECTOR = 0,
	MEDICAL_SECTOR,
	PUBLIC_ADMINSTRATION,
	CONSTRUCTION,
	TRANSPORT,
	AGRICULTURE,
	FOOD_AND_HOSPITATLITY,
	MANUFACTURING_MINING_ETC,
	IORMATION_COMMUNICATION,
	TRADE,
	FINANCIAL_OPERATIONS_SERVICES,
	PERSONAL_CULTURAL_SERVICES
} WorkAffiliation;

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
} EdgeInfo_;

struct PersonEvent
{
	EventType_ m_nEventType;
	std::string m_strEventName;
	std::string m_strLocation;
	std::string m_strDate;
	std::string m_strEventComments;
	PersonEvent() : m_nEventType(EVENT_TYPE_START),
					m_strEventName(""), m_strLocation(""), m_strDate(""),
					m_strEventComments("")
	{
	}

	inline PersonEvent &operator=(const PersonEvent &copyObj)
	{
		if (&copyObj != this)
		{
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
	std::int8_t m_nDay;
	std::int8_t m_nMonth;
	int m_nYear;
	_dob() : m_nDay(1), m_nMonth(1), m_nYear(1900)
	{
	}
	inline _dob &operator=(const _dob &copyObj)
	{
		if (&copyObj != this)
		{
			m_nDay = copyObj.m_nDay;
			m_nMonth = copyObj.m_nMonth;
			m_nYear = copyObj.m_nYear;
		}
		return *this;
	}
};

typedef struct _childrenInfo
{
	bool m_bMature; // mature[>18]
	bool m_bMale;	// gendertype true=male
	long m_nIndex;	// index of respective list
	_childrenInfo() : m_bMature(false), m_bMale(true), m_nIndex(-1)
	{
	}
	_childrenInfo(bool bmtr, bool bm, long indx) : m_bMature(bmtr), m_bMale(bm), m_nIndex(indx)
	{
	}
	inline _childrenInfo &operator=(const _childrenInfo &copyObj)
	{
		if (&copyObj != this)
		{
			m_bMature = copyObj.m_bMature;
			m_bMale = copyObj.m_bMale;
			m_nIndex = copyObj.m_nIndex;
		}
		return *this;
	}
	inline _childrenInfo(const _childrenInfo &copyObj)
	{
		if (&copyObj == this)
		{
			return;
		}
		m_bMature = copyObj.m_bMature;
		m_bMale = copyObj.m_bMale;
		m_nIndex = copyObj.m_nIndex;
	}
} ChildInfo;

typedef struct _edgeParentChild
{
	bool m_bMatureChild;  // true=mature
	bool m_bParentGender; // true=male
	bool m_bChildGender;  // true = child gender male
	long m_nParentIndex;  // Male/Female vector index
	long m_nChildIndex;	  // mature / Child vector index
	_edgeParentChild() : m_bMatureChild(false), m_bParentGender(true), m_bChildGender(true), m_nParentIndex(-1), m_nChildIndex(-1)
	{
	}
	_edgeParentChild(bool bPgen, long pindx, bool bmtrChild, bool bCgen, long cindx) : m_bMatureChild(bmtrChild), m_bParentGender(bPgen), m_nParentIndex(pindx), m_bChildGender(bCgen), m_nChildIndex(cindx)
	{
	}
	inline _edgeParentChild &operator=(const _edgeParentChild &copyObj)
	{
		if (&copyObj != this)
		{
			m_bMatureChild = copyObj.m_bMatureChild;
			m_bParentGender = copyObj.m_bParentGender;
			m_nParentIndex = copyObj.m_nParentIndex;
			m_bChildGender = copyObj.m_bChildGender;
			m_nChildIndex = copyObj.m_nChildIndex;
		}
		return *this;
	}

	inline _edgeParentChild(const _edgeParentChild &copyObj)
	{
		if (&copyObj == this)
		{
			return;
		}
		m_bMatureChild = copyObj.m_bMatureChild;
		m_bParentGender = copyObj.m_bParentGender;
		m_nParentIndex = copyObj.m_nParentIndex;
		m_bChildGender = copyObj.m_bChildGender;
		m_nChildIndex = copyObj.m_nChildIndex;
	}
} ParentChildInfo;

struct iniAddress
{
	std::string name;
	std::string address;
	iniAddress() : name("NA"), address("NA")
	{
	}

	iniAddress(std::string str1, std::string str2) : name(str1), address(str2)
	{
	}

	inline iniAddress &operator=(const iniAddress &copyObj)
	{
		if (&copyObj != this)
		{
			name = copyObj.name;
			address = copyObj.address;
		}
		return *this;
	}

	inline iniAddress(const iniAddress &copyObj)
	{
		if (&copyObj == this)
		{
			return;
		}
		name = copyObj.name;
		address = copyObj.address;
	}
};

typedef struct _edgeSiblings
{
	bool m_bMatureP1; // true=mature[>18 years of age]
	bool m_bMatureP2; // true=mature[>18 years of age]
	bool m_bGenderP1; // true=male
	bool m_bGenderP2; // true = male
	long m_nIndexP1;  // Male/Female vector index
	long m_nIndexP2;  // Male/Female vector index
	_edgeSiblings() : m_bMatureP1(false), m_bMatureP2(false), m_bGenderP1(true), m_bGenderP2(true), m_nIndexP1(-1), m_nIndexP2(-1)
	{
	}
	_edgeSiblings(bool bmtur1, bool bgen1, long pindx1, bool bmtur2, bool bgen2, long pindx2) : m_bMatureP1(bmtur1), m_bGenderP1(bgen1), m_nIndexP1(pindx1), m_bMatureP2(bmtur2), m_bGenderP2(bgen2), m_nIndexP2(pindx2)
	{
	}
	inline _edgeSiblings &operator=(const _edgeSiblings &copyObj)
	{
		if (&copyObj != this)
		{
			m_bMatureP1 = copyObj.m_bMatureP1;
			m_bMatureP2 = copyObj.m_bMatureP2;
			m_bGenderP1 = copyObj.m_bGenderP1;
			m_bGenderP2 = copyObj.m_bGenderP2;
			m_nIndexP1 = copyObj.m_nIndexP1;
			m_nIndexP2 = copyObj.m_nIndexP2;
		}
		return *this;
	}

	inline _edgeSiblings(const _edgeSiblings &copyObj)
	{
		if (&copyObj == this)
		{
			return;
		}
		m_bMatureP1 = copyObj.m_bMatureP1;
		m_bMatureP2 = copyObj.m_bMatureP2;
		m_bGenderP1 = copyObj.m_bGenderP1;
		m_bGenderP2 = copyObj.m_bGenderP2;
		m_nIndexP1 = copyObj.m_nIndexP1;
		m_nIndexP2 = copyObj.m_nIndexP2;
	}
} SiblingsInfo;

typedef struct _edgeMarriages
{
	bool m_bGenderP1; // true=male
	bool m_bGenderP2; // true = male
	long m_nIndexP1;  // Male/Female vector index
	long m_nIndexP2;  // Male/Female vector index
	_edgeMarriages() : m_bGenderP1(true), m_bGenderP2(true), m_nIndexP1(-1), m_nIndexP2(-1)
	{
	}
	_edgeMarriages(bool bgen1, long pindx1, bool bgen2, long pindx2) : m_bGenderP1(bgen1), m_nIndexP1(pindx1), m_bGenderP2(bgen2), m_nIndexP2(pindx2)
	{
	}
	inline _edgeMarriages &operator=(const _edgeMarriages &copyObj)
	{
		if (&copyObj != this)
		{
			m_bGenderP1 = copyObj.m_bGenderP1;
			m_bGenderP2 = copyObj.m_bGenderP2;
			m_nIndexP1 = copyObj.m_nIndexP1;
			m_nIndexP2 = copyObj.m_nIndexP2;
		}
		return *this;
	}

	inline _edgeMarriages(const _edgeMarriages &copyObj)
	{
		if (&copyObj == this)
		{
			return;
		}
		m_bGenderP1 = copyObj.m_bGenderP1;
		m_bGenderP2 = copyObj.m_bGenderP2;
		m_nIndexP1 = copyObj.m_nIndexP1;
		m_nIndexP2 = copyObj.m_nIndexP2;
	}
} MarriagesInfo;

struct _PopulationData
{
	std::string m_strCountry;
	std::string m_strPrefec;
	std::string m_strSubPrefec;
	std::string m_strLocalitycode;
	std::string m_strLocalityName;
	long m_nPopulation;
	long m_nArea; // in hectors
	double m_longitude;
	double m_latitude;
	const size_t nColms = 9; // number of columns
	_PopulationData() : m_strCountry("sweden"),
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
	inline _PopulationData &operator=(const _PopulationData &copyObj)
	{
		if (&copyObj != this)
		{
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

struct Personality_traits
{
	double extraversion_score;
	double agreeableness_score;
	double conscientiousness_score;
	double neuroticism_score;
	double openness_score;

	Personality_traits() : extraversion_score(0.1),
						   agreeableness_score(0.1),
						   conscientiousness_score(0.1),
						   neuroticism_score(0.1),
						   openness_score(0.1)
	{
	}

	inline Personality_traits &operator=(const Personality_traits &copyObj)
	{
		if (&copyObj != this)
		{
			extraversion_score = copyObj.extraversion_score;
			agreeableness_score = copyObj.agreeableness_score;
			conscientiousness_score = copyObj.conscientiousness_score;
			neuroticism_score = copyObj.neuroticism_score;
			openness_score = copyObj.openness_score;
		}
		return *this;
	}
};
