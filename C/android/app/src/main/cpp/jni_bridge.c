// app/src/main/cpp/jni_bridge.c
//
// JNI glue layer. This replaces main.c and webkit_handler.c as the entry point
// for the C backend on Android. It exposes config_init() and api_handle_request()
// to Java through the JNI naming convention:
//
//   Java_<package_underscored>_<ClassName>_<methodName>
//
// No GTK, no WebKit2GTK — those are entirely absent here.

#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>

#include "api_handler.h"
#include "config_manager.h"
#include "api_protocol.h"

#define TAG "ConfigEditorJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

// ── Helpers ──────────────────────────────────────────────────────────────────

static char g_files_dir[512] = {0};

// Build the full path to cfg.yml inside the app's private files directory.
// On Linux the app used a relative "cfg.yml"; on Android we need an absolute path.
static void set_config_path(const char *files_dir) {
    snprintf(g_files_dir, sizeof(g_files_dir), "%s", files_dir);
    // config_manager.c uses CONFIG_FILE = "cfg.yml"; on Android we override
    // this by changing the working directory to filesDir so relative paths work.
    if (chdir(files_dir) != 0) {
        LOGE("chdir to filesDir failed: %s", files_dir);
    }
    LOGI("Config directory set to: %s", files_dir);
}

// ── JNI: ConfigBridge.nativeInit ─────────────────────────────────────────────

JNIEXPORT void JNICALL
Java_com_example_configeditor_ConfigBridge_nativeInit(JNIEnv *env, jobject thiz,
                                                       jstring files_dir_jstr) {
    (void)thiz;
    const char *files_dir = (*env)->GetStringUTFChars(env, files_dir_jstr, NULL);
    if (!files_dir) {
        LOGE("nativeInit: failed to get filesDir string");
        return;
    }

    set_config_path(files_dir);

    if (config_init()) {
        LOGI("config_init() succeeded");
    } else {
        LOGE("config_init() failed");
    }

    (*env)->ReleaseStringUTFChars(env, files_dir_jstr, files_dir);
}

// ── JNI: ConfigBridge.nativeHandleRequest ────────────────────────────────────

JNIEXPORT jstring JNICALL
Java_com_example_configeditor_ConfigBridge_nativeHandleRequest(JNIEnv *env, jobject thiz,
                                                                jstring path_jstr,
                                                                jstring method_jstr,
                                                                jstring body_jstr) {
    (void)thiz;

    const char *path   = (*env)->GetStringUTFChars(env, path_jstr,   NULL);
    const char *method = (*env)->GetStringUTFChars(env, method_jstr, NULL);
    const char *body   = (*env)->GetStringUTFChars(env, body_jstr,   NULL);

    if (!path || !method) {
        LOGE("nativeHandleRequest: null path or method");
        if (path)   (*env)->ReleaseStringUTFChars(env, path_jstr,   path);
        if (method) (*env)->ReleaseStringUTFChars(env, method_jstr, method);
        if (body)   (*env)->ReleaseStringUTFChars(env, body_jstr,   body);
        return (*env)->NewStringUTF(env, "{\"status\":\"ERROR\",\"message\":\"null args\"}");
    }

    LOGI("nativeHandleRequest: path=%s method=%s", path, method);

    size_t body_len = body ? strlen(body) : 0;

    ApiRequest  request;
    ApiResponse response;

    jstring result;

    if (api_parse_request(path, method, body, body_len, &request) &&
        api_handle_request(&request, &response)) {
        result = (*env)->NewStringUTF(env, response.body);
    } else {
        // api_handle_request returns 0 for 404s but still fills response.body
        // so we return whatever it wrote (the 404 JSON).
        if (response.body[0] != '\0') {
            result = (*env)->NewStringUTF(env, response.body);
        } else {
            result = (*env)->NewStringUTF(env,
                "{\"status\":\"ERROR\",\"message\":\"Request handling failed\"}");
        }
    }

    (*env)->ReleaseStringUTFChars(env, path_jstr,   path);
    (*env)->ReleaseStringUTFChars(env, method_jstr, method);
    if (body) (*env)->ReleaseStringUTFChars(env, body_jstr, body);

    return result;
}
