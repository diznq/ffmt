#pragma once
#include <array>
#include <string>
#include <string_view>
#include <vector>

#include <cstdarg>
#include <cinttypes>
#include <cstdint>

namespace ffmt {

struct value_wrapper {
    enum class reftype {
        empty,
        vstr, str, cstr, sv,
        i1, i8, i16, i32, i64, u8, u16, u32, u64, f32, f64, f80, ts, dt,
        obj, arr
    };

    union u {
        char c;
        short h;
        int i;
        long l;
        long long ll;
        void *p;
        long double ld;
        double d;
        float f;
    };

    uintptr_t ref = 0;
    reftype type = reftype::empty;

    value_wrapper() : ref(0), type(reftype::empty) {}
    value_wrapper(const value_wrapper& a) : ref(a.ref), type(a.type) {}
    value_wrapper& operator=(const value_wrapper& a) { ref = a.ref; type = a.type; return *this; }
    value_wrapper(value_wrapper&& a) : ref(std::move(a.ref)), type(std::move(a.type)) {}

    value_wrapper(const std::string& value) : ref((uintptr_t)&value), type(reftype::str) {}
    value_wrapper(const std::string_view& value) : ref((uintptr_t)&value), type(reftype::sv) {}
    value_wrapper(const char* value) : ref((uintptr_t)value), type(reftype::cstr) {}

    value_wrapper(const int8_t& value) : ref((uintptr_t)value), type(reftype::i8) {}
    value_wrapper(const int16_t& value) : ref((uintptr_t)value), type(reftype::i16) {}
    value_wrapper(const int32_t& value) : ref((uintptr_t)value), type(reftype::i32) {}
    value_wrapper(const int64_t& value) : ref((uintptr_t)value), type(reftype::i64) {}
    value_wrapper(const uint8_t& value) : ref((uintptr_t)value), type(reftype::u8) {}
    value_wrapper(const uint16_t& value) : ref((uintptr_t)value), type(reftype::u16) {}
    value_wrapper(const uint32_t& value) : ref((uintptr_t)value), type(reftype::u32) {}
    value_wrapper(const uint64_t& value) : ref((uintptr_t)value), type(reftype::u64) {}

    value_wrapper(const float& value) : ref((uintptr_t)&value), type(reftype::f32) {}
    value_wrapper(const double& value) : ref((uintptr_t)&value), type(reftype::f64) {}
    value_wrapper(const long double& value) : ref((uintptr_t)&value), type(reftype::f80) {}
    value_wrapper(const bool& value) : ref((uintptr_t)value), type(reftype::i1) {}
};

template<size_t no>
static std::string native_format(std::string_view fmt, ...) {
    const size_t len = fmt.length();
    size_t read_args = 0;
    std::vector<char> vec;
    std::string_view initial_format{ "%" };
    std::string str;
    std::string formatting;
    std::string output;
    char buf[32];
    char c = 0;

    output.reserve(fmt.size() * 4);

    std::array<value_wrapper, no> anys;
    va_list args;
    va_start(args, fmt);
    for (size_t i = 0; i < no; i++) {
        anys[i] = va_arg(args, value_wrapper);
    }
    va_end(args);

    // go over the format string and detect all {} occurences
    for (size_t i = 0; i < len; i++) {
        c = fmt[i];
        if (c == '{' && read_args < no) {
            // if we hit {, handle it right here and extract pos:format if possible
            bool has_format = false, has_pos = false;
            size_t arg_no = read_args;
            size_t buf_size = 0;
            size_t start = i;
            std::string_view pos;
            std::string_view sv;

            // be default we assume % as format
            formatting.assign(initial_format);

            while (++i < len && fmt[i] != '}') {
                if (fmt[i] == ':' && !has_format) {
                    has_format = true;
                } else if (has_format) [[likely]] {
                    // we are reading stuff after :
                    formatting += fmt[i];
                } else [[unlikely]] {
                    // we are reading stuff before :
                    has_pos = true;
                    pos = std::string_view{ fmt }.substr(start + 1, i + 1 - start);
                }
            }

            if (fmt[i] != '}') [[unlikely]] {
                // give up if format is incorrect
                break;
            } else if (has_pos) [[unlikely]] {
                // convert position to number (yeah, not ideal, but does the job well, stopping at :)
                arg_no = atoi(pos.data());
            }

            if(arg_no >= no) [[unlikely]] {
                break;
            }

            value_wrapper& a = anys[arg_no];
            read_args++;

            // do the formatting
            switch (a.type) {
            case value_wrapper::reftype::i16: [[unlikely]]
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(int16_t*)&a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(int16_t*)&a.ref);
                }
                break;
            case value_wrapper::reftype::i32:
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(int32_t*)&a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(int32_t*)&a.ref);
                }
                break;
            case value_wrapper::reftype::i64:
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(int64_t*)&a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(int64_t*)&a.ref);
                }
                break;

            case value_wrapper::reftype::u8: [[unlikely]]
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(uint8_t*)&a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(uint8_t*)&a.ref);
                }
                break;
            case value_wrapper::reftype::u16: [[unlikely]]
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(uint16_t*)&a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(uint16_t*)&a.ref);
                }
                break;
            case value_wrapper::reftype::u32:
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(uint32_t*)&a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(uint32_t*)&a.ref);
                }
                break;
            case value_wrapper::reftype::u64:
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(uint64_t*)&a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(uint64_t*)&a.ref);
                }
                break;

            case value_wrapper::reftype::f32:
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(float*)a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(float*)a.ref);
                }
                break;
            case value_wrapper::reftype::f64:
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(double*)a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(double*)a.ref);
                }
                break;
            case value_wrapper::reftype::f80:
                if (has_format) {
                    snprintf(buf, sizeof(buf), formatting.c_str(), *(long double*)a.ref);
                    output += buf;
                } else [[likely]] {
                    output += std::to_string(*(long double*)a.ref);
                }
                break;

            case value_wrapper::reftype::cstr:
                if (has_format) {
                    buf_size = (size_t)snprintf(NULL, 0, formatting.c_str(), (const char*)a.ref);
                    vec.reserve(buf_size + 1);
                    buf_size = sprintf(vec.data(), formatting.c_str(), (const char*)a.ref);
                    output += std::string_view{ vec.data(), buf_size };
                } else [[likely]] {
                    output += (const char*)a.ref;
                }
                break;
            case value_wrapper::reftype::sv:
                if (has_format) {
                    str.assign(*(std::string_view*)a.ref);
                    buf_size = (size_t)snprintf(NULL, 0, formatting.c_str(), str.c_str());
                    vec.reserve(buf_size + 1);
                    buf_size = sprintf(vec.data(), formatting.c_str(), str.c_str());
                    output += std::string_view{ vec.data(), buf_size };
                } else [[likely]] {
                    output += *(std::string_view*)a.ref;
                }
                break;
            case value_wrapper::reftype::str:
                if (has_format) {
                    buf_size = (size_t)snprintf(NULL, 0, formatting.c_str(), ((std::string*)a.ref)->c_str());
                    vec.reserve(buf_size + 1);
                    buf_size = sprintf(vec.data(), formatting.c_str(), ((std::string*)a.ref)->c_str());
                    output += std::string_view{ vec.data(), buf_size };
                } else [[likely]] {
                    output += *(std::string*)a.ref;
                }
                break;
            }
        } else {
            output += c;
        }
    }

    return output;
}

static std::string format(std::string_view str) {
    return std::string{ str };
}

template<class ... Args>
std::string format(std::string_view fmt, const Args& ... args) {
    return native_format<sizeof...(Args)>(fmt, value_wrapper(args)...);
}

template<class ... Args>
int print(std::string_view fmt, const Args& ... args) {
    return printf("%s", native_format<sizeof...(Args)>(fmt, value_wrapper(args)...).c_str());
}
}