fn last_or_push<'a>(vec: &'a mut Vec<String>) -> &'a String {
    //if !vec.is_empty() {
    //    let s = vec.last().unwrap();
    //    return s;
    //}
    if let Some(s) = vec.last() {
        return s;
    }
    vec.push("".to_string());
    vec.last().unwrap()
}
fn main() {
    last_or_push(&mut Vec::new());
    println!("Hello, world!");
}
