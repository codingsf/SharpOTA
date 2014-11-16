package org.stagex.hooksdk.utils;

import org.stagex.hooksdk.BuildConfig;

import android.util.Log;

public class Logger {

	private static boolean DEBUG = BuildConfig.DEBUG;
	private static final String TAG = "HookSDK";

	public static void d(String fmt, Object... args) {
		if (DEBUG)
			Log.d(TAG, String.format(fmt, args));
	}

	public static void i(String fmt, Object... args) {
		Log.i(TAG, String.format(fmt, args));
	}
}
