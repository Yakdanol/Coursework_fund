#ifndef JSON_BUILDER_H
#define JSON_BUILDER_H

#include "forJson/single_include/nlohmann/json.hpp"
#include "../logger/logger.h"

class json_builder
{

public:

    virtual logger *build(const std::string &path) = 0;

    virtual ~json_builder();

};

#endif //JSON_BUILDER_H