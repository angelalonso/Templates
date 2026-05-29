#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <stddef.h>

typedef enum {
    API_METHOD_GET,
    API_METHOD_POST,
    API_METHOD_UNKNOWN
} ApiMethod;

typedef struct {
    char path[256];
    ApiMethod method;
    char body[4096];
    size_t body_len;
} ApiRequest;

typedef struct {
    char status[32];
    char content_type[64];
    char body[8192];
    size_t body_len;
} ApiResponse;

// Parse a request from raw data
int api_parse_request(const char *uri, const char *method, const char *body, 
                      size_t body_len, ApiRequest *request);

// Handle API request and produce response
int api_handle_request(const ApiRequest *request, ApiResponse *response);

// Format response for transport
const char *api_format_response(const ApiResponse *response);

#endif
