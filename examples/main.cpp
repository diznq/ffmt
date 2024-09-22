#include <ffmt.hpp>

int main() {
    std::string name { "Joe" };
    std::string_view middle_name { "Doe" };
    ffmt::print("Hello, {} {} {}!\n", name, middle_name, "Smith");
    ffmt::print("Hello, {2} {1} {0}!\n", "Smith", middle_name, name);
    ffmt::print("{} + {} = {}\n", 1.0, 2.14, 1.0 + 2.14);
    ffmt::print("{} + {} = {}\n", 1, 2, 1 + 2);
}