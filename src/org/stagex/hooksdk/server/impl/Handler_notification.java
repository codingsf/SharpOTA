package org.stagex.hooksdk.server.impl;

import org.stagex.hooksdk.utils.Logger;
import org.stagex.hooksdk.utils.MiscUtils;

import android.content.Context;
import android.os.Parcel;

public class Handler_notification implements IHandler {

	private static final int TRANSACTION_enqueueNotificationWithTag;

	static {
		TRANSACTION_enqueueNotificationWithTag = MiscUtils.getTransactionCode(
				"android.app.INotificationManager$Stub",
				"enqueueNotificationWithTag");
	}

	@Override
	public IHandlerResult handle(Context context, String[] pkgs, int code,
			Parcel data) {
		if (code == TRANSACTION_enqueueNotificationWithTag) {
			Logger.i("rejected enqueueNotificationWithTag for %s",
					MiscUtils.join(pkgs));
			return IHandlerCommon.REJECT;
		}
		return IHandlerCommon.IGNORE;
	}

}
