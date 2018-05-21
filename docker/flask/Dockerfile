FROM alpine:latest

COPY . /app

WORKDIR /app

RUN apk update; \
    apk add --no-cache python3 ; \
    pip3 install -r requirements.txt

ENTRYPOINT ["python3"]

CMD ["app.py"]


