package org.stagex.hooksdk.utils;

import java.io.IOException;

import android.net.LocalSocket;

public class IoUtils {

	/*
	 * early Android KK bug workaround
	 * 
	 * LocalSocket.close() won't actual close
	 */
	public static void close(LocalSocket sock) {
		// TODO:
		if (sock != null) {
			try {
				sock.close();
			} catch (IOException e) {
			}
		}
	}
}
