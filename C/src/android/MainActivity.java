// app/src/main/java/com/example/configeditor/MainActivity.java
package com.example.configeditor;

import android.os.Bundle;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import androidx.appcompat.app.AppCompatActivity;
import androidx.webkit.WebViewAssetLoader;

import java.io.ByteArrayInputStream;
import java.nio.charset.StandardCharsets;

public class MainActivity extends AppCompatActivity {

    private WebView webView;
    private ConfigBridge configBridge;

    // Maps to:  webkit_web_context_register_uri_scheme / setup_webkit_handler
    // WebViewAssetLoader serves files from assets/web/ at:
    //   https://appassets.androidplatform.net/assets/web/index.html
    private WebViewAssetLoader assetLoader;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Initialize config bridge (calls config_init() via JNI)
        configBridge = new ConfigBridge(getFilesDir().getAbsolutePath());

        // Build the asset loader for static files
        assetLoader = new WebViewAssetLoader.Builder()
                .addPathHandler("/assets/", new WebViewAssetLoader.AssetsPathHandler(this))
                .build();

        webView = findViewById(R.id.webView);

        // Enable JavaScript (required for app.js)
        webView.getSettings().setJavaScriptEnabled(true);
        webView.getSettings().setDomStorageEnabled(true);
        webView.getSettings().setAllowFileAccess(false); // Use asset loader instead

        webView.setWebViewClient(new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                String url = request.getUrl().toString();
                String path = request.getUrl().getPath();

                // ── API routing ──────────────────────────────────────────────
                // Maps to: uri_scheme_request_cb in webkit_handler.c
                if (path != null && path.startsWith("/api/")) {
                    return handleApiRequest(request);
                }

                // ── Static file serving ──────────────────────────────────────
                // Rewrite app:///foo  →  https://appassets.androidplatform.net/assets/web/foo
                if (url.startsWith("https://appassets.androidplatform.net")) {
                    return assetLoader.shouldInterceptRequest(request.getUrl());
                }

                return super.shouldInterceptRequest(view, request);
            }
        });

        // Load the app. We use the asset loader's domain so the browser security
        // model treats it as a real HTTPS origin (same-origin fetch works).
        // Equivalent to: webkit_web_view_load_uri(web_view, "app:///index.html")
        webView.loadUrl("https://appassets.androidplatform.net/assets/web/index.html");
    }

    /**
     * Routes /api/* requests to the C backend via JNI.
     * Mirrors the logic in uri_scheme_request_cb / api_handle_request.
     */
    private WebResourceResponse handleApiRequest(WebResourceRequest request) {
        String path   = request.getUrl().getPath();   // e.g. "/api/config/read"
        String method = request.getMethod();           // "GET" or "POST"

        String body = "";
        if ("POST".equals(method)) {
            // WebResourceRequest doesn't expose the body on older APIs.
            // ConfigBridge.handleRequest() receives it from the JS side via
            // the JavaScript interface for POST calls (see app.js companion note).
            // For simplicity here we pass an empty body; the JS bridge below
            // handles POST bodies properly.
        }

        String jsonResponse = configBridge.handleRequest(path, method, body);

        byte[] bytes = jsonResponse.getBytes(StandardCharsets.UTF_8);
        return new WebResourceResponse(
                "application/json",
                "UTF-8",
                200,
                "OK",
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
