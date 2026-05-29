// src/backend/webkit_handler.c
#include "webkit_handler.h"
#include "../middleware/api_protocol.h"
#include "api_handler.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>

static char web_files_path[PATH_MAX] = {0};

static void init_web_path(void) {
    if (web_files_path[0] != '\0') {
        return;
    }
    
    // Try to find the web directory
    const char* search_paths[] = {
        "src/web",                    // Development path
        "../share/config-editor/web", // Installed path relative to bin
        "/usr/local/share/config-editor/web", // System install path
        "./web",                      // Current directory web folder
        NULL
    };
    
    // Get executable path
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        char* exe_dir = dirname(exe_path);
        
        // Try relative to executable
        char test_path[PATH_MAX];
        snprintf(test_path, sizeof(test_path), "%s/../share/config-editor/web", exe_dir);
        struct stat st;
        if (stat(test_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            strncpy(web_files_path, test_path, sizeof(web_files_path) - 1);
            return;
        }
        
        snprintf(test_path, sizeof(test_path), "%s/web", exe_dir);
        if (stat(test_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            strncpy(web_files_path, test_path, sizeof(web_files_path) - 1);
            return;
        }
    }
    
    // Try all search paths
    for (int i = 0; search_paths[i] != NULL; i++) {
        struct stat st;
        if (stat(search_paths[i], &st) == 0 && S_ISDIR(st.st_mode)) {
            strncpy(web_files_path, search_paths[i], sizeof(web_files_path) - 1);
            return;
        }
    }
    
    // Fallback to current directory
    strncpy(web_files_path, ".", sizeof(web_files_path) - 1);
}

static char* read_file(const char* path, size_t* size) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size <= 0) {
        fclose(file);
        return NULL;
    }
    
    char* buffer = (char*)g_malloc(file_size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    // Cast to void to explicitly ignore unused result
    (void)fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    *size = file_size;
    
    fclose(file);
    return buffer;
}

static void serve_static_file(WebKitURISchemeRequest *request, const char* path, const char* mime_type) {
    init_web_path();
    
    char full_path[PATH_MAX];
    
    // Handle different path requests
    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
        snprintf(full_path, sizeof(full_path), "%s/index.html", web_files_path);
    } else if (strcmp(path, "/style.css") == 0) {
        snprintf(full_path, sizeof(full_path), "%s/style.css", web_files_path);
    } else if (strcmp(path, "/app.js") == 0) {
        snprintf(full_path, sizeof(full_path), "%s/app.js", web_files_path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s%s", web_files_path, path);
    }
    
    size_t file_size = 0;
    char* content = read_file(full_path, &file_size);
    
    if (content && file_size > 0) {
        GInputStream *input_stream = g_memory_input_stream_new_from_data(
            content, file_size, g_free);
        webkit_uri_scheme_request_finish(request, input_stream, file_size, mime_type);
        g_object_unref(input_stream);
        return;
    }
    
    // 404 Not Found
    char error_html[512];
    snprintf(error_html, sizeof(error_html), 
             "<html><body style='font-family:monospace;padding:20px'>"
             "<h1>404 Not Found</h1>"
             "<p>File: %s</p>"
             "<p>Web path: %s</p>"
             "</body></html>", 
             path, web_files_path);
    
    GInputStream *input_stream = g_memory_input_stream_new_from_data(
        g_strdup(error_html), strlen(error_html), g_free);
    webkit_uri_scheme_request_finish(request, input_stream, -1, "text/html");
    g_object_unref(input_stream);
}

// Callback for handling URI scheme requests
static void uri_scheme_request_cb(WebKitURISchemeRequest *request, gpointer user_data) {
    (void)user_data;
    
    const gchar *path = webkit_uri_scheme_request_get_path(request);
    const gchar *method = webkit_uri_scheme_request_get_http_method(request);
    
    // Parse body for POST requests
    char body[4096] = {0};
    size_t body_len = 0;
    
    if (method && g_strcmp0(method, "POST") == 0) {
        GInputStream *input_stream = webkit_uri_scheme_request_get_http_body(request);
        if (input_stream) {
            char buffer[1024];
            gssize bytes_read;
            GError *error = NULL;
            
            while ((bytes_read = g_input_stream_read(input_stream, buffer, sizeof(buffer) - 1, NULL, &error)) > 0) {
                if (body_len + bytes_read < sizeof(body) - 1) {
                    memcpy(body + body_len, buffer, bytes_read);
                    body_len += bytes_read;
                }
            }
            body[body_len] = '\0';
            
            if (error) {
                g_error_free(error);
            }
        }
    }
    
    // Handle API endpoints
    if (path && g_str_has_prefix(path, "/api/")) {
        ApiRequest api_request;
        ApiResponse api_response;
        
        if (api_parse_request(path, method, body, body_len, &api_request)) {
            if (api_handle_request(&api_request, &api_response)) {
                GInputStream *input_stream = g_memory_input_stream_new_from_data(
                    g_strdup(api_response.body), api_response.body_len, g_free);
                webkit_uri_scheme_request_finish(request, input_stream, api_response.body_len, 
                                                api_response.content_type);
                g_object_unref(input_stream);
                return;
            }
        }
        
        // API Error response
        const char* error_json = "{\"status\":\"ERROR\",\"message\":\"API Error\"}";
        GInputStream *input_stream = g_memory_input_stream_new_from_data(
            g_strdup(error_json), strlen(error_json), g_free);
        webkit_uri_scheme_request_finish(request, input_stream, -1, "application/json");
        g_object_unref(input_stream);
        return;
    }
    
    // Serve static files
    if (!path || g_str_equal(path, "/") || g_str_equal(path, "/index.html")) {
        serve_static_file(request, "/index.html", "text/html");
    } else if (g_str_has_suffix(path, ".css")) {
        serve_static_file(request, path, "text/css");
    } else if (g_str_has_suffix(path, ".js")) {
        serve_static_file(request, path, "application/javascript");
    } else {
        serve_static_file(request, path, "text/plain");
    }
}

void setup_webkit_handler(WebKitWebView *web_view) {
    (void)web_view;
    
    init_web_path();
    
    webkit_web_context_register_uri_scheme(
        webkit_web_context_get_default(),
        API_SCHEME,
        (WebKitURISchemeRequestCallback)uri_scheme_request_cb,
        NULL,
        NULL
    );
}
