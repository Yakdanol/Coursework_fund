#include "logger_holder.h"
#include "logger_concrete.h"

logger *logger_holder::guard_log(
        const std::string &message,
        logger::severity level) const
{
    logger *logger_temp = get_logger();

    if (logger_temp == nullptr)
    {
        std::cout << current_date_time() << " " << enum_to_string(level) << " " << message << std::endl;
        return nullptr;
    }

    return logger_temp->log(message, level);
}
