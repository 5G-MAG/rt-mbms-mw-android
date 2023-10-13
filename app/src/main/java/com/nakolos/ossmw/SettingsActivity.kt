package com.nakolos.ossmw

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.preference.Preference
import androidx.preference.PreferenceFragmentCompat
import com.google.android.gms.oss.licenses.OssLicensesMenuActivity

class SettingsActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.settings_activity)
        if (savedInstanceState == null) {
            supportFragmentManager
                .beginTransaction()
                .replace(R.id.settings, SettingsFragment())
                .commit()
        }
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
    }

    class SettingsFragment : PreferenceFragmentCompat() {
        override fun onCreatePreferences(savedInstanceState: Bundle?, rootKey: String?) {
            setPreferencesFromResource(R.xml.root_preferences, rootKey)

        }
        override fun onCreate(savedInstanceState: Bundle?) {
            super.onCreate(savedInstanceState)
            findPreference<Preference>("version_string")?.summary = BuildConfig.VERSION_NAME

            findPreference<Preference>("show_license")?.setOnPreferenceClickListener {
                startActivity(Intent(getActivity(), LicenseActivity::class.java))
                true
            }
            findPreference<Preference>("show_oss_licenses")?.setOnPreferenceClickListener {
                startActivity(Intent(getActivity(), OssLicensesMenuActivity::class.java))
                true
            }
            findPreference<Preference>("show_oss_licenses2")?.setOnPreferenceClickListener {
                startActivity(Intent(getActivity(), CppOssNoticesActivity::class.java))
                true
            }
        }
    }
}