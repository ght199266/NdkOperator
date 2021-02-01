package com.lly.ndkoperator.jpeg

import android.Manifest.permission.WRITE_EXTERNAL_STORAGE
import android.content.pm.PackageManager
import android.graphics.BitmapFactory
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.View
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.lly.ndkoperator.R
import java.io.File
import java.util.jar.Manifest

class JpegActivity : AppCompatActivity() {

    private val normalPath = Environment.getExternalStorageDirectory().absolutePath + "/video.jpg"
    private val cryptPath = Environment.getExternalStorageDirectory().absolutePath + "/test.jpg"

    val permission = arrayOf(android.Manifest.permission.READ_EXTERNAL_STORAGE, android.Manifest.permission.WRITE_EXTERNAL_STORAGE);

    init {
        System.loadLibrary("native-lib")
    }

    companion object {

        //图片加密
        external fun cryptFile(inFile: String, cryptFil: String)

        //图片压缩
        external fun compressImage(souseFilePath: String, outFilePath: String)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_jpeg)
//
//        val bitmap = BitmapFactory.decodeFile(normalPath)
//        Log.v("test", "bitmap:=$bitmap")
    }


    fun onCrypt(view: View) {
        if (ContextCompat.checkSelfPermission(
                this,
                android.Manifest.permission.READ_EXTERNAL_STORAGE
            ) != PackageManager.PERMISSION_GRANTED
        ) {
        }
        ActivityCompat.requestPermissions(
            this,
            arrayOf(android.Manifest.permission.READ_EXTERNAL_STORAGE, android.Manifest.permission.WRITE_EXTERNAL_STORAGE),
            1
        )
        if (File(normalPath).exists()) {
            cryptFile(normalPath, cryptPath)
        }
    }

    fun doCompress(view: View) {
        //压缩图片
        compressImage(normalPath, cryptPath)
    }
}