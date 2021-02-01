#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<android/log.h>
#include <android/bitmap.h>
#include <setjmp.h>


#define TAG "lly-jni"// 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型

//压缩
typedef uint8_t BYTE;



// 加密的秘钥
char password[] = "Good good study day day up";

void crypt_file(char *path, char *path1);


//统一编译方式
extern "C" {
#include "jpeglib.h"
#include "jconfig.h"
#include "turbojpeg.h"
}



struct my_error_mgr {
    struct jpeg_error_mgr pub;
    jmp_buf setjmp_buffer; /* for return to caller */
};
typedef struct my_error_mgr *my_error_ptr;

METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr) cinfo->err;
    (*cinfo->err->output_message)(cinfo);
    LOGD("jpeg_message_table[%d]:%s", myerr->pub.msg_code,
         myerr->pub.jpeg_message_table[myerr->pub.msg_code]);
    longjmp(myerr->setjmp_buffer, 1);
}


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


int write_JPEG_file(BYTE *data, int w, int h, int quality,
                    const char *outFilename, jboolean optimize) {
    //jpeg的结构体，保存的比如宽、高、位深、图片格式等信息
    struct jpeg_compress_struct cinfo;

    /* Step 1: allocate and initialize JPEG compression object */

    /* We set up the normal JPEG error routines, then override error_exit. */
    struct my_error_mgr jem;
    cinfo.err = jpeg_std_error(&jem.pub);
    jem.pub.error_exit = my_error_exit;
    /* Establish the setjmp return context for my_error_exit to use. */
    if (setjmp(jem.setjmp_buffer)) {
        /* If we get here, the JPEG code has signaled an error.
         and return.
         */
        return -1;
    }
    jpeg_create_compress(&cinfo);

    /* Step 2: specify data destination (eg, a file) */

    FILE *outfile = fopen(outFilename, "wb");
    if (outfile == nullptr) {
        LOGD("can't open %s", outFilename);
        return -1;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    /* Step 3: set parameters for compression */

    cinfo.image_width = w;      /* image width and height, in pixels */
    cinfo.image_height = h;
    cinfo.input_components = 3;           /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB;       /* colorspace of input image */


    cinfo.optimize_coding = optimize;
    //哈夫曼编码和算术编码，TRUE=arithmetic coding, FALSE=Huffman
    if (optimize) {
        cinfo.arith_code = false;
    } else {
        cinfo.arith_code = true;
    }
    // 其它参数 全部设置默认参数
    jpeg_set_defaults(&cinfo);
    //设置质量
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);


    /* Step 4: Start compressor */

    jpeg_start_compress(&cinfo, TRUE);


    /* Step 5: while (scan lines remain to be written) */
    /*           jpeg_write_scanlines(...); */

    JSAMPROW row_pointer[1];
    int row_stride;
    //一行的RGB数量
    row_stride = cinfo.image_width * 3; /* JSAMPLEs per row in image_buffer */
    //一行一行遍历
    while (cinfo.next_scanline < cinfo.image_height) {
        //得到一行的首地址
        row_pointer[0] = &data[cinfo.next_scanline * row_stride];
        //此方法会将jcs.next_scanline加1
        jpeg_write_scanlines(&cinfo, row_pointer, 1);//row_pointer就是一行的首地址，1：写入的行数
    }
    /* Step 6: Finish compression */
    jpeg_finish_compress(&cinfo);
    /* After finish_compress, we can close the output file. */
    fclose(outfile);
    outfile = nullptr;

    /* Step 7: release JPEG compression object */

    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_compress(&cinfo);

    /* And we're done! */
    return 0;
}





/**
 * 压缩图片
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_lly_ndkoperator_jpeg_JpegActivity_00024Companion_compressImage(JNIEnv *env, jobject thiz, jstring souse_file_path,
                                                                        jstring out_file_path) {

    jclass bitmap_factory = env->FindClass("android/graphics/BitmapFactory");
    jmethodID jDecodeFileMethodID = env->GetStaticMethodID(bitmap_factory, "decodeFile", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    jobject jBitmap = env->CallStaticObjectMethod(bitmap_factory, jDecodeFileMethodID, souse_file_path);

    if (jBitmap == nullptr) {
        LOGD("bitmap %s", "Bitmap 生成失败");
        return;
    }

    AndroidBitmapInfo android_bitmap_info;
    AndroidBitmap_getInfo(env, jBitmap, &android_bitmap_info);

    // 获取bitmap的 宽，高，format
    int bitmap_width = android_bitmap_info.width;
    int bitmap_height = android_bitmap_info.height;
    int format = android_bitmap_info.format;

    if (format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        return;
    }
    // 2.解析Bitmap的像素信息，并转换成RGB数据,保存到二维byte数组里面
    BYTE *pixelscolor;
    // 2.1 锁定画布
    AndroidBitmap_lockPixels(env, jBitmap, (void **) &pixelscolor);
    // 2.2 解析初始化参数值
    BYTE *data;
    BYTE r, g, b;
    data = (BYTE *) malloc(bitmap_width * bitmap_height * 3);//每一个像素都有三个信息RGB
    BYTE *tmpData;
    tmpData = data;//临时保存data的首地址
    int i = 0, j = 0;
    int color;
    //2.3 解析每一个像素点里面的rgb值(去掉alpha值)，保存到一维数组data里面
    for (i = 0; i < bitmap_height; ++i) {
        for (j = 0; j < bitmap_width; ++j) {
            //获取二维数组的每一个像素信息首地址
            color = *((int *) pixelscolor);
            r = ((color & 0x00FF0000) >> 16);
            g = ((color & 0x0000FF00) >> 8);
            b = ((color & 0x000000FF));
            //保存到data数据里面
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data = data + 3;
            // 一个像素包括argb四个值，每+4就是取下一个像素点
            pixelscolor += 4;
        }
    }
    // 2.4. 解锁Bitmap
    AndroidBitmap_unlockPixels(env, jBitmap);


    // jstring --> c char
    char *path = (char *) (env)->GetStringUTFChars(out_file_path, 0);

    LOGD("path= %s", path);

    jboolean optimize = true;
    int quality = 30;

    // Libjpeg进行压缩
    int resultCode = write_JPEG_file(tmpData, bitmap_width, bitmap_height, quality, path, optimize);
    if (resultCode == -1) {
        LOGD("compress fail %s", "压缩失败");
        return;
    }
    LOGD("compress fail %s", "压缩成功");
    env->ReleaseStringUTFChars(out_file_path, path);
    free(tmpData);
    tmpData = nullptr;
}
