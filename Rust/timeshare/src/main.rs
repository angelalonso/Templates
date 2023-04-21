use std::{thread, time::Duration};

const USER1_ON_MILLIS: u64 = 10;
const USER1_OFF_MILLIS: u64 = 20;
const USER2_ON_MILLIS: u64 = 40;
const USER2_OFF_MILLIS: u64 = 30;

fn print_users(u1: bool, u2: bool) {
    let pt1: String = if u1 {
        "-|".to_string()
    } else {
        "| ".to_string()
    };
    let pt2: String = if u2 {
        "-|".to_string()
    } else {
        "| ".to_string()
    };
    println!("{}    {}", pt1, pt2);
}

fn timeshare() {
    let mut timesofar: u64 = 0;
    let share: u64 = 10;
    let mut user1: bool = false;
    let mut user2: bool = false;

    loop {
        if (timesofar == 0) || (timesofar % (USER1_OFF_MILLIS + USER1_ON_MILLIS) == 0) {
            user1 = true;
        } else if (timesofar == USER1_ON_MILLIS)
            || (timesofar % (USER1_OFF_MILLIS + USER1_ON_MILLIS) == USER1_ON_MILLIS)
        {
            user1 = false;
        };
        if (timesofar == 0) || (timesofar % (USER2_OFF_MILLIS + USER2_ON_MILLIS) == 0) {
            user2 = true;
        } else if (timesofar == USER2_ON_MILLIS)
            || (timesofar % (USER2_OFF_MILLIS + USER2_ON_MILLIS) == USER2_ON_MILLIS)
        {
            user2 = false;
        };
        print!("{timesofar} - ");
        print_users(user1, user2);
        // advance
        timesofar += share;
        thread::sleep(Duration::from_millis(share));
    }
}

fn main() {
    timeshare();
}
