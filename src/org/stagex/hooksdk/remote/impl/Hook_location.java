package org.stagex.hooksdk.remote.impl;

import org.stagex.hooksdk.remote.IPC;

import android.os.Parcel;

public class Hook_location implements IHook {

	@Override
	public int transact(int code, Parcel data, Parcel reply, int flags) {
		return IPC.getInstance().filterTransaction("location", code, data,
				reply);
	}

}
