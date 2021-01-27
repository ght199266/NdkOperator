package com.lly.ndkoperator

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import com.lly.ndkoperator.http.HttpParamsNative

class MainActivity : AppCompatActivity() {

    private val requestParams = "userName=admin&password=123456"
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        findViewById<Button>(R.id.btn_md5).setOnClickListener {
            Log.v("test", "md5:" + HttpParamsNative.httpMd5Encryption(requestParams))
        }
    }

}