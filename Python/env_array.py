#!/usr/bin/env python3

from dotenv import load_dotenv
from os import getenv

load_dotenv()                                                                                                                                                                                            
ARRAY = getenv('ARRAY').split(',')

print(ARRAY)
if 'value3' in ARRAY:
    print("Found value3")
if 'value2' in ARRAY:
    print("Found value2")
