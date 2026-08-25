#include "WoW/Source/Game/gametime.h"

#include <algorithm>
#include <chrono>
#include <ctime>

namespace {

constexpr float kDefaultMinutesPerSecond = 0.016666668F;
constexpr int kMinutesPerDay = 1440;

std::uint32_t AsyncTimeMs() {
    return static_cast<std::uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
}

int NormalizeMinutes(int minutes) {
    minutes %= kMinutesPerDay;
    if (minutes < 0) {
        minutes += kMinutesPerDay;
    }
    return minutes;
}

} // namespace

double TWO_PI_295 = 0.0;
double OO_TWO_PI_296 = 0.0;
double MAX_DUEL_DISTANCE_SQUARED_101 = 0.0;
double MAX_TRADE_DISTANCE_SQUARED_101 = 0.0;
double MAX_SHOP_DISTANCE_SQUARED_101 = 0.0;
double MAX_INSPECT_DISTANCE_SQUARED_101 = 0.0;
double MAX_LOOT_DISTANCE_SQUARED_101 = 0.0;
double MAX_BIND_DISTANCE_SQUARED_101 = 0.0;
double MAX_SITCHAIR_DISTANCE_SQUARED_101 = 0.0;
double MAX_SITCHAIRUSE_DISTANCE_SQUARED_101 = 0.0;
double MAX_SHARE_DISTANCE_SQUARED_101 = 0.0;
double SECONDS_PER_HEARTBEAT_101 = 0.0;
double SPELL_FOCUS_DISTANCE_SQUARED_100 = 0.0;
double MAX_OBJ_INTEREST_RADIUS_SQUARED_101 = 0.0;

int _E1_301() {
    return 2139095040;
}

int _E2_301() {
    return _E1_301();
}

CGameTime::CGameTime()
    : WowTime(),
      m_timeBias(0),
      m_dateBias(0),
      m_gameMinutesElapsed(0),
      m_gameMinutesPerRealSecond(kDefaultMinutesPerSecond),
      m_gameMinutesThisTick(0.0F),
      m_timeDifferential(0),
      m_lastTickMinute(AsyncTimeMs()),
      m_dayProgression(0.0) {}

CGameTime::~CGameTime() {
    Destroy();
}

void CGameTime::Destroy() {
    for (HGAMETIMECALLBACK__* callback : m_callbacks) {
        if (callback != nullptr) {
            callback->registered = false;
            delete callback;
        }
    }
    m_callbacks.clear();
}

void CGameTime::SetTimeDateBias(int timeBias, int dateBias, unsigned char update) {
    if (m_timeBias != 0) {
        WowTime.SetHourAndMinutes(NormalizeMinutes(WowTime.GetHourAndMinutes() - m_timeBias));
    }

    const int previousDateBias = m_dateBias;
    m_timeBias = timeBias;
    if (previousDateBias != 0) {
        WowTime.SetDaysSinceEpoch(WowTime.GetDaysSinceEpoch() - previousDateBias);
    }
    m_dateBias = dateBias;

    if (update != 0) {
        GameTimeSetTime(&WowTime);
    }
}

void CGameTime::GameTimeSetTime(const ::WowTime* time) {
    if (time == nullptr) {
        return;
    }

    ::WowTime biasTime = *time;
    if (m_timeBias != 0) {
        biasTime.SetHourAndMinutes(NormalizeMinutes(m_timeBias + biasTime.GetHourAndMinutes()));
    }
    if (m_dateBias != 0) {
        biasTime.SetDaysSinceEpoch(m_dateBias + biasTime.GetDaysSinceEpoch());
    }

    WowTime = biasTime;
    // The recovered implementation advances once after assigning the new
    // value.  This keeps callback and day-progression state synchronized.
    const int previousMinute = WowTime.m_minute;
    if (previousMinute != 0) {
        --WowTime.m_minute;
    } else {
        WowTime.m_minute = 59;
        WowTime.m_hour = WowTime.m_hour != 0 ? WowTime.m_hour - 1 : 23;
    }
    TickMinute();
}

void CGameTime::GameTimeUpdate(float elapsedSeconds) {
    float gameMinutes = elapsedSeconds * m_gameMinutesPerRealSecond + m_gameMinutesThisTick;
    m_gameMinutesThisTick = gameMinutes;

    if (m_timeDifferential != 0 && gameMinutes >= 1.0F) {
        const int elapsedWholeMinutes = static_cast<int>(gameMinutes);
        const int correctedMinutes = std::min(m_timeDifferential, elapsedWholeMinutes);
        m_timeDifferential -= correctedMinutes;
        m_gameMinutesThisTick = gameMinutes - static_cast<float>(correctedMinutes);
    }

    while (m_gameMinutesThisTick >= 1.0F) {
        m_gameMinutesThisTick -= 1.0F;
        TickMinute();
    }
}

void CGameTime::GameTimeSync(unsigned char reset) {
    const std::time_t now = std::time(nullptr);
    const std::tm* local = std::localtime(&now);
    if (local == nullptr) {
        return;
    }

    ::WowTime time;
    time.m_minute = local->tm_min;
    time.m_hour = local->tm_hour;
    time.m_weekday = local->tm_wday;
    time.m_monthDay = local->tm_mday - 1;
    time.m_month = local->tm_mon;
    time.m_year = local->tm_year - 100;

    if (reset != 0) {
        GameTimeSetTime(&time);
    } else {
        GameTimeSync(&time, 0);
    }
}

void CGameTime::GameTimeSync(const ::WowTime* time, unsigned char reset) {
    if (time == nullptr) {
        return;
    }

    ::WowTime biasTime = *time;
    if (m_timeBias != 0) {
        biasTime.SetHourAndMinutes(NormalizeMinutes(m_timeBias + biasTime.GetHourAndMinutes()));
    }
    if (m_dateBias != 0) {
        biasTime.SetDaysSinceEpoch(m_dateBias + biasTime.GetDaysSinceEpoch());
    }

    const int currentMinutes = WowTime.GetHourAndMinutes();
    const int delta = biasTime.GetHourAndMinutes() - currentMinutes;
    int correction = 0;
    int deferredMinutes = 0;

    if (reset != 0 || delta > 0) {
        const int forward = NormalizeMinutes(delta);
        for (int i = 0; i < forward; ++i) {
            TickMinute();
        }
    } else {
        deferredMinutes = -delta;
        correction = deferredMinutes;
    }

    WowTime = biasTime;
    if (correction != 0) {
        m_timeDifferential += correction;
        WowTime.SetHourAndMinutes(NormalizeMinutes(correction + WowTime.GetHourAndMinutes()));
    }
}

double CGameTime::GameTimeGetMinutesPerSecond() const {
    return m_gameMinutesPerRealSecond;
}

double CGameTime::GameTimeSetMinutesPerSecond(float minutesPerSecond) {
    const double previous = m_gameMinutesPerRealSecond;
    m_gameMinutesPerRealSecond = std::clamp(minutesPerSecond,
                                            kDefaultMinutesPerSecond, 60.0F);
    return previous;
}

HGAMETIMECALLBACK__* CGameTime::GameTimeRegisterCallback(
    ::WowTime* time, GameTimeCallback callback, void* user) {
    if (time == nullptr || callback == nullptr || time->m_hour < 0 || time->m_minute < 0) {
        return nullptr;
    }

    auto* entry = new HGAMETIMECALLBACK__{};
    entry->owner = this;
    entry->time = *time;
    entry->callback = callback;
    entry->userData = user;
    entry->registered = true;
    m_callbacks.push_back(entry);
    return entry;
}

void CGameTime::GameTimeUnregisterCallback(HGAMETIMECALLBACK__* callbackHandle) {
    if (callbackHandle == nullptr) {
        return;
    }

    const auto it = std::find(m_callbacks.begin(), m_callbacks.end(), callbackHandle);
    if (it != m_callbacks.end()) {
        (*it)->registered = false;
        delete *it;
        m_callbacks.erase(it);
    }
}

double CGameTime::GameTimeGetDayProgression() const {
    double progression = static_cast<double>(
        static_cast<std::uint32_t>(AsyncTimeMs() - m_lastTickMinute)) * 0.001 *
        m_gameMinutesPerRealSecond + m_dayProgression;
    while (progression > static_cast<double>(kMinutesPerDay)) {
        progression -= kMinutesPerDay;
    }
    return progression * 0.00069444446;
}

void CGameTime::TickMinute() {
    const int minutes = NormalizeMinutes(WowTime.GetHourAndMinutes() + 1);
    WowTime.SetHourAndMinutes(minutes);
    ++m_gameMinutesElapsed;
    PerformCallbacks(minutes);
    m_lastTickMinute = AsyncTimeMs();
    m_dayProgression = static_cast<double>(minutes) + m_gameMinutesThisTick;
}

void CGameTime::PerformCallbacks(int minutes) {
    // The original timestamp hash table stores callbacks by minute-of-day.
    // Copying the matching entries preserves the original ability for a
    // callback to unregister itself while callbacks are being dispatched.
    std::vector<HGAMETIMECALLBACK__*> pending;
    for (HGAMETIMECALLBACK__* entry : m_callbacks) {
        if (entry != nullptr && entry->registered &&
            entry->time.GetHourAndMinutes() == minutes) {
            pending.push_back(entry);
        }
    }
    for (HGAMETIMECALLBACK__* entry : pending) {
        if (entry->registered && entry->callback != nullptr) {
            entry->callback(&WowTime, entry->userData);
        }
    }
}

void _E4_297() { TWO_PI_295 = 3.1415927 + 3.1415927; }
int _E5_297() { _E4_297(); return 0; }
void _E7_297() { OO_TWO_PI_296 = 1.0 / TWO_PI_295; }
int _E8_297() { _E7_297(); return 0; }
void _E13_13() { MAX_DUEL_DISTANCE_SQUARED_101 = 10.0 * 10.0; }
int _E14_18() { _E13_13(); return 0; }
void _E16_25() { MAX_TRADE_DISTANCE_SQUARED_101 = 11.111111 * 11.111111; }
int _E17_20() { _E16_25(); return 0; }
void _E19_52() { MAX_SHOP_DISTANCE_SQUARED_101 = 5.5555553 * 5.5555553; }
int _E20_69() { _E19_52(); return 0; }
void _E22_60() { MAX_INSPECT_DISTANCE_SQUARED_101 = 5.5555553 * 5.5555553; }
int _E23_74() { _E22_60(); return 0; }
void _E25_61() { MAX_LOOT_DISTANCE_SQUARED_101 = 5.0 * 5.0; }
int _E26_70() { _E25_61(); return 0; }
void _E28_99() { MAX_BIND_DISTANCE_SQUARED_101 = 10.0 * 10.0; }
int _E29_70() { _E28_99(); return 0; }
void _E31_96() { MAX_SITCHAIR_DISTANCE_SQUARED_101 = 0.5 * 0.5; }
int _E32_62() { _E31_96(); return 0; }
void _E34_73() { MAX_SITCHAIRUSE_DISTANCE_SQUARED_101 = 3.0 * 3.0; }
int _E35_43() { _E34_73(); return 0; }
void _E37_68() { MAX_SHARE_DISTANCE_SQUARED_101 = 100.0 * 100.0; }
int _E38_42() { _E37_68(); return 0; }
void _E40_69() { SECONDS_PER_HEARTBEAT_101 = 5.0; }
int _E41_38() { _E40_69(); return 0; }
void _E43_67() { SPELL_FOCUS_DISTANCE_SQUARED_100 = 50.0 * 50.0; }
int _E44_40() { _E43_67(); return 0; }
void _E46_65() { MAX_OBJ_INTEREST_RADIUS_SQUARED_101 = 100.0 * 100.0; }
int _E47_41() { _E46_65(); return 0; }
