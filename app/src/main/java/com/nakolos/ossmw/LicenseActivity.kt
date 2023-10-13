package com.nakolos.ossmw

import android.os.Bundle
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.nakolos.ossmw.databinding.ActivityLicenseBinding

class LicenseActivity : AppCompatActivity() {

    private lateinit var binding: ActivityLicenseBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityLicenseBinding.inflate(layoutInflater)
        setContentView(binding.root)
        title = "License"

        var actionbar = getSupportActionBar()
        actionbar?.setDisplayHomeAsUpEnabled(true)

        var licenseText = findViewById<TextView>(R.id.license_text_id)
        licenseText.setText(R.string.app_license_text);
    }
}