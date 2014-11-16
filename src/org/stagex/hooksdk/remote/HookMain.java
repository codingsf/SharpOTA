package org.stagex.hooksdk.remote;

import java.lang.reflect.Method;

import android.app.Application;

import org.stagex.hooksdk.remote.impl.Hook_notification;
import org.stagex.hooksdk.remote.impl.IHook;
import org.stagex.hooksdk.utils.Logger;

public class HookMain {

	public static Application sApplication = null;

	/*
	 * inject will be on the main thread
	 */
	public static void main(String[] args) {
		// TOOD: determine where I am
		// for application, better to obtain Context
		try {
			Class<?> clz_ActivityThread = Class
					.forName("android.app.ActivityThread");
			Method mtd_currentApplication = clz_ActivityThread
					.getDeclaredMethod("currentApplication");
			sApplication = (Application) mtd_currentApplication.invoke(null);
		} catch (Throwable e) {
			Logger.i("HookMain: %s", e.toString());
			return;
		}
		// hook interested stuffs in application
		IHook hook;
		Logger.i("HookMain: hook %s", "notification");
		hook = new Hook_notification();
		HookService.hook("notification", hook);
		// hook interested stuffs in system_server

	}
}
