package com.lly.ndkoperator.http

/**
 * HttpParamsNative[v 1.0.0]
 * classes:com.lly.ndkoperator.http.HttpParamsNative
 * @author lly
 * @date 2021/1/27
 * @time 14:30
 * @description
 */
object HttpParamsNative {


    init {
        System.loadLibrary("native-lib")
    }

    external fun httpMd5Encryption(params: String): String

}