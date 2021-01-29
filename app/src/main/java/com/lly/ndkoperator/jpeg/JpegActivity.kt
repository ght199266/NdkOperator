package com.lly.ndkoperator.jpeg

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.lly.ndkoperator.R
import java.io.File

class JpegActivity : AppCompatActivity() {

    init {
        System.loadLibrary("native-lib")
    }

     companion object {

        external fun cryptFile(inFile: String, cryptFil: String)
    }


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_jpeg)
    }

}