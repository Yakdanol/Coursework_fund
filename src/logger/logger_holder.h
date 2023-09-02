#ifndef LOGGER_HOLDER_H
#define LOGGER_HOLDER_H

#include "logger.h"

class logger_holder
{

public:

    virtual ~logger_holder() = default;

    logger *guard_log(
            const std::string &message,
            logger::severity level) const;

    virtual logger *get_logger() const = 0;

};

#endif //LOGGER_HOLDER_H
