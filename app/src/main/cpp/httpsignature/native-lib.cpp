#include <jni.h>
#include <string>
#include<android/log.h>

#include "md5.h"

//追加字符串
static char *applyStr = "02a21f4f2b2fe";

//App應用報名和簽名
static const char *PACKET_NAME = "com.lly.ndkoperator";
static const char *SIGNATUR = "308201dd30820146020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b3009060355040613025553301e170d3139303632373133333933385a170d3439303631393133333933385a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330819f300d06092a864886f70d010101050003818d003081890281810080f75eb41e08a69f8eda1c5cf84e6b019fbf9a926dea52f4b263c63055f592aba692abcb3013cead131969ef44120bd13b7d55d3089f4e75c2b4001d6b9e6748f892f4dc07962772d5bf29da96f2c1015706f5a5a333b1566fa34fddf9afd6c09ecd7312f7bf134f6ea6174092e925c4030325ed3afa7179b031167e556232cd0203010001300d06092a864886f70d0101050500038181003badbd39b04a41d8ca141d14ebc4f0b14a05a234fe193d7e8020ff11325b9d51201f5bd44ce73183517578b698851455787612a7ebaad57085e6c3aa8028cbc36c832d08e049661bf846f01bf583d0071eaf5034df3d2ace0e1a4f004c7d66fad0800f11ff1fddfc109ff3ce6d423da8c7a56ad4a49e12160abb41ac8f8ca0d1";


#define TAG "lly-jni"// 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型

extern "C"
JNIEXPORT jstring JNICALL
Java_com_lly_ndkoperator_http_HttpParamsNative_httpMd5Encryption(JNIEnv *env, jobject thiz,
                                                                 jstring str) {
    const char *params = env->GetStringUTFChars(str, nullptr);

    std::string sign_str(params);
    //截取再追加,shij
    sign_str = sign_str.substr(0, sign_str.length() - 2);
    sign_str.append(applyStr, sign_str.length());
    //Md5加密
    MD5_CTX md5Ctx = {0};
    MD5Init(&md5Ctx);
    MD5Update(&md5Ctx, (unsigned char *) sign_str.c_str(), sign_str.length());
    unsigned char digest[16];
    MD5Final(digest, &md5Ctx);
    int i;
    char szMd5[32] = {0};
    for (i = 0; i < 16; i++) {
        sprintf(szMd5, "%s%02x", szMd5, digest[i]);
    }
    env->ReleaseStringUTFChars(str, params);
    return env->NewStringUTF(reinterpret_cast<const char *>(szMd5));
}


/**
 * 验证签名
 *      val pageInfo =
            packageManager.getPackageInfo(this.packageName, PackageManager.GET_SIGNATURES)
        val signatures = pageInfo.signatures
        val byteArray = signatures[0].toCharsString()
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_lly_ndkoperator_http_HttpParamsNative_verifyAppSign(JNIEnv *env, jobject thiz,
                                                             jobject ctx) {
    jclass clz = env->GetObjectClass(ctx);
    jmethodID jGetPageNameMethodID = env->GetMethodID(clz, "getPackageName",
                                                      "()Ljava/lang/String;");
    auto pageName = (jstring) env->CallObjectMethod(ctx, jGetPageNameMethodID);
    const char *str = env->GetStringUTFChars(pageName, nullptr);
    if (strcmp(str, PACKET_NAME) != 0) {
        LOGD("test %s", "包名不一致");
        return;
    }
    //packageManager
    jmethodID jGetPackageManagerMethodID = env->GetMethodID(clz, "getPackageManager",
                                                            "()Landroid/content/pm/PackageManager;");
    jobject packageManagerObject = static_cast<jclass>(env->CallObjectMethod(ctx,
                                                                             jGetPackageManagerMethodID));
    //getPageInfo
    jclass packageManagerClass = env->GetObjectClass(packageManagerObject);
    jmethodID jGetPackageInfoMethodID = env->GetMethodID(packageManagerClass, "getPackageInfo",
                                                         "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject packageInfo = env->CallObjectMethod(packageManagerObject, jGetPackageInfoMethodID,
                                                pageName, 0x00000040);
    //获取 SIGNATURES
    jclass packageInfoClass = env->GetObjectClass(packageInfo);
    jfieldID jSignaturesFieldID = env->GetFieldID(packageInfoClass, "signatures",
                                                  "[Landroid/content/pm/Signature;");
    auto signatureArray = static_cast<jobjectArray>(env->GetObjectField(packageInfo,
                                                                        jSignaturesFieldID));
    jobject signature = env->GetObjectArrayElement(signatureArray, 0);
    //toCharsString
    jclass signatureClass = env->GetObjectClass(signature);
    jmethodID jtoCharsStringMethodID = env->GetMethodID(signatureClass, "toCharsString",
                                                        "()Ljava/lang/String;");

    auto signData = static_cast<jstring>(env->CallObjectMethod(signature, jtoCharsStringMethodID));

    const char *sign = env->GetStringUTFChars(signData, nullptr);

    if (strcmp(SIGNATUR, sign) != 0) {
        LOGD("test %s", "签名不正确");
        return;
    }
    LOGD("test %s", "签名正确");
}