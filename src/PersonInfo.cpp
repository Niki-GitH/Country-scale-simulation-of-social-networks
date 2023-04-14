#include "../include/PersonInfo.h"
#include "../include/PopulationDB.h"
#include <cstdarg>
#include <iostream>
#include <sstream> 
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <type_traits>
#include <exception>
#include <algorithm>

CPersonInfo::CPersonInfo():
	m_bHasChildren(false),
	m_bIsEmployed(false),
	m_nDOB_Day(0),
	m_nDOB_Month(0),
	m_nDOB_Year(0),
	m_nSSN("111111111"),
	m_nChildCount(0),
	m_nGender(GENDER_TYPE_FEMALE),
	m_nMaritalStatus(MARITAL_STATUS_SINGLE),
	m_nGraduation(GRAD_TYPE_NONE),
	m_strFirstName(""),
	m_strMiddleName(""),
	m_strLastName(""),
	m_strBirthPlace(""),
	m_strFatherName(""),
	m_strMotherName(""),
	m_strSpouseName(""),
	m_strHomeAddress(""),
	m_strWorkAddress(""),
	m_strEmail("aa.bb@xyz.com"),
	m_strDateofDeath(""),
	m_stFinancialStatus(FINANCE_STATUS_POOR),
	m_nAge(5),
	m_dIncome(1000),
	m_uniqueID("")
{
}

void CPersonInfo::UpdateGraduationDetatils(std::string GraduationType)
{
	std::transform(GraduationType.begin(), GraduationType.end(), GraduationType.begin(), ::toupper);
	std::string type1 = "SENIOR";
	std::string type2 = "ADVANCED";
	std::string type3 = "DIPLOMA";
	std::string type4 = "BACHELOR";
	std::string type5 = "GRADUATE";
	std::string type6 = "MASTER";
	std::string type7 = "DOCTORAL";
	if (GraduationType.find(type1) != std::string::npos) {
		m_nGraduation = GRAD_TYPE_SENIOR_HIGH;
	}
	else if(GraduationType.find(type2) != std::string::npos) {
		m_nGraduation = GRAD_TYPE_ADVANCED_DIPLOMA;
	}
	else if (GraduationType.find(type3) != std::string::npos) {
		m_nGraduation = GRAD_TYPE_DIPLOMA;
	}
	else if (GraduationType.find(type4) != std::string::npos) {
		m_nGraduation = GRAD_TYPE_BACHELOR;
	}
	else if (GraduationType.find(type5) != std::string::npos) {
		m_nGraduation = GRAD_TYPE_GRADUATE;
	}
	else if (GraduationType.find(type6) != std::string::npos) {
		m_nGraduation = GRAD_TYPE_MASTER;
	}
	else if (GraduationType.find(type7) != std::string::npos) {
		m_nGraduation = GRAD_TYPE_DOCTORAL;
	}
	else
	{
		m_nGraduation = GRAD_TYPE_NONE;
	}
}

void CPersonInfo::UpdateFamilyDetatils(std::string childName, CPersonInfo& othrPrsn)
{
	m_listChildNames.push_back(childName);
	m_vChildren.push_back(othrPrsn);
}

void CPersonInfo::UpdateMaritalDetatils(std::string spouseName, CPersonInfo& othrPrsn)
{
	m_strSpouseName = spouseName;
	m_vSpouses.push_back(othrPrsn);
}

CPersonInfo::CPersonInfo(std::string FirstName, std::string Lastname, _dob stdob, GenderInfo_ gender, std::string Address)
	: m_strFirstName(FirstName),
	m_strLastName(Lastname),
	m_nGender(gender),
	m_nDOB_Day(stdob.m_nDay),
	m_nDOB_Month(stdob.m_nMonth),
	m_nDOB_Year(stdob.m_nYear),
	m_strHomeAddress(Address)
{
   //std::cout<<"PINFO "<<m_strFirstName<<std::endl;
   //std::cout<<"PINFO "<<m_strLastName<<std::endl;
	m_nAge = 2023 - stdob.m_nYear;
}

CPersonInfo::~CPersonInfo(void)
{
}


CPersonInfo::CPersonInfo(const CPersonInfo& copyObj)
{
	if (&copyObj == this) 
	{ 
		return; 
	}
	m_bHasChildren = copyObj.m_bHasChildren;
	m_bIsEmployed = copyObj.m_bIsEmployed;
	m_nDOB_Day = copyObj.m_nDOB_Day;
	m_nDOB_Month = copyObj.m_nDOB_Month;
	m_nDOB_Year = copyObj.m_nDOB_Year;
	m_nSSN = copyObj.m_nSSN;
	m_nChildCount = copyObj.m_nChildCount;
	m_nGender = copyObj.m_nGender;
	m_nMaritalStatus = copyObj.m_nMaritalStatus;
	m_nGraduation = copyObj.m_nGraduation;
	m_strFirstName = copyObj.m_strFirstName;
	m_strMiddleName = copyObj.m_strMiddleName;
	m_strLastName = copyObj.m_strLastName;
	m_strBirthPlace = copyObj.m_strBirthPlace;
	m_strFatherName = copyObj.m_strFatherName;
	m_strMotherName = copyObj.m_strMotherName;
	m_strSpouseName = copyObj.m_strSpouseName;
	m_strHomeAddress = copyObj.m_strHomeAddress;
	m_strWorkAddress = copyObj.m_strWorkAddress;
	m_listChildNames = copyObj.m_listChildNames;
	m_strEmail = copyObj.m_strEmail;
	m_listEvents = copyObj.m_listEvents;
	m_strDateofDeath = copyObj.m_strDateofDeath;
	m_stDeathDate = copyObj.m_stDeathDate;
	m_stFinancialStatus = copyObj.m_stFinancialStatus;
	m_nAge = copyObj.m_nAge;
	m_uniqueID = copyObj.m_uniqueID;
	m_dIncome = copyObj.m_dIncome;
	m_schoolsattended = copyObj.m_schoolsattended;
	m_vfriends = copyObj.m_vfriends;
	m_vChildren = copyObj.m_vChildren;
	m_vSpouses = copyObj.m_vSpouses;
	m_vParents = copyObj.m_vParents;
	m_vColleagues = copyObj.m_vColleagues;
	m_vClassmates = copyObj.m_vClassmates;
	m_vCommunity_members = copyObj.m_vCommunity_members;
	m_vCommunication_contacts = copyObj.m_vCommunication_contacts;
	m_vTransportation_contacts = copyObj.m_vTransportation_contacts;
}

CPersonInfo & CPersonInfo::operator=(const CPersonInfo & copyObj)
{
	if (&copyObj != this) {
		m_bHasChildren = copyObj.m_bHasChildren;
		m_bIsEmployed = copyObj.m_bIsEmployed;
		m_nDOB_Day = copyObj.m_nDOB_Day;
		m_nDOB_Month = copyObj.m_nDOB_Month;
		m_nDOB_Year = copyObj.m_nDOB_Year;
		m_nSSN = copyObj.m_nSSN;
		m_nChildCount = copyObj.m_nChildCount;
		m_nGender = copyObj.m_nGender;
		m_nMaritalStatus = copyObj.m_nMaritalStatus;
		m_nGraduation = copyObj.m_nGraduation;
		m_strFirstName = copyObj.m_strFirstName;
		m_strMiddleName = copyObj.m_strMiddleName;
		m_strLastName = copyObj.m_strLastName;
		m_strBirthPlace = copyObj.m_strBirthPlace;
		m_strFatherName = copyObj.m_strFatherName;
		m_strMotherName = copyObj.m_strMotherName;
		m_strSpouseName = copyObj.m_strSpouseName;
		m_strHomeAddress = copyObj.m_strHomeAddress;
		m_strWorkAddress = copyObj.m_strWorkAddress;
		m_listChildNames = copyObj.m_listChildNames;
		m_strEmail = copyObj.m_strEmail;
		m_listEvents = copyObj.m_listEvents;
		m_strDateofDeath = copyObj.m_strDateofDeath;
		m_stDeathDate = copyObj.m_stDeathDate;
		m_stFinancialStatus = copyObj.m_stFinancialStatus;
		m_nAge = copyObj.m_nAge;
		m_uniqueID = copyObj.m_uniqueID;
		m_dIncome = copyObj.m_dIncome;
		m_schoolsattended = copyObj.m_schoolsattended;
		m_vfriends = copyObj.m_vfriends;
		m_vChildren = copyObj.m_vChildren;
		m_vSpouses = copyObj.m_vSpouses;
		m_vParents = copyObj.m_vParents;
		m_vColleagues = copyObj.m_vColleagues;
		m_vClassmates = copyObj.m_vClassmates;
		m_vCommunity_members = copyObj.m_vCommunity_members;
		m_vCommunication_contacts = copyObj.m_vCommunication_contacts;
		m_vTransportation_contacts = copyObj.m_vTransportation_contacts;
	}
	return *this;
}



void CPersonInfo::AddEvent(const PersonEvent & newEvent, CPersonInfo& othrPrsn)
{
	PersonEvent ev = newEvent;
	bool bupdate = true;
	switch (newEvent.m_nEventType)
	{
	case EVENT_TYPE_GRADUATION:
	{
		ev.m_strEventName = "Graduation";
		UpdateGraduationDetatils(newEvent.m_strEventComments);
	}
	break;
	case EVENT_TYPE_NEW_CHILD:
	{
		ev.m_strEventName = "Child";
		UpdateFamilyDetatils(newEvent.m_strEventComments, othrPrsn);
	}
	break;
	case EVENT_TYPE_MARRIAGE:
	{
		ev.m_strEventName = "Marriage";
		UpdateMaritalDetatils(newEvent.m_strEventComments, othrPrsn);
	}
	break;
	case EVENT_TYPE_NEW_JOB:
	{
		ev.m_strEventName = "JobChange";
		m_strWorkAddress = newEvent.m_strEventComments;
	}
	break;
	case EVENT_TYPE_DEATH:
	{
		ev.m_strEventName = "Death";
		m_strDateofDeath = getDODstr();
	}
	break;
	case EVENT_TYPE_START:
	case EVENT_TYPE_END:
	default:
		bupdate = false;
		break;
	}

	if (bupdate)
	{
		m_listEvents.push_back(ev);
	}
	
	long temp = static_cast<long>(m_listEvents.size());
	if (CPopulationDB::m_nTotalEvents < temp)
	{
		CPopulationDB::m_nTotalEvents = temp;
	}
}

const std::string CPersonInfo::GetFormatedString()
{
	//FName,Mname,Lname,DOB,Gender,BirthPlace,FatherName,MotherName,SSN,Email,HomeAddress,EducationLevel,MaritalStatus,SpouseName,HasChildren,NumOfChild,ChildrenNames,IsEmployed,OfficeAddress
	std::string strName = m_strFirstName + "," + m_strMiddleName + "," + m_strLastName;
	std::string strParentName = m_strFatherName + "," + m_strMotherName;
	std::string dob = vformat(", D.O.B[%d-%d-%d],%d , ", m_nDOB_Day, m_nDOB_Month, m_nDOB_Year,m_nAge);
	std::string ssn = ", " + m_nSSN + ", " ;
	std::string hasChild = (m_bHasChildren) ? ", YES, " : ", NO, ";
	std::string NoOfChild = std::to_string(m_listChildNames.size())+ ", ";//vformat("%d, ", m_nChildCount);
	std::string IsEmpld = (m_bIsEmployed) ? ", YES, " : ", NO, ";
	std::string childNames = " ";
	for (auto child : m_listChildNames)
	{
		childNames = childNames + child + "/";
	}
	std::string events = " ";
	for (int i = 0; i < m_listEvents.size(); ++i)
	{
		events = ", " + m_listEvents[i].m_strEventName + ", " + m_listEvents[i].m_strEventComments+ "/" + m_listEvents[i].m_strDate;
	}

	std::string strFormtdInfo;
	strFormtdInfo = strName +  dob + getGenderStr() + "," + m_strBirthPlace + "," + strParentName + ssn + m_strEmail + "," +m_strHomeAddress;
	strFormtdInfo = strFormtdInfo + "," + getEducationStr() + "," + getMStsStr() + "," + m_strSpouseName + hasChild + NoOfChild;
	strFormtdInfo = strFormtdInfo + childNames + IsEmpld + m_strWorkAddress + events;
	return strFormtdInfo;
}

std::string CPersonInfo::getDOBstr()
{
	return vformat("%d-%d-%d", m_nDOB_Day, m_nDOB_Month, m_nDOB_Year);
}

std::string CPersonInfo::getDODstr()
{
	return vformat("%d-%d-%d", m_stDeathDate.m_nDay, m_stDeathDate.m_nMonth, m_stDeathDate.m_nYear);
}


const std::string CPersonInfo::vformat(const char * const zcFormat, ...) {

	// initialize use of the variable argument array
	std::va_list vaArgs;
	va_start(vaArgs, zcFormat);

	// reliably acquire the size
	// from a copy of the variable argument array
	// and a functionally reliable call to mock the formatting
	std::va_list vaArgsCopy;
	va_copy(vaArgsCopy, vaArgs);
	const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
	va_end(vaArgsCopy);

	// return a formatted string without risking memory mismanagement
	// and without assuming any compiler or platform specific behavior
	std::vector<char> zc(iLen + 1);
	std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
	va_end(vaArgs);
	return std::string(zc.data(), iLen);
}

const std::string CPersonInfo::getEducationStr()
{
	std::string mgrad = "Single";
	switch (m_nGraduation)
	{
	case GRAD_TYPE_SENIOR_HIGH:
		mgrad = "Senior High";
		break;
	case GRAD_TYPE_DIPLOMA:
		mgrad = "DIPLOMA";
		break;
	case GRAD_TYPE_ADVANCED_DIPLOMA:
		mgrad = "Advanced DIPLOMA";
		break;
	case GRAD_TYPE_BACHELOR:
		mgrad = "BACHELOR";
		break;
	case GRAD_TYPE_GRADUATE:
		mgrad = "GRADUATE";
		break;
	case GRAD_TYPE_MASTER:
		mgrad = "MASTER";
		break;
	case GRAD_TYPE_DOCTORAL:
		mgrad = "DOCTORAL";
		break;
	case GRAD_TYPE_NONE:
	default:
		mgrad = "Uneducated";
		break;
	}
	return mgrad;
}

const std::string CPersonInfo::getMStsStr()
{
	std::string msts = "Single";
	if (m_nMaritalStatus == MARITAL_STATUS_MARRIED)
	{
		msts = "Married";
	}
	else if (m_nMaritalStatus == MARITAL_STATUS_WIDOWED)
	{
		msts = "Widowed";
	}
	else if (m_nMaritalStatus == MARITAL_STATUS_DIVORCED)
	{
		msts = "Divorced";
	}
	else if (m_nMaritalStatus == MARITAL_STATUS_SEPERATED)
	{
		msts = "Seperated";
	}
	return msts;
}

const std::string CPersonInfo::getGenderStr()
{
	std::string gen = "Male";
	if (m_nGender == GENDER_TYPE_FEMALE)
	{
		gen = "Female";
	}
	else if (m_nGender == GENDER_TYPE_TRANS)
	{
		gen = "Transgen";
	}
	return gen;
}
