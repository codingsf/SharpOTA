package org.stagex.hooksdk.utils;

import java.lang.reflect.Method;
import java.util.List;

import android.app.ActivityManager;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.os.IBinder;

public class ProcessUtils {

	/*
	 * TODO: find a way to cache
	 */
	@SuppressWarnings("unchecked")
	public static String[] getPackageNameForPid(int pid) {
		try {
			Class<?> clz_ServiceManager = Class
					.forName("android.os.ServiceManager");
			Method mtd_getService = clz_ServiceManager.getDeclaredMethod(
					"getService", String.class);
			Object obj_activity = mtd_getService.invoke(null, "activity");
			Class<?> clz_ActivityManagerNative = Class
					.forName("android.app.ActivityManagerNative");
			Method mtd_asInterface = clz_ActivityManagerNative
					.getDeclaredMethod("asInterface", IBinder.class);
			Object obj_IActivityManager = mtd_asInterface.invoke(null,
					obj_activity);
			Class<?> clz_IActivityManager = obj_IActivityManager.getClass();
			Method mtd_getRunningAppProcesses = clz_IActivityManager
					.getDeclaredMethod("getRunningAppProcesses");
			List<ActivityManager.RunningAppProcessInfo> processes = (List<RunningAppProcessInfo>) mtd_getRunningAppProcesses
					.invoke(obj_IActivityManager);
			for (ActivityManager.RunningAppProcessInfo p : processes) {
				if (p.pid == pid)
					return p.pkgList;
			}
		} catch (Exception e) {
			Logger.i("getPackageNameForPid: %s", e.toString());
		}
		return null;
	}
}
