/**
 * @file datetime.hpp
 * @brief Modern C++20 DateTime Library - Main Header
 * @author zuudevs (zuudevs@gmail.com)
 * @version 1.0
 * @date 2025-11-24
 * 
 * @mainpage DateTime Library Documentation
 * 
 * @section intro_sec Introduction
 * 
 * A lightweight, high-performance C++20 datetime library with:
 * - Extended year range (1-9999)
 * - Nanosecond precision
 * - Full compile-time support (constexpr)
 * - Zero-copy string formatting
 * - ISO 8601 support
 * - C++20 spaceship operators
 * - Modular design
 * 
 * @section install_sec Installation
 * 
 * This is a header-only library. Simply include datetime.hpp:
 * @code
 * #include "datetime.hpp"
 * @endcode
 * 
 * @section usage_sec Basic Usage
 * 
 * @subsection date_usage Date Operations
 * @code
 * // Create a date
 * constexpr dt::Date d(2024, 12, 25);
 * 
 * // Get components
 * int year = d.year();           // 2024
 * int month = d.month();         // 12
 * int day_of_week = d.day_of_week(); // 0-6 (0=Monday)
 * 
 * // Arithmetic
 * dt::Date tomorrow = d;
 * tomorrow.add_days(1);
 * 
 * // Formatting
 * std::cout << d.format("%Y-%m-%d") << std::endl;  // "2024-12-25"
 * std::cout << d.format("%A, %B %d, %Y") << std::endl;  // "Wednesday, December 25, 2024"
 * @endcode
 * 
 * @subsection time_usage Time Operations
 * @code
 * // Create a time
 * constexpr dt::Time t(14, 30, 45, 123456789);
 * 
 * // Get components
 * int hour = t.hour();           // 14
 * int minute = t.minute();       // 30
 * int second = t.second();       // 45
 * int nano = t.nanosecond();     // 123456789
 * 
 * // Arithmetic
 * dt::Time later = t;
 * later.add_hours(2);
 * 
 * // Formatting
 * std::cout << t.format("%H:%M:%S") << std::endl;      // "14:30:45"
 * std::cout << t.format("%H:%M:%S.%N") << std::endl;   // "14:30:45.123456789"
 * @endcode
 * 
 * @subsection datetime_usage DateTime Operations
 * @code
 * // Create a datetime
 * constexpr dt::DateTime dt(2024, 12, 25, 14, 30, 45);
 * 
 * // Get current time
 * auto now = dt::DateTime::now();
 * 
 * // Arithmetic with overflow handling
 * dt::DateTime future = dt;
 * future.add_days(7);
 * future.add_hours(25);  // Automatically handles day overflow
 * 
 * // Calculate differences
 * int64_t seconds_diff = future.seconds_between(dt);
 * 
 * // ISO 8601 formatting
 * std::cout << dt.to_iso8601() << std::endl;  // "2024-12-25T14:30:45"
 * @endcode
 * 
 * @subsection constexpr_usage Compile-Time Calculations
 * @code
 * // All operations are constexpr-compatible
 * constexpr dt::Date christmas(2024, 12, 25);
 * constexpr int day_of_week = christmas.day_of_week();
 * constexpr bool is_leap = dt::is_leap_year(2024);
 * 
 * static_assert(day_of_week == 2);  // Wednesday
 * static_assert(is_leap == true);
 * @endcode
 * 
 * @section features_sec Features
 * 
 * @subsection format_sec Formatting Options
 * 
 * Date Format Specifiers:
 * - %Y: Year (0001-9999)
 * - %m: Month (01-12)
 * - %d: Day (01-31)
 * - %w: Day of week (0-6, 0=Monday)
 * - %j: Day of year (001-366)
 * - %q: Quarter (1-4)
 * - %W: Week number (01-53)
 * - %B: Full month name ("January")
 * - %b: Short month name ("Jan")
 * - %A: Full weekday name ("Monday")
 * - %a: Short weekday name ("Mon")
 * 
 * Time Format Specifiers:
 * - %H: Hour (00-23)
 * - %M: Minute (00-59)
 * - %S: Second (00-59)
 * - %f: Millisecond (000-999)
 * - %u: Microsecond (000000-999999)
 * - %N: Nanosecond (000000000-999999999)
 * 
 * @subsection perf_sec Performance
 * 
 * - Zero-copy formatting where possible
 * - Optimized integer-to-string conversion
 * - Cache-friendly memory layout
 * - Minimal branching in hot paths
 * - Constexpr for compile-time evaluation
 * 
 * @copyright Unrestricted - Public Domain
 */

#pragma once

// Include all components
#include "date_core.hpp"
#include "time_core.hpp"
#include "datetime_core.hpp"

/**
 * @namespace zuu
 * @brief Main namespace for datetime operations
 */
namespace zuu {

// Version information
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 1;

/**
 * @brief Get library version string
 * @return Version string in format "X.Y.Z"
 */
constexpr const char* version() noexcept {
    return "1.0.1";
}

} // namespace zuu

/**
 * @example examples.cpp
 * Basic usage examples for Date, Time, and DateTime classes
 */
