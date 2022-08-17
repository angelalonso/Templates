/// https://blog.x5ff.xyz/blog/async-tests-tokio-rust/

#[cfg(test)]
#[allow(non_snake_case)]
mod tests {
    use super::*;
    use futures::{Future};
    use std::fmt::Debug;
  
// TODO: make this work
//    fn get_printable_future(r: dyn Future<Output = usize>) -> impl Future + Debug {
//        futures::future::ok::<_, ()>(r)
//    }
    
    #[test]
    fn test_str_len() {
        assert_eq!(str_len("x5ff"), 4);
    }
  
    // ... the other async test
  
    macro_rules! aw {
        ($e:expr) => {
            tokio_test::block_on($e)
        };
    }
  
    #[test]
    fn test_str_len_async_2() {
        assert_eq!(aw!(str_len_async("x5ff")), 4);
    }
  
//    #[async_std::test]
//    async fn test_str_len_async_3() {
//        assert_eq!(get_printable_future(str_len_async("x5ff")), 4);
//    }
}

// -------------------------

fn str_len(s: &str) -> usize {
    s.len()
}

async fn str_len_async(s: &str) -> usize {
    // do something awaitable ideally... 
    s.len()
}

// -------------------------

fn main() {
    println!("Hello, world!");
}
