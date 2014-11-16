package org.stagex.hooksdk;

import android.app.Application;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;

public class ExampleApplication extends Application {

	private ServiceConnection mServiceConn = new ServiceConnection() {

		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {

		}

		@Override
		public void onServiceDisconnected(ComponentName name) {

		}

	};

	@Override
	public void onCreate() {
		super.onCreate();
		Intent service = new Intent();
		service.setClassName("org.stagex.hooksdk",
				"org.stagex.hooksdk.server.HookSDKIpcService");
		bindService(service, mServiceConn, Context.BIND_AUTO_CREATE);
	}

	@Override
	public void onTerminate() {
		super.onTerminate();
		unbindService(mServiceConn);
	}
}
