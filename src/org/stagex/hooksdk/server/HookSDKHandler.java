package org.stagex.hooksdk.server;

import java.util.HashMap;
import java.util.Map;

import android.content.Context;
import android.net.Credentials;
import android.os.Parcel;

import org.stagex.hooksdk.server.impl.Handler_location;
import org.stagex.hooksdk.server.impl.Handler_notification;
import org.stagex.hooksdk.server.impl.IHandler;
import org.stagex.hooksdk.server.impl.IHandlerCommon;
import org.stagex.hooksdk.utils.Logger;
import org.stagex.hooksdk.utils.ProcessUtils;

public class HookSDKHandler {

	private static HookSDKHandler sInstance;

	public static HookSDKHandler getInstance(Context context) {
		synchronized (HookSDKHandler.class) {
			if (sInstance == null)
				sInstance = new HookSDKHandler(context);
		}
		return sInstance;
	}

	private Context mContext;
	private Map<String, IHandler> mHandlers = null;

	private HookSDKHandler(Context context) {
		mContext = context;
		mHandlers = new HashMap<String, IHandler>();
		mHandlers.put("notification", new Handler_notification());
		mHandlers.put("location", new Handler_location());
	}

	public byte[] handle(Credentials cred, byte[] req) {
		Parcel payload = Parcel.obtain();
		Parcel data = Parcel.obtain();
		Parcel reply = Parcel.obtain();
		try {
			int pos = payload.dataPosition();
			payload.unmarshall(req, 0, req.length);
			payload.setDataPosition(pos);
			String service = payload.readString();
			if (service == null) {
				Logger.i("service is null");
				return IHandlerCommon.IGNORE.toByteArray();
			}
			IHandler handler = mHandlers.get(service);
			if (handler == null) {
				Logger.i("service \'%s` handler is unknown", service);
				return IHandlerCommon.IGNORE.toByteArray();
			}
			int code = payload.readInt();
			pos = data.dataPosition();
			data.unmarshall(req, payload.dataPosition(), payload.dataAvail());
			data.setDataPosition(pos);
			String[] pkgs = null;
			if (cred != null)
				pkgs = ProcessUtils.getPackageNameForPid(cred.getPid());
			IHandler.IHandlerResult result = handler.handle(mContext, pkgs,
					code, data);
			if (result == null) {
				Logger.i("service %s handler returned null", service);
				return IHandlerCommon.IGNORE.toByteArray();
			}
			return result.toByteArray();
		} catch (Throwable t) {
			Logger.i("HookSDKHandler: %s", t.toString());
			return IHandlerCommon.IGNORE.toByteArray();
		} finally {
			reply.recycle();
			data.recycle();
			payload.recycle();
		}
	}
}
