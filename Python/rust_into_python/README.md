# Steps to generate an .so we can use from Python

## On the rust code dir
python3 -m venv .env
source .env/bin/activate
pip install maturin
maturin develop
deactivate

## On the python code
'''
import importlib.util
my_spec = importlib.util.spec_from_file_location("myrustcode", 
        "./path/to/the/myrustcode.so")
myrustcode = importlib.util.module_from_spec(my_spec)
my_spec.loader.exec_module(myrustcode)
'''
