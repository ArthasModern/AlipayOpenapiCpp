#ifndef JSONUTIL_H
#define JSONUTIL_H

#include "cJSON.h"
#include "json_type.h"

namespace JsonUtil {

    string objectToString(const JsonType &obj);

    JsonType stringToObject(const string &json);

}

#endif // JSONUTIL_H
