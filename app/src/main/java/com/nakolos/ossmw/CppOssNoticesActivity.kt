package com.nakolos.ossmw

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView

class CppOssNoticesActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_cpp_oss_notices)
        var cpp_notices_text = findViewById<TextView>(R.id.cpp_notices_text_id)
        cpp_notices_text.setText(R.string.cpp_notices_text);
    }
}