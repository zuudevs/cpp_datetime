/**
 * @file time.hpp
 * @brief High-precision time-of-day representation
 * @author zuudevs (zuudevs@gmail.com)
 * @version 1.0
 * @date 2025-11-24
 */

#pragma once

#include "datetime_config.hpp"
#include <chrono>
#include <string>
#include <string_view>
#include <compare>

namespace dt {

/**
 * @class Time
 * @brief Represents a time of day with nanosecond precision
 * 
 * @details
 * Stores time from 00:00:00.000000000 to 23:59:59.999999999
 * Memory: 8 bytes (uint64_t for nanoseconds)
 * 
 * Features:
 * - Nanosecond precision
 * - Constexpr support
 * - C++20 spaceship operator
 * - Efficient arithmetic operations
 */
class Time {
private:
    /// Total nanoseconds since midnight (0 to 86,399,999,999,999)
    uint64_t total_nanos_ = 0;

public:
    /**
     * @brief Default constructor - creates midnight (00:00:00.000000000)
     */
    constexpr Time() noexcept = default;
    
    /**
     * @brief Construct time from components
     * @param h Hour [0-23]
     * @param min Minute [0-59]
     * @param s Second [0-59] (default: 0)
     * @param ns Nanosecond [0-999999999] (default: 0)
     * @throw std::out_of_range if any component is invalid
     */
    constexpr Time(int h, int min, int s = 0, int ns = 0) {
        if (!is_valid_time(h, min, s, ns)) {
            throw std::out_of_range("Invalid time components");
        }
        total_nanos_ = static_cast<uint64_t>(h) * detail::NANOS_PER_HOUR +
                       static_cast<uint64_t>(min) * detail::NANOS_PER_MINUTE +
                       static_cast<uint64_t>(s) * detail::NANOS_PER_SECOND +
                       static_cast<uint64_t>(ns);
    }
    
    /**
     * @brief Construct from total nanoseconds since midnight
     * @param nanos Total nanoseconds [0-86399999999999]
     * @throw std::out_of_range if value exceeds 24 hours
     */
    constexpr explicit Time(uint64_t nanos) : total_nanos_(nanos) {
        if (nanos >= detail::NANOS_PER_DAY) {
            throw std::out_of_range("Nanoseconds exceed 24 hours");
        }
    }

    // ========================================================================
    // Component Accessors
    // ========================================================================
    
    /**
     * @brief Get hour component
     * @return Hour [0-23]
     */
    [[nodiscard]] constexpr int hour() const noexcept {
        return static_cast<int>(total_nanos_ / detail::NANOS_PER_HOUR);
    }
    
    /**
     * @brief Get minute component
     * @return Minute [0-59]
     */
    [[nodiscard]] constexpr int minute() const noexcept {
        return static_cast<int>((total_nanos_ % detail::NANOS_PER_HOUR) / detail::NANOS_PER_MINUTE);
    }
    
    /**
     * @brief Get second component
     * @return Second [0-59]
     */
    [[nodiscard]] constexpr int second() const noexcept {
        return static_cast<int>((total_nanos_ % detail::NANOS_PER_MINUTE) / detail::NANOS_PER_SECOND);
    }
    
    /**
     * @brief Get nanosecond component
     * @return Nanosecond [0-999999999]
     */
    [[nodiscard]] constexpr int nanosecond() const noexcept {
        return static_cast<int>(total_nanos_ % detail::NANOS_PER_SECOND);
    }
    
    /**
     * @brief Get millisecond component
     * @return Millisecond [0-999]
     */
    [[nodiscard]] constexpr int millisecond() const noexcept {
        return static_cast<int>((total_nanos_ % detail::NANOS_PER_SECOND) / detail::NANOS_PER_MILLISECOND);
    }
    
    /**
     * @brief Get microsecond component
     * @return Microsecond [0-999999]
     */
    [[nodiscard]] constexpr int microsecond() const noexcept {
        return static_cast<int>((total_nanos_ % detail::NANOS_PER_SECOND) / detail::NANOS_PER_MICROSECOND);
    }

    // ========================================================================
    // Total Value Accessors
    // ========================================================================
    
    /**
     * @brief Get total seconds since midnight
     * @return Total seconds [0-86399]
     */
    [[nodiscard]] constexpr int64_t total_seconds() const noexcept {
        return static_cast<int64_t>(total_nanos_ / detail::NANOS_PER_SECOND);
    }
    
    /**
     * @brief Get total milliseconds since midnight
     * @return Total milliseconds [0-86399999]
     */
    [[nodiscard]] constexpr int64_t total_milliseconds() const noexcept {
        return static_cast<int64_t>(total_nanos_ / detail::NANOS_PER_MILLISECOND);
    }
    
    /**
     * @brief Get total microseconds since midnight
     * @return Total microseconds [0-86399999999]
     */
    [[nodiscard]] constexpr int64_t total_microseconds() const noexcept {
        return static_cast<int64_t>(total_nanos_ / detail::NANOS_PER_MICROSECOND);
    }
    
    /**
     * @brief Get total nanoseconds since midnight
     * @return Total nanoseconds [0-86399999999999]
     */
    [[nodiscard]] constexpr uint64_t total_nanoseconds() const noexcept {
        return total_nanos_;
    }

    // ========================================================================
    // Static Factory Methods
    // ========================================================================
    
    /**
     * @brief Get current system time
     * @return Time object representing current time of day
     */
    [[nodiscard]] static Time now() noexcept {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        
        // Get nanoseconds within current day
        nanos %= detail::NANOS_PER_DAY;
        if (nanos < 0) nanos += detail::NANOS_PER_DAY;
        
        return Time(static_cast<uint64_t>(nanos));
    }
    
    /**
     * @brief Create Time from total seconds
     * @param seconds Total seconds since midnight
     * @return Time object
     */
    [[nodiscard]] static constexpr Time from_seconds(int64_t seconds) noexcept {
        seconds %= detail::SECONDS_PER_DAY;
        if (seconds < 0) seconds += detail::SECONDS_PER_DAY;
        return Time(static_cast<uint64_t>(seconds) * detail::NANOS_PER_SECOND);
    }
    
    /**
     * @brief Create Time from total milliseconds
     * @param milliseconds Total milliseconds since midnight
     * @return Time object
     */
    [[nodiscard]] static constexpr Time from_milliseconds(int64_t milliseconds) noexcept {
        int64_t nanos = milliseconds * detail::NANOS_PER_MILLISECOND;
        nanos %= detail::NANOS_PER_DAY;
        if (nanos < 0) nanos += detail::NANOS_PER_DAY;
        return Time(static_cast<uint64_t>(nanos));
    }

    // ========================================================================
    // Arithmetic Operations
    // ========================================================================
    
    /**
     * @brief Add seconds to this time
     * @param seconds Number of seconds to add (can be negative)
     * @return Reference to this Time for chaining
     * @note Wraps around 24-hour boundary
     */
    constexpr Time& add_seconds(int64_t seconds) noexcept {
        int64_t total_secs = total_seconds() + seconds;
        total_secs %= detail::SECONDS_PER_DAY;
        if (total_secs < 0) total_secs += detail::SECONDS_PER_DAY;
        
        total_nanos_ = static_cast<uint64_t>(total_secs) * detail::NANOS_PER_SECOND + 
                       (total_nanos_ % detail::NANOS_PER_SECOND);
        return *this;
    }
    
    /**
     * @brief Add minutes to this time
     * @param minutes Number of minutes to add (can be negative)
     * @return Reference to this Time for chaining
     */
    constexpr Time& add_minutes(int64_t minutes) noexcept {
        return add_seconds(minutes * detail::SECONDS_PER_MINUTE);
    }
    
    /**
     * @brief Add hours to this time
     * @param hours Number of hours to add (can be negative)
     * @return Reference to this Time for chaining
     */
    constexpr Time& add_hours(int64_t hours) noexcept {
        return add_seconds(hours * detail::SECONDS_PER_HOUR);
    }
    
    /**
     * @brief Add milliseconds to this time
     * @param milliseconds Number of milliseconds to add (can be negative)
     * @return Reference to this Time for chaining
     */
    constexpr Time& add_milliseconds(int64_t milliseconds) noexcept {
        int64_t nanos = static_cast<int64_t>(total_nanos_) + 
                        milliseconds * detail::NANOS_PER_MILLISECOND;
        nanos %= detail::NANOS_PER_DAY;
        if (nanos < 0) nanos += detail::NANOS_PER_DAY;
        total_nanos_ = static_cast<uint64_t>(nanos);
        return *this;
    }
    
    /**
     * @brief Add nanoseconds to this time
     * @param nanoseconds Number of nanoseconds to add (can be negative)
     * @return Reference to this Time for chaining
     */
    constexpr Time& add_nanoseconds(int64_t nanoseconds) noexcept {
        int64_t nanos = static_cast<int64_t>(total_nanos_) + nanoseconds;
        nanos %= detail::NANOS_PER_DAY;
        if (nanos < 0) nanos += detail::NANOS_PER_DAY;
        total_nanos_ = static_cast<uint64_t>(nanos);
        return *this;
    }

    // ========================================================================
    // Formatting
    // ========================================================================
    
    /**
     * @brief Format time as string
     * @param fmt Format string (default: "%H:%M:%S")
     * 
     * Format specifiers:
     * - %H: Hour (00-23)
     * - %M: Minute (00-59)
     * - %S: Second (00-59)
     * - %f: Millisecond (000-999)
     * - %u: Microsecond (000000-999999)
     * - %N: Nanosecond (000000000-999999999)
     * - %%: Literal %
     * 
     * @return Formatted string
     */
    [[nodiscard]] std::string format(std::string_view fmt = "%H:%M:%S") const {
        std::string result;
        result.reserve(fmt.size() + 16);
        
        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                ++i;
                switch (fmt[i]) {
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
                    default: result += fmt[i]; break;
                }
            } else {
                result += fmt[i];
            }
        }
        return result;
    }

    // ========================================================================
    // Comparison Operators
    // ========================================================================
    
    /**
     * @brief Three-way comparison (C++20 spaceship operator)
     */
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const Time& other) const noexcept {
        return total_nanos_ <=> other.total_nanos_;
    }
    
    /**
     * @brief Equality comparison
     */
    [[nodiscard]] constexpr bool operator==(const Time& other) const noexcept {
        return total_nanos_ == other.total_nanos_;
    }

    // ========================================================================
    // Utility Methods
    // ========================================================================
    
    /**
     * @brief Check if time is midnight
     * @return true if 00:00:00.000000000
     */
    [[nodiscard]] constexpr bool is_midnight() const noexcept {
        return total_nanos_ == 0;
    }
    
    /**
     * @brief Check if time is noon
     * @return true if 12:00:00.000000000
     */
    [[nodiscard]] constexpr bool is_noon() const noexcept {
        return total_nanos_ == 12 * detail::NANOS_PER_HOUR;
    }
    
    /**
     * @brief Check if time is AM (before noon)
     * @return true if hour < 12
     */
    [[nodiscard]] constexpr bool is_am() const noexcept {
        return hour() < 12;
    }
    
    /**
     * @brief Check if time is PM (after noon)
     * @return true if hour >= 12
     */
    [[nodiscard]] constexpr bool is_pm() const noexcept {
        return hour() >= 12;
    }
    
    /**
     * @brief Get hour in 12-hour format
     * @return Hour [1-12]
     */
    [[nodiscard]] constexpr int hour12() const noexcept {
        int h = hour();
        if (h == 0) return 12;
        if (h > 12) return h - 12;
        return h;
    }
};

} // namespace dt
