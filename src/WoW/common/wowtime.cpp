#include "WoW/common/wowtime.h"

#include <cstdio>
#include <ctime>
#include <limits>

namespace {

constexpr const char* kWeekDays[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
};

template <typename... Args>
void Format(char* destination, std::size_t capacity, const char* format, Args... args) {
    if (destination == nullptr || capacity == 0) {
        return;
    }
    std::snprintf(destination, capacity, format, args...);
}

} // namespace

// These two names are compiler-generated initialization helpers in the
// recovered object.  Keep them as real symbols because other recovered units
// refer to the original startup thunk names.
float INFINITY_300 = 0.0F;
unsigned int INFINITY_ENCODING_292 = 0x7F800000U;

int _E1_300() {
    INFINITY_300 = std::numeric_limits<float>::infinity();
    return static_cast<int>(INFINITY_ENCODING_292);
}

int _E2_300() {
    return _E1_300();
}

WowTime::WowTime()
    : m_minute(-1),
      m_hour(-1),
      m_weekday(-1),
      m_monthDay(-1),
      m_month(-1),
      m_year(-1),
      m_flags(0) {}

int WowTime::GetDaysSinceEpoch() const {
    if (m_year < 0 || m_month < 0 || m_monthDay < 0) {
        return 0;
    }

    std::tm date{};
    date.tm_year = m_year + 100;
    date.tm_mon = m_month;
    date.tm_mday = m_monthDay + 1;
    date.tm_isdst = -1;
    const std::time_t seconds = std::mktime(&date);
    if (seconds == static_cast<std::time_t>(-1)) {
        return -1;
    }
    return static_cast<int>(seconds / 86400);
}

void WowTime::SetDaysSinceEpoch(int days) {
    // The original x86 code performs this arithmetic as a signed 32-bit
    // operation before passing it to localtime.
    const std::int32_t seconds32 = static_cast<std::int32_t>(
        static_cast<std::int64_t>(days) * 86400 + 43200);
    const std::time_t seconds = static_cast<std::time_t>(seconds32);
    const std::tm* date = std::localtime(&seconds);
    if (date != nullptr) {
        m_year = date->tm_year - 100;
        m_month = date->tm_mon;
        m_monthDay = date->tm_mday - 1;
        m_weekday = date->tm_wday;
    }
}

int WowTime::GetHourAndMinutes() const {
    if (m_hour < 0 || m_minute < 0) {
        return 0;
    }
    return m_minute + 60 * m_hour;
}

void WowTime::SetHourAndMinutes(int minutes) {
    m_hour = minutes / 60;
    m_minute = minutes % 60;
}

int WowTime::CompareYear(const WowTime& compareTime) const {
    if (m_year > compareTime.m_year) return 1;
    if (m_year < compareTime.m_year) return -1;
    return 0;
}

int WowTime::CompareMonth(const WowTime& compareTime) const {
    if (m_month > compareTime.m_month) return 1;
    if (m_month < compareTime.m_month) return -1;
    return 0;
}

int WowTime::CompareDay(const WowTime& compareTime) const {
    if (m_monthDay > compareTime.m_monthDay) return 1;
    if (m_monthDay < compareTime.m_monthDay) return -1;
    return 0;
}

int WowTime::CompareWeekday(const WowTime& compareTime) const {
    if (m_weekday > compareTime.m_weekday) return 1;
    if (m_weekday < compareTime.m_weekday) return -1;
    return 0;
}

int WowTime::CompareHour(const WowTime& compareTime) const {
    if (m_hour > compareTime.m_hour) return 1;
    if (m_hour < compareTime.m_hour) return -1;
    return 0;
}

int WowTime::CompareMinute(const WowTime& compareTime) const {
    if (m_minute > compareTime.m_minute) return 1;
    if (m_minute < compareTime.m_minute) return -1;
    return 0;
}

bool WowTime::InRange(const WowTime& valMin, const WowTime& valMax) const {
    if (valMin <= valMax) {
        return *this >= valMin && *this < valMax;
    }
    return *this >= valMin || *this < valMax;
}

bool WowTime::operator<(const WowTime& compareTime) const {
    if (&compareTime == this) return false;
    if (compareTime.m_year >= 0 && m_year >= 0) {
        const int result = CompareYear(compareTime);
        if (result != 0) return result < 0;
    }
    if (compareTime.m_month >= 0 && m_month >= 0) {
        const int result = CompareMonth(compareTime);
        if (result != 0) return result < 0;
    }
    if (compareTime.m_monthDay >= 0 && m_monthDay >= 0) {
        const int result = CompareDay(compareTime);
        if (result != 0) return result < 0;
    }
    if (compareTime.m_weekday >= 0 && m_weekday >= 0) {
        const int result = CompareWeekday(compareTime);
        if (result != 0) return result < 0;
    }
    if (compareTime.m_hour >= 0 && m_hour >= 0) {
        const int result = CompareHour(compareTime);
        if (result != 0) return result < 0;
    }
    if (compareTime.m_minute < 0 || m_minute < 0) return false;
    const int result = CompareMinute(compareTime);
    return result != 0 && result < 0;
}

bool WowTime::operator<=(const WowTime& compareTime) const {
    return *this == compareTime || *this < compareTime;
}

bool WowTime::operator>(const WowTime& compareTime) const {
    if (&compareTime == this) return false;
    if (compareTime.m_year >= 0 && m_year >= 0) {
        const int result = CompareYear(compareTime);
        if (result != 0) return result > 0;
    }
    if (compareTime.m_month >= 0 && m_month >= 0) {
        const int result = CompareMonth(compareTime);
        if (result != 0) return result > 0;
    }
    if (compareTime.m_monthDay >= 0 && m_monthDay >= 0) {
        const int result = CompareDay(compareTime);
        if (result != 0) return result > 0;
    }
    if (compareTime.m_weekday >= 0 && m_weekday >= 0) {
        const int result = CompareWeekday(compareTime);
        if (result != 0) return result > 0;
    }
    if (compareTime.m_hour >= 0 && m_hour >= 0) {
        const int result = CompareHour(compareTime);
        if (result != 0) return result > 0;
    }
    if (compareTime.m_minute < 0 || m_minute < 0) return false;
    const int result = CompareMinute(compareTime);
    return result != 0 && result > 0;
}

bool WowTime::operator>=(const WowTime& compareTime) const {
    return *this == compareTime || *this > compareTime;
}

bool WowTime::operator==(const WowTime& compareTime) const {
    // The original treats non-positive fields in the right-hand value as
    // wildcards, and only compares a left-hand field when it is positive.
    if (compareTime.m_year > 0 && m_year > 0 && compareTime.m_year != m_year) return false;
    if (compareTime.m_month > 0 && m_month > 0 && compareTime.m_month != m_month) return false;
    if (compareTime.m_monthDay > 0 && m_monthDay > 0 && compareTime.m_monthDay != m_monthDay) return false;
    if (compareTime.m_weekday > 0 && m_weekday > 0 && compareTime.m_weekday != m_weekday) return false;
    if (compareTime.m_hour > 0 && m_hour > 0 && compareTime.m_hour != m_hour) return false;
    return compareTime.m_minute <= 0 || m_minute <= 0 || compareTime.m_minute == m_minute;
}

bool WowTime::operator!=(const WowTime& compareTime) const {
    return !(*this == compareTime);
}

void __fastcall WowTime::WowEncodeTime(std::uint32_t& minute, int minuteValue, int hour,
                            int weekday, int monthday, int month, int year,
                            int flags) {
    // The recovered binary reports invalid ranges through SErr and continues;
    // masking below intentionally preserves the resulting packed value.
    minute = (static_cast<std::uint32_t>(minuteValue) & 0x3FU)
           | ((static_cast<std::uint32_t>(hour) & 0x1FU) << 6)
           | ((static_cast<std::uint32_t>(weekday) & 0x07U) << 11)
           | ((static_cast<std::uint32_t>(monthday) & 0x3FU) << 14)
           | ((static_cast<std::uint32_t>(month) & 0x0FU) << 20)
           | ((static_cast<std::uint32_t>(year) & 0x1FU) << 24)
           | ((static_cast<std::uint32_t>(flags) & 0x03U) << 29);
}

void __fastcall WowTime::WowDecodeTime(std::uint32_t minute, int* minuteValue, int* hour,
                            int* weekday, int* monthday, int* month, int* year,
                            int* flags) {
    if (minuteValue != nullptr) {
        const unsigned int value = minute & 0x3FU;
        *minuteValue = value == 63U ? -1 : static_cast<int>(value);
    }
    if (hour != nullptr) {
        const unsigned int value = (minute >> 6) & 0x1FU;
        *hour = value == 31U ? -1 : static_cast<int>(value);
    }
    if (weekday != nullptr) {
        const unsigned int value = (minute >> 11) & 0x07U;
        *weekday = value == 7U ? -1 : static_cast<int>(value);
    }
    if (monthday != nullptr) {
        const unsigned int value = (minute >> 14) & 0x3FU;
        *monthday = value == 63U ? -1 : static_cast<int>(value);
    }
    if (month != nullptr) {
        const unsigned int value = (minute >> 20) & 0x0FU;
        *month = value == 15U ? -1 : static_cast<int>(value);
    }
    if (year != nullptr) {
        const unsigned int value = (minute >> 24) & 0x1FU;
        *year = value == 31U ? -1 : static_cast<int>(value);
    }
    if (flags != nullptr) {
        const unsigned int value = (minute >> 29) & 0x03U;
        *flags = value == 3U ? -1 : static_cast<int>(value);
    }
}

void __fastcall WowTime::WowEncodeTime(std::uint32_t& minute, const WowTime& time) {
    WowEncodeTime(minute, time.m_minute, time.m_hour, time.m_weekday,
                  time.m_monthDay, time.m_month, time.m_year, time.m_flags);
}

void __fastcall WowTime::WowDecodeTime(std::uint32_t minute, WowTime* time) {
    if (time == nullptr) return;
    WowDecodeTime(minute, &time->m_minute, &time->m_hour, &time->m_weekday,
                  &time->m_monthDay, &time->m_month, &time->m_year, &time->m_flags);
}

const char* __fastcall WowTime::WowGetTimeString(std::uint32_t minute, char* string, int maxlen) {
    WowTime time;
    WowDecodeTime(minute, &time);
    return WowGetTimeString(&time, string, maxlen);
}

const char* __fastcall WowTime::WowGetTimeString(WowTime* time, char* string, int maxlen) {
    std::uint32_t encoded = 0;
    WowEncodeTime(encoded, *time);
    if (encoded == 0) {
        Format(string, maxlen > 0 ? static_cast<std::size_t>(maxlen) : 0,
               "%s", "Not Set");
        return string;
    }

    char buffMonth[8]{};
    char buffMonthDay[8]{};
    char buffYear[8]{};
    char buffWeekDay[8]{};
    char buffHour[8]{};
    char buffMinute[8]{};

    if (time->m_year >= 0) Format(buffYear, sizeof(buffYear), "%i", time->m_year + 2000);
    else Format(buffYear, sizeof(buffYear), "%s", "A");
    if (time->m_month >= 0) Format(buffMonth, sizeof(buffMonth), "%i", time->m_month + 1);
    else Format(buffMonth, sizeof(buffMonth), "%s", "A");
    if (time->m_monthDay >= 0) Format(buffMonthDay, sizeof(buffMonthDay), "%i", time->m_monthDay + 1);
    else Format(buffMonthDay, sizeof(buffMonthDay), "%s", "A");
    if (time->m_weekday >= 0 && time->m_weekday < 7) Format(buffWeekDay, sizeof(buffWeekDay), "%s", kWeekDays[time->m_weekday]);
    else Format(buffWeekDay, sizeof(buffWeekDay), "%s", "Any");
    if (time->m_hour >= 0) Format(buffHour, sizeof(buffHour), "%i", time->m_hour);
    else Format(buffHour, sizeof(buffHour), "%s", "A");
    if (time->m_minute >= 0) Format(buffMinute, sizeof(buffMinute), "%2.2i", time->m_minute);
    else Format(buffMinute, sizeof(buffMinute), "%s", "A");

    Format(string, maxlen > 0 ? static_cast<std::size_t>(maxlen) : 0,
           "%s/%s/%s (%s) %s:%s", buffMonth, buffMonthDay, buffYear,
           buffWeekDay, buffHour, buffMinute);
    return string;
}
