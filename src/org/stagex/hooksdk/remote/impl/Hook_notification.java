package org.stagex.hooksdk.remote.impl;

import org.stagex.hooksdk.remote.IPC;

import android.os.Parcel;

public class Hook_notification implements IHook {

	@Override
	public int transact(int code, Parcel data, Parcel reply, int flags) {
		// local implementation example
		// Logger.i("notification service: code = %d", code);
		// return 0;
		return IPC.getInstance().filterTransaction("notification", code,
				data, reply);
	}

}
