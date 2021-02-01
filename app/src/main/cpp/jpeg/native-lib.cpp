#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<android/log.h>

// 加密的秘钥
char password[] = "Good good study day day up";


void crypt_file(char *path, char *path1);

extern "C"
JNIEXPORT void JNICALL
Java_com_lly_ndkoperator_jpeg_JpegActivity_00024Companion_cryptFile(JNIEnv *env, jobject thiz, jstring in_file, jstring crypt_fil) {
    char *normal_path = const_cast<char *>(env->GetStringUTFChars(in_file, nullptr));
    char *crypt_path = const_cast<char *>(env->GetStringUTFChars(crypt_fil, nullptr));
    crypt_file(normal_path, crypt_path);

}


void crypt_file(char *normalPath, char *cryptPath) {

    FILE *normal_fp = fopen(normalPath, "rb");
    FILE *crypt_fp = fopen(cryptPath, "wb");

    //一次读取一个字符
    int ch;
    int i = 0; //循环使用密码中的字母进行异或运算
    int pwd_len = strlen(password); //密码的长度
    while ((ch = fgetc(normal_fp)) != EOF) { //End of File
        //写入（异或运算）
        fputc(ch ^ password[i % pwd_len], crypt_fp);
        i++;
    }
    // 关闭
    fclose(crypt_fp);
    fclose(normal_fp);
}


