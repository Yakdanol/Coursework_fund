#ifndef LOGGER_BUILDER_H
#define LOGGER_BUILDER_H

#include "logger.h"

class logger_builder
{

public:

    virtual logger_builder* add_stream(std::string const&, logger::severity) = 0;

    virtual logger_builder* clear() = 0;

    virtual logger* build() const = 0;

    virtual ~logger_builder();

};

#endif // LOGGER_BUILDER_H
