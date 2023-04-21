use std::sync::mpsc;
use std::thread;
use std::time::Duration;

fn do_it() {
    let (tx1, rx) = mpsc::channel();

    thread::spawn(move || {
        let val_min = 15;
        let val_max = 90;
        for val in val_min..val_max {
            tx1.send(format!("{}", val * 100)).unwrap();
            thread::sleep(Duration::from_millis(10));
        }
        for val in val_min..val_max {
            tx1.send(format!("{}", (val_max - val) * 100)).unwrap();
            thread::sleep(Duration::from_millis(10));
        }
    });

    for received in rx {
        let handle = thread::spawn(move || {
            additional(received);
        });
        handle.join().unwrap();
    }
}

fn additional(msg: String) {
    thread::sleep(Duration::from_millis(20));
    println!("Got: {}", msg);
}

fn main() {
    do_it();
}
