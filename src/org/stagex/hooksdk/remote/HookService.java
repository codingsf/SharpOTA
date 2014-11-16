package org.stagex.hooksdk.remote;

import java.io.FileDescriptor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.HashMap;

import org.stagex.hooksdk.remote.impl.IHook;
import org.stagex.hooksdk.utils.Logger;

import android.annotation.SuppressLint;
import android.os.IBinder;
import android.os.IInterface;
import android.os.Parcel;
import android.os.RemoteException;

public class HookService {

	@SuppressLint("NewApi")
	private static class BinderStub implements IBinder {

		private IBinder mBinder;
		private IHook mCallback;

		public BinderStub(IBinder binder, IHook callback) {
			mBinder = binder;
			mCallback = callback;
		}

		public boolean transact(int code, Parcel data, Parcel reply, int flags)
				throws RemoteException {
			int handled = mCallback.transact(code, data, reply, flags);
			if (handled == IHook.HOOK_IGNORED)
				return mBinder.transact(code, data, reply, flags);
			if (handled == IHook.HOOK_TRANSACTION_REJECTED)
				throw new RemoteException("transaction rejected by HookSDK");
			if (handled == IHook.HOOK_TRANSACTION_PROCEEDED)
				return true;
			Logger.i("invalid transact return, fallback to original binder!");
			return mBinder.transact(code, data, reply, flags);
		}

		public IInterface queryLocalInterface(String descriptor) {
			return mBinder.queryLocalInterface(descriptor);
		}

		@Override
		public void dump(FileDescriptor fd, String[] args)
				throws RemoteException {
			mBinder.dump(fd, args);
		}

		@Override
		public void dumpAsync(FileDescriptor fd, String[] args)
				throws RemoteException {
			mBinder.dumpAsync(fd, args);
		}

		@Override
		public String getInterfaceDescriptor() throws RemoteException {
			return mBinder.getInterfaceDescriptor();
		}

		@Override
		public boolean isBinderAlive() {
			return mBinder.isBinderAlive();
		}

		@Override
		public void linkToDeath(DeathRecipient recipient, int flags)
				throws RemoteException {
			mBinder.linkToDeath(recipient, flags);
		}

		@Override
		public boolean pingBinder() {
			return mBinder.pingBinder();
		}

		@Override
		public boolean unlinkToDeath(DeathRecipient recipient, int flags) {
			return mBinder.unlinkToDeath(recipient, flags);
		}

	}

	private static IBinder sBinderStub;

	private static Object getCache(String name) {
		try {
			Class<?> clz = Class.forName("android.os.ServiceManager");
			Method mtd = clz.getDeclaredMethod("getService", String.class);
			return mtd.invoke(null, name);
		} catch (Exception e) {
			return null;
		}
	}

	@SuppressWarnings("unchecked")
	private static void setCache(String name, Object binder) {
		try {
			Class<?> clz = Class.forName("android.os.ServiceManager");
			Field fid = clz.getDeclaredField("sCache");
			fid.setAccessible(true);
			Object val = fid.get(null);
			if (val instanceof HashMap) {
				HashMap<String, IBinder> map = (HashMap<String, IBinder>) val;
				map.put(name, (IBinder) binder);
			}
		} catch (Exception e) {
		}
	}

	public static synchronized IBinder hook(String name, IHook callback) {
		IBinder service = (IBinder) getCache(name);
		sBinderStub = new BinderStub(service, callback);
		setCache(name, sBinderStub);
		return service;
	}
}
