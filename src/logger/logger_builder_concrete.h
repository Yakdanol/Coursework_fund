#ifndef LOGGER_BUILDER_CONCRETE_H
#define LOGGER_BUILDER_CONCRETE_H

#include "logger_builder.h"
#include "logger_concrete.h"
#include <iostream>

class logger_builder_concrete final : public logger_builder
{

private:

	std::vector<std::pair<std::string, logger::severity> > _streams;

public:

	logger_builder* add_stream(
		std::string const& name_stream,
		logger::severity level) override;

	logger_builder* clear() override;

	logger* build() const override;

};

#endif // LOGGER_BUILDER_CONCRETE_H
