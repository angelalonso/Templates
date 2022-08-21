use serde::{Deserialize, Serialize};

#[derive(Debug, Deserialize, Serialize)]
pub struct Response<T> {
    pub results: Vec<T>,
}

impl<T> Response<T> {
    #[allow(dead_code)]
    pub fn new() -> Self {
        Self { results: vec![] }
    }
}
