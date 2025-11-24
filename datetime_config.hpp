/**
 * @file datetime_config.hpp
 * @brief DateTime Library Configuration and Utilities
 * @author zuudevs (zuudevs@gmail.com)
 * @version 1.1.1
 * @date 2025-11-24
 */

#pragma once

#include <cstdint>
#include <array>
#include <string>

namespace zuu {

/**
 * @namespace zuu::detail
 * @brief Internal implementation details
 */
namespace detail {
    // Time constants
    constexpr uint32_t HOURS_PER_DAY = 24;
    constexpr uint32_t MINUTES_PER_HOUR = 60;
    constexpr uint32_t SECONDS_PER_MINUTE = 60;
    constexpr uint32_t SECONDS_PER_HOUR = 3600;
    constexpr uint32_t SECONDS_PER_DAY = 86400;
    constexpr uint32_t MINUTES_PER_DAY = 1440;
    
    // Nanosecond precision
    constexpr uint32_t NANOS_PER_MICROSECOND = 1000;
    constexpr uint32_t NANOS_PER_MILLISECOND = 1'000'000;
    constexpr uint32_t NANOS_PER_SECOND = 1'000'000'000;
    constexpr uint64_t NANOS_PER_MINUTE = 60'000'000'000ULL;
    constexpr uint64_t NANOS_PER_HOUR = 3'600'000'000'000ULL;
    constexpr uint64_t NANOS_PER_DAY = 86'400'000'000'000ULL;
    
    // Calendar constants
    constexpr uint32_t DAYS_IN_YEAR = 365;
    constexpr uint32_t DAYS_IN_LEAP_YEAR = 366;
    constexpr uint32_t MONTHS_PER_YEAR = 12;
    
    // Year range
    constexpr int MIN_YEAR = 1;
    constexpr int MAX_YEAR = 9999;
    
    /**
     * @brief Days in each month (non-leap year)
     */
    constexpr std::array<uint8_t, 12> DAYS_PER_MONTH = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    
    /**
     * @brief Cumulative day count to start of each month (non-leap year)
     */
    constexpr std::array<uint16_t, 13> CUMULATIVE_DAYS = {
        0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
    };
    
    /**
     * @brief Month names (English)
     */
    constexpr std::array<const char*, 12> MONTH_NAMES = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    
    /**
     * @brief Month abbreviations (English)
     */
    constexpr std::array<const char*, 12> MONTH_ABBREV = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    
    /**
     * @brief Day of week names (English)
     */
    constexpr std::array<const char*, 7> WEEKDAY_NAMES = {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"
    };
    
    /**
     * @brief Day of week abbreviations (English)
     */
    constexpr std::array<const char*, 7> WEEKDAY_ABBREV = {
        "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
    };
    
    /**
     * @brief Fast integer division by 10
     */
    constexpr uint32_t div10(uint32_t n) noexcept {
        return n / 10;
    }
    
    /**
     * @brief Fast modulo 10
     */
    constexpr uint32_t mod10(uint32_t n) noexcept {
        return n % 10;
    }
    
    /**
     * @brief Convert 2-digit number to string with zero padding
     */
    inline void append_2digits(std::string& str, uint32_t val) {
        str += static_cast<char>('0' + div10(val));
        str += static_cast<char>('0' + mod10(val));
    }
    
    /**
     * @brief Convert 4-digit number to string with zero padding
     */
    inline void append_4digits(std::string& str, uint32_t val) {
        str += static_cast<char>('0' + (val / 1000));
        str += static_cast<char>('0' + ((val / 100) % 10));
        str += static_cast<char>('0' + ((val / 10) % 10));
        str += static_cast<char>('0' + (val % 10));
    }
    
    /**
     * @brief Convert 9-digit number to string with zero padding
     */
    inline void append_9digits(std::string& str, uint32_t val) {
        char buffer[9];
        for (int i = 8; i >= 0; --i) {
            buffer[i] = '0' + (val % 10);
            val /= 10;
        }
        str.append(buffer, 9);
    }

} // namespace detail

// ============================================================================
// Calendar Utility Functions
// ============================================================================

/**
 * @brief Determine if a year is a leap year
 * @param year The year to check (1-9999)
 * @return true if leap year, false otherwise
 */
constexpr bool is_leap_year(int year) noexcept {
    return (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
}

/**
 * @brief Get the number of days in a specific month
 * @param month Month (1-12)
 * @param year Year for leap year detection
 * @return Number of days (28-31), or 0 if invalid
 */
constexpr int days_in_month(int month, int year) noexcept {
    if (month < 1 || month > 12) return 0;
    if (month != 2) return detail::DAYS_PER_MONTH[month - 1];
    return is_leap_year(year) ? 29 : 28;
}

/**
 * @brief Calculate total days from year 1 to target year
 * @param year Target year (1-9999)
 * @return Total days from year 1 to January 1 of target year
 */
constexpr int32_t days_since_epoch(int year) noexcept {
    --year;
    return year * 365 + year / 4 - year / 100 + year / 400;
}

/**
 * @brief Get number of days in a year
 * @param year The year to check
 * @return 366 if leap year, 365 otherwise
 */
constexpr int days_in_year(int year) noexcept {
    return is_leap_year(year) ? detail::DAYS_IN_LEAP_YEAR : detail::DAYS_IN_YEAR;
}

/**
 * @brief Validate year is in valid range
 * @param year Year to validate
 * @return true if year is between 1 and 9999
 */
constexpr bool is_valid_year(int year) noexcept {
    return year >= detail::MIN_YEAR && year <= detail::MAX_YEAR;
}

/**
 * @brief Validate month is in valid range
 * @param month Month to validate
 * @return true if month is between 1 and 12
 */
constexpr bool is_valid_month(int month) noexcept {
    return month >= 1 && month <= detail::MONTHS_PER_YEAR;
}

/**
 * @brief Validate day is valid for given month and year
 * @param day Day to validate
 * @param month Month (1-12)
 * @param year Year for leap year detection
 * @return true if day is valid for the given month/year
 */
constexpr bool is_valid_day(int day, int month, int year) noexcept {
    return day >= 1 && day <= days_in_month(month, year);
}

/**
 * @brief Validate complete date
 * @param year Year (1-9999)
 * @param month Month (1-12)
 * @param day Day (1-31)
 * @return true if date is valid
 */
constexpr bool is_valid_date(int year, int month, int day) noexcept {
    return is_valid_year(year) && is_valid_month(month) && is_valid_day(day, month, year);
}

/**
 * @brief Validate hour component
 * @param hour Hour to validate
 * @return true if hour is between 0 and 23
 */
constexpr bool is_valid_hour(int hour) noexcept {
    return hour >= 0 && hour < detail::HOURS_PER_DAY;
}

/**
 * @brief Validate minute component
 * @param minute Minute to validate
 * @return true if minute is between 0 and 59
 */
constexpr bool is_valid_minute(int minute) noexcept {
    return minute >= 0 && minute < detail::MINUTES_PER_HOUR;
}

/**
 * @brief Validate second component
 * @param second Second to validate
 * @return true if second is between 0 and 59
 */
constexpr bool is_valid_second(int second) noexcept {
    return second >= 0 && second < detail::SECONDS_PER_MINUTE;
}

/**
 * @brief Validate nanosecond component
 * @param nanosecond Nanosecond to validate
 * @return true if nanosecond is between 0 and 999999999
 */
constexpr bool is_valid_nanosecond(int nanosecond) noexcept {
    return nanosecond >= 0 && nanosecond < static_cast<int>(detail::NANOS_PER_SECOND);
}

/**
 * @brief Validate complete time
 * @param hour Hour (0-23)
 * @param minute Minute (0-59)
 * @param second Second (0-59)
 * @param nanosecond Nanosecond (0-999999999)
 * @return true if time is valid
 */
constexpr bool is_valid_time(int hour, int minute, int second, int nanosecond) noexcept {
    return is_valid_hour(hour) && is_valid_minute(minute) && 
           is_valid_second(second) && is_valid_nanosecond(nanosecond);
}

} // namespace zuu
