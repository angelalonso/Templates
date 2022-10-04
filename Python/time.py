#!/usr/bin/python3
from datetime import datetime as dt
import time

now = time.time()

test = dt.fromtimestamp(now)

result = dt.strftime(test, '%H:%M:%S.%f')

print(result)
