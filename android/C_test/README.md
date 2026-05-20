# What is this

Proof of concept to build and run android applications without Android Studio.
I chose C because that's the next language I want to learn.

# Requirements

## Prepare apt packages
sudo apt update && sudo apt install -y openjdk-11-jdk wget unzip zip curl

## Create SDK directory
mkdir -p ~/android-sdk/cmdline-tools && cd ~/android-sdk

## Set environment variables (add to ~/.bashrc for permanence)
export ANDROID_HOME=$HOME/android-sdk
export ANDROID_SDK_ROOT=$ANDROID_HOME
export PATH=$PATH:$ANDROID_HOME/cmdline-tools/latest/bin
export PATH=$PATH:$ANDROID_HOME/platform-tools
export PATH=$PATH:$ANDROID_HOME/build-tools/33.0.0

## Download the command-line tools (get latest version from Google)
cd ~/android-sdk/cmdline-tools
wget https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip
unzip commandlinetools-linux-*.zip
mv cmdline-tools latest

# Accept licenses (you'll need to type 'y' for each)
yes | ~/android-sdk/cmdline-tools/latest/bin/sdkmanager --licenses

# Install required components
~/android-sdk/cmdline-tools/latest/bin/sdkmanager "platform-tools"
~/android-sdk/cmdline-tools/latest/bin/sdkmanager "build-tools;33.0.0"
~/android-sdk/cmdline-tools/latest/bin/sdkmanager "platforms;android-33"
~/android-sdk/cmdline-tools/latest/bin/sdkmanager "ndk;26.1.10909125"

# Set NDK environment variable
export ANDROID_NDK_HOME=$ANDROID_HOME/ndk/26.1.10909125

# build
./build.sh

# Emulator
sdkmanager "emulator"
export PATH=$PATH:$ANDROID_HOME/emulator

sdkmanager "system-images;android-33;google_apis;x86_64"

avdmanager create avd -n accelerometer_test -k "system-images;android-33;google_apis;x86_64" --device "pixel_6"

emulator -avd accelerometer_test

### Performance Tip 
sudo apt install qemu-kvm
sudo adduser $USER kvm

## Send events
# Connect to emulator console (port 5554 for first emulator)
telnet localhost 5554

# Then send sensor commands:
sensor set acceleration 9.8:0:0    # X:9.8, Y:0, Z:0
sensor set acceleration 0:-9.8:0   # Tilted
