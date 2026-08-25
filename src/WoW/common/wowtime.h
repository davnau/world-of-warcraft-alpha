#pragma once

#include <cstdint>

// Recovered from WoW/Common/WowTime.cpp and its x86 disassembly.
// The fields are intentionally public: the original type is a plain value
// object used directly in network and game-time records.
struct WowTime {
    int m_minute;
    int m_hour;
    int m_weekday;
    int m_monthDay;
    int m_month;
    int m_year;
    int m_flags;

    WowTime();

    int GetDaysSinceEpoch() const;
    void SetDaysSinceEpoch(int days);
    int GetHourAndMinutes() const;
    void SetHourAndMinutes(int minutes);

    int CompareYear(const WowTime& compareTime) const;
    int CompareMonth(const WowTime& compareTime) const;
    int CompareDay(const WowTime& compareTime) const;
    int CompareWeekday(const WowTime& compareTime) const;
    int CompareHour(const WowTime& compareTime) const;
    int CompareMinute(const WowTime& compareTime) const;
    bool InRange(const WowTime& valMin, const WowTime& valMax) const;

    bool operator<(const WowTime& compareTime) const;
    bool operator<=(const WowTime& compareTime) const;
    bool operator>(const WowTime& compareTime) const;
    bool operator>=(const WowTime& compareTime) const;
    bool operator==(const WowTime& compareTime) const;
    bool operator!=(const WowTime& compareTime) const;

    static void __fastcall WowEncodeTime(std::uint32_t& minute, int minuteValue, int hour,
                              int weekday, int monthday, int month, int year,
                              int flags);
    static void __fastcall WowDecodeTime(std::uint32_t minute, int* minuteValue, int* hour,
                              int* weekday, int* monthday, int* month, int* year,
                              int* flags);
    static void __fastcall WowEncodeTime(std::uint32_t& minute, const WowTime& time);
    static void __fastcall WowDecodeTime(std::uint32_t minute, WowTime* time);

    static const char* __fastcall WowGetTimeString(WowTime* time, char* string, int maxlen);
    static const char* __fastcall WowGetTimeString(std::uint32_t minute, char* string, int maxlen);
};

static_assert(sizeof(WowTime) == 28, "WowTime layout must remain PDB-compatible.");
