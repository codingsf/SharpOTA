package org.stagex.hooksdk.server.impl;

import org.stagex.hooksdk.remote.impl.IHook;

import android.content.Context;
import android.os.Parcel;

public interface IHandler {

	public static class IHandlerResult {
		public int result;
		public Parcel reply;

		public static IHandlerResult obtain() {
			IHandlerResult hr = new IHandlerResult();
			hr.result = IHook.HOOK_TRANSACTION_PROCEEDED;
			hr.reply = Parcel.obtain();
			return hr;
		}

		public byte[] toByteArray() {
			int size = 4;
			if (reply != null)
				size += reply.dataSize();
			else
				size += 4;
			byte[] data = new byte[size];
			data[0] = (byte) (result & 0xff);
			data[1] = (byte) ((result >> 8) & 0xff);
			data[2] = (byte) ((result >> 16) & 0xff);
			data[3] = (byte) ((result >> 24) & 0xff);
			if (reply != null) {
				size = reply.dataSize();
				data[4] = (byte) (size & 0xff);
				data[5] = (byte) ((size >> 8) & 0xff);
				data[6] = (byte) ((size >> 16) & 0xff);
				data[7] = (byte) ((size >> 24) & 0xff);
				if (size > 0)
					System.arraycopy(reply.marshall(), 0, data, 8, size);
			}
			return data;
		}

		@Override
		public void finalize() {
			if (reply != null)
				reply.recycle();
		}
	}

	/*
	 * @param context
	 * 
	 * @param pkgs
	 * 
	 * @param code
	 * 
	 * @param data
	 */
	public IHandlerResult handle(Context context, String[] pkgs, int code,
			Parcel data);
}
