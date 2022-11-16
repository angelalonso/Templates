use std::thread;
use std::time::Duration;


fn processes() {
    const NTHREADS: usize = 8;
    let mut thread_handles = vec![];
    for i in 0..NTHREADS {
        thread_handles.push(
            thread::spawn(move ||
                          for j in 0..1000 { println!("{}", j) }
                          )
                           );
    }
    for handle in thread_handles {
        handle.join();
    }
}

fn main() {
    processes();
}
