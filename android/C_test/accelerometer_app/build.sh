#!/bin/bash

# Set paths
export ANDROID_HOME=$HOME/android-sdk
export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/26.1.10909125
export PATH=$PATH:$ANDROID_HOME/build-tools/33.0.0
export PATH=$PATH:$ANDROID_HOME/platform-tools

echo "Step 1: Compiling native code..."
$ANDROID_NDK_HOME/ndk-build

if [ $? -ne 0 ]; then
    echo "NDK build failed!"
    exit 1
fi

echo "Step 2: Creating APK..."
aapt package -f \
    -M AndroidManifest.xml \
    -S res/ \
    -I $ANDROID_HOME/platforms/android-33/android.jar \
    -F app-unaligned.apk \
    libs/

echo "Step 3: Adding native library to APK..."
cd libs/armeabi-v7a
zip ../../app-unaligned.apk libmain.so
cd ../..

echo "Step 4: Aligning APK..."
zipalign -f -p 4 app-unaligned.apk app-unsigned.apk

echo "Step 5: Signing APK..."
# Generate debug keystore if it doesn't exist
if [ ! -f ~/.android/debug.keystore ]; then
    keytool -genkey -v -keystore ~/.android/debug.keystore \
        -alias androiddebugkey -keyalg RSA -keysize 2048 -validity 10000 \
        -storepass android -keypass android \
        -dname "CN=Android Debug, O=Android, C=US"
fi

apksigner sign --ks ~/.android/debug.keystore \
    --ks-pass pass:android \
    --out app-signed.apk \
    app-unsigned.apk

echo "Build complete! APK is: app-signed.apk"
