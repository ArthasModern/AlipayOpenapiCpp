#include <iostream>
#include "stdlib.h"

#define _DEBUG

#include "openapi/openapi_client.h"

using namespace std;

/** ++++++++++++++++++++++++++++++++++++++++++++++++ **/
/** 此处替换为开发者在支付宝开放平台申请的应用ID **/
string appId = "2016000066668888";

/** 此处替换为开发者使用openssl生成的rsa私钥 **/
string pKey = "-----BEGIN RSA PRIVATE KEY-----\n"
		"MIICXQIBAAKBgQDTI0/RaV/YKWDGbKkQGYpD9I/UljBCCf3rWm09sXiif8MN5rLA\n"
		"3TjC4gZ478n6Dys5yO23h1HGVTWu+mQ8071+pwLHGQ+dyDNrGWR89VLb9yanOeRf\n"
		"efOcN19ATZgGAzheM28E/iqaYkh8F2NlCjOiZAsBG6eVvxachwVAQUIWwwIDAQAB\n"
		"AoGAdVr8Q46JenHNW50L/2niw1DNHUF5g0tgeo+hhpf9UH0pIrHnC3Iq2Y+eP1ww\n"
		"7K+/u/elwcwSNOYp159PVcvvV9LwPwH29DdH6KEWIDiyFpjbXPcMMFwgakyLnFTL\n"
		"sxxa6DYznFokT+IPkF6esoypa7VQFU1RIal5Sgphq7CGCDECQQDqyL3QjYT6ffLd\n"
		"NRiMBB13+eIxvXGy5AEQcH4pNt6kYHWONCWeZ34miNp2UliIBvBHZ1uuGoO4F/Jx\n"
		"2sWwWlSpAkEA5jeQGFx/RDzzi0qPMpSOR50d2IC4NbbresY+hgJEBbI6n5hPR1ts\n"
		"MUuO1e3L5I5rzRKNzD1um1DdSgmqaqmHiwJBANLnRpNsPRMjRqHtS0Kjg7E9mDIk\n"
		"Qll3NXmGA96T+oXgXFlEgLJ9tzV4Y/471GlFClyp/RI1oTMi19fstP7I9hkCQBjr\n"
		"bseUS5phVqN/QJzjA7uwwChNVqNJ15eEmgP7fs13C213GS3KMZ3sZdu2T9m/qN+b\n"
		"4Il5JN3fFPUMssu06h0CQQCGBdmtRLi+9ws57qTPHR/BdHGUxdBRWllc9sGVVaRw\n"
		"+EOMGXus6/BssTRjwplx7w8uUR0U3s1KYDJMHMCjW25x\n"
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
