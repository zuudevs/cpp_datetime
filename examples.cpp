/**
 * @file examples.cpp
 * @brief Comprehensive examples for DateTime Library v1.1.1
 */

#include "datetime.hpp"
#include <iostream>
#include <cassert>

// ============================================================================
// Example 1: Basic Date Operations
// ============================================================================
void example_date_basics() {
    std::cout << "\n=== Date Basics ===" << std::endl;
    
    // Creating dates
    zuu::Date christmas(2024, 12, 25);
    zuu::Date today = zuu::Date::today();
    
    std::cout << "Christmas 2024: " << christmas.format("%A, %B %d, %Y") << std::endl;
    std::cout << "Today: " << today.format("%Y-%m-%d") << std::endl;
    
    // Date components
    std::cout << "\nChristmas details:" << std::endl;
    std::cout << "  Year: " << christmas.year() << std::endl;
    std::cout << "  Month: " << christmas.month() << std::endl;
    std::cout << "  Day: " << christmas.day() << std::endl;
    std::cout << "  Day of week: " << christmas.day_of_week() << " (0=Monday)" << std::endl;
    std::cout << "  Day of year: " << christmas.day_of_year() << std::endl;
    std::cout << "  Quarter: " << christmas.quarter() << std::endl;
    std::cout << "  Week number: " << christmas.week_number() << std::endl;
    std::cout << "  Is weekend? " << (christmas.is_weekend() ? "Yes" : "No") << std::endl;
    std::cout << "  Is leap year? " << (christmas.is_leap_year() ? "Yes" : "No") << std::endl;
}

// ============================================================================
// Example 2: Date Arithmetic
// ============================================================================
void example_date_arithmetic() {
    std::cout << "\n=== Date Arithmetic ===" << std::endl;
    
    zuu::Date date(2024, 12, 25);
    std::cout << "Start date: " << date.format() << std::endl;
    
    // Add days
    zuu::Date future = date;
    future.add_days(10);
    std::cout << "After adding 10 days: " << future.format() << std::endl;
    
    // Subtract days
    zuu::Date past = date;
    past.add_days(-30);
    std::cout << "After subtracting 30 days: " << past.format() << std::endl;
    
    // Add months
    zuu::Date next_month = date;
    next_month.add_months(1);
    std::cout << "After adding 1 month: " << next_month.format() << std::endl;
    
    // Add years
    zuu::Date next_year = date;
    next_year.add_years(1);
    std::cout << "After adding 1 year: " << next_year.format() << std::endl;
    
    // Calculate difference
    zuu::Date date1(2024, 1, 1);
    zuu::Date date2(2024, 12, 31);
    int32_t days_diff = date2.days_between(date1);
    std::cout << "\nDays between Jan 1 and Dec 31, 2024: " << days_diff << std::endl;
}

// ============================================================================
// Example 3: Time Operations
// ============================================================================
void example_time_basics() {
    std::cout << "\n=== Time Basics ===" << std::endl;
    
    // Creating times
    zuu::Time morning(8, 30, 0);
    zuu::Time precise(14, 30, 45, 123456789);
    zuu::Time now = zuu::Time::now();
    
    std::cout << "Morning: " << morning.format() << std::endl;
    std::cout << "Precise time: " << precise.format("%H:%M:%S.%N") << std::endl;
    std::cout << "Current time: " << now.format() << std::endl;
    
    // Time components
    std::cout << "\nPrecise time components:" << std::endl;
    std::cout << "  Hour: " << precise.hour() << std::endl;
    std::cout << "  Minute: " << precise.minute() << std::endl;
    std::cout << "  Second: " << precise.second() << std::endl;
    std::cout << "  Millisecond: " << precise.millisecond() << std::endl;
    std::cout << "  Microsecond: " << precise.microsecond() << std::endl;
    std::cout << "  Nanosecond: " << precise.nanosecond() << std::endl;
    std::cout << "  Total seconds: " << precise.total_seconds() << std::endl;
    
    // 12-hour format
    std::cout << "\n12-hour format:" << std::endl;
    std::cout << "  Hour: " << precise.hour12() << " " << (precise.is_pm() ? "PM" : "AM") << std::endl;
}

// ============================================================================
// Example 4: Time Arithmetic
// ============================================================================
void example_time_arithmetic() {
    std::cout << "\n=== Time Arithmetic ===" << std::endl;
    
    zuu::Time time(14, 30, 0);
    std::cout << "Start time: " << time.format() << std::endl;
    
    // Add hours
    zuu::Time later = time;
    later.add_hours(2);
    std::cout << "After adding 2 hours: " << later.format() << std::endl;
    
    // Add minutes
    zuu::Time later2 = time;
    later2.add_minutes(90);
    std::cout << "After adding 90 minutes: " << later2.format() << std::endl;
    
    // Add seconds (with wrap-around)
    zuu::Time night(23, 30, 0);
    night.add_hours(2);  // Should wrap to 01:30
    std::cout << "23:30 + 2 hours = " << night.format() << std::endl;
    
    // Subtract time
    zuu::Time morning(8, 0, 0);
    morning.add_minutes(-30);  // Should wrap to 07:30
    std::cout << "08:00 - 30 minutes = " << morning.format() << std::endl;
}

// ============================================================================
// Example 5: DateTime Operations
// ============================================================================
void example_datetime_basics() {
    std::cout << "\n=== DateTime Basics ===" << std::endl;
    
    // Creating datetime
    zuu::DateTime dt1(2024, 12, 25, 14, 30, 45);
    zuu::DateTime now = zuu::DateTime::now();
    
    std::cout << "Christmas: " << dt1.format() << std::endl;
    std::cout << "Now: " << now.format() << std::endl;
    
    // ISO 8601 formats
    std::cout << "\nISO 8601 formats:" << std::endl;
    std::cout << "  Basic: " << dt1.to_iso8601() << std::endl;
    std::cout << "  Milliseconds: " << dt1.to_iso8601_ms() << std::endl;
    std::cout << "  Microseconds: " << dt1.to_iso8601_us() << std::endl;
    std::cout << "  Nanoseconds: " << dt1.to_iso8601_ns() << std::endl;
    
    // Unix timestamp
    std::cout << "\nUnix timestamps:" << std::endl;
    std::cout << "  Seconds: " << dt1.to_unix_timestamp() << std::endl;
    std::cout << "  Milliseconds: " << dt1.to_unix_timestamp_ms() << std::endl;
}

// ============================================================================
// Example 6: DateTime Arithmetic with Overflow
// ============================================================================
void example_datetime_arithmetic() {
    std::cout << "\n=== DateTime Arithmetic ===" << std::endl;
    
    zuu::DateTime dt(2024, 12, 31, 23, 30, 0);
    std::cout << "Start: " << dt.format() << std::endl;
    
    // Add hours with day overflow
    zuu::DateTime future = dt;
    future.add_hours(2);
    std::cout << "After adding 2 hours: " << future.format() << std::endl;
    
    // Add seconds with day overflow
    zuu::DateTime future2 = dt;
    future2.add_seconds(7200);  // 2 hours
    std::cout << "After adding 7200 seconds: " << future2.format() << std::endl;
    
    // Complex arithmetic
    zuu::DateTime complex(2024, 1, 31, 10, 0, 0);
    std::cout << "\nComplex arithmetic from: " << complex.format() << std::endl;
    complex.add_months(1);   // Feb 29 (leap year adjustment)
    complex.add_days(1);     // Mar 1
    complex.add_hours(25);   // +1 day, +1 hour
    std::cout << "After complex operations: " << complex.format() << std::endl;
    
    // Calculate difference
    zuu::DateTime dt1(2024, 1, 1, 0, 0, 0);
    zuu::DateTime dt2(2024, 1, 2, 12, 30, 45);
    int64_t seconds_diff = dt2.seconds_between(dt1);
    std::cout << "\nSeconds between datetimes: " << seconds_diff << std::endl;
    std::cout << "That's " << seconds_diff / 3600 << " hours and " 
              << (seconds_diff % 3600) / 60 << " minutes" << std::endl;
}

// ============================================================================
// Example 7: Advanced Formatting
// ============================================================================
void example_advanced_formatting() {
    std::cout << "\n=== Advanced Formatting ===" << std::endl;
    
    zuu::DateTime dt(2024, 7, 4, 14, 30, 45, 123456789);
    
    std::cout << "Various formats:" << std::endl;
    std::cout << "  ISO 8601:        " << dt.format("%Y-%m-%dT%H:%M:%S") << std::endl;
    std::cout << "  US format:       " << dt.format("%m/%d/%Y %H:%M:%S") << std::endl;
    std::cout << "  European:        " << dt.format("%d/%m/%Y %H:%M:%S") << std::endl;
    std::cout << "  Long format:     " << dt.format("%A, %B %d, %Y at %H:%M:%S") << std::endl;
    std::cout << "  Short format:    " << dt.format("%a, %b %d, %Y") << std::endl;
    std::cout << "  With subseconds: " << dt.format("%Y-%m-%d %H:%M:%S.%f") << std::endl;
    std::cout << "  Full precision:  " << dt.format("%Y-%m-%d %H:%M:%S.%N") << std::endl;
    std::cout << "  Day of year:     " << dt.format("Day %j of %Y") << std::endl;
    std::cout << "  Quarter:         " << dt.format("Q%q %Y") << std::endl;
}

// ============================================================================
// Example 8: Constexpr Compile-Time Operations
// ============================================================================
void example_constexpr() {
    std::cout << "\n=== Compile-Time Operations ===" << std::endl;
    
    // All these are evaluated at compile time!
    constexpr zuu::Date christmas(2024, 12, 25);
    constexpr int day_of_week = christmas.day_of_week();
    constexpr int day_of_year = christmas.day_of_year();
    constexpr bool is_leap = zuu::is_leap_year(2024);
    
    constexpr zuu::Time noon(12, 0, 0);
    constexpr int total_seconds = noon.total_seconds();
    
    constexpr zuu::DateTime dt(2024, 12, 25, 12, 0, 0);
    constexpr int year = dt.year();
    
    std::cout << "All values computed at compile-time:" << std::endl;
    std::cout << "  Christmas day of week: " << day_of_week << std::endl;
    std::cout << "  Christmas day of year: " << day_of_year << std::endl;
    std::cout << "  2024 is leap year: " << is_leap << std::endl;
    std::cout << "  Seconds at noon: " << total_seconds << std::endl;
    std::cout << "  DateTime year: " << year << std::endl;
    
    // Static assertions (compile-time checks)
    static_assert(day_of_week == 2, "Christmas 2024 is Wednesday");
    static_assert(is_leap == true, "2024 is a leap year");
    static_assert(total_seconds == 43200, "Noon is 43200 seconds");
}

// ============================================================================
// Example 9: Comparisons
// ============================================================================
void example_comparisons() {
    std::cout << "\n=== Comparisons ===" << std::endl;
    
    zuu::DateTime dt1(2024, 12, 25, 10, 0, 0);
    zuu::DateTime dt2(2024, 12, 25, 15, 0, 0);
    zuu::DateTime dt3(2024, 12, 26, 10, 0, 0);
    
    std::cout << "dt1: " << dt1.format() << std::endl;
    std::cout << "dt2: " << dt2.format() << std::endl;
    std::cout << "dt3: " << dt3.format() << std::endl;
    std::cout << std::endl;
    
    std::cout << "dt1 == dt2: " << (dt1 == dt2) << std::endl;
    std::cout << "dt1 < dt2: " << (dt1 < dt2) << std::endl;
    std::cout << "dt1 <= dt2: " << (dt1 <= dt2) << std::endl;
    std::cout << "dt2 > dt1: " << (dt2 > dt1) << std::endl;
    std::cout << "dt3 > dt2: " << (dt3 > dt2) << std::endl;
}

// ============================================================================
// Example 10: Real-World Use Cases
// ============================================================================
void example_realworld() {
    std::cout << "\n=== Real-World Use Cases ===" << std::endl;
    
    // Birthday countdown
    zuu::Date today = zuu::Date::today();
    zuu::Date birthday(2025, 1, 15);
    int days_until = birthday.days_between(today);
    std::cout << "Days until birthday: " << days_until << std::endl;
    
    // Meeting scheduler (avoid weekends)
    zuu::Date meeting(2024, 12, 27);  // Friday
    if (meeting.is_weekend()) {
        meeting.add_days(2);
    }
    std::cout << "Meeting scheduled for: " << meeting.format("%A, %B %d") << std::endl;
    
    // Shift scheduler
    zuu::DateTime shift_start(2024, 12, 25, 22, 0, 0);
    zuu::DateTime shift_end = shift_start;
    shift_end.add_hours(8);
    std::cout << "Shift: " << shift_start.format("%H:%M") 
              << " to " << shift_end.format("%H:%M") 
              << " (" << shift_end.format("%Y-%m-%d") << ")" << std::endl;
    
    // Time tracking
    zuu::DateTime clock_in(2024, 12, 25, 9, 0, 0);
    zuu::DateTime clock_out(2024, 12, 25, 17, 30, 0);
    int64_t work_seconds = clock_out.seconds_between(clock_in);
    int hours = work_seconds / 3600;
    int minutes = (work_seconds % 3600) / 60;
    std::cout << "Work duration: " << hours << " hours " << minutes << " minutes" << std::endl;
}

// ============================================================================
// Main
// ============================================================================
int main() {
    std::cout << "DateTime Library v" << zuu::version() << " - Examples\n";
    std::cout << "=============================================" << std::endl;
    
    try {
        example_date_basics();
        example_date_arithmetic();
        example_time_basics();
        example_time_arithmetic();
        example_datetime_basics();
        example_datetime_arithmetic();
        example_advanced_formatting();
        example_constexpr();
        example_comparisons();
        example_realworld();
        
        std::cout << "\n=============================================" << std::endl;
        std::cout << "All examples completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
