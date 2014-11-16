package org.stagex.hooksdk.server;

public class HookSDK {

	private static HookSDK sInstance = null;

	private HookSDK() {

	}

	public static HookSDK getInstance() {
		synchronized (HookSDK.class) {
			if (sInstance == null)
				sInstance = new HookSDK();
		}
		return sInstance;
	}

	/*
	 * 
	 */
	public int hookSystemServer() {
		return -1;
	}

	/*
	 * 
	 */
	public int hookZygote() {
		return -1;
	}

	/*
	 * 
	 */
	public int hookAll() {
		return -1;
	}
}
