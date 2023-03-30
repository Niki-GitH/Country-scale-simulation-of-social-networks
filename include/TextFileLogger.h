#pragma once
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
class CTextFileLogger
{
private:
	//variables

	//Log file name.
	const static std::string m_sFileName;
	
	//Singleton logger class object pointer.
	static CTextFileLogger* m_pInstance;

	//Log file stream object.
	static std::ofstream m_Logfile;

	//keep track of file size
	static std::size_t  m_lFileSizeInBytes;


public:
	
	/*
	Logs a message
	In     : std::string -> message
	return : void
	*/
	void Log(const std::string& sMessage);
	
	/*
	Logs a message with format
	In     : const char * -> format
	In     : arguments list
	return : void
	*/
	void Log(const char * format, ...);
	

	/*
	operator << overloaded function to Logs a message
	In     : std::string -> message
	return : 
	*/
	CTextFileLogger& operator<<(const std::string& sMessage);
	
	/*
	Get an instance of logger
	In     : None
	return : pointer to CTextFileLogger
	*/
	static CTextFileLogger* GetLogger();
private:
	
	/*
	Default constructor for the Logger class.
	*/
	CTextFileLogger() = default;

	~CTextFileLogger() = default;

	/**
	Copy constructor for the Logger class.
	*/
	CTextFileLogger(const CTextFileLogger&) = delete; 

	/**
	*   assignment operator for the Logger class.
	*/
	CTextFileLogger& operator=(const CTextFileLogger&) = delete;

	std::string CurrentDateTime();
	
	bool IsFileBig();

	void CloseCurrentAndCreateNewFile();
};


