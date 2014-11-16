package org.stagex.hooksdk.server;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;

public class HookSDKIpcService extends Service {

	// XXX:
	// we need to change server from socket to AIDL
	// cos in some situation we have to pass binders
	private HookSDKIpcServer mServer = new HookSDKIpcServer(this);

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}

	@Override
	public void onCreate() {
		super.onCreate();
		mServer.start();
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		mServer.stop();
	}
}
