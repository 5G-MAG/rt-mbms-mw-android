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

import android.annotation.SuppressLint
import android.app.*
import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.graphics.Color
import android.os.Build
import android.os.IBinder
import android.os.PowerManager
import android.telephony.MbmsGroupCallSession
import android.telephony.mbms.GroupCall
import android.telephony.mbms.GroupCallCallback
import android.telephony.mbms.MbmsGroupCallSessionCallback
import android.util.Log
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.preference.PreferenceManager
import java.io.*
import java.net.*
import java.util.*
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors


private const val TAG = "NakolosMW"

class NakolosMwService : Service() {

    private var groupCallSession: MbmsGroupCallSession? = null
    private lateinit var groupCallSessionCallback: MbmsGroupCallSessionCallback
    private lateinit var groupCallCallback: GroupCallCallback
    private var groupCall: GroupCall? = null

    private var connectingToMiddleware : Boolean = false
    private var executorService : ExecutorService = Executors.newFixedThreadPool(2)
    private var receiveThread: Thread? = null
    private lateinit var udpReceiver: UdpReceiver
    private var wakeLock: PowerManager.WakeLock? = null
    private var isServiceStarted = false

    fun log(msg: String) {
        Log.d("NAKOLOS MW Service", msg)
    }


    @RequiresApi(Build.VERSION_CODES.Q)
    fun connectToMiddleware() {
        if (!connectingToMiddleware) {
            connectingToMiddleware = true
            Log.i(TAG, "Connecting to middleware in RTP mode")
            executorService.execute {
                groupCallSession =
                    MbmsGroupCallSession.create(
                        applicationContext,
                        executorService,
                        groupCallSessionCallback
                    )
            }
        }
    }
    @RequiresApi(Build.VERSION_CODES.Q)
    fun disconnectFromMiddleware() {
        executorService.execute {
            groupCall?.close()
            groupCallSession?.close()
        }
    }

    @Throws(IOException::class)
    fun readAssetFile(fileName: String): String? {
        val input: InputStream = applicationContext.assets.open(fileName)
        val reader: Reader = InputStreamReader(input)
        val sb = StringBuilder()
        val buffer = CharArray(16384) // read 16k blocks
        var len: Int // how much content was read?
        while (reader.read(buffer).also { len = it } > 0) {
            sb.append(buffer, 0, len)
        }
        reader.close()
        return sb.toString()
    }

    @RequiresApi(Build.VERSION_CODES.Q)
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {

        groupCallCallback = @SuppressLint("NewApi")
        object : GroupCallCallback {
            override fun onError(errorCode: Int, message: String?) {
                Log.e(TAG, "GroupCallCallback onError: $errorCode - $message")
                super.onError(errorCode, message)
            }

            override fun onBroadcastSignalStrengthUpdated(signalStrength: Int) {
                Log.d(TAG, "GroupCallCallback onBroadcastSignalStrengthUpdated: $signalStrength")
                super.onBroadcastSignalStrengthUpdated(signalStrength)
            }

            override fun onGroupCallStateChanged(state: Int, reason: Int) {
                Log.d(TAG, "GroupCallCallback onGroupCallStateChanged: $state, reason $reason")
                super.onGroupCallStateChanged(state, reason)
            }
        }

        groupCallSessionCallback = @SuppressLint("NewApi")
        object : MbmsGroupCallSessionCallback {
            override fun onAvailableSaisUpdated(
                currentSais: MutableList<Int>,
                availableSais: MutableList<MutableList<Int>>
            ) {
                Log.d(TAG, "MW session onAvailableSaisUpdated: $currentSais, $availableSais")
                super.onAvailableSaisUpdated(currentSais, availableSais)
            }
            override fun onError(errorCode: Int, message: String?){
                Log.e(TAG, "MW session onError code: $errorCode, error: $message")
            }
            override fun onMiddlewareReady() {
                Log.d(TAG, "Middleware ready!")
              //  middlewareStatusCb(0, "Middleware ready!")

             //   var prefs = PreferenceManager.getDefaultSharedPreferences(applicationContext);
                var tmgi : Long = -1
            //    var set_tmgi = prefs.getString("tmgi", "1009f165")
                var set_tmgi = "1009f165"
                try {
                    tmgi = set_tmgi.toLong(16)
                } catch (e: Exception) {}

                if (tmgi == -1L ) {
                    Log.e(TAG, "$set_tmgi is not a valid TMGI")
                } else {
                    groupCall = groupCallSession?.startGroupCall(
                        tmgi,
                        listOf(0), listOf(0), executorService, groupCallCallback
                    )
                }
            }
            override fun onServiceInterfaceAvailable(interfaceName: String, index: Int) {

                udpReceiver = UdpReceiver(applicationContext, interfaceName)
                receiveThread = Thread(udpReceiver)
                receiveThread!!.start()
             //  startReceiver(addr.hostAddress)
               // modemDataLinkAvailableCb(interfaceName, index)
            }
        }

        if (intent != null) {
            val action = intent.action
            log("using an intent with action $action")
            when (action) {
                Actions.START.name -> startService()
                Actions.STOP.name -> stopService()
                else -> log("This should never happen. No action in the received intent")
            }
        } else {
            log(
                "with a null intent. It has been probably restarted by the system."
            )
        }

                return START_STICKY
    }

    override fun onCreate() {
        super.onCreate()
        udpReceiver = UdpReceiver(applicationContext,"")
        log("The service has been created".toUpperCase())
        var notification = createNotification()
        startForeground(1, notification)
    }

    override fun onDestroy() {
        super.onDestroy()
        log("The service has been destroyed".toUpperCase())
        Toast.makeText(this, "Service destroyed", Toast.LENGTH_SHORT).show()
    }

    override fun onBind(intent: Intent?): IBinder? {
        TODO("Not yet implemented")
    }

    @RequiresApi(Build.VERSION_CODES.Q)
    private fun startService() {
        if (isServiceStarted) return
        log("Starting the NAKOLOS MW foreground service task")
        Toast.makeText(this, "NAKOLOS MW started", Toast.LENGTH_SHORT).show()
        isServiceStarted = true

        wakeLock =
            (getSystemService(Context.POWER_SERVICE) as PowerManager).run {
                newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "EndlessService::lock").apply {
                    acquire()
                }
            }

        val sPrefs: SharedPreferences = PreferenceManager.getDefaultSharedPreferences(applicationContext!!)
        var app_uuid = sPrefs.getString("key_uuid", null)
        if (app_uuid.isNullOrBlank()) {
            app_uuid = UUID.randomUUID().toString();
            val editor = sPrefs.edit()
            editor.putString("key_uuid", app_uuid)
            editor.commit()
        }

        val userDeviceName: String = app_uuid
        //    Settings.Global.getString(contentResolver, Settings.Global.DEVICE_NAME)

        var prefs = PreferenceManager.getDefaultSharedPreferences(applicationContext);
        var api_key = prefs.getString("api_key", "")

        startNativeMiddleware(userDeviceName, api_key!!)

        var service_announcement = readAssetFile("bootstrap.multipart.hls")
        //var service_announcement = readAssetFile("bootstrap.multipart.debug")
        if (service_announcement != null) {
            setLocalServiceAnnouncement(service_announcement)
        }
        connectToMiddleware()
    }

    @RequiresApi(Build.VERSION_CODES.Q)
    private fun stopService() {
        try {
            wakeLock?.let {
                if (it.isHeld) {
                    it.release()
                }
            }
            stopForeground(true)
            stopSelf()
        } catch (e: Exception) {
            log("Service stopped without being started: ${e.message}")
        }
        isServiceStarted = false
        udpReceiver.running = false
        stopNativeMiddleware()
        disconnectFromMiddleware()
        Toast.makeText(this, "NAKOLOS MW stopped", Toast.LENGTH_SHORT).show()
    }
   // override fun onBind(intent: Intent): IBinder? {
   //     return null
   // }
    private fun createNotification(): Notification {
        val notificationChannelId = "NAKOLOS MW"

        // depending on the Android API that we're dealing with we will have
        // to use a specific method to create the notification
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {

            val notificationManager = getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager;
            val channel = NotificationChannel(
                notificationChannelId,
                "NAKOLOS middleware notifications channel",
                NotificationManager.IMPORTANCE_HIGH
            ).let {
                it.description = "NAKOLOS middleware notifications channel"
                it.enableLights(true)
                it.lightColor = Color.RED
                it.enableVibration(false)
                it.vibrationPattern = longArrayOf(100, 200, 300, 400, 500, 400, 300, 200, 400)
                it
            }
            notificationManager.createNotificationChannel(channel)
        }

        val pendingIntent: PendingIntent = Intent(this, MainActivity::class.java).let { notificationIntent ->
            PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_IMMUTABLE)
        }

        val builder: Notification.Builder = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) Notification.Builder(
            this,
            notificationChannelId
        ) else Notification.Builder(this)

        return builder
            .setContentTitle("NAKOLOS")
            .setContentText("NAKOLOS middleware is running")
            .setContentIntent(pendingIntent)
            .setSmallIcon(R.drawable.ic_stat_name)
            .setTicker("Ticker text")
            .setPriority(Notification.PRIORITY_HIGH) // for under android 26 compatibility
            .build()
    }

    external fun startNativeMiddleware(device_name: String, api_key: String): Boolean
    external fun setLocalServiceAnnouncement(sa: String): Boolean
    external fun stopNativeMiddleware(): Boolean
    companion object {
        init {
            System.loadLibrary("libflute")
        }
    }
}

class UdpReceiver(applicationContext: Context, interfaceName: String) : Runnable {
    private val interfaceName = interfaceName
    private val applicationContext = applicationContext
    private val DEFAULT_MAX_PACKET_SIZE = 2048

    private var multicastSocket: MulticastSocket? = null
    private var address: InetAddress? = null
    private var socketAddress: InetSocketAddress? = null


    private var socket: DatagramSocket? = null

    private var packetBuffer: ByteArray = ByteArray(DEFAULT_MAX_PACKET_SIZE)
    private var packet: DatagramPacket = DatagramPacket(packetBuffer, 0, DEFAULT_MAX_PACKET_SIZE)

    var running = true
    public override fun run() {
        address = InetAddress.getByName("239.11.4.50")
        socketAddress = InetSocketAddress(address, 9988)

        multicastSocket = MulticastSocket(socketAddress)
        multicastSocket!!.networkInterface = NetworkInterface.getByName(interfaceName)
        // multicastSocket!!.joinGroup(address)
        multicastSocket!!.joinGroup(socketAddress, multicastSocket!!.networkInterface)
        socket = multicastSocket


        while (running){
            try {
                socket!!.receive(packet)
                handlePacket(packet.data, packet.length)
               // Log.e(TAG, "${packet.length} bytes received")
            } catch (e: IOException) {
                Log.e(TAG, "${e.message}")
            }
        }
    }

    external fun handlePacket(buffer: ByteArray, size: Int): Boolean

}