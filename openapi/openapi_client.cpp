#include "openapi_client.h"


const string OpenapiClient::default_charset      = "utf-8";
const string OpenapiClient::default_url          = "https://openapi.alipay.com/gateway.do";
const string OpenapiClient::default_sign_type    = "RSA";
const string OpenapiClient::default_version      = "2.0";

const string OpenapiClient::KEY_APP_ID           = "app_id";
const string OpenapiClient::KEY_METHOD           = "method";
const string OpenapiClient::KEY_CHARSET          = "charset";
const string OpenapiClient::KEY_SIGN_TYPE        = "sign_type";
const string OpenapiClient::KEY_SIGN             = "sign";
const string OpenapiClient::KEY_TIMESTAMP        = "timestamp";
const string OpenapiClient::KEY_VERSION          = "version";
const string OpenapiClient::KEY_BIZ_CONTENT      = "biz_content";


OpenapiClient::OpenapiClient(const string &appId,
                             const string &privateKey,
                             const string &url,
                             const string &charset,
                             const string &alipayPublicKey)
    : appId(appId),
      privateKey(privateKey),
      signType(default_sign_type),
      version(default_version),
      url(url),
      charset(charset),
      alipayPublicKey(alipayPublicKey) {

}

JsonMap OpenapiClient::invoke(const string &method, const JsonMap &contentMap, const StringMap &extendParamMap) {

    string content = JsonUtil::objectToString(JsonType(contentMap));
    string responseContent = invoke(method, content, extendParamMap);
    JsonType jsonObj = JsonUtil::stringToObject(responseContent);
    return jsonObj.toMap();
}

string OpenapiClient::invoke(const string &method, const string &content, const StringMap &extendParamMap) {

    time_t t = time(0);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %X", localtime(&t));

    StringMap requestPairs;
    requestPairs.insert(StringMap::value_type(OpenapiClient::KEY_APP_ID, appId));
    requestPairs.insert(StringMap::value_type(OpenapiClient::KEY_BIZ_CONTENT, content));
    requestPairs.insert(StringMap::value_type(OpenapiClient::KEY_CHARSET, charset));
    requestPairs.insert(StringMap::value_type(OpenapiClient::KEY_METHOD, method));
    requestPairs.insert(StringMap::value_type(OpenapiClient::KEY_SIGN_TYPE, signType));
    requestPairs.insert(StringMap::value_type(OpenapiClient::KEY_TIMESTAMP, tmp));
    requestPairs.insert(StringMap::value_type(OpenapiClient::KEY_VERSION, version));

    /** 追加外部传入的网关的补充参数，如notify_url等 **/
    for (StringMap::const_iterator iter = extendParamMap.begin(); iter != extendParamMap.end(); ++iter) {
        requestPairs.insert(StringMap::value_type(iter->first, iter->second));
    }

    string wholeContent = buildContent(requestPairs);
    string sign = OpenapiClient::rsaSign(wholeContent, privateKey);
    requestPairs.insert(StringMap::value_type(OpenapiClient::KEY_SIGN, sign));

    wholeContent = buildContent(requestPairs);
    DebugLog("Request:%s", wholeContent.c_str());

    HttpClient httpClient;
    string responseStr = httpClient.sendSyncRequest(url, requestPairs);

    DebugLog("Response:%s", responseStr.c_str());

    string responseContent = analyzeResponse(responseStr);
    return responseContent;
}

/**
 *
 * STL map default sort order by key
 *
 * STL map 默认按照key升序排列
 * 这里要注意如果使用的map必须按key升序排列
 *
 */
string OpenapiClient::buildContent(const StringMap &contentPairs) {

    string content;
    for (StringMap::const_iterator iter = contentPairs.begin();
         iter != contentPairs.end(); ++iter) {
        if (!content.empty()) {
            content.push_back('&');
        }
        content.append(iter->first);
        content.push_back('=');
        content.append(iter->second);
    }
    return content;
}

string OpenapiClient::analyzeResponse(const string &responseStr) {

    JsonType responseObj = JsonUtil::stringToObject(responseStr);
    JsonMap responseMap = responseObj.toMap();
    //获取返回报文中的alipay_xxx_xxx_response的内容;
    int beg = responseStr.find("_response\"");
    int end = responseStr.rfind("\"sign\"");
    if (beg < 0 || end < 0) {
        return string();
    }
    beg = responseStr.find('{', beg);
    end = responseStr.rfind('}', end);
    //注意此处将map转为json之后的结果需要与支付宝返回报文中原格式与排序一致;
    //排序规则是节点中的各个json节点key首字母做字典排序;
    //Response的Json值内容需要包含首尾的“{”和“}”两个尖括号，双引号也需要参与验签;
    //如果字符串中包含“http://”的正斜杠，需要先将正斜杠做转义，默认打印出来的字符串是已经做过转义的;
    //此处转换之后的json字符串默认为"Compact"模式，即紧凑模式，不要有空格与换行;
    string responseContent = responseStr.substr(beg, end - beg + 1);

    DebugLog("ResponseContent:%s", responseContent.c_str());

    //此处为校验支付宝返回报文中的签名;
    //如果支付宝公钥为空，则默认跳过该步骤，不校验签名;
    //如果支付宝公钥不为空，则认为需要校验签名;
    if (!alipayPublicKey.empty()) {

        DebugLog("AlipayPublicKey:%s", alipayPublicKey.c_str());

        JsonMap::const_iterator iter = responseMap.find(OpenapiClient::KEY_SIGN);
        if (iter == responseMap.end()) {
            DebugLog("Cannot get Sign from response, Verify Failed");
            return string();
        }
        //获取返回报文中的sign;
        string responseSign = iter->second.toString();

        DebugLog("ResponseSign:%s", responseSign.c_str());

        //调用验签方法;
        bool verifyResult = OpenapiClient::rsaVerify(responseContent, responseSign, alipayPublicKey);

        if (!verifyResult) {
            DebugLog("Verify Failed");
            return string();
        }
        DebugLog("Verify Success");
    } else {
        DebugLog("AlipayPublicKey is empty, Skip the Verify");
    }

    return responseContent;
}

string OpenapiClient::rsaSign(const string &content, const string &key) {

    string signed_str;
    const char *key_cstr = key.c_str();
    int key_len = strlen(key_cstr);
    BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSAPrivateKey(p_key_bio, NULL, NULL, NULL);

    if (p_rsa != NULL) {

        const char *cstr = content.c_str();
        unsigned char hash[SHA_DIGEST_LENGTH] = {0};
        SHA1((unsigned char *)cstr, strlen(cstr), hash);
        unsigned char sign[XRSA_KEY_BITS / 8] = {0};
        unsigned int sign_len = sizeof(sign);
        int r = RSA_sign(NID_sha1, hash, SHA_DIGEST_LENGTH, sign, &sign_len, p_rsa);

        if (0 != r && sizeof(sign) == sign_len) {
            signed_str = base64Encode(sign, sign_len);
        }
    }

    RSA_free(p_rsa);
    BIO_free(p_key_bio);
    return signed_str;
}

bool OpenapiClient::rsaVerify(const string &content, const string &sign, const string &key) {

    bool result = false;
    const char *key_cstr = key.c_str();
    int key_len = strlen(key_cstr);
    BIO *p_key_bio = BIO_new_mem_buf((void *)key_cstr, key_len);
    RSA *p_rsa = PEM_read_bio_RSA_PUBKEY(p_key_bio, NULL, NULL, NULL);

    if (p_rsa != NULL) {
        const char *cstr = content.c_str();
        unsigned char hash[SHA_DIGEST_LENGTH] = {0};
        SHA1((unsigned char *)cstr, strlen(cstr), hash);
        unsigned char sign_cstr[XRSA_KEY_BITS / 8] = {0};
        int len = XRSA_KEY_BITS / 8;
        base64Decode(sign, sign_cstr, len);
        unsigned int sign_len = XRSA_KEY_BITS / 8;
        int r = RSA_verify(NID_sha1, hash, SHA_DIGEST_LENGTH, (unsigned char *)sign_cstr, sign_len, p_rsa);

        if (r > 0) {
            result = true;
        }
    }

    RSA_free(p_rsa);
    BIO_free(p_key_bio);
    return result;
}

string OpenapiClient::base64Encode(const unsigned char *bytes, int len) {

    BIO *bmem = NULL;
    BIO *b64 = NULL;
    BUF_MEM *bptr = NULL;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, bytes, len);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    string str = string(bptr->data, bptr->length);
    BIO_free_all(b64);
    return str;
}

bool OpenapiClient::base64Decode(const string &str, unsigned char *bytes, int &len) {

    const char *cstr = str.c_str();
    BIO *bmem = NULL;
    BIO *b64 = NULL;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bmem = BIO_new_mem_buf((void *)cstr, strlen(cstr));
    b64 = BIO_push(b64, bmem);
    len = BIO_read(b64, bytes, len);

    BIO_free_all(b64);
    return len > 0;
}


string OpenapiClient::getAppId() {
    return appId;
}

string OpenapiClient::getSignType() {
    return signType;
}

string OpenapiClient::getVersion() {
    return version;
}

string OpenapiClient::getCharset() {
    return charset;
}

string OpenapiClient::getUrl() {
    return url;
}

string OpenapiClient::getAlipayPublicKey() {
    return alipayPublicKey;
}
