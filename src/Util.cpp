#include "../include/Util.h"
#include <algorithm>

const std::string CUtil::WHITESPACE = " \n\r\t\f\v";
std::atomic<int> CUtil::m_nTotalEvents{0};

CUtil::~CUtil(void)
{
}

std::string CUtil::ltrim(const std::string &s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string CUtil::rtrim(const std::string &s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string CUtil::trim(const std::string &s)
{
	return rtrim(ltrim(s));
}

std::vector<std::string> CUtil::split(const std::string &s, char seperator)
{
	std::vector<std::string> output;

	std::string::size_type prev_pos = 0, pos = 0;

	while ((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring(s.substr(prev_pos, pos - prev_pos));

		output.push_back(substring);

		prev_pos = ++pos;
	}

	output.push_back(s.substr(prev_pos, pos - prev_pos)); // Last word

	return output;
}

void CUtil::ToLower(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void CUtil::ToUpper(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

std::vector<std::vector<double>> CUtil::multiplymat(std::vector<std::vector<double>> m1, std::vector<std::vector<double>> m2)
{
	// std::vector<std::vector<double>> res
	size_t R1 = m1.size();
	size_t C1 = m1[0].size();
	size_t R2 = m2.size();
	size_t C2 = m2[0].size();
	std::vector<std::vector<double>> rslt(R1, std::vector<double>(C2, 0.0));
	for (int i = 0; i < R1; i++)
	{
		for (int j = 0; j < C2; j++)
		{
			rslt[i][j] = 0;
			for (int k = 0; k < R2; k++)
			{
				rslt[i][j] += m1[i][k] * m2[k][j];
			}
		}
	}
	return rslt;
}

EventType_ CUtil::getLifeEventFromString(std::string event)
{
	EventType_ rtnVal = EVENT_TYPE_START;
	const std::string txt1 = "GRADUATION";
	const std::string txt2 = "NEW_CHILD";
	const std::string txt3 = "MARRIAGE";
	const std::string txt4 = "HOME_PURCHASE";
	const std::string txt5 = "NEW_JOB";
	const std::string txt6 = "DEATH";
	if (event.find(txt1) != std::string::npos)
	{
		rtnVal = EVENT_TYPE_GRADUATION;
	}
	else if (event.find(txt2) != std::string::npos)
	{
		rtnVal = EVENT_TYPE_NEW_CHILD;
	}
	else if (event.find(txt3) != std::string::npos)
	{
		rtnVal = EVENT_TYPE_MARRIAGE;
	}
	else if (event.find(txt4) != std::string::npos)
	{
		rtnVal = EVENT_TYPE_HOME_PURCHASE;
	}
	else if (event.find(txt5) != std::string::npos)
	{
		rtnVal = EVENT_TYPE_NEW_JOB;
	}
	else if (event.find(txt6) != std::string::npos)
	{
		rtnVal = EVENT_TYPE_DEATH;
	}
	else
	{
		rtnVal = EVENT_TYPE_START;
	}
	return rtnVal;
}
