#include "json_util.h"


namespace JsonUtil {

    cJSON * obj2json(const JsonType &obj) {

        //创建json根节点
        cJSON * pJsonRoot = NULL;
        JsonList jsonList;
        JsonMap jsonMap;

        switch (obj.type()) {
        case JsonType::VoidType:
            pJsonRoot = cJSON_CreateNull();
            break;
        case JsonType::NumberType:
            pJsonRoot = cJSON_CreateNumber(obj.toNumber());
            break;
        case JsonType::BooleanType:
            pJsonRoot = cJSON_CreateBool(obj.toBoolean());
            break;
        case JsonType::StringType:
            pJsonRoot = cJSON_CreateString(obj.toString().c_str());
            break;
        case JsonType::ListType:
            jsonList.clear();
            jsonList = obj.toList();
            pJsonRoot = cJSON_CreateArray();
            for (JsonList::const_iterator iter = jsonList.begin();
                 iter != jsonList.end(); ++iter) {
                cJSON_AddItemToArray(pJsonRoot, obj2json(*iter));
            }
            break;
        case JsonType::MapType:
            jsonMap.clear();
            jsonMap = obj.toMap();
            pJsonRoot = cJSON_CreateObject();
            for (JsonMap::const_iterator iter = jsonMap.begin();
                 iter != jsonMap.end(); ++iter) {
                cJSON_AddItemToObject(pJsonRoot, iter->first.toString().c_str(), obj2json(iter->second));
            }
            break;
        default:
            break;
        }
        return pJsonRoot;
    }

    JsonType json2obj(const cJSON *pJsonRoot) {

        //创建json根节点
        cJSON *pChild = NULL;
        JsonList jsonList;
        JsonMap jsonMap;

        //解析类型
        if (pJsonRoot) {
            switch (pJsonRoot->type) {
            case cJSON_NULL:
                return JsonType();
                break;
            case cJSON_False:
                return JsonType(false);
                break;
            case cJSON_True:
                return JsonType(true);
                break;
            case cJSON_Number:
                return JsonType(pJsonRoot->valuedouble);
                break;
            case cJSON_String:
                return JsonType(pJsonRoot->valuestring);
                break;
            case cJSON_Array:
                jsonList.clear();
                pChild = pJsonRoot->child;
                while(pChild) {
                    jsonList.push_back(json2obj(pChild));
                    pChild = pChild->next;
                }
                return JsonType(jsonList);
                break;
            case cJSON_Object:
                jsonMap.clear();
                pChild = pJsonRoot->child;
                while(pChild) {
                    jsonMap.insert(JsonMap::value_type(JsonType(pChild->string), json2obj(pChild)));
                    pChild = pChild->next;
                }
                return JsonType(jsonMap);
                break;
            default:
                break;
            }
        }
        return JsonType();
    }

    string objectToString(const JsonType &obj) {
        cJSON *pJson = obj2json(obj);
        string str;
        if (pJson) {
            const char *pCh = cJSON_PrintUnformatted(pJson);
            if (pCh) {
                str = pCh;
                free((void*)pCh);
            }
            cJSON_Delete(pJson);
        }
        return str;
    }

    JsonType stringToObject(const string &json) {
        cJSON *pJson = cJSON_Parse(json.c_str());
        JsonType obj = json2obj(pJson);
        if (pJson) {
            cJSON_Delete(pJson);
        }
        return obj;
    }

}
