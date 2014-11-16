package org.stagex.hooksdk.server.impl;

import org.stagex.hooksdk.remote.impl.IHook;

public class IHandlerCommon {

	public static final IHandler.IHandlerResult REJECT;
	public static final IHandler.IHandlerResult IGNORE;

	static {
		REJECT = new IHandler.IHandlerResult();
		REJECT.result = IHook.HOOK_TRANSACTION_REJECTED;
		REJECT.reply = null;
		IGNORE = new IHandler.IHandlerResult();
		IGNORE.result = IHook.HOOK_IGNORED;
		IGNORE.reply = null;
	}
}
