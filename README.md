# ffmt
## a single header formatting library

We all know that std::format takes ages to compile.

ffmt (f* fmt) aims to solve the issue by providing a simple to use formatter based on C's sprintf capabilities

Supported features:

- `{}` for default formatting
- `{n}` for position based formatting
- `{:fmt}` for specifying a format (same as sprint, i.e. `{:08d}`)
- `{n:fmt}` for both position and format

Usage:

```cpp
#include <ffmt.hpp>

// prints Hello, Joe
ffmt::print("Hello, {}\n", "Joe");

// prints My name is John Doe
ffmt::print("My name is {1} {0}\n", "Doe", "John");

// stores pi = 3.140000
std::string result = ffmt::print("pi = {}", 3.14);
```