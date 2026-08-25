#pragma once

// The activity identifiers are part of the client-wide instrumentation ABI.
// The recovered Scrn and Gx callers use the original numeric values, so keep
// the complete table here instead of treating ACTIVITY as an opaque counter.
enum ACTIVITY : unsigned {
    ACTIVITY_ANIMATE = 0,
    ACTIVITY_ANIMEVENTS = 1,
    ACTIVITY_ANIMSEQEND = 2,
    ACTIVITY_EVENTHANDLERS = 3,
    ACTIVITY_EVENTIDLE = 4,
    ACTIVITY_EVENTKEYANDCHAR = 5,
    ACTIVITY_EVENTMOUSE = 6,
    ACTIVITY_EVENTNET = 7,
    ACTIVITY_EVENTPAINT = 8,
    ACTIVITY_EVENTPOLL = 9,
    ACTIVITY_FRAMEMANAGER = 10,
    ACTIVITY_OBJMGR = 11,
    ACTIVITY_LIGHTING = 12,
    ACTIVITY_MODEL = 13,
    ACTIVITY_PARTICLE = 14,
    ACTIVITY_RENDER = 15,
    ACTIVITY_SOUND = 16,
    ACTIVITY_WORLD = 17,
    ACTIVITY_PERFDISPLAY = 18,
    ACTIVITY_PROG0 = 19,
    ACTIVITY_PROG1 = 20,
    ACTIVITY_PROG2 = 21,
    ACTIVITY_PROG3 = 22,
    ACTIVITY_OTHER = 23,
    ACTIVITY_COUNT = 24,
    ACTIVITIES = ACTIVITY_COUNT,
    ACTIVITY_DEFAULT = ACTIVITY_OTHER
};
void ActivityBegin(ACTIVITY activity);
void ActivityEnd(ACTIVITY activity);
double ActivityGetTimePercent(ACTIVITY activity);
unsigned ActivityGetCalls(ACTIVITY activity);
double ActivityGetTime(ACTIVITY activity);
void ActivitySuspend();
void ActivityResume();
void ActivityResetTimes();
