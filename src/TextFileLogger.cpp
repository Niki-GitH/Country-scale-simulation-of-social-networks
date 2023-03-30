#include <time.h>
#include <cstdio>
#include <iomanip>
#include <ctime>
#include <stdio.h>
#include <vector>
#include <stdlib.h> 
#include <chrono>
#include "../include/TextFileLogger.h"

const std::string CTextFileLogger::m_sFileName = "SimulatorLog.txt";
CTextFileLogger* CTextFileLogger::m_pInstance = NULL;
std::ofstream CTextFileLogger::m_Logfile;
size_t  CTextFileLogger::m_lFileSizeInBytes = 0;

#define LOG_FILE_MAX_SIZE_BYTES 5120047  //5Mb

CTextFileLogger* CTextFileLogger::GetLogger() {
	if (m_pInstance == NULL) {
		m_pInstance = new CTextFileLogger();
		m_Logfile.open(m_sFileName.c_str(), std::ios::out | std::ios::app);
	}
	return m_pInstance;
}

void CTextFileLogger::Log(const char * zcFormat, ...)
{
	if (IsFileBig())
	{
		CloseCurrentAndCreateNewFile();
	}
	
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
	std::string strMSg(zc.data(), iLen);
	std::string datetime = CurrentDateTime();
	m_Logfile << datetime << ":  ";
	m_Logfile << strMSg << "\n";
	m_lFileSizeInBytes = m_lFileSizeInBytes + datetime.size() + strMSg.size() + 4;

	
}

void CTextFileLogger::Log(const std::string& sMessage)
{
	if (IsFileBig())
	{
		CloseCurrentAndCreateNewFile();
	}

	std::string datetime = CurrentDateTime();
	m_Logfile << datetime << ":  ";
	m_Logfile << sMessage << "\n";
	m_lFileSizeInBytes = m_lFileSizeInBytes + datetime.size() + sMessage.size() + 4;
}

CTextFileLogger& CTextFileLogger::operator<<(const std::string& sMessage)
{
	if (IsFileBig())
	{
		CloseCurrentAndCreateNewFile();
	}
	std::string datetime = CurrentDateTime();
	m_Logfile << "\n" << datetime << ":  ";
	m_Logfile << sMessage << "\n";
	m_lFileSizeInBytes = m_lFileSizeInBytes + datetime.size() + sMessage.size() + 4;
	return *this;
}


std::string CTextFileLogger::CurrentDateTime()
{
	/*time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H-%M-%S", timeinfo);
	std::string strtime(buffer);
	return strtime;*/
    auto timepoint = std::chrono::system_clock::now();
    auto coarse = std::chrono::system_clock::to_time_t(timepoint);
    auto fine = std::chrono::time_point_cast<std::chrono::milliseconds>(timepoint);

    char buffer[sizeof "9999-12-31 23:59:59.999"];
    std::snprintf(buffer + std::strftime(buffer, sizeof buffer - 3,
                                         "%F %T.", std::localtime(&coarse)),
                  4, "%03lu", fine.time_since_epoch().count() % 1000);
         std::string strtime(buffer);
	return strtime;
}


bool CTextFileLogger::IsFileBig()
{
	if (m_lFileSizeInBytes >= LOG_FILE_MAX_SIZE_BYTES)
	{
		m_lFileSizeInBytes = 0;
		return true;
	}
	return false;
}


void CTextFileLogger::CloseCurrentAndCreateNewFile()
{
	m_Logfile.flush();
	m_Logfile.close();
	std::string renameFile = "SimulatorLog_";

	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y_%H-%M-%S", timeinfo);
	std::string strtime(buffer);
	renameFile = renameFile + strtime + ".txt";
	
	if (std::rename(m_sFileName.c_str(), renameFile.c_str()) != 0)
	{
		std::perror("Error renaming file");
	}
	// Create/Open file
	m_Logfile.open(m_sFileName.c_str(), std::ios::out | std::ios::app);
}
