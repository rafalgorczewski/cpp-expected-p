# cpp-expected-p
Optional-like utility class for convenient way of error handling. P version.

The main idea was to provide a utility class which resembles the Rust way of error handling. You can use any type for unexpected (error) value, therefore you can provide your own way of handling errors. You can use it similary to `std::optional`.

Has to be compiled with concepts supporting compiler.

## Example usage
```cpp
hqrp::expected<int, std::string> foo(bool b) {
    if (b) {
        return { 42 };
    } else {
        return { "Tut mir leid, keine zeit." };
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char ** argv) {
    std::cout << foo(true).value_or_terminate() << std::endl;
}
```
Yields `10`.

Passing false to `foo(bool)` yields:
```
./a.out
Process terminated. Error message:
"Tut mir leid, keine zeit."
terminate called without an active exception
[1]    11449 abort (core dumped)  ./a.out
```
