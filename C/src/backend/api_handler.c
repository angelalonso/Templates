// src/backend/api_handler.c
#include "api_handler.h"
#include "config_manager.h"
#include "../middleware/api_protocol.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>

int api_parse_request(const char *uri, const char *method, const char *body,
                      size_t body_len, ApiRequest *request) {
    if (!uri || !request) return 0;
    
    printf("[API] Parsing request: uri='%s', method='%s', body_len=%zu\n", 
           uri, method ? method : "NULL", body_len);
    
    // Parse method
    if (method && strcmp(method, "GET") == 0) {
        request->method = API_METHOD_GET;
        printf("[API] Method: GET\n");
    } else if (method && strcmp(method, "POST") == 0) {
        request->method = API_METHOD_POST;
        printf("[API] Method: POST\n");
    } else {
        request->method = API_METHOD_UNKNOWN;
        printf("[API] Method: UNKNOWN\n");
    }
    
    // Extract path from URI (remove query parameters)
    const char *query = strchr(uri, '?');
    if (query) {
        size_t path_len = query - uri;
        if (path_len >= sizeof(request->path)) path_len = sizeof(request->path) - 1;
        memcpy(request->path, uri, path_len);
        request->path[path_len] = '\0';
    } else {
        strncpy(request->path, uri, sizeof(request->path) - 1);
        request->path[sizeof(request->path) - 1] = '\0';
    }
    printf("[API] Path: %s\n", request->path);
    
    // Copy body
    if (body && body_len > 0) {
        size_t copy_len = body_len;
        if (copy_len >= sizeof(request->body)) copy_len = sizeof(request->body) - 1;
        memcpy(request->body, body, copy_len);
        request->body[copy_len] = '\0';
        request->body_len = copy_len;
        printf("[API] Body: %s\n", request->body);
    } else {
        request->body[0] = '\0';
        request->body_len = 0;
        printf("[API] Body: (empty)\n");
    }
    
    return 1;
}

static void handle_read_config(ApiResponse *response) {
    char buffer[4096];
    printf("[API] Reading config...\n");
    
    if (config_read_text(buffer, sizeof(buffer))) {
        printf("[API] Config read: '%s'\n", buffer);
        struct json_object *json_obj = json_object_new_object();
        json_object_object_add(json_obj, "text", json_object_new_string(buffer));
        json_object_object_add(json_obj, "status", json_object_new_string(API_STATUS_OK));
        
        const char *json_str = json_object_to_json_string(json_obj);
        printf("[API] Response JSON: %s\n", json_str);
        
        strncpy(response->body, json_str, sizeof(response->body) - 1);
        response->body[sizeof(response->body) - 1] = '\0';
        response->body_len = strlen(response->body);
        
        strcpy(response->status, "200");
        strcpy(response->content_type, "application/json");
        
        json_object_put(json_obj);
        printf("[API] Read config response prepared\n");
    } else {
        printf("[API] No config found, returning empty\n");
        struct json_object *json_obj = json_object_new_object();
        json_object_object_add(json_obj, "text", json_object_new_string(""));
        json_object_object_add(json_obj, "status", json_object_new_string(API_STATUS_OK));
        
        const char *json_str = json_object_to_json_string(json_obj);
        strncpy(response->body, json_str, sizeof(response->body) - 1);
        response->body[sizeof(response->body) - 1] = '\0';
        response->body_len = strlen(response->body);
        
        strcpy(response->status, "200");
        strcpy(response->content_type, "application/json");
        
        json_object_put(json_obj);
    }
}

static void handle_write_config(const ApiRequest *request, ApiResponse *response) {
    printf("[API] Handling write config request\n");
    printf("[API] Request body: %s\n", request->body);
    
    struct json_object *parsed_json = json_tokener_parse(request->body);
    struct json_object *text_obj = NULL;
    
    if (!parsed_json) {
        printf("[API] ERROR: Failed to parse JSON\n");
        struct json_object *error_obj = json_object_new_object();
        json_object_object_add(error_obj, "status", json_object_new_string(API_STATUS_ERROR));
        json_object_object_add(error_obj, "message", json_object_new_string("Invalid JSON"));
        
        const char *json_str = json_object_to_json_string(error_obj);
        strncpy(response->body, json_str, sizeof(response->body) - 1);
        response->body[sizeof(response->body) - 1] = '\0';
        response->body_len = strlen(response->body);
        strcpy(response->status, "400");
        strcpy(response->content_type, "application/json");
        
        json_object_put(error_obj);
        return;
    }
    
    printf("[API] JSON parsed successfully\n");
    
    if (json_object_object_get_ex(parsed_json, "text", &text_obj)) {
        const char *text = json_object_get_string(text_obj);
        printf("[API] Extracted text field: '%s'\n", text);
        
        if (config_write_text(text)) {
            printf("[API] Config written successfully\n");
            struct json_object *success_obj = json_object_new_object();
            json_object_object_add(success_obj, "status", json_object_new_string(API_STATUS_OK));
            
            const char *json_str = json_object_to_json_string(success_obj);
            strncpy(response->body, json_str, sizeof(response->body) - 1);
            response->body[sizeof(response->body) - 1] = '\0';
            response->body_len = strlen(response->body);
            strcpy(response->status, "200");
            strcpy(response->content_type, "application/json");
            
            json_object_put(success_obj);
            printf("[API] Success response prepared\n");
        } else {
            printf("[API] ERROR: Failed to write config\n");
            struct json_object *error_obj = json_object_new_object();
            json_object_object_add(error_obj, "status", json_object_new_string(API_STATUS_ERROR));
            json_object_object_add(error_obj, "message", json_object_new_string("Write failed - check file permissions"));
            
            const char *json_str = json_object_to_json_string(error_obj);
            strncpy(response->body, json_str, sizeof(response->body) - 1);
            response->body[sizeof(response->body) - 1] = '\0';
            response->body_len = strlen(response->body);
            strcpy(response->status, "500");
            strcpy(response->content_type, "application/json");
            
            json_object_put(error_obj);
        }
    } else {
        printf("[API] ERROR: Missing 'text' field in JSON\n");
        struct json_object *error_obj = json_object_new_object();
        json_object_object_add(error_obj, "status", json_object_new_string(API_STATUS_ERROR));
        json_object_object_add(error_obj, "message", json_object_new_string("Missing 'text' field"));
        
        const char *json_str = json_object_to_json_string(error_obj);
        strncpy(response->body, json_str, sizeof(response->body) - 1);
        response->body[sizeof(response->body) - 1] = '\0';
        response->body_len = strlen(response->body);
        strcpy(response->status, "400");
        strcpy(response->content_type, "application/json");
        
        json_object_put(error_obj);
    }
    
    json_object_put(parsed_json);
}

int api_handle_request(const ApiRequest *request, ApiResponse *response) {
    if (!request || !response) {
        printf("[API] ERROR: Invalid request or response pointer\n");
        return 0;
    }
    
    printf("[API] Handling request: path='%s', method=%d\n", request->path, request->method);
    
    // Initialize response
    memset(response, 0, sizeof(ApiResponse));
    
    if (strcmp(request->path, API_ENDPOINT_READ) == 0 && request->method == API_METHOD_GET) {
        printf("[API] Handling READ request\n");
        handle_read_config(response);
        return 1;
    }
    
    if (strcmp(request->path, API_ENDPOINT_WRITE) == 0 && request->method == API_METHOD_POST) {
        printf("[API] Handling WRITE request\n");
        handle_write_config(request, response);
        return 1;
    }
    
    printf("[API] ERROR: Endpoint not found: %s\n", request->path);
    // 404 Not Found
    struct json_object *error_obj = json_object_new_object();
    json_object_object_add(error_obj, "status", json_object_new_string(API_STATUS_ERROR));
    json_object_object_add(error_obj, "message", json_object_new_string("Endpoint not found"));
    
    const char *json_str = json_object_to_json_string(error_obj);
    strncpy(response->body, json_str, sizeof(response->body) - 1);
    response->body[sizeof(response->body) - 1] = '\0';
    response->body_len = strlen(response->body);
    strcpy(response->status, "404");
    strcpy(response->content_type, "application/json");
    
    json_object_put(error_obj);
    return 0;
}

const char *api_format_response(const ApiResponse *response) {
    static char formatted[8192];
    int len = snprintf(formatted, sizeof(formatted), 
             "HTTP/1.1 %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "\r\n"
             "%s",
             response->status,
             response->content_type,
             response->body_len,
             response->body);
    
    printf("[API] Formatted response (length %d):\n%s\n", len, formatted);
    
    // Ensure null termination
    if (len >= (int)sizeof(formatted)) {
        formatted[sizeof(formatted) - 1] = '\0';
    }
    
    return formatted;
}
