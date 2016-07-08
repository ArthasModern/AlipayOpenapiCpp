#include "http_client.h"

#define HTTP_TIMEOUT_MSECS    120000

string HttpClient::proxyIp;
string HttpClient::proxyPort;
string HttpClient::proxyUserName;
string HttpClient::proxyPassword;

HttpClient::HttpClient() {
    CURLcode init_ret = curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    responseEntity.clear();
}

HttpClient::~HttpClient() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

void HttpClient::setProxy(const string &proxyIp,
                          const string &proxyPort,
                          const string &proxyUserName,
                          const string &proxyPassword) {
    HttpClient::proxyIp       = proxyIp;
    HttpClient::proxyPort     = proxyPort;
    HttpClient::proxyUserName = proxyUserName;
    HttpClient::proxyPassword = proxyPassword;
}

size_t HttpClient::write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    int length = size * nmemb;
    HttpClient *client = (HttpClient *)stream;
    client->responseEntity = string((char *)ptr, length);
    return length;
}

string HttpClient::sendSyncRequest(const string &url,
                                   const string &requestEntity,
                                   const HeaderMap &headers) {
    if(curl == NULL) {
        DebugLog("%s | %s", url.c_str(), "curl easy init error");
        return string();
    }

    struct curl_slist * headerlist = NULL;
    string headitem;
    for (HeaderMap::const_iterator iter = headers.begin(); iter != headers.end(); ++iter) {
        headitem = iter->first;
        headitem += " : ";
        headitem += iter->second;
        headerlist = curl_slist_append(headerlist, headitem.c_str());
        headitem.clear();
    }

    //curl_easy_setopt(curl, CURLOPT_VERBOSE,          1L);//调试信息打开
    //curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER,   0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,   0L);
    curl_easy_setopt(curl, CURLOPT_HEADER,           1);
    curl_easy_setopt(curl, CURLOPT_URL,              url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST,             1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,       requestEntity.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,       headerlist);

    if (!proxyIp.empty() && !proxyPort.empty()) {
        string proxyStr = proxyIp;
        proxyStr += ":";
        proxyStr += proxyPort;
        curl_easy_setopt(curl, CURLOPT_PROXY,        proxyStr.c_str());
        if (!proxyUserName.empty() && !proxyPassword.empty()) {
            string proxyUserPwd = proxyUserName;
            proxyUserPwd += ":";
            proxyUserPwd += proxyPassword;
            curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, proxyUserPwd.c_str());
        }
    }

    DebugLog("send:%s", requestEntity.c_str());

    responseEntity.clear();

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,    write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,        this);

    curl_easy_perform(curl);
    curl_slist_free_all(headerlist);

    DebugLog("resv:%s", responseEntity.c_str());

    return responseEntity;
}

string HttpClient::sendSyncRequest(const string &url,
                                   const ParamsMap &paramsMap,
                                   const HeaderMap &headers) {
    string requestEntity;
    string item;
    for (ParamsMap::const_iterator iter = paramsMap.begin(); iter != paramsMap.end(); ++iter) {
        const char *key = iter->first.c_str();
        char *encodedKey = curl_easy_escape(curl, key, strlen(key));
        if (encodedKey) {
            item = encodedKey;
        }
        item += "=";
        const char *value = iter->second.c_str();
        char *encodedValue = curl_easy_escape(curl, value, strlen(value));
        if (encodedValue) {
            item += encodedValue;
        }
        if (!requestEntity.empty()) {
            requestEntity.push_back('&');
        }
        requestEntity.append(item);
        item.clear();
        if (encodedKey) {
            curl_free(encodedKey);
        }
        if (encodedValue) {
            curl_free(encodedValue);
        }
    }
    string responseStr = sendSyncRequest(url, requestEntity, headers);
    return responseStr;
}
