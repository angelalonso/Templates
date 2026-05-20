#include <stdio.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/sensor.h>
#include <stdlib.h>
#include <string.h>

// Define the sensor delay constant if not available
#ifndef SENSOR_DELAY_NORMAL
#define SENSOR_DELAY_NORMAL 3
#endif

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AccelApp", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "AccelApp", __VA_ARGS__))

struct engine {
    struct android_app* app;
    ASensorManager* sensorManager;
    const ASensor* accelerometer;
    ASensorEventQueue* sensorEventQueue;
    float lastX, lastY, lastZ;
};

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    
    switch(cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("Window initialized - setting up sensors");
            
            // Get sensor manager
            engine->sensorManager = ASensorManager_getInstance();
            if (!engine->sensorManager) {
                LOGE("Failed to get sensor manager");
                break;
            }
            
            // Get accelerometer sensor
            engine->accelerometer = ASensorManager_getDefaultSensor(
                engine->sensorManager, ASENSOR_TYPE_ACCELEROMETER);
            
            if (!engine->accelerometer) {
                LOGE("No accelerometer sensor found!");
                break;
            }
            
            LOGI("Accelerometer found: %s", ASensor_getName(engine->accelerometer));
            
            // Create event queue
            engine->sensorEventQueue = ASensorManager_createEventQueue(
                engine->sensorManager, app->looper, LOOPER_ID_USER, NULL, NULL);
            
            if (!engine->sensorEventQueue) {
                LOGE("Failed to create sensor event queue");
                break;
            }
            
            // Enable sensor with 20,000 microseconds delay (50Hz)
            int rate = 20000;  // microseconds between events
            if (ASensorEventQueue_enableSensor(engine->sensorEventQueue, engine->accelerometer) < 0) {
                LOGE("Failed to enable accelerometer");
            }
            
            if (ASensorEventQueue_setEventRate(engine->sensorEventQueue, engine->accelerometer, rate) < 0) {
                LOGE("Failed to set sensor event rate");
            }
            
            LOGI("Accelerometer enabled successfully");
            break;
            
        case APP_CMD_TERM_WINDOW:
            LOGI("Window destroyed - cleaning up sensors");
            if (engine->sensorEventQueue && engine->accelerometer) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue, engine->accelerometer);
                ASensorManager_destroyEventQueue(engine->sensorManager, engine->sensorEventQueue);
                engine->sensorEventQueue = NULL;
            }
            break;
            
        case APP_CMD_LOST_FOCUS:
            LOGI("App lost focus");
            break;
            
        case APP_CMD_GAINED_FOCUS:
            LOGI("App gained focus");
            break;
    }
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    return 0;  // Return 0 if event isn't handled
}

void android_main(struct android_app* app) {
    struct engine engine;
    memset(&engine, 0, sizeof(engine));
    
    app->userData = &engine;
    app->onAppCmd = engine_handle_cmd;
    app->onInputEvent = engine_handle_input;
    engine.app = app;
    
    LOGI("Starting accelerometer app");
    
    // Main loop
    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;
        
        // Poll for events (0 = don't wait, return immediately)
        while ((ident = ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) {
                source->process(app, source);
            }
            
            // Process sensor events
            if (ident == LOOPER_ID_USER && engine.sensorEventQueue) {
                ASensorEvent event;
                // Read all pending events
                while (ASensorEventQueue_getEvents(engine.sensorEventQueue, &event, 1) > 0) {
                    if (event.type == ASENSOR_TYPE_ACCELEROMETER) {
                        engine.lastX = event.acceleration.x;
                        engine.lastY = event.acceleration.y;
                        engine.lastZ = event.acceleration.z;
                        
                        // Calculate magnitude (for shaking detection)
                        float magnitude = sqrt(engine.lastX * engine.lastX + 
                                              engine.lastY * engine.lastY + 
                                              engine.lastZ * engine.lastZ);
                        
                        // Log the accelerometer data
                        LOGI("X: %7.2f  Y: %7.2f  Z: %7.2f  |  Mag: %5.2f", 
                             engine.lastX, engine.lastY, engine.lastZ, magnitude);
                    }
                }
            }
        }
        
        // Check if we need to exit
        if (app->destroyRequested) {
            LOGI("Destroy requested - exiting");
            return;
        }
    }
}
