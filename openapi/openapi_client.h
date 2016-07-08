#ifndef OPENAPICLIENT_H
#define OPENAPICLIENT_H

#include "http/http_client.h"
#include "json/json_util.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <time.h>

#ifndef XRSA_KEY_BITS
#define XRSA_KEY_BITS (1024)
#endif

/** STL map default sort order by key **/
typedef map<string, string> StringMap;

/**
 * @brief The OpenapiClient class
 */
class OpenapiClient {
public:
    OpenapiClient(const string &appId,
                  const string &privateKey,
                  const string &url = default_url,
                  const string &charset = default_charset,
                  const string &alipayPublicKey = string());

public:
    static const string default_charset;
    static const string default_url;
    static const string default_sign_type;
    static const string default_version;

    static const string KEY_APP_ID;
    static const string KEY_METHOD;
    static const string KEY_CHARSET;
    static const string KEY_SIGN_TYPE;
    static const string KEY_SIGN;
    static const string KEY_TIMESTAMP;
    static const string KEY_VERSION;
    static const string KEY_BIZ_CONTENT;

private:
    string appId;
    string privateKey;
    string signType;
    string version;
    string charset;
    string url;

    string alipayPublicKey;

public:
    string invoke(const string &method, const string &content, const StringMap &extendParamMap = StringMap());
    JsonMap invoke(const string &method, const JsonMap &contentMap, const StringMap &extendParamMap = StringMap());


private:
    /**
     *
     * STL map default sort order by key
     *
     * STL map 默认按照key升序排列
     * 这里要注意如果使用的map必须按key升序排列
     *
     */
    string buildContent(const StringMap &contentPairs);
    string analyzeResponse(const string &responseStr);

public:
    static string base64Encode(const unsigned char *bytes, int len);
    static bool base64Decode(const string &str, unsigned char *bytes, int &len);

    static string rsaSign(const string &content, const string &key);
    static bool rsaVerify(const string &content, const string &sign, const string &key);

public:
    string getAppId();

    string getSignType();

    string getVersion();

    string getCharset();

    string getUrl();

    string getAlipayPublicKey();

};

#endif // OPENAPICLIENT_H
