#pragma once
#include <fstream>
#include <sstream> 
#include <vector>
#include <limits>
#include <cstdio>
#include <fstream>
#include <cmath>
#include <atomic>
#include "defines.h"

class CUtil
{
private:
	CUtil(void);

	virtual ~CUtil(void);

	static const std::string WHITESPACE;
	
public:
	static std::atomic<int> m_nTotalEvents;

	static std::string ltrim(const std::string &s);

	static std::string rtrim(const std::string &s);

	static std::string trim(const std::string &s);
	static std::vector<std::string> split(const std::string& s, char seperator);

	static void ToLower(std::string& str);
	static void ToUpper(std::string& str);
	static std::vector<std::vector<double>> multiplymat(std::vector<std::vector<double>> m1, std::vector<std::vector<double>> m2);
	
	static EventType_ getLifeEventFromString(std::string event);
};


