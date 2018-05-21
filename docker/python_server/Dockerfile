FROM python:3.7-rc-stretch

RUN apt update

WORKDIR /app

COPY . /app

RUN pip install -r requirements.txt

EXPOSE 8001

CMD ["python", "-u", "main.py"]
