#include <jni.h>
#include <string>
#include<android/log.h>

#include "md5.h"

//追加字符串
static char *applyStr = "test";


#define TAG "myDemo-jni" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lly_ndkoperator_http_HttpParamsNative_httpMd5Encryption(JNIEnv *env, jobject thiz,
                                                                 jstring str) {

    const char *params = env->GetStringUTFChars(str, nullptr);
    std::string sign_str(params);
//    sign_str.append(applyStr, sign_str.length());
    sign_str.substr(0, 4);
    LOGD("333333 %s", sign_str.c_str());


    MD5_CTX md5Ctx = {0};
    MD5Init(&md5Ctx);
    MD5Update(&md5Ctx, (unsigned char *) sign_str.c_str(), sign_str.length());
    unsigned char digest[16];
    MD5Final(digest, &md5Ctx);
    int i = 0;
    char szMd5[32] = {0};
    for (i = 0; i < 16; i++) {
        sprintf(szMd5, "%s%02x", szMd5, digest[i]);
    }
    //20379e75e2cb0e52d809f4430e614ddd
    //20379e75e2cb0e52d809f4430e614ddd
    //53a24f9e4e5909733ef781264abb318b
    env->ReleaseStringUTFChars(str, params);
    return env->NewStringUTF(reinterpret_cast<const char *>(szMd5));
}