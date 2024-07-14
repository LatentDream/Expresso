#[no_mangle]
pub extern "C" fn add_from_rust(left: usize, right: usize) -> usize {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add_from_rust(2, 2);
        assert_eq!(result, 4);
    }
}
