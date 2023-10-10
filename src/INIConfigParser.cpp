#include "../include/INIConfigParser.h"
#include "INIConfigParser.h"

CINIConfigParser::CINIConfigParser(const std::string &filename)
{
	std::ifstream file(filename);
	std::string line, current_section;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			parseLine(line, current_section);
		}
		file.close();
	}
	else
	{
		throw std::runtime_error("Failed to open file: " + filename);
	}
}

long CINIConfigParser::GetIntValue(const std::string &section, const std::string &key) const
{
	std::lock_guard<std::mutex> lock(mutex_);
	//std::cout << section << key << " Size = "<<data.size();
	auto section_iter = data.find(section);
	if (section_iter != data.end())
	{
		const auto &section_data = section_iter->second;
		auto key_iter = section_data.find(key);
		if (key_iter != section_data.end())
		{

			return std::stol(key_iter->second);
		}
		else
		{
			//std::cout << "could not key\n";
		}
	}
	else
	{
		std::cout << "could not find section key\n";
	}
	//std::cout << " returned 0" << std::endl;
	return 0;
}

double CINIConfigParser::GetDblValue(const std::string &section, const std::string &key) const
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto section_iter = data.find(section);
	if (section_iter != data.end())
	{
		const auto &section_data = section_iter->second;
		auto key_iter = section_data.find(key);
		if (key_iter != section_data.end())
		{
			return std::stod(key_iter->second);
		}
	}

	return 0;
}

std::string CINIConfigParser::GetStringValue(const std::string &section, const std::string &key) const
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto section_iter = data.find(section);
	if (section_iter != data.end())
	{
		const auto &section_data = section_iter->second;
		auto key_iter = section_data.find(key);
		if (key_iter != section_data.end())
		{
			return key_iter->second;
		}
	}
	return 0;
}

iniAddress CINIConfigParser::GetOfficeAddress_(int index)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto section_iter = data.find(SECTION_OFFICES);
	if (section_iter != data.end())
	{
		const auto &section_data = section_iter->second;
		std::string key = "office_" + std::to_string(index + 1);
		auto entry_iter = section_data.find(key);
		if (entry_iter != section_data.end())
		{
			return parseAddress(entry_iter->second);
		}
	}
	iniAddress retEmpty;
	return retEmpty;
}

iniAddress CINIConfigParser::GetSchoolNameNAddress_(int index)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto section_iter = data.find(SECTION_SCHOOLS);
	if (section_iter != data.end())
	{
		const auto &section_data = section_iter->second;
		std::string key = "school_" + std::to_string(index + 1);
		auto entry_iter = section_data.find(key);
		if (entry_iter != section_data.end())
		{
			return parseAddress(entry_iter->second);
		}
	}
	iniAddress retEmpty;
	return retEmpty;
}

iniAddress CINIConfigParser::GetUnivstyNameNAddress_(int index)
{
	std::lock_guard<std::mutex> lock(mutex_);
	auto section_iter = data.find(SECTION_UNIS);
	if (section_iter != data.end())
	{
		const auto &section_data = section_iter->second;
		std::string key = "university_" + std::to_string(index + 1);
		auto entry_iter = section_data.find(key);
		if (entry_iter != section_data.end())
		{
			return parseAddress(entry_iter->second);
		}
	}
	iniAddress retEmpty;
	return retEmpty;
}

std::string CINIConfigParser::getFolderPath()
{
    return m_folder_path;
}

void CINIConfigParser::setFolderPath(std::string path)
{
	m_folder_path = path;
}

void CINIConfigParser::printMapData()
{
	for(auto& map:data)
	{
		std::cout << "---" << map.first << "---\n";
		for(auto& val:map.second)
		{
			std::cout<<val.first<<":"<<val.second<<std::endl;
		}
	}
}

void CINIConfigParser::parseLine(const std::string &line, std::string &current_section)
{
	 
	if (line.find(";") != std::string::npos)
	{
		// skiip comments
		//std::cout<<"skipping line: "<<line<<"\n";
		return;
	}

	//std::cout<<line<<std::endl;
	if (!line.empty())
	{
		bool section_falg1 = (line.find("[") != std::string::npos) ? true : false;
		bool section_falg2 = (line.find("]") != std::string::npos) ? true : false;
		//std::cout<<"Flags :"<<section_falg1<<section_falg2<<std::endl;

		if (section_falg1 && section_falg2)
		{
			
			current_section = line;
			size_t delm_pos1 = line.find('[');
			size_t delm_pos2 = line.find(']');
			current_section = current_section.substr(delm_pos1+1,delm_pos2-1);
			//current_section.erase(std::remove(current_section.begin(), current_section.end(), ']'), current_section.end());
			//current_section.erase(std::remove(current_section.begin(), current_section.end(), '['), current_section.end());
			//current_section = line.substr(1, line.size() - 2);
			//std::cout<<"new section added "<<current_section<<std::endl;
		}
		else
		{
			//std::cout<<"new section added "<<current_section;
			size_t equals_pos = line.find('=');
			
			if (equals_pos != std::string::npos)
			{
				std::string keyvalue = line;
				std::string key = keyvalue.substr(0, equals_pos);
				std::string value = keyvalue.substr(equals_pos + 1);
				
				//key = trim(key);
				//value = trim(value);

				key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
				value.erase(std::remove(value.begin(), value.end(), ' '), value.end());

				data[current_section][key] = value;
				//std::cout <<data[current_section][key] << std::endl;
			}
		}
	}
	else
	{
		std::cout<<"line is empty\n"<<line;
	}
}

// Trim leading and trailing spaces from a string
std::string CINIConfigParser::trim(const std::string &str)
{
	size_t first = str.find_first_not_of(' ');
	if (std::string::npos == first)
	{
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

/*
template <typename T>
T CINIConfigParser::convertValue(const std::string& str) const {
	if constexpr (std::is_same_v<T, long>) {
		return std::stol(str);
	}
	else if constexpr (std::is_same_v<T, double>) {
		return std::stod(str);
	}
	else if constexpr (std::is_same_v<T, std::string>) {
		return str;
	}
	else {
		throw std::runtime_error("Unsupported value type");
	}
}
*/

iniAddress CINIConfigParser::parseAddress(const std::string &entry) const
{
	std::istringstream ss(entry);
	std::string name, address;
	std::getline(ss, name, '#');
	std::getline(ss, address);
	return iniAddress(name, address);
}
