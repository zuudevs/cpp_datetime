/**
 * @file date.hpp
 * @brief Calendar date representation with extended year support
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
 * @class Date
 * @brief Represents a calendar date (year 1-9999)
 * 
 * @details
 * Stores a Gregorian calendar date with extended year range.
 * Memory: 4 bytes (uint16_t year + uint8_t month + uint8_t day)
 * 
 * Features:
 * - Year range: 1-9999
 * - Automatic leap year handling
 * - Constexpr support
 * - Day-of-week calculation (Zeller's congruence)
 * - Day-of-year calculation
 */
class Date {
private:
    uint16_t year_ = 1;   ///< Year (1-9999)
    uint8_t month_ = 1;   ///< Month (1-12)
    uint8_t day_ = 1;     ///< Day (1-31)

public:
    /**
     * @brief Default constructor - creates January 1, year 1 (0001-01-01)
     */
    constexpr Date() noexcept = default;
    
    /**
     * @brief Construct date from year, month, day
     * @param y Year [1-9999]
     * @param m Month [1-12]
     * @param d Day [1-31]
     * @throw std::out_of_range if date is invalid
     */
    constexpr Date(int y, int m, int d) {
        if (!is_valid_date(y, m, d)) {
            throw std::out_of_range("Invalid date");
        }
        year_ = static_cast<uint16_t>(y);
        month_ = static_cast<uint8_t>(m);
        day_ = static_cast<uint8_t>(d);
    }

    // ========================================================================
    // Component Accessors
    // ========================================================================
    
    /**
     * @brief Get year component
     * @return Year [1-9999]
     */
    [[nodiscard]] constexpr int year() const noexcept { return year_; }
    
    /**
     * @brief Get month component
     * @return Month [1-12]
     */
    [[nodiscard]] constexpr int month() const noexcept { return month_; }
    
    /**
     * @brief Get day component
     * @return Day [1-31]
     */
    [[nodiscard]] constexpr int day() const noexcept { return day_; }

    // ========================================================================
    // Calendar Calculations
    // ========================================================================
    
    /**
     * @brief Get day of week using Zeller's congruence
     * @return Day of week: 0=Monday, 1=Tuesday, ..., 6=Sunday
     */
    [[nodiscard]] constexpr int day_of_week() const noexcept {
        int y = year_, m = month_;
        if (m < 3) {
            m += 12;
            y--;
        }
        int q = day_;
        int k = y % 100;
        int j = y / 100;
        int h = (q + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 - 2 * j) % 7;
        return (h + 6) % 7;  // Convert to Monday=0 format
    }
    
    /**
     * @brief Get day of year
     * @return Day of year [1-366]
     */
    [[nodiscard]] constexpr int day_of_year() const noexcept {
        int doy = detail::CUMULATIVE_DAYS[month_ - 1] + day_;
        if (month_ > 2 && is_leap_year(year_)) {
            doy++;
        }
        return doy;
    }
    
    /**
     * @brief Get week number (ISO 8601 week date)
     * @return Week number [1-53]
     */
    [[nodiscard]] constexpr int week_number() const noexcept {
        // ISO 8601: Week 1 is first week with Thursday
        Date jan1(year_, 1, 1);
        int dow_jan1 = jan1.day_of_week();
        int doy = day_of_year();
        
        // Adjust for starting Monday
        int week = (doy + dow_jan1 - 1) / 7;
        
        if (dow_jan1 <= 3) {  // Thursday or earlier
            week++;
        }
        
        if (week == 0) {
            // Belongs to last week of previous year
            Date last_year(year_ - 1, 12, 31);
            return last_year.week_number();
        }
        
        if (week == 53) {
            // Check if it belongs to week 1 of next year
            Date next_jan1(year_ + 1, 1, 1);
            int dow_next_jan1 = next_jan1.day_of_week();
            if (dow_next_jan1 <= 3) {
                return 1;
            }
        }
        
        return week;
    }
    
    /**
     * @brief Get quarter of year
     * @return Quarter [1-4]
     */
    [[nodiscard]] constexpr int quarter() const noexcept {
        return (month_ - 1) / 3 + 1;
    }
    
    /**
     * @brief Check if this is a leap year
     * @return true if leap year
     */
    [[nodiscard]] constexpr bool is_leap_year() const noexcept {
        return dt::is_leap_year(year_);
    }

    // ========================================================================
    // Static Factory Methods
    // ========================================================================
    
    /**
     * @brief Get today's date from system clock
     * @return Date object representing current date
     */
    [[nodiscard]] static Date today() noexcept {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm* tm_info = std::localtime(&time_t_now);
        return Date(tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
    }
    
    /**
     * @brief Create Date from day of year
     * @param year Year [1-9999]
     * @param day_of_year Day of year [1-366]
     * @return Date object
     */
    [[nodiscard]] static constexpr Date from_day_of_year(int year, int day_of_year) noexcept {
        if (!is_valid_year(year) || day_of_year < 1 || day_of_year > days_in_year(year)) {
            return Date();
        }
        
        bool leap = is_leap_year(year);
        int month = 1;
        int remaining = day_of_year;
        
        for (int m = 1; m <= 12; ++m) {
            int days = days_in_month(m, year);
            if (remaining <= days) {
                month = m;
                break;
            }
            remaining -= days;
        }
        
        return Date(year, month, remaining);
    }

    // ========================================================================
    // Arithmetic Operations
    // ========================================================================
    
    /**
     * @brief Add days to this date
     * @param days Number of days to add (can be negative)
     * @return Reference to this Date for chaining
     */
    constexpr Date& add_days(int32_t days) noexcept {
        if (days == 0) return *this;
        
        int y = year_;
        int m = month_;
        int d = day_ + days;
        
        // Handle positive days
        while (d > days_in_month(m, y)) {
            d -= days_in_month(m, y);
            if (++m > 12) {
                m = 1;
                y++;
            }
        }
        
        // Handle negative days
        while (d < 1) {
            if (--m < 1) {
                m = 12;
                y--;
            }
            d += days_in_month(m, y);
        }
        
        if (is_valid_date(y, m, d)) {
            year_ = static_cast<uint16_t>(y);
            month_ = static_cast<uint8_t>(m);
            day_ = static_cast<uint8_t>(d);
        }
        
        return *this;
    }
    
    /**
     * @brief Add months to this date
     * @param months Number of months to add (can be negative)
     * @return Reference to this Date for chaining
     */
    constexpr Date& add_months(int32_t months) noexcept {
        if (months == 0) return *this;
        
        int total_months = (year_ - 1) * 12 + (month_ - 1) + months;
        int new_year = total_months / 12 + 1;
        int new_month = total_months % 12 + 1;
        
        if (new_year < detail::MIN_YEAR) new_year = detail::MIN_YEAR;
        if (new_year > detail::MAX_YEAR) new_year = detail::MAX_YEAR;
        
        // Adjust day if it exceeds days in new month
        int new_day = day_;
        int max_day = days_in_month(new_month, new_year);
        if (new_day > max_day) {
            new_day = max_day;
        }
        
        if (is_valid_date(new_year, new_month, new_day)) {
            year_ = static_cast<uint16_t>(new_year);
            month_ = static_cast<uint8_t>(new_month);
            day_ = static_cast<uint8_t>(new_day);
        }
        
        return *this;
    }
    
    /**
     * @brief Add years to this date
     * @param years Number of years to add (can be negative)
     * @return Reference to this Date for chaining
     */
    constexpr Date& add_years(int32_t years) noexcept {
        return add_months(years * 12);
    }
    
    /**
     * @brief Calculate days between two dates
     * @param other Other date
     * @return Number of days (positive if this > other)
     */
    [[nodiscard]] constexpr int32_t days_between(const Date& other) const noexcept {
        int32_t days1 = days_since_epoch(year_) + day_of_year() - 1;
        int32_t days2 = days_since_epoch(other.year_) + other.day_of_year() - 1;
        return days1 - days2;
    }

    // ========================================================================
    // Formatting
    // ========================================================================
    
    /**
     * @brief Format date as string
     * @param fmt Format string (default: "%Y-%m-%d")
     * 
     * Format specifiers:
     * - %Y: Year (0001-9999)
     * - %m: Month (01-12, zero-padded)
     * - %d: Day (01-31, zero-padded)
     * - %w: Day of week (0-6, 0=Monday)
     * - %j: Day of year (001-366, zero-padded)
     * - %q: Quarter (1-4)
     * - %W: Week number (01-53, zero-padded)
     * - %B: Full month name (e.g., "January")
     * - %b: Short month name (e.g., "Jan")
     * - %A: Full weekday name (e.g., "Monday")
     * - %a: Short weekday name (e.g., "Mon")
     * - %%: Literal %
     * 
     * @return Formatted string
     */
    [[nodiscard]] std::string format(std::string_view fmt = "%Y-%m-%d") const {
        std::string result;
        result.reserve(fmt.size() + 16);
        
        for (size_t i = 0; i < fmt.size(); ++i) {
            if (fmt[i] == '%' && i + 1 < fmt.size()) {
                ++i;
                switch (fmt[i]) {
                    case 'Y': detail::append_4digits(result, year_); break;
                    case 'm': detail::append_2digits(result, month_); break;
                    case 'd': detail::append_2digits(result, day_); break;
                    case 'w': result += static_cast<char>('0' + day_of_week()); break;
                    case 'j': {
                        int doy = day_of_year();
                        result += static_cast<char>('0' + doy / 100);
                        result += static_cast<char>('0' + (doy / 10) % 10);
                        result += static_cast<char>('0' + doy % 10);
                        break;
                    }
                    case 'q': result += static_cast<char>('0' + quarter()); break;
                    case 'W': detail::append_2digits(result, week_number()); break;
                    case 'B': result += detail::MONTH_NAMES[month_ - 1]; break;
                    case 'b': result += detail::MONTH_ABBREV[month_ - 1]; break;
                    case 'A': result += detail::WEEKDAY_NAMES[day_of_week()]; break;
                    case 'a': result += detail::WEEKDAY_ABBREV[day_of_week()]; break;
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
    [[nodiscard]] constexpr std::strong_ordering operator<=>(const Date& other) const noexcept {
        if (auto cmp = year_ <=> other.year_; cmp != 0) return cmp;
        if (auto cmp = month_ <=> other.month_; cmp != 0) return cmp;
        return day_ <=> other.day_;
    }
    
    /**
     * @brief Equality comparison
     */
    [[nodiscard]] constexpr bool operator==(const Date& other) const noexcept {
        return year_ == other.year_ && month_ == other.month_ && day_ == other.day_;
    }

    // ========================================================================
    // Utility Methods
    // ========================================================================
    
    /**
     * @brief Check if this is a weekend (Saturday or Sunday)
     * @return true if Saturday or Sunday
     */
    [[nodiscard]] constexpr bool is_weekend() const noexcept {
        int dow = day_of_week();
        return dow == 5 || dow == 6;  // Saturday or Sunday
    }
    
    /**
     * @brief Check if this is a weekday (Monday-Friday)
     * @return true if Monday through Friday
     */
    [[nodiscard]] constexpr bool is_weekday() const noexcept {
        return !is_weekend();
    }
    
    /**
     * @brief Get the first day of the month
     * @return Date representing first day of this month
     */
    [[nodiscard]] constexpr Date first_day_of_month() const noexcept {
        return Date(year_, month_, 1);
    }
    
    /**
     * @brief Get the last day of the month
     * @return Date representing last day of this month
     */
    [[nodiscard]] constexpr Date last_day_of_month() const noexcept {
        return Date(year_, month_, days_in_month(month_, year_));
    }
    
    /**
     * @brief Get the first day of the year
     * @return Date representing January 1 of this year
     */
    [[nodiscard]] constexpr Date first_day_of_year() const noexcept {
        return Date(year_, 1, 1);
    }
    
    /**
     * @brief Get the last day of the year
     * @return Date representing December 31 of this year
     */
    [[nodiscard]] constexpr Date last_day_of_year() const noexcept {
        return Date(year_, 12, 31);
    }
};

} // namespace dt
