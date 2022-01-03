#import ./rust_brain/target/release/librust_brain as rust_brain
import importlib.util
spec = importlib.util.spec_from_file_location("rust_brain", "./rust_brain/target/release/librust_brain.so")
rust_brain = importlib.util.module_from_spec(spec)
spec.loader.exec_module(rust_brain)

print(rust_brain.return_sum(4, 7))
