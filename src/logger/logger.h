#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <ctime>

class logger
{

public:

	enum class severity
	{
		TRACE,
		DEBUG,
		INFORMATION,
		WARNING,
		ERROR,
		CRITICAL
	};

	virtual logger* log(const std::string&,severity) = 0;

    virtual ~logger();

};

#endif // LOGGER_H