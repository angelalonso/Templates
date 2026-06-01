# Usage Instructions
## Install dependencies (Ubuntu/Debian)
sudo apt-get install build-essential cmake clang clang-tidy clang-format valgrind gcovr mingw-w64

# Or use Docker for consistent environment
docker build -f Dockerfile.cross -t config-app-builder .
Development Workflow
bash
# Format code
make format

# Lint code
make lint

# Build and test locally
make build
make test

# Run with valgrind
make valgrind

# Cross-compile for all platforms
make cross

# Using Docker (reproducible builds)
make docker-build
make docker-test
make docker-lint

## NEWER  Testing Android

docker run -d --privileged \
  -p 6080:6080 \
  -p 5554:5554 \
  -p 5555:5555 \
  -e EMULATOR_DEVICE="Samsung Galaxy S10" \
  -e WEB_VNC=true \
  --device /dev/kvm \
  --name my-android-emulator \
  budtmo/docker-android:emulator_11.0

adb connect localhost:5555

adb install your_app.apk
adb shell pm list packages | grep config
adb shell am force-stop com.android.launcher3
adb shell am start -W -n com.android.launcher3/com.android.launcher3.Launcher
adb shell dumpsys package your.package.name | grep -A 5 "android.intent.category.LAUNCHER"
adb shell monkey -p your.package.name -c android.intent.category.LAUNCHER 1

ID=$(docker ps -q --filter "name=my-android-emulator"); docker kill $ID; docker rm $ID

