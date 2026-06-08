// app/src/main/java/com/example/configeditor/MainActivity.java
package com.example.configeditor;

import android.os.Bundle;
import android.util.Log;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import androidx.appcompat.app.AppCompatActivity;
import androidx.webkit.WebViewAssetLoader;

import java.io.ByteArrayInputStream;
import java.nio.charset.StandardCharsets;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "ConfigEditor";

    private WebView webView;
    private ConfigBridge configBridge;
    private WebViewAssetLoader assetLoader;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        logAssetContents();

        configBridge = new ConfigBridge(getFilesDir().getAbsolutePath());

        // Handler mounted at "/assets/" — AssetsPathHandler strips this prefix
        // and resolves the remainder against the APK's assets/ folder.
        //
        // URL:  https://appassets.androidplatform.net/assets/web/index.html
        //        strip "/assets/" → "web/index.html"
        //        looks for:         assets/web/index.html  ✓
        assetLoader = new WebViewAssetLoader.Builder()
                .addPathHandler("/assets/", new WebViewAssetLoader.AssetsPathHandler(this))
                .build();

        webView = findViewById(R.id.webView);
        webView.getSettings().setJavaScriptEnabled(true);
        webView.getSettings().setDomStorageEnabled(true);
        webView.getSettings().setAllowFileAccess(false);
        webView.addJavascriptInterface(configBridge, "ConfigBridge");

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url  = request.getUrl().toString();
                String path = request.getUrl().getPath();

                Log.d(TAG, "intercept: " + url);

                // ── API routing ──────────────────────────────────────────────
                if (path != null && path.startsWith("/api/")) {
                    return handleApiRequest(request);
                }

                // ── Static file serving ──────────────────────────────────────
                if (url.startsWith("https://appassets.androidplatform.net")) {
                    WebResourceResponse response = assetLoader.shouldInterceptRequest(request.getUrl());
                    if (response == null) {
                        Log.e(TAG, "assetLoader returned null for: " + url);
                    } else {
                        Log.d(TAG, "assetLoader serving: " + url);
                    }
                    return response;
                }

                return super.shouldInterceptRequest(view, request);
            }
        });

        // Load index.html — path after the domain maps directly into assets/:
        //   /assets/web/index.html  →  assets/web/index.html in APK
        webView.loadUrl("https://appassets.androidplatform.net/assets/web/index.html");
    }

    private void logAssetContents() {
        try {
            String[] top = getAssets().list("");
            Log.d(TAG, "assets/ top-level: " + java.util.Arrays.toString(top));
            String[] web = getAssets().list("web");
            Log.d(TAG, "assets/web/: " + java.util.Arrays.toString(web));
        } catch (Exception e) {
            Log.e(TAG, "Error listing assets: " + e.getMessage());
        }
    }

    private WebResourceResponse handleApiRequest(WebResourceRequest request) {
        String path   = request.getUrl().getPath();
        String method = request.getMethod();

        Log.d(TAG, "API request: " + method + " " + path);

        String jsonResponse = configBridge.handleRequest(path, method, "");
        byte[] bytes = jsonResponse.getBytes(StandardCharsets.UTF_8);
        return new WebResourceResponse(
                "application/json", "UTF-8", 200, "OK",
                buildCorsHeaders(),
                new ByteArrayInputStream(bytes)
        );
    }

    private java.util.Map<String, String> buildCorsHeaders() {
        java.util.Map<String, String> headers = new java.util.HashMap<>();
        headers.put("Access-Control-Allow-Origin", "*");
        headers.put("Content-Type", "application/json");
        return headers;
    }

    @Override
    public void onBackPressed() {
        if (webView.canGoBack()) {
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }
}
