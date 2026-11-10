// Copyright Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

package org.citra.citra_emu.utils

import android.app.PendingIntent
import android.app.Service
import android.content.Context
import android.content.Intent
import android.content.pm.ServiceInfo
import android.os.Build
import android.os.IBinder
import androidx.core.app.NotificationCompat
import androidx.core.content.ContextCompat
import org.citra.citra_emu.R

/**
 * Foreground service that runs while a game is open.
 *
 * Emulation is already paused when the screen turns off, but without a foreground service
 * Android treats the app as an ordinary background app and may freeze or kill it while the
 * device sleeps, which ends the game. This keeps the process alive so the paused game is
 * still there when the device wakes up, like closing and reopening a 3DS.
 */
class EmulationKeepAliveService : Service() {
    override fun onBind(intent: Intent?): IBinder? = null

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        val openAppIntent = packageManager.getLaunchIntentForPackage(packageName)
        val contentIntent = openAppIntent?.let {
            PendingIntent.getActivity(
                this,
                0,
                it,
                PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
            )
        }

        val notification = NotificationCompat.Builder(
            this,
            getString(R.string.app_notification_channel_id)
        )
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle(getString(R.string.app_name))
            .setContentText(getString(R.string.app_notification_running))
            .setPriority(NotificationCompat.PRIORITY_LOW)
            .setOngoing(true)
            .setSilent(true)
            .setContentIntent(contentIntent)
            .build()

        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.UPSIDE_DOWN_CAKE) {
                startForeground(
                    NOTIFICATION_ID,
                    notification,
                    ServiceInfo.FOREGROUND_SERVICE_TYPE_SPECIAL_USE
                )
            } else {
                startForeground(NOTIFICATION_ID, notification)
            }
        } catch (e: Exception) {
            Log.error("[EmulationKeepAliveService] Unable to start foreground service: $e")
            stopSelf()
        }
        return START_NOT_STICKY
    }

    override fun onTaskRemoved(rootIntent: Intent?) {
        // The user swiped the app away; don't keep running without it.
        stopSelf()
        super.onTaskRemoved(rootIntent)
    }

    companion object {
        private const val NOTIFICATION_ID = 0xA2A4

        fun start(context: Context) {
            try {
                ContextCompat.startForegroundService(
                    context,
                    Intent(context, EmulationKeepAliveService::class.java)
                )
            } catch (e: Exception) {
                // Android refuses to start foreground services from the background.
                // This is only called from a visible activity, but never crash over it.
                Log.warning("[EmulationKeepAliveService] Unable to start: $e")
            }
        }

        fun stop(context: Context) {
            context.stopService(Intent(context, EmulationKeepAliveService::class.java))
        }
    }
}
