package org.stagex.hooksdk.server.impl;

import org.stagex.hooksdk.utils.MiscUtils;

import android.content.Context;
import android.location.Location;
import android.os.Parcel;

public class Handler_location implements IHandler {

	private static final int TRANSACTION_getLastLocation;
	private static final int TRANSACTION_addGpsStatusListener;

	static {
		TRANSACTION_getLastLocation = MiscUtils.getTransactionCode(
				"android.location.ILocationManager$Stub", "getLastLocation");
		TRANSACTION_addGpsStatusListener = MiscUtils.getTransactionCode(
				"android.location.ILocationManager$Stub",
				"addGpsStatusListener");
	}

	@Override
	public IHandlerResult handle(Context context, String[] pkgs, int code,
			Parcel data) {
		// fake getLastLocation
		if (code == TRANSACTION_getLastLocation) {
			// only for Android 4.4.4, other version is unchecked
			IHandlerResult hr = IHandlerResult.obtain();
			Parcel reply = hr.reply;
			// this should be carefully examined
			reply.writeNoException();
			reply.writeInt(1);
			Location loc = new Location("gps");
			// fill something here
			loc.writeToParcel(reply, 0);
			return hr;
		}
		return IHandlerCommon.IGNORE;
	}

}
