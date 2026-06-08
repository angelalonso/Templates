#!/usr/bin/env bash
# build-android.sh
# Run from your repo root:  ./src/android/build-android.sh
set -e

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
IMAGE_NAME="config-editor-android"
APK_OUT="$REPO_ROOT/src/android/app/build/outputs/apk/debug"

echo "==> Building Docker image..."
docker build \
    -f "$REPO_ROOT/docker/Dockerfile.android" \
    -t "$IMAGE_NAME" \
    "$REPO_ROOT"   # <-- build context is repo root so COPY . . gets everything

echo "==> Compiling APK inside container..."
mkdir -p "$APK_OUT"
docker run --rm \
    -v "$APK_OUT":/out \
    "$IMAGE_NAME" \
    bash -c "./gradlew assembleDebug --no-daemon && cp app/build/outputs/apk/debug/app-debug.apk /out/"

echo ""
echo "==> Done! APK is at:"
echo "    $APK_OUT/app-debug.apk"
