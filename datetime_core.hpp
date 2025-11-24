/**
 * @file datetime_core.hpp
 * @brief Combined date and time representation
 * @author zuudevs (zuudevs@gmail.com)
 * @version 1.1.1
 * @date 2025-11-24
 */

#pragma once

#include "date_core.hpp"
#include "time_core.hpp"

namespace zuu {

/**
 * @class DateTime
 * @brief Represents a complete timestamp with nanosecond precision
 * 
 * @details
 * Combines Date and Time into a single moment in time.
 * Memory: 12 bytes (4 bytes date + 8 bytes time)
 * 
 * Features:
 * - Nanosecond precision timestamps
 * - Constexpr support
 * - ISO 8601 format
 * - Combined date/time arithmetic
 * - Automatic overflow handling
 */
class DateTime {
private:
    Date date_;
    Time time_;

public:
    /**
     * @brief Default constructor - creates 0001-01-01 00:00:00.000000000
     */
    constexpr DateTime() noexcept = default;
    
    /**
     * @brief Construct from Date and Time components
     * @param d Date object
     * @param t Time object (default: midnight)
     */
    constexpr DateTime(const Date& d, const Time& t = Time()) noexcept 
        : date_(d), time_(t) {}
    
    /**
     * @brief Construct from separate components
     * @param year Year [1-9999]
     * @param month Month [1-12]
     * @param day Day [1-31]
     * @param hour Hour [0-23] (default: 0)
     * @param minute Minute [0-59] (default: 0)
     * @param second Second [0-59] (default: 0)
     * @param nanosecond Nanosecond [0-999999999] (default: 0)
     * @throw std::out_of_range if any component is invalid
     */
    constexpr DateTime(int year, int month, int day, 
                      int hour = 0, int minute = 0, int second = 0, int nanosecond = 0)
        : date_(year, month, day), time_(hour, minute, second, nanosecond) {}

    // ========================================================================
    // Component Accessors
    // ========================================================================
    
    /**
     * @brief Get date component
     * @return Const reference to Date
     */
    [[nodiscard]] constexpr const Date& get_date() const noexcept { return date_; }
    
    /**
     * @brief Get time component
     * @return Const reference to Time
     */
    [[nodiscard]] constexpr const Time& get_time() const noexcept { return time_; }
    
    /**
     * @brief Get mutable date component
     * @return Reference to Date
     */
    [[nodiscard]] constexpr Date& get_date() noexcept { return date_; }
    
    /**
     * @brief Get mutable time component
     * @return Reference to Time
     */
    [[nodiscard]] constexpr Time& get_time() noexcept { return time_; }
    
    // Convenience accessors for date components
    [[nodiscard]] constexpr int year() const noexcept { return date_.year(); }
    [[nodiscard]] constexpr int month() const noexcept { return date_.month(); }
    [[nodiscard]] constexpr int day() const noexcept { return date_.day(); }
    [[nodiscard]] constexpr int day_of_week() const noexcept { return date_.day_of_week(); }
    [[nodiscard]] constexpr int day_of_year() const noexcept { return date_.day_of_year(); }
    [[nodiscard]] constexpr int quarter() const noexcept { return date_.quarter(); }
    
    // Convenience accessors for time components
    [[nodiscard]] constexpr int hour() const noexcept { return time_.hour(); }
    [[nodiscard]] constexpr int minute() const noexcept { return time_.minute(); }
    [[nodiscard]] constexpr int second() const noexcept { return time_.second(); }
    [[nodiscard]] constexpr int nanosecond() const noexcept { return time_.nanosecond(); }
    [[nodiscard]] constexpr int millisecond() const noexcept { return time_.millisecond(); }
    [[nodiscard]] constexpr int microsecond() const noexcept { return time_.microsecond(); }

    // ========================================================================
    // Static Factory Methods
    // ========================================================================
    
    /**
     * @brief Get current system datetime
     * @return DateTime object representing current moment
     */
    [[nodiscard]] static DateTime now() noexcept {
        return DateTime(Date::today(), Time::now());
    }
    
    /**
     * @brief Create DateTime from Unix timestamp (seconds since epoch)
     * @param seconds Unix timestamp in seconds
     * @return DateTime object
     * @note Epoch is 1970-01-01 00:00:00 UTC
     */
    [[nodiscard]] static DateTime from_unix_timestamp(int64_t seconds) noexcept {
        // Unix epoch: 1970-01-01
        Date epoch(1970, 1, 1);
        
        int64_t days = seconds / detail::SECONDS_PER_DAY;
        int64_t remaining_seconds = seconds % detail::SECONDS_PER_DAY;
        
        if (remaining_seconds < 0) {
            days--;
            remaining_seconds += detail::SECONDS_PER_DAY;
        }
        
        epoch.add_days(static_cast<int32_t>(days));
        Time time = Time::from_seconds(remaining_seconds);
        
        return DateTime(epoch, time);
    }

    // ========================================================================
    // Arithmetic Operations
    // ========================================================================
    
    /**
     * @brief Add days to this datetime
     * @param days Number of days to add (can be negative)
     * @return Reference to this DateTime for chaining
     */
    constexpr DateTime& add_days(int32_t days) noexcept {
        date_.add_days(days);
        return *this;
    }
    
    /**
     * @brief Add months to this datetime
     * @param months Number of months to add (can be negative)
     * @return Reference to this DateTime for chaining
     */
    constexpr DateTime& add_months(int32_t months) noexcept {
        date_.add_months(months);
        return *this;
    }
    
    /**
     * @brief Add years to this datetime
     * @param years Number of years to add (can be negative)
     * @return Reference to this DateTime for chaining
     */
    constexpr DateTime& add_years(int32_t years) noexcept {
        date_.add_years(years);
        return *this;
    }
    
    /**
     * @brief Add seconds to this datetime
     * @param seconds Number of seconds to add (can be negative)
     * @return Reference to this DateTime for chaining
     * @note Automatically handles day overflow
     */
    constexpr DateTime& add_seconds(int64_t seconds) noexcept {
        if (seconds == 0) return *this;
        
        int64_t total_secs = time_.total_seconds() + seconds;
        
        // Calculate day overflow
        int32_t day_overflow = 0;
        if (total_secs >= detail::SECONDS_PER_DAY) {
            day_overflow = static_cast<int32_t>(total_secs / detail::SECONDS_PER_DAY);
            total_secs %= detail::SECONDS_PER_DAY;
        } else if (total_secs < 0) {
            day_overflow = static_cast<int32_t>((total_secs - detail::SECONDS_PER_DAY + 1) / detail::SECONDS_PER_DAY);
            total_secs -= day_overflow * detail::SECONDS_PER_DAY;
        }
        
        // Update date if there's overflow
        if (day_overflow != 0) {
            date_.add_days(day_overflow);
        }
        
        // Create new time from remaining seconds, preserving nanoseconds
        int ns = time_.nanosecond();
        time_ = Time::from_seconds(total_secs);
        time_.add_nanoseconds(ns);
        
        return *this;
    }
    
    /**
     * @brief Add minutes to this datetime
     * @param minutes Number of minutes to add (can be negative)
     * @return Reference to this DateTime for chaining
     */
    constexpr DateTime& add_minutes(int64_t minutes) noexcept {
        return add_seconds(minutes * detail::SECONDS_PER_MINUTE);
    }
    
    /**
     * @brief Add hours to this datetime
     * @param hours Number of hours to add (can be negative)
     * @return Reference to this DateTime for chaining
     */
    constexpr DateTime& add_hours(int64_t hours) noexcept {
        return add_seconds(hours * detail::SECONDS_PER_HOUR);
    }
    
    /**
     * @brief Add milliseconds to this datetime
     * @param milliseconds Number of milliseconds to add (can be negative)
     * @return Reference to this DateTime for chaining
     */
    constexpr DateTime& add_milliseconds(int64_t milliseconds) noexcept {
        int64_t nanos = milliseconds * detail::NANOS_PER_MILLISECOND;
        int64_t total_nanos = static_cast<int64_t>(time_.total_nanoseconds()) + nanos;
        
        // Calculate day overflow
        int32_t day_overflow = 0;
        if (total_nanos >= static_cast<int64_t>(detail::NANOS_PER_DAY)) {
            day_overflow = static_cast<int32_t>(total_nanos / detail::NANOS_PER_DAY);
            total_nanos %= detail::NANOS_PER_DAY;
        } else if (total_nanos < 0) {
            day_overflow = static_cast<int32_t>((total_nanos - detail::NANOS_PER_DAY + 1) / detail::NANOS_PER_DAY);
            total_nanos -= day_overflow * detail::NANOS_PER_DAY;
        }
        
        if (day_overflow != 0) {
            date_.add_days(day_overflow);
        }
        
        time_ = Time(static_cast<uint64_t>(total_nanos));
        return *this;
    }
    
    /**
     * @brief Add nanoseconds to this datetime
     * @param nanoseconds Number of nanoseconds to add (can be negative)
     * @return Reference to this DateTime for chaining
     */
    constexpr DateTime& add_nanoseconds(int64_t nanoseconds) noexcept {
        int64_t total_nanos = static_cast<int64_t>(time_.total_nanoseconds()) + nanoseconds;
        
        int32_t day_overflow = 0;
        if (total_nanos >= static_cast<int64_t>(detail::NANOS_PER_DAY)) {
            day_overflow = static_cast<int32_t>(total_nanos / detail::NANOS_PER_DAY);
            total_nanos %= detail::NANOS_PER_DAY;
        } else if (total_nanos < 0) {
            day_overflow = static_cast<int32_t>((total_nanos - detail::NANOS_PER_DAY + 1) / detail::NANOS_PER_DAY);
            total_nanos -= day_overflow * detail::NANOS_PER_DAY;
        }
        
        if (day_overflow != 0) {
            date_.add_days(day_overflow);
        }
        
        time_ = Time(static_cast<uint64_t>(total_nanos));
        return *this;
    }
    
    /**
     * @brief Calculate total seconds between two datetimes
     * @param other Other datetime
     * @return Number of seconds (positive if this > other)
     */
    [[nodiscard]] constexpr int64_t seconds_between(const DateTime& other) const noexcept {
        int32_t day_diff = date_.days_between(other.date_);
        int64_t sec_diff = time_.total_seconds() - other.time_.total_seconds();
        return static_cast<int64_t>(day_diff) * detail::SECONDS_PER_DAY + sec_diff;
    }

    // ========================================================================
    // Formatting
    // ========================================================================
    
    /**
     * @brief Format datetime as string
     * @param fmt Format string (default: "%Y-%m-%d %H:%M:%S")
     * 
     * Supports all Date and Time format specifiers
     * @return Formatted string
     */
    [[nodiscard]] std::string format(std::string_view fmt = "%Y-%m-%d %H:%M:%S") const {
        std::string result;
        result.reserve(fmt.size() + 32);
        
        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                ++i;
                char spec = fmt[i];
                
                // Try date formats first
                switch (spec) {
                    case 'Y': detail::append_4digits(result, year()); break;
                    case 'm': detail::append_2digits(result, month()); break;
                    case 'd': detail::append_2digits(result, day()); break;
                    case 'w': result += static_cast<char>('0' + day_of_week()); break;
                    case 'j': {
                        int doy = day_of_year();
                        result += static_cast<char>('0' + doy / 100);
                        result += static_cast<char>('0' + (doy / 10) % 10);
                        result += static_cast<char>('0' + doy % 10);
                        break;
                    }
                    case 'q': result += static_cast<char>('0' + quarter()); break;
                    case 'B': result += detail::MONTH_NAMES[month() - 1]; break;
                    case 'b': result += detail::MONTH_ABBREV[month() - 1]; break;
                    case 'A': result += detail::WEEKDAY_NAMES[day_of_week()]; break;
                    case 'a': result += detail::WEEKDAY_ABBREV[day_of_week()]; break;
                    
                    // Time formats
                    case 'H': detail::append_2digits(result, hour()); break;
                    case 'M': detail::append_2digits(result, minute()); break;
                    case 'S': detail::append_2digits(result, second()); break;
                    case 'f': {
                        int ms = millisecond();
                        result += static_cast<char>('0' + ms / 100);
                        result += static_cast<char>('0' + (ms / 10) % 10);
                        result += static_cast<char>('0' + ms % 10);
                        break;
                    }
                    case 'u': {
                        int us = microsecond();
                        char buf[6];
                        for (int j = 5; j >= 0; --j) {
                            buf[j] = '0' + (us % 10);
                            us /= 10;
                        }
                        result.append(buf, 6);
                        break;
                    }
                    case 'N': detail::append_9digits(result, nanosecond()); break;
                    case '%': result += '%'; break;
                    default: result += spec; break;
                }
            } else {
                result += fmt[i];
            }
        }
        return result;
    }
    
    /**
     * @brief Format as ISO 8601 timestamp
     * @return String in format: YYYY-MM-DDTHH:MM:SS
     */
    [[nodiscard]] std::string to_iso8601() const {
        return format("%Y-%m-%dT%H:%M:%S");
    }
    
    /**
     * @brief Format as ISO 8601 with milliseconds
     * @return String in format: YYYY-MM-DDTHH:MM:SS.fff
     */
    [[nodiscard]] std::string to_iso8601_ms() const {
        return format("%Y-%m-%dT%H:%M:%S.%f");
    }
    
    /**
     * @brief Format as ISO 8601 with microseconds
     * @return String in format: YYYY-MM-DDTHH:MM:SS.uuuuuu
     */
    [[nodiscard]] std::string to_iso8601_us() const {
        return format("%Y-%m-%dT%H:%M:%S.%u");
    }
    
    /**
     * @brief Format as ISO 8601 with nanoseconds
     * @return String in format: YYYY-MM-DDTHH:MM:SS.nnnnnnnnn
     */
    [[nodiscard]] std::string to_iso8601_ns() const {
        return format("%Y-%m-%dT%H:%M:%S.%N");
    }

    // ========================================================================
    // Comparison Operators
    // ========================================================================
    
    /**
     * @brief Three-way comparison (C++20 spaceship operator)
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const DateTime& other) const noexcept {
        if (auto cmp = date_ <=> other.date_; cmp != 0) return cmp;
        return time_ <=> other.time_;
    }
    
    /**
     * @brief Equality comparison
     */
    [[nodiscard]] constexpr bool operator==(const DateTime& other) const noexcept {
        return date_ == other.date_ && time_ == other.time_;
    }

    // ========================================================================
    // Conversion Methods
    // ========================================================================
    
    /**
     * @brief Convert to Unix timestamp (seconds since 1970-01-01)
     * @return Unix timestamp in seconds
     */
    [[nodiscard]] constexpr int64_t to_unix_timestamp() const noexcept {
        Date epoch(1970, 1, 1);
        int32_t day_diff = date_.days_between(epoch);
        return static_cast<int64_t>(day_diff) * detail::SECONDS_PER_DAY + time_.total_seconds();
    }
    
    /**
     * @brief Convert to Unix timestamp in milliseconds
     * @return Unix timestamp in milliseconds
     */
    [[nodiscard]] constexpr int64_t to_unix_timestamp_ms() const noexcept {
        return to_unix_timestamp() * 1000 + millisecond();
    }
};

} // namespace dt
