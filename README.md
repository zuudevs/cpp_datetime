# DateTime Library - A modern, high-performance C++20 datetime library with nanosecond precision and full compile-time support.

[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-orange.svg)](https://github.com/zugyonozz/fstring)

## 🚀 Features

- **Extended Year Range**: 1-9999 (Gregorian calendar)
- **Nanosecond Precision**: Store time with 10⁻⁹ second accuracy
- **Compile-Time Support**: Full `constexpr` support for all operations
- **Zero-Copy Formatting**: Optimized string formatting
- **ISO 8601 Support**: Standard datetime formatting
- **C++20 Spaceship Operators**: Modern comparison operators
- **Modular Design**: Separate files for easy integration
- **Header-Only**: No compilation required
- **No Dependencies**: Only requires C++20 standard library

## 📦 Installation

This is a header-only library. Just copy the files to your project:

```bash
your_project/
├── include/
│   ├── datetime.hpp              # Main header (include this)
│   ├── datetime_config.hpp
│   ├── date_core.hpp
│   ├── time_core.hpp
│   └── datetime_core.hpp
```

Then include in your code:

```cpp
#include "datetime.hpp"
```

## 🔧 Requirements

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- Standard library with `<chrono>`, `<string>`, `<compare>`

## 📚 Quick Start

### Date Operations

```cpp
#include "datetime.hpp"
#include <iostream>

int main() {
    // Create a date
    dt::Date christmas(2024, 12, 25);
    
    // Get components
    std::cout << "Year: " << christmas.year() << std::endl;
    std::cout << "Month: " << christmas.month() << std::endl;
    std::cout << "Day: " << christmas.day() << std::endl;
    std::cout << "Day of week: " << christmas.day_of_week() << std::endl;
    
    // Arithmetic
    christmas.add_days(7);
    
    // Formatting
    std::cout << christmas.format("%A, %B %d, %Y") << std::endl;
    // Output: "Wednesday, January 01, 2025"
}
```

### Time Operations

```cpp
// Create time with nanosecond precision
dt::Time precise(14, 30, 45, 123456789);

// Get components
int hour = precise.hour();           // 14
int minute = precise.minute();       // 30
int second = precise.second();       // 45
int nano = precise.nanosecond();     // 123456789

// Arithmetic
precise.add_hours(2);
precise.add_minutes(30);

// Formatting
std::cout << precise.format("%H:%M:%S.%N") << std::endl;
// Output: "17:00:45.123456789"
```

### DateTime Operations

```cpp
// Create datetime
dt::DateTime dt(2024, 12, 25, 14, 30, 45);

// Get current datetime
auto now = dt::DateTime::now();

// Arithmetic with automatic overflow
dt.add_hours(25);  // Automatically handles day overflow

// Calculate differences
int64_t seconds_diff = now.seconds_between(dt);

// ISO 8601 formatting
std::cout << dt.to_iso8601() << std::endl;
// Output: "2024-12-26T15:30:45"
```

## 📖 API Reference

### Date Class

#### Construction
```cpp
dt::Date()                        // Default: 0001-01-01
dt::Date(int year, int month, int day)
dt::Date::today()                 // Current date
dt::Date::from_day_of_year(int year, int doy)
```

#### Accessors
```cpp
int year() const                  // 1-9999
int month() const                 // 1-12
int day() const                   // 1-31
int day_of_week() const          // 0-6 (0=Monday)
int day_of_year() const          // 1-366
int week_number() const          // 1-53 (ISO 8601)
int quarter() const              // 1-4
bool is_leap_year() const
bool is_weekend() const
bool is_weekday() const
```

#### Arithmetic
```cpp
Date& add_days(int32_t days)
Date& add_months(int32_t months)
Date& add_years(int32_t years)
int32_t days_between(const Date& other) const
```

#### Formatting
```cpp
std::string format(std::string_view fmt = "%Y-%m-%d") const
```

**Format Specifiers:**
- `%Y` - Year (0001-9999)
- `%m` - Month (01-12)
- `%d` - Day (01-31)
- `%w` - Day of week (0-6)
- `%j` - Day of year (001-366)
- `%q` - Quarter (1-4)
- `%W` - Week number (01-53)
- `%B` - Full month name ("January")
- `%b` - Short month name ("Jan")
- `%A` - Full weekday name ("Monday")
- `%a` - Short weekday name ("Mon")

### Time Class

#### Construction
```cpp
dt::Time()                        // Default: 00:00:00.000000000
dt::Time(int h, int min, int s = 0, int ns = 0)
dt::Time::now()                   // Current time
dt::Time::from_seconds(int64_t seconds)
dt::Time::from_milliseconds(int64_t ms)
```

#### Accessors
```cpp
int hour() const                  // 0-23
int minute() const                // 0-59
int second() const                // 0-59
int nanosecond() const            // 0-999999999
int millisecond() const           // 0-999
int microsecond() const           // 0-999999
int hour12() const                // 1-12
bool is_am() const
bool is_pm() const
bool is_midnight() const
bool is_noon() const
int64_t total_seconds() const
uint64_t total_nanoseconds() const
```

#### Arithmetic
```cpp
Time& add_hours(int64_t hours)
Time& add_minutes(int64_t minutes)
Time& add_seconds(int64_t seconds)
Time& add_milliseconds(int64_t ms)
Time& add_nanoseconds(int64_t ns)
```

#### Formatting
```cpp
std::string format(std::string_view fmt = "%H:%M:%S") const
```

**Format Specifiers:**
- `%H` - Hour (00-23)
- `%M` - Minute (00-59)
- `%S` - Second (00-59)
- `%f` - Millisecond (000-999)
- `%u` - Microsecond (000000-999999)
- `%N` - Nanosecond (000000000-999999999)

### DateTime Class

#### Construction
```cpp
dt::DateTime()                    // Default: 0001-01-01 00:00:00.000000000
dt::DateTime(const Date& d, const Time& t = Time())
dt::DateTime(int year, int month, int day, int h = 0, int min = 0, int s = 0, int ns = 0)
dt::DateTime::now()               // Current datetime
dt::DateTime::from_unix_timestamp(int64_t seconds)
```

#### Accessors
All Date and Time accessors, plus:
```cpp
const Date& get_date() const
const Time& get_time() const
int64_t to_unix_timestamp() const
int64_t to_unix_timestamp_ms() const
```

#### Arithmetic
```cpp
DateTime& add_days(int32_t days)
DateTime& add_months(int32_t months)
DateTime& add_years(int32_t years)
DateTime& add_hours(int64_t hours)
DateTime& add_minutes(int64_t minutes)
DateTime& add_seconds(int64_t seconds)
DateTime& add_milliseconds(int64_t ms)
DateTime& add_nanoseconds(int64_t ns)
int64_t seconds_between(const DateTime& other) const
```

#### Formatting
```cpp
std::string format(std::string_view fmt = "%Y-%m-%d %H:%M:%S") const
std::string to_iso8601() const
std::string to_iso8601_ms() const
std::string to_iso8601_us() const
std::string to_iso8601_ns() const
```

### Utility Functions

```cpp
constexpr bool is_leap_year(int year)
constexpr int days_in_month(int month, int year)
constexpr int days_in_year(int year)
constexpr int32_t days_since_epoch(int year)
constexpr bool is_valid_date(int year, int month, int day)
constexpr bool is_valid_time(int hour, int minute, int second, int nanosecond)
```

## 🎯 Advanced Examples

### Compile-Time Calculations

```cpp
// All evaluated at compile-time!
constexpr dt::Date christmas(2024, 12, 25);
constexpr int day_of_week = christmas.day_of_week();
constexpr bool is_leap = dt::is_leap_year(2024);

static_assert(day_of_week == 2);  // Wednesday
static_assert(is_leap == true);
```

### Date Arithmetic with Overflow

```cpp
dt::DateTime dt(2024, 12, 31, 23, 30, 0);
dt.add_hours(2);  // Becomes 2025-01-01 01:30:00

dt::Date end_of_month(2024, 1, 31);
end_of_month.add_months(1);  // Becomes 2024-02-29 (leap year)
```

### Time Duration Calculation

```cpp
dt::DateTime start(2024, 12, 25, 9, 0, 0);
dt::DateTime end(2024, 12, 25, 17, 30, 0);
int64_t work_seconds = end.seconds_between(start);
int hours = work_seconds / 3600;
int minutes = (work_seconds % 3600) / 60;
std::cout << hours << "h " << minutes << "m" << std::endl;
// Output: "8h 30m"
```

### Custom Formatting

```cpp
dt::DateTime dt(2024, 7, 4, 14, 30, 45);

// Various formats
dt.format("%Y-%m-%d %H:%M:%S")           // "2024-07-04 14:30:45"
dt.format("%A, %B %d, %Y at %H:%M")      // "Thursday, July 04, 2024 at 14:30"
dt.format("%m/%d/%Y %H:%M:%S")           // "07/04/2024 14:30:45"
dt.format("%d/%m/%Y %H:%M:%S")           // "04/07/2024 14:30:45"
dt.format("Day %j of %Y, Quarter %q")    // "Day 186 of 2024, Quarter 3"
```

## ⚡ Performance

- **Memory Efficient**: Date (4 bytes), Time (8 bytes), DateTime (12 bytes)
- **Cache-Friendly**: Optimized memory layout
- **Zero-Copy**: Minimal allocations in formatting
- **Constexpr**: Compile-time evaluation where possible
- **O(1) Operations**: Most operations are constant time

## 🔍 Comparison with Other Libraries

| Feature | This Library | std::chrono | date.h (Howard Hinnant) |
|---------|-------------|-------------|------------------------|
| Header-Only | ✅ | ✅ | ✅ |
| C++20 | ✅ | ✅ | ❌ (C++11) |
| Nanosecond Precision | ✅ | ✅ | ❌ |
| Extended Year Range | ✅ (1-9999) | ❌ | ✅ |
| Constexpr Support | ✅ Full | ⚠️ Partial | ⚠️ Partial |
| Simple API | ✅ | ❌ | ⚠️ |
| Custom Formatting | ✅ | ❌ | ✅ |
| Modular Design | ✅ | N/A | N/A |

## 📝 License

Public Domain / Unlicense - Free for any use, commercial or non-commercial.

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## 📧 Contact

- Author: zuudevs
- Email: zuudevs@gmail.com

## 🔖 Version History

### v1.1.1 (2025-11-24)
- ✅ Fixed `total_nanos` type (uint32_t → uint64_t)
- ✅ Fixed `add_seconds()` overflow handling
- ✅ Fixed `DateTime::add_seconds()` logic
- ✅ Optimized formatting functions
- ✅ Added modular file structure
- ✅ Added millisecond/microsecond accessors
- ✅ Added week number calculation
- ✅ Added quarter calculation
- ✅ Added weekend/weekday checks
- ✅ Added Unix timestamp conversion
- ✅ Improved documentation
- ✅ Added comprehensive examples

### v1.0.0 (Initial Release)
- Basic Date, Time, DateTime classes
- ISO 8601 support
- C++20 spaceship operators

## 🎓 Learn More

See `examples.cpp` for comprehensive usage examples covering:
- Basic operations
- Date/time arithmetic
- Formatting options
- Compile-time calculations
- Real-world use cases
