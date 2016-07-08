#ifndef JSONTYPE_H
#define JSONTYPE_H

#include <string>
#include <list>
#include <map>

using std::string;
using std::list;
using std::map;

class JsonType;

typedef list<JsonType> JsonList;
typedef map<JsonType, JsonType> JsonMap;

class JsonType {
public:
    JsonType()
        : m_type(JsonType::VoidType) {

    }

    JsonType(double num)
        : m_type(JsonType::NumberType),
          num_value(num) {

    }

    JsonType(bool bl)
        : m_type(JsonType::BooleanType),
          bl_value(bl) {

    }

    JsonType(const char *str)
        : m_type(JsonType::StringType),
          str_value(str) {

    }

    JsonType(const char *str, int len)
        : m_type(JsonType::StringType),
          str_value(str, len) {

    }

    JsonType(const string &str)
        : m_type(JsonType::StringType),
          str_value(str) {

    }

    JsonType(const JsonList &list)
        : m_type(JsonType::ListType),
          json_list(list) {

    }

    JsonType(const JsonMap &map)
        : m_type(JsonType::MapType),
          json_map(map) {

    }

    virtual ~JsonType() {

    }

    bool operator<(const JsonType &val) const {
        if (this->type() < val.type()) {
            return true;
        } else if (this->type() == val.type()) {
            switch (this->type()) {
            case JsonType::VoidType:
                return false;
                break;
            case JsonType::NumberType:
                return this->toNumber() < val.toNumber();
                break;
            case JsonType::BooleanType:
                return this->toBoolean() < val.toBoolean();
                break;
            case JsonType::StringType:
                return this->toString() < val.toString();
                break;
            case JsonType::ListType:
                return this->toList() < val.toList();
                break;
            case JsonType::MapType:
                return this->toMap() < val.toMap();
                break;
            default:
                break;
            }
        }
        return false;
    }

public:
    enum Type {
        VoidType, NumberType, BooleanType, StringType, ListType, MapType
    };

private:
    Type m_type;

    double   num_value;
    bool     bl_value;
    string   str_value;
    JsonList json_list;
    JsonMap  json_map;

public:
    Type type() const {
        return m_type;
    }

    double toNumber() const {
        return num_value;
    }

    bool toBoolean() const {
        return bl_value;
    }

    string toString() const {
        return str_value;
    }

    JsonList toList() const {
        return json_list;
    }

    JsonMap toMap() const {
        return json_map;
    }
};


#endif // JSONTYPE_H
