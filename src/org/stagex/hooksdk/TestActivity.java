package org.stagex.hooksdk;

import org.stagex.hooksdk.remote.HookMain;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class TestActivity extends ActionBarActivity {

	private void test() {
		// use this to trigger binder so we can test hook
		// better to make IpcService non-independent process
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
