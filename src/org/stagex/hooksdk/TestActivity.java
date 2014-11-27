package org.stagex.hooksdk;

import org.stagex.hooksdk.remote.HookMain;
import org.stagex.hooksdk.utils.Logger;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class TestActivity extends ActionBarActivity {

	static {
		System.loadLibrary("java_hook_test");
	}

	private int testJavaHook_add(int a, int b) {
		Logger.d("a = %d, b = %d", a, b);
		return a + b;
	}

	private native void testJavaHook();

	private void test() {
		// use this to trigger binder so we can test hook
		// better to make IpcService non-independent process
		testJavaHook_add(1, 2);
		testJavaHook();
		testJavaHook_add(1, 2);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		HookMain.main(null);
		Button test = (Button) findViewById(R.id.test);
		test.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				test();
			}
		});
	}
}
