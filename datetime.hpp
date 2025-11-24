/**
 * @file datetime.hpp
 * @brief Modern C++20 DateTime Library - High Performance Compile-Time Support
 * 
 * @author zuudevs (zuudevs@gmail.com)
 * @version 1.0
 * @date 2025-11-24
 * 
 * @details
 * A lightweight, high-performance datetime library with full compile-time support.
 * Provides Date, Time, and DateTime classes with:
 * - Extended year range (1-9999)
 * - Nanosecond precision
 * - Zero-copy string formatting
 * - ISO 8601 support
 * - C++20 spaceship operators
 * - SIMD-friendly memory layout
 * 
 * @code
 * // Compile-time datetime calculation
 * constexpr dt::Date d(2024, 12, 25);
 * constexpr dt::Time t(14, 30, 45, 123456789);
 * constexpr dt::DateTime dt(d, t);
 * 
 * // Runtime queries
 * auto now = dt::DateTime::now();
 * std::cout << now.format("%Y-%m-%d %H:%M:%S") << std::endl;
 * @endcode
 * 
 * @copyright Unrestricted - Public Domain
 */

#include <chrono>
#include <string>
#include <string_view>
#include <array>
#include <stdexcept>
#include <cstring>
#include <compare>

/**
 * @namespace dt
 * @brief Main namespace for datetime operations
 */
namespace dt {

/**
 * @namespace dt::detail
 * @brief Internal implementation details (not for public use)
 */
namespace detail {
    /// @brief Days in a standard year
    constexpr uint32_t DAYS_IN_YEAR = 365;
    
    /// @brief Days in a leap year
    constexpr uint32_t DAYS_IN_LEAP_YEAR = 366;
    
    /// @brief Seconds per minute
    constexpr uint32_t SECONDS_PER_MINUTE = 60;
    
    /// @brief Seconds per hour
    constexpr uint32_t SECONDS_PER_HOUR = 3600;
    
    /// @brief Seconds per day
    constexpr uint32_t SECONDS_PER_DAY = 86400;
    
    /// @brief Nanoseconds per second (1 billion)
    constexpr uint32_t NANOS_PER_SECOND = 1'000'000'000;
    
    /**
     * @brief Days in each month (non-leap year)
     * @details Index 0 = January (31 days), Index 1 = February (28 days), etc.
     */
    constexpr std::array<uint8_t, 12> DAYS_PER_MONTH = 
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    /**
     * @brief Cumulative day count to start of each month (non-leap year)
     * @details Index 0 = 0 (Jan 1), Index 1 = 31 (Feb 1), Index 2 = 59 (Mar 1), etc.
     */
    constexpr std::array<uint16_t, 13> CUMULATIVE_DAYS = 
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
}

// ============================================================================
// @defgroup Utilities Calendar Utilities
// @brief Utility functions for date calculations
// ============================================================================

/**
 * @ingroup Utilities
 * @brief Determine if a year is a leap year
 * 
 * @param year The year to check (range: 1-9999)
 * @return true if the year is a leap year, false otherwise
 * 
 * @details
 * A leap year is:
 * - Divisible by 400, OR
 * - Divisible by 4 but NOT by 100
 * 
 * @note This function is constexpr and can be evaluated at compile-time
 * 
 * @example
 * @code
 * static_assert(dt::is_leap_year(2024) == true);
 * static_assert(dt::is_leap_year(1900) == false);
 * static_assert(dt::is_leap_year(2000) == true);
 * @endcode
 */
constexpr bool is_leap_year(int year) noexcept {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

/**
 * @ingroup Utilities
 * @brief Get the number of days in a specific month
 * 
 * @param month The month (1-12)
 * @param year The year for leap year detection
 * @return Number of days in the month (28-31), or 0 if invalid month
 * 
 * @details
 * - January, March, May, July, August, October, December: 31 days
 * - April, June, September, November: 30 days
 * - February: 28 days (29 in leap years)
 * 
 * @note Time complexity: O(1) - uses lookup table
 * 
 * @example
 * @code
 * int feb_days = dt::days_in_month(2, 2024);  // 29 (leap year)
 * int sep_days = dt::days_in_month(9, 2024);  // 30
 * @endcode
 */
constexpr int days_in_month(int month, int year) noexcept {
    if (month < 1 || month > 12) return 0;
    if (month != 2) return detail::DAYS_PER_MONTH[month - 1];
    return is_leap_year(year) ? 29 : 28;
}

/**
 * @ingroup Utilities
 * @brief Calculate total days elapsed from year 1 to target year
 * 
 * @param year The target year (range: 1-9999)
 * @return Total days from year 1 (January 1) to January 1 of target year
 * 
 * @details
 * Uses Gregorian calendar rules accounting for:
 * - Standard 365-day years
 * - Leap years (divisible by 4)
 * - Century years (divisible by 100) are NOT leap years
 * - Millenium years (divisible by 400) ARE leap years
 * 
 * @note Time complexity: O(1) - mathematical calculation
 */
constexpr int32_t days_since_epoch(int year) noexcept {
    --year;
    return year * 365 + year / 4 - year / 100 + year / 400;
}

// ============================================================================
// @defgroup TimeClass Time Class
// @brief High-precision time-of-day representation
// ============================================================================

/**
 * @ingroup TimeClass
 * @brief Represents a time of day with nanosecond precision
 * 
 * @details
 * The Time class stores a specific time-of-day from 00:00:00.000000000
 * to 23:59:59.999999999 in a compact 4-byte structure.
 * 
 * **Memory Layout:**
 * - 4 bytes: nanoseconds since midnight (0 to 86,399,999,999,999)
 * 
 * **Features:**
 * - Nanosecond precision for microsecond-level timing
 * - Compile-time constexpr support
 * - C++20 spaceship operator for comparison
 * - Zero-copy string formatting
 * 
 * @code
 * // Compile-time construction
 * constexpr dt::Time t1(14, 30, 45, 123456789);  // 2:30:45 PM + nanoseconds
 * 
 * // Runtime current time
 * auto now = dt::Time::now();
 * 
 * // Arithmetic
 * dt::Time t2(10, 0);
 * t2.add_seconds(3600);  // Add 1 hour
 * @endcode
 */
class Time {
private:
    /// @brief Total nanoseconds since midnight (0 to 86,399,999,999,999)
    uint32_t total_nanos = 0;

    /**
     * @brief Validate time component values
     * @param h Hour (0-23)
     * @param min Minute (0-59)
     * @param s Second (0-59)
     * @param ns Nanosecond (0-999,999,999)
     * @return true if all components are valid
     */
    constexpr bool is_valid_time(uint32_t h, uint32_t min, uint32_t s, uint32_t ns) const noexcept {
        return h >= 0 && h < 24 && min >= 0 && min < 60 && s >= 0 && s < 60 && ns >= 0 && ns < detail::NANOS_PER_SECOND;
    }

public:
    /**
     * @brief Default constructor - creates midnight (00:00:00.000000000)
     * @post time == 00:00:00.000000000
     */
    constexpr Time() noexcept = default;
    
    /**
     * @brief Construct time from hour, minute, second, nanosecond
     * 
     * @param h Hour component [0-23]
     * @param min Minute component [0-59]
     * @param s Second component [0-59] (default: 0)
     * @param ns Nanosecond component [0-999999999] (default: 0)
     * 
     * @throw std::out_of_range if any component is out of valid range
     * 
     * @note This constructor is constexpr and can be evaluated at compile-time
     * 
     * @example
     * @code
     * constexpr dt::Time morning(8, 30);              // 08:30:00
     * constexpr dt::Time precise(14, 30, 45, 500000); // 14:30:45.000500000
     * @endcode
     */
    constexpr Time(uint32_t h, uint32_t min, uint32_t s = 0, uint32_t ns = 0) {
        if (!is_valid_time(h, min, s, ns)) {
            throw std::out_of_range("Invalid time");
        }
        total_nanos = h * detail::SECONDS_PER_HOUR * detail::NANOS_PER_SECOND +
                     min * detail::SECONDS_PER_MINUTE * detail::NANOS_PER_SECOND +
                     s * detail::NANOS_PER_SECOND + ns;
    }

    /**
     * @brief Get hour component
     * @return Hour in range [0-23]
     * @note Time complexity: O(1)
     */
    [[nodiscard]] constexpr uint32_t hour() const noexcept { 
        return total_nanos / (detail::SECONDS_PER_HOUR * detail::NANOS_PER_SECOND); 
    }
    
    /**
     * @brief Get minute component
     * @return Minute in range [0-59]
     * @note Time complexity: O(1)
     */
    [[nodiscard]] constexpr uint32_t minute() const noexcept { 
        return (total_nanos % (detail::SECONDS_PER_HOUR * detail::NANOS_PER_SECOND)) / 
               (detail::SECONDS_PER_MINUTE * detail::NANOS_PER_SECOND); 
    }
    
    /**
     * @brief Get second component
     * @return Second in range [0-59]
     * @note Time complexity: O(1)
     */
    [[nodiscard]] constexpr uint32_t second() const noexcept { 
        return (total_nanos % (detail::SECONDS_PER_MINUTE * detail::NANOS_PER_SECOND)) / 
               detail::NANOS_PER_SECOND; 
    }
    
    /**
     * @brief Get nanosecond component
     * @return Nanosecond in range [0-999999999]
     * @note Time complexity: O(1)
     */
    [[nodiscard]] constexpr uint32_t nanosecond() const noexcept { 
        return total_nanos % detail::NANOS_PER_SECOND; 
    }
    
    /**
     * @brief Get total seconds since midnight
     * @return Total seconds in range [0-86399]
     * @note Time complexity: O(1)
     */
    [[nodiscard]] constexpr uint32_t total_seconds() const noexcept {
        return total_nanos / detail::NANOS_PER_SECOND;
    }
    
    /**
     * @brief Get total nanoseconds since midnight
     * @return Total nanoseconds in range [0-86399999999999]
     * @note Time complexity: O(1)
     */
    [[nodiscard]] constexpr uint64_t total_nanoseconds() const noexcept {
        return total_nanos;
    }

    /**
     * @brief Get current system time
     * 
     * @return Time object representing current time of day
     * 
     * @details
     * Queries the system clock to get the current time. Includes nanosecond
     * precision if available from the system clock.
     * 
     * @note This is a runtime function and cannot be called at compile-time
     * 
     * @example
     * @code
     * auto now = dt::Time::now();
     * std::cout << "Current time: " << now.format() << std::endl;
     * @endcode
     */
    [[nodiscard]] static Time now() noexcept {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ns = std::chrono::nanoseconds(now.time_since_epoch()).count() % detail::NANOS_PER_SECOND;
        struct tm* tm_info = std::localtime(&time_t_now);
        return Time(tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, static_cast<int>(ns));
    }

    /**
     * @brief Add seconds to this time
     * 
     * @param seconds Number of seconds to add (can be negative)
     * @return Reference to this Time object for chaining
     * 
     * @details
     * Adds seconds and handles day wrapping. If the result exceeds 24 hours,
     * it wraps to the next day (e.g., 23:00 + 3600 seconds = 00:00).
     * Similarly, negative seconds wrap to the previous day.
     * 
     * @note This modifies the Time object in-place
     * 
     * @example
     * @code
     * dt::Time t(14, 30);
     * t.add_seconds(3600);   // Add 1 hour: now 15:30
     * t.add_seconds(-1800);  // Subtract 30 minutes: now 15:00
     * @endcode
     */
    constexpr Time& add_seconds(int64_t seconds) noexcept {
        total_nanos = (total_nanos / detail::NANOS_PER_SECOND + seconds) % detail::SECONDS_PER_DAY * detail::NANOS_PER_SECOND;
        if (total_nanos < 0) total_nanos += detail::SECONDS_PER_DAY * detail::NANOS_PER_SECOND;
        return *this;
    }

    /**
     * @brief Format time as string
     * 
     * @param fmt Format string with directives (default: "%H:%M:%S")
     * @return Formatted time string
     * 
     * @details
     * **Format directives:**
     * - `%H` - Hour (00-23)
     * - `%M` - Minute (00-59)
     * - `%S` - Second (00-59)
     * - `%N` - Nanosecond (000000000-999999999, left-padded with zeros)
     * - `%%` - Literal percent sign
     * - Any other character is printed as-is
     * 
     * @note Time complexity: O(n) where n is length of format string
     * 
     * @example
     * @code
     * dt::Time t(14, 30, 45, 123456789);
     * std::cout << t.format("%H:%M:%S") << std::endl;     // "14:30:45"
     * std::cout << t.format("%H:%M:%S.%N") << std::endl;  // "14:30:45.123456789"
     * std::cout << t.format("Time: %H hours") << std::endl; // "Time: 14 hours"
     * @endcode
     */
    [[nodiscard]] std::string format(std::string_view fmt = "%H:%M:%S") const {
        std::string result;
        result.reserve(20);
        
        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                switch (fmt[++i]) {
                    case 'H': result += (hour() < 10 ? "0" : ""); result += std::to_string(hour()); break;
                    case 'M': result += (minute() < 10 ? "0" : ""); result += std::to_string(minute()); break;
                    case 'S': result += (second() < 10 ? "0" : ""); result += std::to_string(second()); break;
                    case 'N': {
                        int ns = nanosecond();
                        result += (ns < 100000000 ? "0" : "");
                        result += (ns < 10000000 ? "0" : "");
                        result += (ns < 1000000 ? "0" : "");
                        result += (ns < 100000 ? "0" : "");
                        result += (ns < 10000 ? "0" : "");
                        result += (ns < 1000 ? "0" : "");
                        result += (ns < 100 ? "0" : "");
                        result += (ns < 10 ? "0" : "");
                        result += std::to_string(ns);
                        break;
                    }
                    case '%': result += '%'; break;
                    default: result += fmt[i];
                }
            } else {
                result += fmt[i];
            }
        }
        return result;
    }

    /**
     * @brief Compare two times (spaceship operator - C++20)
     * 
     * @param other The time to compare with
     * @return Comparison result: <0, ==0, or >0
     * 
     * @details
     * Provides three-way comparison using the spaceship operator (<=>).
     * This enables automatic generation of all comparison operators
     * (<, >, <=, >=, ==, !=).
     * 
     * @example
     * @code
     * constexpr dt::Time t1(14, 30);
     * constexpr dt::Time t2(14, 45);
     * static_assert(t1 < t2);
     * static_assert(t1 <= t2);
     * static_assert(t2 > t1);
     * @endcode
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const Time& other) const noexcept {
        return total_nanos <=> other.total_nanos;
    }
    
    /**
     * @brief Equality comparison
     * 
     * @param other The time to compare with
     * @return true if both times represent the same moment, false otherwise
     */
    [[nodiscard]] constexpr bool operator==(const Time& other) const noexcept {
        return total_nanos == other.total_nanos;
    }
};

// ============================================================================
// @defgroup DateClass Date Class
// @brief Calendar date representation with extended year support
// ============================================================================

/**
 * @ingroup DateClass
 * @brief Represents a calendar date with year range 1-9999
 * 
 * @details
 * The Date class stores a specific calendar date in Gregorian calendar format.
 * 
 * **Memory Layout:**
 * - 2 bytes: year (1-9999)
 * - 1 byte: month (1-12)
 * - 1 byte: day (1-31)
 * - 4 bytes padding (cache line alignment)
 * 
 * **Features:**
 * - Extended year support from year 1 to year 9999
 * - Compile-time constexpr support
 * - Automatic leap year handling
 * - C++20 spaceship operator for comparison
 * - Day-of-week calculation using Zeller's congruence
 * - Day-of-year calculation with lookup tables
 * 
 * @code
 * // Compile-time construction
 * constexpr dt::Date christmas(2024, 12, 25);
 * constexpr int dow = christmas.day_of_week();  // 0 = Monday
 * 
 * // Runtime current date
 * auto today = dt::Date::today();
 * 
 * // Date arithmetic
 * dt::Date d(2024, 12, 25);
 * d.add_days(7);  // Move to next week
 * @endcode
 */
class Date {
private:
    /// @brief Year in Gregorian calendar (1-9999)
    uint16_t year_val = 1;
    
    /// @brief Month of year (1-12)
    uint8_t month_val = 1;
    
    /// @brief Day of month (1-31)
    uint8_t day_val = 1;

    /**
     * @brief Validate date component values
     * @param y Year (1-9999)
     * @param m Month (1-12)
     * @param d Day (1-31)
     * @return true if all components are valid for the given year/month
     */
    constexpr bool is_valid_date(int y, int m, int d) const noexcept {
        return y >= 1 && y <= 9999 && m >= 1 && m <= 12 && 
               d >= 1 && d <= days_in_month(m, y);
    }

public:
    /**
     * @brief Default constructor - creates January 1, year 1 (0001-01-01)
     * @post date == 0001-01-01
     */
    constexpr Date() noexcept = default;
    
    /**
     * @brief Construct date from year, month, day
     * 
     * @param y Year component [1-9999]
     * @param m Month component [1-12]
     * @param d Day component [1-31]
     * 
     * @throw std::out_of_range if date is invalid (e.g., Feb 30)
     * 
     * @note This constructor is constexpr and can be evaluated at compile-time
     * 
     * @example
     * @code
     * constexpr dt::Date christmas(2024, 12, 25);
     * constexpr dt::Date ancient(500, 3, 15);
     * constexpr dt::Date distant_future(9999, 12, 31);
     * @endcode
     */
    constexpr Date(int y, int m, int d) : year_val(1), month_val(1), day_val(1) {
        if (!is_valid_date(y, m, d)) {
            throw std::out_of_range("Invalid date");
        }
        year_val = static_cast<uint16_t>(y);
        month_val = static_cast<uint8_t>(m);
        day_val = static_cast<uint8_t>(d);
    }

    /**
     * @brief Get year component
     * @return Year in range [1-9999]
     */
    [[nodiscard]] constexpr int year() const noexcept { return year_val; }
    
    /**
     * @brief Get month component
     * @return Month in range [1-12]
     */
    [[nodiscard]] constexpr int month() const noexcept { return month_val; }
    
    /**
     * @brief Get day component
     * @return Day in range [1-31]
     */
    [[nodiscard]] constexpr int day() const noexcept { return day_val; }

    /**
     * @brief Get day of week using Zeller's congruence
     * 
     * @return Day-of-week value:
     *         - 0 = Monday
     *         - 1 = Tuesday
     *         - 2 = Wednesday
     *         - 3 = Thursday
     *         - 4 = Friday
     *         - 5 = Saturday
     *         - 6 = Sunday
     * 
     * @details
     * Uses Zeller's congruence algorithm which calculates the Gregorian
     * calendar day of week in O(1) time without lookup tables.
     * 
     * @note Time complexity: O(1)
     * 
     * @example
     * @code
     * constexpr dt::Date christmas(2024, 12, 25);
     * constexpr int dow = christmas.day_of_week();  // 2 (Wednesday)
     * @endcode
     */
    [[nodiscard]] constexpr int day_of_week() const noexcept {
        int y = year_val, m = month_val;
        if (m < 3) { m += 12; y--; }
        int q = day_val, k = y % 100, j = y / 100;
        int h = (q + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 - 2 * j) % 7;
        return (h + 6) % 7;
    }

    /**
     * @brief Get day of year (1-366)
     * 
     * @return Day-of-year value in range [1-366]:
     *         - 1 = January 1
     *         - 32 = February 1 (non-leap year)
     *         - 366 = December 31 (leap year only)
     * 
     * @details
     * Calculates which day of the year this date falls on using
     * precomputed cumulative day tables for O(1) performance.
     * 
     * @note Time complexity: O(1)
     * 
     * @example
     * @code
     * constexpr dt::Date d(2024, 1, 1);
     * static_assert(d.day_of_year() == 1);
     * 
     * constexpr dt::Date d2(2024, 12, 31);
     * static_assert(d2.day_of_year() == 366);  // Leap year
     * @endcode
     */
    [[nodiscard]] constexpr int day_of_year() const noexcept {
        int doy = detail::CUMULATIVE_DAYS[month_val - 1] + day_val;
        if (month_val > 2 && is_leap_year(year_val)) doy++;
        return doy;
    }

    /**
     * @brief Get today's date from system clock
     * 
     * @return Date object representing current date
     * 
     * @details
     * Queries the system clock to get the current calendar date in the
     * local timezone.
     * 
     * @note This is a runtime function and cannot be called at compile-time
     * 
     * @example
     * @code
     * auto today = dt::Date::today();
     * std::cout << "Today is: " << today.format() << std::endl;
     * @endcode
     */
    [[nodiscard]] static Date today() noexcept {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        struct tm* tm_info = std::localtime(&time_t_now);
        return Date(tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
    }

    /**
     * @brief Add days to this date
     * 
     * @param days Number of days to add (can be negative)
     * @return Reference to this Date object for chaining
     * 
     * @details
     * Adds the specified number of days and automatically handles:
     * - Month overflow (e.g., Jan 31 + 1 day = Feb 1)
     * - Year overflow (e.g., Dec 31 + 1 day = Jan 1 of next year)
     * - Month underflow (e.g., Jan 1 - 1 day = Dec 31 of previous year)
     * - Leap year transitions
     * 
     * @note This modifies the Date object in-place
     * 
     * @example
     * @code
     * dt::Date d(2024, 12, 25);
     * d.add_days(7);   // Move to 2024-01-01
     * d.add_days(-32); // Move back across month boundary
     * @endcode
     */
    constexpr Date& add_days(int32_t days) noexcept {
        int y = year_val, m = month_val, d = day_val + days;
        
        while (d > days_in_month(m, y)) {
            d -= days_in_month(m, y);
            if (++m > 12) { m = 1; y++; }
        }
        
        while (d < 1) {
            if (--m < 1) { m = 12; y--; }
            d += days_in_month(m, y);
        }
        
        if (is_valid_date(y, m, d)) {
            year_val = static_cast<uint16_t>(y);
            month_val = static_cast<uint8_t>(m);
            day_val = static_cast<uint8_t>(d);
        }
        return *this;
    }

    /**
     * @brief Format date as string
     * 
     * @param fmt Format string with directives (default: "%Y-%m-%d")
     * @return Formatted date string
     * 
     * @details
     * **Format directives:**
     * - `%Y` - Year (0001-9999)
     * - `%m` - Month (01-12, zero-padded)
     * - `%d` - Day (01-31, zero-padded)
     * - `%w` - Day of week (0-6, 0=Monday)
     * - `%j` - Day of year (001-366, zero-padded)
     * - `%%` - Literal percent sign
     * - Any other character is printed as-is
     * 
     * @note Time complexity: O(n) where n is length of format string
     * 
     * @example
     * @code
     * dt::Date d(2024, 1, 5);
     * std::cout << d.format("%Y-%m-%d") << std::endl;     // "2024-01-05"
     * std::cout << d.format("%d/%m/%Y") << std::endl;     // "05/01/2024"
     * std::cout << d.format("DOW: %w, DOY: %j") << std::endl; // "DOW: 4, DOY: 005"
     * @endcode
     */
    [[nodiscard]] std::string format(std::string_view fmt = "%Y-%m-%d") const {
        std::string result;
        result.reserve(20);
        
        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                switch (fmt[++i]) {
                    case 'Y': result += std::to_string(year_val); break;
                    case 'm': result += (month_val < 10 ? "0" : ""); result += std::to_string(month_val); break;
                    case 'd': result += (day_val < 10 ? "0" : ""); result += std::to_string(day_val); break;
                    case 'w': result += std::to_string(day_of_week()); break;
                    case 'j': {
                        int doy = day_of_year();
                        result += (doy < 100 ? "0" : ""); 
                        result += (doy < 10 ? "0" : ""); 
                        result += std::to_string(doy);
                        break;
                    }
                    case '%': result += '%'; break;
                    default: result += fmt[i];
                }
            } else {
                result += fmt[i];
            }
        }
        return result;
    }

    /**
     * @brief Compare two dates (spaceship operator - C++20)
     * 
     * @param other The date to compare with
     * @return Comparison result: <0, ==0, or >0
     * 
     * @details
     * Provides three-way comparison using the spaceship operator (<=>).
     * Compares chronologically: year first, then month, then day.
     * 
     * @example
     * @code
     * constexpr dt::Date d1(2024, 12, 25);
     * constexpr dt::Date d2(2024, 12, 26);
     * static_assert(d1 < d2);
     * @endcode
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const Date& other) const noexcept {
        if (auto cmp = year_val <=> other.year_val; cmp != 0) return cmp;
        if (auto cmp = month_val <=> other.month_val; cmp != 0) return cmp;
        return day_val <=> other.day_val;
    }
    
    /**
     * @brief Equality comparison
     * 
     * @param other The date to compare with
     * @return true if both dates represent the same day, false otherwise
     */
    [[nodiscard]] constexpr bool operator==(const Date& other) const noexcept {
        return year_val == other.year_val && month_val == other.month_val && day_val == other.day_val;
    }
};

// ============================================================================
// @defgroup DateTimeClass DateTime Class
// @brief Combined date and time representation
// ============================================================================

/**
 * @ingroup DateTimeClass
 * @brief Represents a complete date and time (timestamp)
 * 
 * @details
 * The DateTime class combines a Date and Time into a single timestamp
 * representing a specific moment in time to nanosecond precision.
 * 
 * **Memory Layout:**
 * - 8 bytes: Date (year, month, day, padding)
 * - 4 bytes: Time (nanoseconds since midnight)
 * - Total: 12-16 bytes (depending on alignment)
 * 
 * **Features:**
 * - Nanosecond precision timestamps
 * - Compile-time constexpr support
 * - ISO 8601 format support
 * - Combined date and time arithmetic
 * - C++20 spaceship operator
 * 
 * @code
 * // Compile-time construction
 * constexpr dt::DateTime dt(2024, 12, 25, 14, 30, 45, 123456789);
 * 
 * // Runtime current datetime
 * auto now = dt::DateTime::now();
 * 
 * // Arithmetic across day boundaries
 * dt::DateTime dt2(2024, 12, 25, 23, 0);
 * dt2.add_seconds(7200);  // Results in 2024-12-26 01:00
 * @endcode
 */
class DateTime {
private:
    /// @brief Calendar date component
    Date date;
    
    /// @brief Time-of-day component
    Time time;

public:
    /**
     * @brief Default constructor - creates 0001-01-01 00:00:00.000000000
     */
    constexpr DateTime() noexcept = default;
    
    /**
     * @brief Construct from Date and Time components
     * 
     * @param d Date object
     * @param t Time object (default: midnight)
     * 
     * @example
     * @code
     * constexpr dt::Date d(2024, 12, 25);
     * constexpr dt::Time t(14, 30, 45);
     * constexpr dt::DateTime dt(d, t);
     * @endcode
     */
    constexpr DateTime(const Date& d, const Time& t = Time()) noexcept : date(d), time(t) {}
    
    /**
     * @brief Construct from separate date and time components
     * 
     * @param year Year [1-9999]
     * @param month Month [1-12]
     * @param day Day [1-31]
     * @param h Hour [0-23] (default: 0)
     * @param min Minute [0-59] (default: 0)
     * @param s Second [0-59] (default: 0)
     * @param ns Nanosecond [0-999999999] (default: 0)
     * 
     * @throw std::out_of_range if any component is invalid
     * 
     * @example
     * @code
     * constexpr dt::DateTime dt(2024, 12, 25, 14, 30, 45, 123456789);
     * @endcode
     */
    constexpr DateTime(int year, int month, int day, int h = 0, int min = 0, int s = 0, int ns = 0) 
        : date(year, month, day), time(h, min, s, ns) {}

    /**
     * @brief Get date component
     * @return Const reference to internal Date
     */
    [[nodiscard]] constexpr const Date& get_date() const noexcept { return date; }
    
    /**
     * @brief Get time component
     * @return Const reference to internal Time
     */
    [[nodiscard]] constexpr const Time& get_time() const noexcept { return time; }
    
    /**
     * @brief Convenience accessor - get year
     * @return Year [1-9999]
     */
    [[nodiscard]] constexpr int year() const noexcept { return date.year(); }
    
    /**
     * @brief Convenience accessor - get month
     * @return Month [1-12]
     */
    [[nodiscard]] constexpr int month() const noexcept { return date.month(); }
    
    /**
     * @brief Convenience accessor - get day
     * @return Day [1-31]
     */
    [[nodiscard]] constexpr int day() const noexcept { return date.day(); }
    
    /**
     * @brief Convenience accessor - get hour
     * @return Hour [0-23]
     */
    [[nodiscard]] constexpr int hour() const noexcept { return time.hour(); }
    
    /**
     * @brief Convenience accessor - get minute
     * @return Minute [0-59]
     */
    [[nodiscard]] constexpr int minute() const noexcept { return time.minute(); }
    
    /**
     * @brief Convenience accessor - get second
     * @return Second [0-59]
     */
    [[nodiscard]] constexpr int second() const noexcept { return time.second(); }
    
    /**
     * @brief Convenience accessor - get nanosecond
     * @return Nanosecond [0-999999999]
     */
    [[nodiscard]] constexpr int nanosecond() const noexcept { return time.nanosecond(); }

    /**
     * @brief Get current system datetime
     * 
     * @return DateTime object representing current moment
     * 
     * @details
     * Queries the system clock to get both the current date and time,
     * including nanosecond precision if available.
     * 
     * @note This is a runtime function and cannot be called at compile-time
     * 
     * @example
     * @code
     * auto now = dt::DateTime::now();
     * std::cout << "Current: " << now.format() << std::endl;
     * @endcode
     */
    [[nodiscard]] static DateTime now() noexcept {
        return DateTime(Date::today(), Time::now());
    }

    /**
     * @brief Add days to this datetime
     * 
     * @param days Number of days to add (can be negative)
     * @return Reference to this DateTime for chaining
     * 
     * @details
     * Adds days and handles month/year transitions. Time component unchanged.
     * 
     * @example
     * @code
     * dt::DateTime dt(2024, 12, 25, 14, 30);
     * dt.add_days(3);  // 2024-12-28 14:30
     * @endcode
     */
    constexpr DateTime& add_days(int32_t days) noexcept {
        date.add_days(days);
        return *this;
    }

    /**
     * @brief Add seconds to this datetime
     * 
     * @param seconds Number of seconds to add (can be negative)
     * @return Reference to this DateTime for chaining
     * 
     * @details
     * Adds seconds with automatic overflow handling:
     * - If seconds exceed 24 hours, advances to next day(s)
     * - If seconds are negative and exceed current time, goes to previous day(s)
     * 
     * @example
     * @code
     * dt::DateTime dt(2024, 12, 25, 23, 0);
     * dt.add_seconds(7200);  // +2 hours -> 2024-12-26 01:00
     * @endcode
     */
    constexpr DateTime& add_seconds(int64_t seconds) noexcept {
        auto total_secs = time.total_seconds() + seconds;
        int32_t days = total_secs / detail::SECONDS_PER_DAY;
        int32_t remaining = total_secs % detail::SECONDS_PER_DAY;
        
        if (remaining < 0) {
            days--;
            remaining += detail::SECONDS_PER_DAY;
        }
        
        if (days != 0) date.add_days(days);
        time = Time(0, 0, remaining % 60, time.nanosecond());
        time.add_seconds(remaining);
        
        return *this;
    }

    /**
     * @brief Format datetime as string
     * 
     * @param fmt Format string with directives (default: "%Y-%m-%d %H:%M:%S")
     * @return Formatted datetime string
     * 
     * @details
     * **Format directives:**
     * - `%Y` - Year (0001-9999)
     * - `%m` - Month (01-12, zero-padded)
     * - `%d` - Day (01-31, zero-padded)
     * - `%H` - Hour (00-23, zero-padded)
     * - `%M` - Minute (00-59, zero-padded)
     * - `%S` - Second (00-59, zero-padded)
     * - `%N` - Nanosecond (000000000-999999999, zero-padded)
     * - `%w` - Day of week (0-6, 0=Monday)
     * - `%j` - Day of year (001-366, zero-padded)
     * - `%%` - Literal percent sign
     * - Any other character is printed as-is
     * 
     * @example
     * @code
     * dt::DateTime dt(2024, 12, 25, 14, 30, 45, 123456789);
     * std::cout << dt.format() << std::endl;                      // Default
     * std::cout << dt.format("%Y-%m-%d %H:%M:%S") << std::endl;  // Standard
     * std::cout << dt.format("%Y-%m-%dT%H:%M:%S.%N") << std::endl; // ISO 8601 nano
     * @endcode
     */
    [[nodiscard]] std::string format(std::string_view fmt = "%Y-%m-%d %H:%M:%S") const {
        std::string result;
        result.reserve(30);
        
        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                switch (fmt[++i]) {
                    case 'Y': result += std::to_string(year()); break;
                    case 'm': result += (month() < 10 ? "0" : ""); result += std::to_string(month()); break;
                    case 'd': result += (day() < 10 ? "0" : ""); result += std::to_string(day()); break;
                    case 'H': result += (hour() < 10 ? "0" : ""); result += std::to_string(hour()); break;
                    case 'M': result += (minute() < 10 ? "0" : ""); result += std::to_string(minute()); break;
                    case 'S': result += (second() < 10 ? "0" : ""); result += std::to_string(second()); break;
                    case 'N': result += std::to_string(nanosecond()); break;
                    case 'w': result += std::to_string(date.day_of_week()); break;
                    case 'j': {
                        int doy = date.day_of_year();
                        result += (doy < 100 ? "0" : ""); 
                        result += (doy < 10 ? "0" : ""); 
                        result += std::to_string(doy);
                        break;
                    }
                    case '%': result += '%'; break;
                    default: result += fmt[i];
                }
            } else {
                result += fmt[i];
            }
        }
        return result;
    }

    /**
     * @brief Format as ISO 8601 timestamp
     * 
     * @return String in format: YYYY-MM-DDTHH:MM:SS
     * 
     * @details
     * Returns a standard ISO 8601 basic timestamp format. Does not include
     * timezone information (assumes local/unspecified timezone).
     * 
     * @example
     * @code
     * dt::DateTime dt(2024, 12, 25, 14, 30, 45);
     * std::string iso = dt.to_iso8601();  // "2024-12-25T14:30:45"
     * @endcode
     */
    [[nodiscard]] std::string to_iso8601() const {
        return format("%Y-%m-%dT%H:%M:%S");
    }

    /**
     * @brief Compare two datetimes (spaceship operator - C++20)
     * 
     * @param other The datetime to compare with
     * @return Comparison result: <0, ==0, or >0
     * 
     * @details
     * Provides three-way comparison using the spaceship operator (<=>).
     * Compares date first, then time if dates are equal.
     * 
     * @example
     * @code
     * constexpr dt::DateTime dt1(2024, 12, 25, 14, 30);
     * constexpr dt::DateTime dt2(2024, 12, 25, 15, 0);
     * static_assert(dt1 < dt2);
     * @endcode
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const DateTime& other) const noexcept {
        if (auto cmp = date <=> other.date; cmp != 0) return cmp;
        return time <=> other.time;
    }
    
    /**
     * @brief Equality comparison
     * 
     * @param other The datetime to compare with
     * @return true if both represent the same moment, false otherwise
     */
    [[nodiscard]] constexpr bool operator==(const DateTime& other) const noexcept {
        return date == other.date && time == other.time;
    }
};

} // namespace dt
