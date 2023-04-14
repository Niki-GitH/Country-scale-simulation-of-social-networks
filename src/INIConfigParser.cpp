#include "../include/INIConfigParser.h"

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> CINIConfigParser::m_values;
std::vector<std::pair<std::string, std::string>> CINIConfigParser::m_schools;
std::vector<std::pair<std::string, std::string>> CINIConfigParser::m_universities;
std::vector<std::pair<std::string, std::string>> CINIConfigParser::m_offices;


void CINIConfigParser::parse(const std::string & fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open()) {
		//throw std::runtime_error("INI file not found.");
		return;
	}

	std::string line;
	std::string section = "";
	while (std::getline(file, line)) {
		// remove leading and trailing whitespaces
		line = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");
		if (line.empty() || line[0] == ';') {
			continue;
		}
		if (line[0] == '[' && line[line.size() - 1] == ']') {
			// found a new section
			section = line.substr(1, line.size() - 2);
			continue;
		}
		// split key-value pair by '=' character
		size_t pos = line.find('=');
		if (pos == std::string::npos) {
			//throw std::runtime_error("Invalid INI format.");
			continue;
		}
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		// trim key and value strings
		key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
		value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
		// add key-value pair to data map
		m_values[section][key] = value;

		if (section == SECTION_SCHOOLS)
		{
			if (std::string::npos != value.find('#')) {
				std::vector < std::string > tokens;
				split(value, '#', tokens);
				if (tokens.size() > 1)
				{
					m_schools.push_back(std::make_pair(tokens[0], tokens[1]));
				}
			}
		}

		if (section == SECTION_UNIS)
		{
			if (std::string::npos != value.find('#')) {
				std::vector < std::string > tokens;
				split(value, '#', tokens);
				if (tokens.size() > 1)
				{
					m_universities.push_back(std::make_pair(tokens[0], tokens[1]));
				}
			}
		}

		if (section == SECTION_OFFICES)
		{
			if (std::string::npos != value.find('#')) {
				std::vector < std::string > tokens;
				split(value, '#', tokens);
				if (tokens.size() > 1)
				{
					m_offices.push_back(std::make_pair(tokens[0], tokens[1]));
				}
			}
		}
	}

}

long CINIConfigParser::GetIntValue(const std::string & section, const std::string & key, long defaultValue)
{
	return std::stol(m_values[section][key]);
}

double CINIConfigParser::GetDblValue(const std::string & section, const std::string & key, double defaultValue)
{
	return std::stod(m_values[section][key]);
}

std::string CINIConfigParser::GetStringValue(const std::string & section, const std::string & key, const std::string & defaultValue)
{
	return m_values[section][key];
}

void CINIConfigParser::split(std::string str, char del, std::vector < std::string >& out)
{
	std::string temp = "";
	for (int i = 0; i < (int)str.size(); i++) {
		if ((str[i] != del))
		{
			if ((str[i] == '\r') || (str[i] == '\n'))
				continue;
			temp += str[i];
		}
		else {
			out.push_back(temp);
			temp = "";
		}
	}//for
	out.push_back(temp);
}
