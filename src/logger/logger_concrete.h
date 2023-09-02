#ifndef LOGGER_CONCRETE_H
#define LOGGER_CONCRETE_H

#include "logger.h"
#include <iostream>

std::string current_date_time();

std::string enum_to_string(logger::severity severity);

class logger_concrete final : public logger
{

private:

    std::map<std::string, std::pair<std::ofstream*, severity>> _streams_log;

protected:

    static std::map<std::string, std::pair<std::ofstream*, size_t>> _collection_streams;

public:

	explicit logger_concrete(std::vector<std::pair<std::string, severity>> const& streams);

	logger *log(const std::string& message, severity severity) override;

	~logger_concrete() override;
};

#endif