#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<android/log.h>

// 加密的秘钥
char password[] = "Big god take me fly!";


extern "C"
JNIEXPORT void JNICALL
Java_com_lly_ndkoperator_jpeg_JpegActivity_00024Companion_cryptFile(JNIEnv *env, jobject thiz,
                                                                    jstring in_file,
                                                                    jstring crypt_fil) {

    char *normal_path = const_cast<char *>(env->GetStringUTFChars(in_file, nullptr));


    char *crypt_path = const_cast<char *>(env->GetStringUTFChars(crypt_fil, nullptr));


}