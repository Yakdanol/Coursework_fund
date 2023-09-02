#include "logger_builder_concrete.h"

logger_builder* logger_builder_concrete::add_stream(
    std::string const& name_stream,
    logger::severity level)
{
    _streams.emplace_back(name_stream, level);

    return this;
}

logger_builder* logger_builder_concrete::clear()
{
    _streams.clear();

    return this;
}

logger* logger_builder_concrete::build() const
{
    return new logger_concrete(_streams);
}