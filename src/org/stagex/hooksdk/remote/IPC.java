package org.stagex.hooksdk.remote;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.stagex.hooksdk.server.HookSDKIpcServer;
import org.stagex.hooksdk.utils.IoUtils;
import org.stagex.hooksdk.utils.Logger;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.os.IBinder;
import android.os.Parcel;

public class IPC {

	private static IPC sInstance = null;

	private ServiceConnection mServiceConnection = new ServiceConnection() {

		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {

		}

		@Override
		public void onServiceDisconnected(ComponentName name) {

		}

	};

	private IPC() {

	}

	public static IPC getInstance() {
		synchronized (IPC.class) {
			if (sInstance == null)
				sInstance = new IPC();
		}
		return sInstance;
	}

	private void startIpcService() {
		Intent intent = new Intent();
		intent.setClassName("org.stagex.hooksdk",
				"org.stagex.hooksdk.server.HookSDKIpcService");
		HookMain.sApplication.startService(intent);
		HookMain.sApplication.bindService(intent, mServiceConnection,
				Context.BIND_AUTO_CREATE);
	}

	public int filterTransaction(String service, int code, Parcel data,
			Parcel reply) {
		startIpcService();

		LocalSocket sock = null;
		Parcel payload = Parcel.obtain();
		try {
			// connect to IPC service
			sock = new LocalSocket();
			sock.connect(new LocalSocketAddress(HookSDKIpcServer.SOCKET));
			// serialize
			payload.writeString(service);
			payload.writeInt(code);
			if (data.dataSize() > 0) {
				// writeByteArray = 4 + data
				payload.writeByteArray(data.marshall());
			} else {
				payload.writeInt(0);
			}
			// construct data to send
			byte[] xdata = payload.marshall();
			// head
			byte[] xhead = new byte[4];
			// head: size
			xhead[0] = (byte) (xdata.length & 0xff);
			xhead[1] = (byte) ((xdata.length >> 8) & 0xff);
			xhead[2] = (byte) ((xdata.length >> 16) & 0xff);
			xhead[3] = (byte) ((xdata.length >> 24) & 0xff);
			// send the data
			OutputStream os = sock.getOutputStream();
			os.write(xhead);
			os.write(xdata);
			// receive the response data
			int n = 0;
			InputStream is = sock.getInputStream();
			n = is.read(xhead);
			if (n != xhead.length) {
				Logger.i("invalid head size, expected %d, actual %d",
						xhead.length, n);
				return -1;
			}
			int dataSize = (xhead[0] & 0x000000ff)
					| ((xhead[1] << 8) & 0x0000ff00)
					| ((xhead[2] << 16) & 0x00ff0000)
					| ((xhead[3] << 24) & 0xff000000);
			if (dataSize <= 0 || dataSize > 0x00040000) {
				Logger.i("invalid data size %d", dataSize);
				return -1;
			}
			xdata = new byte[dataSize];
			n = is.read(xdata);
			if (n != dataSize) {
				Logger.i("incomplete data read, expected %d, actual %d",
						dataSize, n);
				return -1;
			}
			// size(4) + result(4) + replySize(4) + replyData(replySize)
			int off = 0;
			// result
			if (dataSize < off + 4) {
				Logger.i("invalid result");
				return -1;
			}
			int result = xdata[off] | (xdata[off + 1] << 8)
					| (xdata[off + 2] << 16) | (xdata[off + 3] << 24);
			off += 4;
			// reply size
			if (dataSize < off + 4) {
				Logger.i("invalid reply size");
				return -1;
			}
			// reply data, if any
			int replySize = xdata[off] | (xdata[off + 1] << 8)
					| (xdata[off + 2] << 16) | (xdata[off + 3] << 24);
			if (dataSize < off + replySize) {
				Logger.i("invalid reply data");
				return -1;
			}
			if (replySize > 0)
				reply.unmarshall(xdata, off, replySize);
			// everything is done
			return result;
		} catch (IOException e) {
			// for any error return -1 to do nothing
			return -1;
		} catch (Throwable t) {
			// for any error return -1 to do nothing
			return -1;
		} finally {
			IoUtils.close(sock);
			payload.recycle();
		}
	}
}
