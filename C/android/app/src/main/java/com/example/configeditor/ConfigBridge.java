// app/src/main/java/com/example/configeditor/ConfigBridge.java
package com.example.configeditor;

import android.webkit.JavascriptInterface;

/**
 * ConfigBridge connects the JavaScript frontend to the C backend.
 *
 * On the C side it calls api_handle_request() via the JNI glue in jni_bridge.c.
 * On the Java/JS side it is exposed as window.ConfigBridge in the WebView.
 *
 * This replaces webkit_handler.c's uri_scheme_request_cb for the API layer.
 *
 * POST body problem:
 *   Android's WebResourceRequest does not expose the request body.
 *   Solution: app.js calls window.ConfigBridge.post(path, bodyJson) directly
 *   for write operations, bypassing fetch() for POST. See the patched app.js.
 */
public class ConfigBridge {

    static {
        System.loadLibrary("configeditor"); // loads libconfigeditor.so built by CMake
    }

    private final String filesDir;

    public ConfigBridge(String filesDir) {
        this.filesDir = filesDir;
        // Calls config_init() via JNI, passing the Android files directory
        // so cfg.yml is stored at <filesDir>/cfg.yml instead of a relative path.
        nativeInit(filesDir);
    }

    /**
     * Called from MainActivity for GET requests intercepted via shouldInterceptRequest.
     * Also called from JS for POST (see @JavascriptInterface below).
     */
    public String handleRequest(String path, String method, String body) {
        return nativeHandleRequest(path, method, body != null ? body : "");
    }

    /**
     * JavaScript interface: window.ConfigBridge.post(path, bodyJson)
     * Used by app.js to send POST bodies that WebResourceRequest can't carry.
     */
    @JavascriptInterface
    public String post(String path, String bodyJson) {
        return nativeHandleRequest(path, "POST", bodyJson != null ? bodyJson : "");
    }

    /**
     * JavaScript interface: window.ConfigBridge.get(path)
     * Optional convenience for JS-initiated GETs.
     */
    @JavascriptInterface
    public String get(String path) {
        return nativeHandleRequest(path, "GET", "");
    }

    // ── JNI declarations (implemented in jni_bridge.c) ───────────────────────

    private native void   nativeInit(String filesDir);
    private native String nativeHandleRequest(String path, String method, String body);
}
