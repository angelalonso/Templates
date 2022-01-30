#![feature(macro_attributes_in_derive_output)]
use pyo3::prelude::*;

/// Formats the sum of two numbers as string.
#[pyfunction]
fn sum_as_string(a: usize, b: usize) -> PyResult<String> {
    Ok((a + b).to_string())
}

#[derive(Debug, Clone)]
#[pyclass]
pub struct Datapoint {
    #[pyo3(get, set)]
    pub x: i32,
    #[pyo3(get, set)]
    pub y: i32,
    #[pyo3(get, set)]
    pub solid: bool,
}
#[pymethods]
impl Datapoint {
    #[new]
    pub fn new(x: i32, y: i32, solid: bool) -> Datapoint {
        Datapoint { x, y, solid }
    }
}
#[derive(Debug, Clone)]
#[pyclass]
pub struct Dataset {
    #[pyo3(get, set)]
    pub set: Vec<Datapoint>,
}
#[pymethods]
impl Dataset {
    #[new]
    pub fn new(set: Vec<Datapoint>) -> Dataset {
        Dataset { set }
    }
    pub fn show(&self) -> Vec<String> {
        let mut result = Vec::new();
        for x in 1..11 {
            let mut line = "".to_owned();
            for y in 1..11 {
                let mut object = false;
                for dp in self.set.clone() {
                    if dp.x == x && dp.y == y && dp.solid {
                        object = true;
                    }
                }
                if object {
                    line.push_str("O");
                } else {
                    line.push_str(".");
                }
            }
            result.push(line);
        }
        result
    }
}


#[pyclass]
pub struct RustStruct {
    #[pyo3(get, set)]
    pub data: String,
    #[pyo3(get, set)]
    pub vector: Vec<u8>,
}
#[pymethods]
impl RustStruct {
    #[new]
    pub fn new(data: String, vector: Vec<u8>) -> RustStruct {
        RustStruct { data, vector }
    }
    pub fn printer(&self) {
        println!("{}", self.data);
        for i in &self.vector {
            println!("{}", i);
        }
    }
    pub fn extend_vector(&mut self, extension: Vec<u8>) {
        println!("{}", self.data);
        for i in extension {
            self.vector.push(i);
        }
    }
}


/// A Python module implemented in Rust.
#[pymodule]
fn rustbrain(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_class::<RustStruct>()?;
    m.add_class::<Datapoint>()?;
    m.add_class::<Dataset>()?;
    m.add_function(wrap_pyfunction!(sum_as_string, m)?)?;
    Ok(())
}
