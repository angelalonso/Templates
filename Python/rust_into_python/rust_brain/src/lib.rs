extern crate cpython;

use cpython::{PyResult, Python, py_module_initializer, py_fn};

py_module_initializer!(rust_brain, |py, m| {
    m.add(py, "__doc__", "This module is implemented in Rust.")?;
    m.add(py, "return_sum", py_fn!(py, return_sum(i: i32, j: i32)))?;
    Ok(())
});

fn return_sum(_py: Python, i: i32, j: i32) -> PyResult<i32> {
    Ok(i + j)
}
