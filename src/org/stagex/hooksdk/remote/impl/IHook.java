package org.stagex.hooksdk.remote.impl;

import android.os.Parcel;

public interface IHook {

	// by pass
	public static final int HOOK_IGNORED = -1;
	// transact should fail
	public static final int HOOK_TRANSACTION_REJECTED = 0;
	// transact is success, reply should be constructed by hook
	public static final int HOOK_TRANSACTION_PROCEEDED = 1;

	public int transact(int code, Parcel data, Parcel reply, int flags);
}
