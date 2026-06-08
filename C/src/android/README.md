
gradle wrappers, generate

docker run --rm \
    -v "$(pwd)/android":/project \
    -w /project \
    eclipse-temurin:17 \
    bash -c "apt-get update -q && apt-get install -q -y unzip wget && \
             wget -q https://services.gradle.org/distributions/gradle-8.4-bin.zip -O /tmp/gradle.zip && \
             unzip -q /tmp/gradle.zip -d /tmp && \
             /tmp/gradle-8.4/bin/gradle wrapper --gradle-version 8.4"
