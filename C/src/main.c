#include <webkit2/webkit2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

static GtkWidget *window = NULL;
static WebKitWebView *web_view = NULL;

// Callback for handling URI scheme requests
static void uri_scheme_request_cb(WebKitURISchemeRequest *request, gpointer user_data) {
    const gchar* path = webkit_uri_scheme_request_get_path(request);
    const gchar* method = webkit_uri_scheme_request_get_http_method(request);
    
    if (g_str_equal(path, "/read-config")) {
        // Read config and return as response
        char buffer[MAX_VALUE_LEN];
        GInputStream *input_stream;
        
        if (read_text_value(buffer, sizeof(buffer)) && strlen(buffer) > 0) {
            input_stream = g_memory_input_stream_new_from_data(
                g_strdup(buffer), strlen(buffer), g_free);
        } else {
            // Return empty string if no config
            input_stream = g_memory_input_stream_new_from_data(
                g_strdup(""), 0, g_free);
        }
        
        webkit_uri_scheme_request_finish(request, input_stream, -1, "text/plain");
        g_object_unref(input_stream);
    } 
    else if (g_str_equal(path, "/write-config") && method && g_str_equal(method, "POST")) {
        // Get the POST data
        GInputStream *input_stream = webkit_uri_scheme_request_get_http_body(request);
        GError *error = NULL;
        char buffer[4096];
        GString *content = g_string_new("");
        gssize bytes_read;
        
        // Read all data from the stream
        if (input_stream) {
            while ((bytes_read = g_input_stream_read(input_stream, buffer, sizeof(buffer) - 1, NULL, &error)) > 0) {
                buffer[bytes_read] = '\0';
                g_string_append(content, buffer);
            }
            
            if (content->len > 0) {
                // Parse the JSON manually to extract the "text" field
                char *json_str = content->str;
                char *text_start = strstr(json_str, "\"text\"");
                
                if (text_start) {
                    // Find the colon after "text"
                    char *colon = strchr(text_start, ':');
                    if (colon) {
                        // Find the opening quote
                        char *quote1 = strchr(colon, '"');
                        if (quote1) {
                            // Find the closing quote
                            char *quote2 = strchr(quote1 + 1, '"');
                            if (quote2) {
                                // Extract the text between quotes
                                int text_len = quote2 - (quote1 + 1);
                                char *text_value = g_malloc(text_len + 1);
                                strncpy(text_value, quote1 + 1, text_len);
                                text_value[text_len] = '\0';
                                
                                // Unescape common JSON escape sequences
                                char *src = text_value;
                                char *dst = text_value;
                                while (*src) {
                                    if (*src == '\\' && *(src + 1)) {
                                        src++;
                                        switch (*src) {
                                            case 'n': *dst++ = '\n'; break;
                                            case 'r': *dst++ = '\r'; break;
                                            case 't': *dst++ = '\t'; break;
                                            case '"': *dst++ = '"'; break;
                                            case '\\': *dst++ = '\\'; break;
                                            default: *dst++ = *src; break;
                                        }
                                    } else {
                                        *dst++ = *src;
                                    }
                                    src++;
                                }
                                *dst = '\0';
                                
                                // Write to config file
                                GInputStream *response_stream;
                                if (update_text_value(text_value)) {
                                    response_stream = g_memory_input_stream_new_from_data(
                                        g_strdup("OK"), 2, g_free);
                                    webkit_uri_scheme_request_finish(request, response_stream, -1, "text/plain");
                                    g_object_unref(response_stream);
                                } else {
                                    response_stream = g_memory_input_stream_new_from_data(
                                        g_strdup("ERROR"), 5, g_free);
                                    webkit_uri_scheme_request_finish(request, response_stream, -1, "text/plain");
                                    g_object_unref(response_stream);
                                }
                                
                                g_free(text_value);
                                g_string_free(content, TRUE);
                                return;
                            }
                        }
                    }
                }
            }
        }
        
        // If we get here, something went wrong
        GInputStream *response_stream = g_memory_input_stream_new_from_data(
            g_strdup("ERROR"), 5, g_free);
        webkit_uri_scheme_request_finish(request, response_stream, -1, "text/plain");
        g_object_unref(response_stream);
        g_string_free(content, TRUE);
        
        if (error) g_error_free(error);
    }
    else {
        // Serve the main HTML file for root path
        if (g_str_equal(path, "/") || g_str_equal(path, "/index.html")) {
            const char* html = 
                "<!DOCTYPE html>"
                "<html>"
                "<head>"
                "<meta charset='UTF-8'>"
                "<title>Configuration Editor</title>"
                "<style>"
                "* { margin: 0; padding: 0; box-sizing: border-box; }\n"
                "body {\n"
                "    font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, sans-serif;\n"
                "    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
                "    min-height: 100vh;\n"
                "    display: flex;\n"
                "    justify-content: center;\n"
                "    align-items: center;\n"
                "    padding: 20px;\n"
                "}\n"
                ".container {\n"
                "    background: white;\n"
                "    border-radius: 10px;\n"
                "    box-shadow: 0 20px 60px rgba(0,0,0,0.3);\n"
                "    width: 100%;\n"
                "    max-width: 800px;\n"
                "    overflow: hidden;\n"
                "}\n"
                ".header {\n"
                "    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
                "    color: white;\n"
                "    padding: 30px;\n"
                "    text-align: center;\n"
                "}\n"
                ".header h1 {\n"
                "    font-size: 28px;\n"
                "    font-weight: 600;\n"
                "}\n"
                ".content {\n"
                "    padding: 30px;\n"
                "}\n"
                ".display-area h2 {\n"
                "    color: #333;\n"
                "    font-size: 18px;\n"
                "    margin-bottom: 15px;\n"
                "    font-weight: 600;\n"
                "}\n"
                ".config-display {\n"
                "    background: #f7f9fc;\n"
                "    border: 1px solid #e1e8ed;\n"
                "    border-radius: 8px;\n"
                "    padding: 20px;\n"
                "    min-height: 200px;\n"
                "    max-height: 400px;\n"
                "    overflow-y: auto;\n"
                "    font-family: 'Monaco', 'Menlo', monospace;\n"
                "    font-size: 14px;\n"
                "    line-height: 1.6;\n"
                "    color: #2c3e50;\n"
                "    white-space: pre-wrap;\n"
                "    word-wrap: break-word;\n"
                "}\n"
                ".button-area {\n"
                "    display: flex;\n"
                "    gap: 15px;\n"
                "    justify-content: center;\n"
                "    margin: 25px 0 20px 0;\n"
                "}\n"
                ".btn {\n"
                "    padding: 10px 30px;\n"
                "    font-size: 14px;\n"
                "    font-weight: 600;\n"
                "    border: none;\n"
                "    border-radius: 5px;\n"
                "    cursor: pointer;\n"
                "    transition: transform 0.2s, box-shadow 0.2s;\n"
                "}\n"
                ".btn:hover {\n"
                "    transform: translateY(-2px);\n"
                "    box-shadow: 0 5px 15px rgba(0,0,0,0.2);\n"
                "}\n"
                ".btn-primary {\n"
                "    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
                "    color: white;\n"
                "}\n"
                ".btn-secondary {\n"
                "    background: #6c757d;\n"
                "    color: white;\n"
                "}\n"
                ".status {\n"
                "    text-align: center;\n"
                "    padding: 10px;\n"
                "    border-radius: 5px;\n"
                "    font-size: 14px;\n"
                "    margin-top: 10px;\n"
                "}\n"
                ".status.success {\n"
                "    background: #d4edda;\n"
                "    color: #155724;\n"
                "}\n"
                ".status.error {\n"
                "    background: #f8d7da;\n"
                "    color: #721c24;\n"
                "}\n"
                ".modal {\n"
                "    display: none;\n"
                "    position: fixed;\n"
                "    z-index: 1000;\n"
                "    left: 0;\n"
                "    top: 0;\n"
                "    width: 100%;\n"
                "    height: 100%;\n"
                "    background-color: rgba(0,0,0,0.5);\n"
                "    animation: fadeIn 0.3s;\n"
                "}\n"
                "@keyframes fadeIn {\n"
                "    from { opacity: 0; }\n"
                "    to { opacity: 1; }\n"
                "}\n"
                ".modal-content {\n"
                "    background-color: white;\n"
                "    margin: 50px auto;\n"
                "    padding: 0;\n"
                "    width: 90%;\n"
                "    max-width: 700px;\n"
                "    border-radius: 10px;\n"
                "    box-shadow: 0 20px 60px rgba(0,0,0,0.3);\n"
                "    animation: slideIn 0.3s;\n"
                "}\n"
                "@keyframes slideIn {\n"
                "    from { transform: translateY(-50px); opacity: 0; }\n"
                "    to { transform: translateY(0); opacity: 1; }\n"
                "}\n"
                ".modal-header {\n"
                "    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n"
                "    color: white;\n"
                "    padding: 20px;\n"
                "    border-radius: 10px 10px 0 0;\n"
                "}\n"
                ".modal-header h2 {\n"
                "    margin: 0;\n"
                "    font-size: 20px;\n"
                "}\n"
                ".modal-body {\n"
                "    padding: 20px;\n"
                "}\n"
                ".modal-body textarea {\n"
                "    width: 100%;\n"
                "    min-height: 300px;\n"
                "    padding: 15px;\n"
                "    font-family: 'Monaco', 'Menlo', monospace;\n"
                "    font-size: 14px;\n"
                "    line-height: 1.6;\n"
                "    border: 1px solid #ddd;\n"
                "    border-radius: 5px;\n"
                "    resize: vertical;\n"
                "}\n"
                ".modal-footer {\n"
                "    padding: 20px;\n"
                "    display: flex;\n"
                "    gap: 15px;\n"
                "    justify-content: center;\n"
                "    border-top: 1px solid #eee;\n"
                "}\n"
                ".close {\n"
                "    float: right;\n"
                "    font-size: 28px;\n"
                "    font-weight: bold;\n"
                "    cursor: pointer;\n"
                "    color: white;\n"
                "}\n"
                ".close:hover {\n"
                "    opacity: 0.8;\n"
                "}\n"
                "</style>"
                "</head>"
                "<body>"
                "<div class='container'>"
                "<div class='header'>"
                "<h1>Configuration Editor</h1>"
                "</div>"
                "<div class='content'>"
                "<div class='display-area'>"
                "<h2>Current Configuration:</h2>"
                "<div class='config-display' id='configDisplay'>Loading...</div>"
                "</div>"
                "<div class='button-area'>"
                "<button class='btn btn-primary' id='editBtn'>Edit</button>"
                "<button class='btn btn-secondary' id='refreshBtn'>Refresh</button>"
                "</div>"
                "<div class='status' id='statusLabel'>Ready</div>"
                "</div>"
                "</div>"
                "<script>"
                "let currentConfig = '';\n"
                "const configDisplay = document.getElementById('configDisplay');\n"
                "const editBtn = document.getElementById('editBtn');\n"
                "const refreshBtn = document.getElementById('refreshBtn');\n"
                "const statusLabel = document.getElementById('statusLabel');\n"
                "function updateStatus(message, isError = false) {\n"
                "    statusLabel.textContent = message;\n"
                "    statusLabel.className = 'status ' + (isError ? 'error' : 'success');\n"
                "    setTimeout(() => {\n"
                "        if (statusLabel.textContent === message) {\n"
                "            statusLabel.className = 'status';\n"
                "            statusLabel.textContent = 'Ready';\n"
                "        }\n"
                "    }, 3000);\n"
                "}\n"
                "async function loadConfig() {\n"
                "    try {\n"
                "        const response = await fetch('/read-config');\n"
                "        if (response.ok) {\n"
                "            currentConfig = await response.text();\n"
                "            if (currentConfig && currentConfig.trim()) {\n"
                "                configDisplay.innerHTML = currentConfig.replace(/[&<>]/g, function(m) {\n"
                "                    if (m === '&') return '&amp;';\n"
                "                    if (m === '<') return '&lt;';\n"
                "                    if (m === '>') return '&gt;';\n"
                "                    return m;\n"
                "                }).replace(/\\n/g, '<br>');\n"
                "                updateStatus('Loaded successfully', false);\n"
                "            } else {\n"
                "                configDisplay.innerHTML = '<em>No configuration found. Click Edit to create new content.</em>';\n"
                "                updateStatus('No configuration found', true);\n"
                "            }\n"
                "        } else {\n"
                "            throw new Error('Failed to load config');\n"
                "        }\n"
                "    } catch (error) {\n"
                "        console.error('Error loading config:', error);\n"
                "        configDisplay.innerHTML = '<em class=\"error\">Error loading configuration</em>';\n"
                "        updateStatus('Error loading configuration', true);\n"
                "    }\n"
                "}\n"
                "async function saveConfig(text) {\n"
                "    try {\n"
                "        const response = await fetch('/write-config', {\n"
                "            method: 'POST',\n"
                "            headers: {\n"
                "                'Content-Type': 'application/json',\n"
                "            },\n"
                "            body: JSON.stringify({text: text})\n"
                "        });\n"
                "        if (response.ok) {\n"
                "            const result = await response.text();\n"
                "            if (result === 'OK') {\n"
                "                currentConfig = text;\n"
                "                updateStatus('Saved successfully!', false);\n"
                "                return true;\n"
                "            } else {\n"
                "                throw new Error('Save failed');\n"
                "            }\n"
                "        } else {\n"
                "            throw new Error('Failed to save config');\n"
                "        }\n"
                "    } catch (error) {\n"
                "        console.error('Error saving config:', error);\n"
                "        updateStatus('Error saving configuration!', true);\n"
                "        return false;\n"
                "    }\n"
                "}\n"
                "function showEditDialog() {\n"
                "    const modal = document.createElement('div');\n"
                "    modal.className = 'modal';\n"
                "    modal.id = 'editModal';\n"
                "    modal.innerHTML = `\n"
                "        <div class='modal-content'>\n"
                "            <div class='modal-header'>\n"
                "                <span class='close'>&times;</span>\n"
                "                <h2>Edit Configuration</h2>\n"
                "                <p>Modify the text below and click Save when done.</p>\n"
                "            </div>\n"
                "            <div class='modal-body'>\n"
                "                <textarea id='configTextarea' placeholder='Enter your configuration text here...'></textarea>\n"
                "            </div>\n"
                "            <div class='modal-footer'>\n"
                "                <button class='btn btn-primary' id='modalSaveBtn'>Save</button>\n"
                "                <button class='btn btn-secondary' id='modalCancelBtn'>Cancel</button>\n"
                "            </div>\n"
                "        </div>\n"
                "    `;\n"
                "    document.body.appendChild(modal);\n"
                "    modal.style.display = 'block';\n"
                "    const textarea = document.getElementById('configTextarea');\n"
                "    textarea.value = currentConfig;\n"
                "    const saveBtn = document.getElementById('modalSaveBtn');\n"
                "    const cancelBtn = document.getElementById('modalCancelBtn');\n"
                "    const closeBtn = modal.querySelector('.close');\n"
                "    const closeModal = () => { modal.remove(); };\n"
                "    saveBtn.onclick = async () => {\n"
                "        const newText = textarea.value;\n"
                "        const success = await saveConfig(newText);\n"
                "        if (success) {\n"
                "            closeModal();\n"
                "            await loadConfig();\n"
                "        }\n"
                "    };\n"
                "    cancelBtn.onclick = closeModal;\n"
                "    closeBtn.onclick = closeModal;\n"
                "    modal.onclick = (e) => { if (e.target === modal) closeModal(); };\n"
                "    textarea.focus();\n"
                "}\n"
                "editBtn.addEventListener('click', showEditDialog);\n"
                "refreshBtn.addEventListener('click', loadConfig);\n"
                "loadConfig();\n"
                "</script>"
                "</body>"
                "</html>";
            
            GInputStream *input_stream = g_memory_input_stream_new_from_data(
                g_strdup(html), strlen(html), g_free);
            webkit_uri_scheme_request_finish(request, input_stream, -1, "text/html");
            g_object_unref(input_stream);
        }
        else {
            webkit_uri_scheme_request_finish_error(request, g_error_new(
                G_IO_ERROR, G_IO_ERROR_NOT_FOUND, "File not found"));
        }
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Configuration Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 700);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    web_view = WEBKIT_WEB_VIEW(webkit_web_view_new());
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(web_view));
    
    // Register custom URI scheme
    webkit_web_context_register_uri_scheme(
        webkit_web_context_get_default(),
        "app",
        (WebKitURISchemeRequestCallback)uri_scheme_request_cb,
        NULL,
        NULL
    );
    
    // Load the app from custom scheme
    webkit_web_view_load_uri(web_view, "app:///index.html");
    
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;
    
    app = gtk_application_new("com.example.configapp", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    
    return status;
}
