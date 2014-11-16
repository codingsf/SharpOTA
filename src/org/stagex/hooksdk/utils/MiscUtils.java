package org.stagex.hooksdk.utils;

import java.lang.reflect.Field;

public class MiscUtils {

	public static String join(String[] strs) {
		if (strs == null)
			return "null";
		StringBuffer str = new StringBuffer();
		for (int i = 0; i < strs.length; i++) {
			str.append(strs[i]);
			if (i < strs.length - 1)
				str.append('|');
		}
		return str.toString();
	}

	public static int getTransactionCode(String cn, String code) {
		try {
			Class<?> clz = Class.forName(cn);
			Field fid = clz.getDeclaredField("TRANSACTION_" + code);
			fid.setAccessible(true);
			return (Integer) fid.get(null);
		} catch (Throwable t) {
			return -1;
		}
	}
}
