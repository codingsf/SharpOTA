package org.stagex.hooksdk.server;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.stagex.hooksdk.utils.Logger;
import org.stagex.hooksdk.utils.IoUtils;

import android.content.Context;
import android.net.Credentials;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;

public class HookSDKIpcServer {

	public static String SOCKET = "\0hooksdk";

	private Context mContext;
	private boolean mRunning = false;
	private LocalServerSocket mServerSocket = null;
	private Thread mServerThread = null;
	private ExecutorService mServerPool = null;

	public HookSDKIpcServer(Context context) {
		mContext = context;
	}

	public void start() {
		if (mServerThread == null) {
			mRunning = true;
			mServerPool = Executors.newCachedThreadPool();
			mServerThread = new Thread(new Runnable() {
				@Override
				public void run() {
					try {
						mServerSocket = new LocalServerSocket(SOCKET);
						for (; mRunning;) {
							final LocalSocket sock = mServerSocket.accept();
							mServerPool.submit(new Runnable() {
								@Override
								public void run() {
									try {
										// who is it, might be useful
										Credentials cred = sock
												.getPeerCredentials();
										Logger.i(
												"accepted connection from UID %d, PID %d",
												cred.getUid(), cred.getPid());
										int n = 0;
										// receive serialized data
										byte[] xhead = new byte[4];
										InputStream is = sock.getInputStream();
										n = is.read(xhead);
										if (n != xhead.length) {
											Logger.i(
													"invalid head size, expected %d, actual %d",
													xhead.length, n);
											return;
										}
										int size = (xhead[0] & 0x000000ff)
												| ((xhead[1] << 8) & 0x0000ff00)
												| ((xhead[2] << 16) & 0x00ff0000)
												| ((xhead[3] << 24) & 0xff000000);
										if (size <= 0 || size > 0x00040000) {
											Logger.i("invalid data size %d",
													size);
											return;
										}
										byte[] xdata = new byte[size];
										n = is.read(xdata);
										if (n != size) {
											Logger.i(
													"incomplete data read, expected %d, actual %d",
													size, n);
											return;
										}
										// process the data
										byte[] respData = HookSDKHandler
												.getInstance(mContext).handle(
														cred, xdata);
										// send response
										OutputStream os = sock
												.getOutputStream();
										// head
										int respSize = 0;
										if (respData != null)
											respSize = respData.length;
										xhead[0] = (byte) (respSize & 0xff);
										xhead[1] = (byte) ((respSize >> 8) & 0xff);
										xhead[2] = (byte) ((respSize >> 16) & 0xff);
										xhead[3] = (byte) ((respSize >> 24) & 0xff);
										os.write(xhead);
										// data
										if (respSize > 0)
											os.write(respData);
									} catch (IOException e) {
										Logger.i("HookSDKIpcServer: %s",
												e.toString());
									} catch (Throwable t) {
										Logger.i("HookSDKIpcServer: %s",
												t.toString());
									} finally {
										IoUtils.close(sock);
									}
								}
							});
						}
					} catch (IOException e) {
						Logger.i("HookSDKIpcServer: %s", e.toString());
					}
					if (mServerSocket != null) {
						try {
							mServerSocket.close();
						} catch (IOException e) {
							Logger.i("HookSDKIpcServer: %s", e.toString());
						}
					}
				}
			});
			mServerThread.start();
		}
	}

	public void stop() {
		mRunning = false;
		try {
			// trigger accept to exit
			LocalSocket sock = null;
			try {
				sock = new LocalSocket();
				sock.connect(new LocalSocketAddress(SOCKET));
			} catch (Throwable t) {
				Logger.i("HookSDKIpcServer: %s", t.toString());
			} finally {
				if (sock != null) {
					try {
						sock.close();
					} catch (Throwable t) {
					}
				}
			}
			// wait to exit
			mServerThread.join();
		} catch (InterruptedException e) {
			Logger.i("HookSDKIpcServer: %s", e.toString());
		}
		mServerThread = null;
		mServerPool.shutdown();
		mServerPool = null;
	}
}
