// Copyright (C) 2023 Klaus Kühnhammer (Österreichische Rundfunksender GmbH & Co KG)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

package com.nakolos.ossmw

import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.media3.common.C
import androidx.media3.common.MediaItem
import androidx.media3.datasource.DataSource
import androidx.media3.datasource.DefaultHttpDataSource
import androidx.media3.datasource.HttpDataSource
import androidx.media3.exoplayer.DefaultLoadControl
import androidx.media3.exoplayer.ExoPlayer
import androidx.media3.exoplayer.hls.HlsMediaSource
import androidx.media3.exoplayer.source.*
import androidx.media3.exoplayer.upstream.DefaultLoadErrorHandlingPolicy
import androidx.media3.exoplayer.upstream.LoadErrorHandlingPolicy
import androidx.media3.ui.PlayerView
import com.nakolos.ossmw.databinding.ActivityMainBinding
import java.net.InetAddress

class RetryOnErrorPolicy : DefaultLoadErrorHandlingPolicy() {
    override fun getRetryDelayMsFor(loadErrorInfo: LoadErrorHandlingPolicy.LoadErrorInfo): Long {
        return if (loadErrorInfo.exception is HttpDataSource.InvalidResponseCodeException) {
            200
        } else {
            C.TIME_UNSET // Anything else is surfaced.
        }
    }
    override fun getMinimumLoadableRetryCount(dataType: Int): Int {
        return Int.MAX_VALUE
    }
}


class MainActivity : AppCompatActivity() {

    private lateinit var video_view1 : PlayerView
    private var player: ExoPlayer? = null
    private lateinit var binding: ActivityMainBinding
    private val handler = Handler(Looper.getMainLooper())
    private lateinit var sourceInfoText: TextView
    private var lastSource : String = ""


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        video_view1 = findViewById(R.id.video_view1)
        sourceInfoText = findViewById(R.id.source_info_text)

        findViewById<Button>(R.id.btnStartService).let {
            it.setOnClickListener {
                actionOnService(Actions.START)
            }
        }

        findViewById<Button>(R.id.btnStopService).let {
            it.setOnClickListener {
                actionOnService(Actions.STOP)
            }
        }

        findViewById<Button>(R.id.btnSettings).let {
            it.setOnClickListener {
                startActivity(Intent(this, SettingsActivity::class.java))
            }
        }

    }
    private fun actionOnService(action: Actions) {
       // if (getServiceState(this) == ServiceState.STOPPED && action == Actions.STOP) return
        Intent(this, NakolosMwService::class.java).also {
            it.action = action.name
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
             //   log("Starting the service in >=26 Mode")
                startForegroundService(it)
                return
            }
        //    log("Starting the service in < 26 Mode")
            startService(it)
        }
    }

    override fun onPause() {
        super.onPause()
        releasePlayer()
        video_view1.keepScreenOn = false
    }

    override fun onResume() {
        super.onResume()
        initPlayer()
        video_view1.keepScreenOn = true

        val policy = RetryOnErrorPolicy()
        val dataSourceFactory: DataSource.Factory = DefaultHttpDataSource.Factory()

        val cdn_ept = "http://127.0.0.1:" +
                3020 + "/" +
                "watchfolder/hls" + "/" +
                "stream_0.m3u8"
        val mediaSource: MediaSource =
            HlsMediaSource.Factory(dataSourceFactory)
                .setLoadErrorHandlingPolicy(policy)
                .createMediaSource(MediaItem.fromUri(cdn_ept))

        mediaSource.addEventListener(
            handler,
            object : MediaSourceEventListener {
                override fun onLoadCompleted(
                    windowIndex: Int,
                    mediaPeriodId: MediaSource.MediaPeriodId?,
                    loadEventInfo: LoadEventInfo,
                    mediaLoadData: MediaLoadData
                ) {
                    if (loadEventInfo.responseHeaders.contains("NAKOLOS-File-Origin")) {
                        for (header in loadEventInfo.responseHeaders.get("NAKOLOS-File-Origin")!!) {
                            sourceInfoText.text = header;
                            if (lastSource != header) {
                                lastSource = header
                            }
                        }
                    }
                }
            })

        player?.setMediaSource(mediaSource)

        player?.prepare()
        player?.play()
    }

    private fun initPlayer() {
        if (player != null) return

        val lcb = DefaultLoadControl.Builder()
            .setBufferDurationsMs(
                5000,
                10000,
                5000,
                5000
            )
            .build()


        player = ExoPlayer.Builder(applicationContext)
            .setMediaSourceFactory(
                DefaultMediaSourceFactory(applicationContext)
                    .setLiveTargetOffsetMs(5000)
                    .setLiveMaxOffsetMs(15000)
            )
            .setLoadControl(lcb)
            .build()

        video_view1.player = player
    }

    private fun releasePlayer() {
        player?.release()
        player = null
        video_view1.player = null
    }
    /**
     * A native method that is implemented by the 'libflute' native library,
     * which is packaged with this application.
     */
  //  external fun stringFromJNI(): String

   // external fun startReceiver(): Boolean

    companion object {
        // Used to load the 'libflute' library on application startup.
        init {
       //     System.loadLibrary("libflute")
        }
    }
}