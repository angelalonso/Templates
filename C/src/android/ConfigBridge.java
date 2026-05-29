// Android/ConfigBridge.java
package com.example.configapp;

import android.webkit.JavascriptInterface;
import android.webkit.WebView;
import org.json.JSONObject;

public class ConfigBridge {
    private WebView webView;
    
    public ConfigBridge(WebView webView) {
        this.webView = webView;
    }
    
    @JavascriptInterface
    public String apiCall(String endpoint, String method, String data) {
        try {
            if (endpoint.equals("/api/config/read") && method.equals("GET")) {
                String text = readConfigText();
                JSONObject response = new JSONObject();
                response.put("status", "OK");
                response.put("text", text);
                return response.toString();
            }
            
            if (endpoint.equals("/api/config/write") && method.equals("POST")) {
                JSONObject request = new JSONObject(data);
                String text = request.getString("text");
                boolean success = writeConfigText(text);
                
                JSONObject response = new JSONObject();
                response.put("status", success ? "OK" : "ERROR");
                return response.toString();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        return "{\"status\":\"ERROR\",\"message\":\"Invalid request\"}";
    }
    
    private native String readConfigText();
    private native boolean writeConfigText(String text);
}
