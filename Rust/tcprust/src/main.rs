use std::{io, thread};

fn main() -> io::Result<()> {
    let mut i = tcprust::Interface::new()?;
    let mut l1 = i.bind(6000)?;
    let jh1 = thread::spawn(move || {
        while let Ok(_stream) = l1.accept() {
            eprintln!("got connection on 6000!");
        }
    });
    jh1.join().unwrap();
    Ok(())
}
