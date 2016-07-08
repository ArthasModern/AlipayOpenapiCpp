#include <iostream>
#include "stdlib.h"

#define _DEBUG

#include "openapi/openapi_client.h"

using namespace std;

/** ++++++++++++++++++++++++++++++++++++++++++++++++ **/
/** 此处替换为开发者在支付宝开放平台申请的应用ID **/
string appId = "2015122301032602";

/** 此处替换为开发者使用openssl生成的rsa私钥 **/
string pKey = "-----BEGIN RSA PRIVATE KEY-----\n"
              "MIICXAIBAAKBgQDnVEsJ2/eN7ggDqlIj8mCTSh5G/OgosETW/shrl0tTeQPMMeUG\n"
              "vNYDCgGuHPgOonwpj/62mWMgnjaZi5lWESySFCCTPlsICUNDqXLTQEfUyXb4J42/\n"
              "VUzmOZolcX+nqg2exbrQphdycYMPSeYxCYkdEzTDncemqhKUBQwswRHRLQIDAQAB\n"
              "AoGBALCdVr0sSBEAjvXdb+EqCDnr+LnSFZ4bFjPfSj16wXP1jMS1w1olim+/kBDS\n"
              "5UCnJYzF936KFXit0pngTy265v8c7MZj9UT0leaEUSSHn9XocdNC/3HdiNGjwvk3\n"
              "09onzmWpfpNghNtgRPsa+Y41j8EgB16jehWAGPtK6rF2+OuxAkEA9P5fBHNJvqmv\n"
              "7gTYQgePdr/DlesIuSa5X6TxviVRTrJrVSPVjzAw2cVUp8fEhkugXfPkngxuAYOo\n"
              "XjLoUuiEnwJBAPG4xT1DZw8XwAH0ZFgsbp7eRmkPQbUVIcGZ3Ed0W+eaFUuGpSPW\n"
              "EOEwa2mXJHV/eU/YcZO7gLnWtz2gabBlKrMCQHOLqMqTiRtIMnZCiLATmsD5zkfd\n"
              "kr6oGdOXGoCAXymLkn2UKfufUdrTa4q59+ESt+63wYIdAKDsNDNBNTss+KcCQGm3\n"
              "7RPxQca5FXIFaEmkDTGQd20FjjBbsKrS1yOtQXDW1A93MQyuWC2VFrVUtE5rVSxV\n"
              "tr7tF8yvmu7qFK9gX78CQBTCdPK2sk4k9nGRqnIr96adYPAcqh7QB/F6xcDM9G44\n"
              "ugNIzgXDjeS9e5AiTYad/YOhRMC9Uwfx9IgYYWIi8v4=\n"
              "-----END RSA PRIVATE KEY-----";

/** 支付宝公钥，用来验证支付宝返回请求的合法性 **/
string aliPubKey = "-----BEGIN PUBLIC KEY-----\n"
        "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDDI6d306Q8fIfCOaTXyiUeJHkr\n"
        "IvYISRcc73s3vF1ZT7XN8RNPwJxo8pWaJMmvyTn9N4HQ632qJBVHf8sxHi/fEsra\n"
        "prwCtzvzQETrNRwVxLO5jVmRGi60j8Ue1efIlzPXV9je9mkjzOmdssymZkh2QhUr\n"
        "CmZYI/FCEa3/cNMW0QIDAQAB\n"
        "-----END PUBLIC KEY-----";

/** 注：appid，私钥，支付宝公钥等信息建议不要写死在代码中 **/
/** 这些信息应以配置等方式保存，此处写在代码中只是为了示例的简便 **/
/** ++++++++++++++++++++++++++++++++++++++++++++++++ **/


/** some examples **/
JsonMap getPrecreateContent();

int main(int argc, char *argv[])
{

    /** 实例化OpenapiClient工具类 **/
    OpenapiClient openapiClient(appId,
                                pKey,
                                OpenapiClient::default_url,
                                OpenapiClient::default_charset,
                                aliPubKey);

    /** ++++++++++++++++++++++++++++++++++++++++++++++++ **/
    /** 各个具体业务接口参数组装模式具体参看Openapi官方文档 **/
    /** https://doc.open.alipay.com/ **/
    // demo1:当面付预下单示例
    string method = "alipay.trade.precreate";
    JsonMap contentMap = getPrecreateContent();

    /** ++++++++++++++++++++++++++++++++++++++++++++++++ **/

    /** ++++++++++++++++++++++++++++++++++++++++++++++++ **/
    /** 网关扩展参数，例如商户需要回传notify_url等，可以在extendParamMap中传入 **/
    /** 这是一个可选项，如不需要，可不传 **/
    /* StringMap extendParamMap;
    extendParamMap.insert(StringMap::value_type("notify_url", "http://api.test.alipay.net/atinterface/receive_notify.htm"));
    */

    /** ++++++++++++++++++++++++++++++++++++++++++++++++ **/

    /** 调用Openapi网关 **/
    JsonMap respMap;
    respMap = openapiClient.invoke(method, contentMap);
    /* 如果有扩展参数，则按如下方式传入
    respMap = openapiClient.invoke(method, contentMap, extendParamMap);
    */

    /** 解析支付宝返回报文 **/
    JsonMap::const_iterator iter = respMap.find("code");
    if (iter != respMap.end()) {
        string respCode = iter->second.toString();
        DebugLog("code:%s", respCode.c_str());
    } else {
        DebugLog("cannot get code from response");
    }

    iter = respMap.find("msg");
    if (iter != respMap.end()) {
        string respMsg = iter->second.toString();
        DebugLog("msg:%s", respMsg.c_str());
    } else {
        DebugLog("cannot get msg from response");
    }

    system("pause");
    return 0;
}

/**
 * 组装支付宝预下单业务请求
 */
JsonMap getPrecreateContent() {

    JsonMap contentMap;
    contentMap.insert(JsonMap::value_type(JsonType("out_trade_no"), JsonType("20160606121212")));
    contentMap.insert(JsonMap::value_type(JsonType("total_amount"), JsonType(0.01)));
    contentMap.insert(JsonMap::value_type(JsonType("subject"), JsonType("好东西")));

    return contentMap;
}
