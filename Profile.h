#pragma once

#if !defined PROFILE_LEVEL_ON && !defined PROFILE_LEVEL_DEEP && !defined PROFILE_LEVEL_OFF
#error "Must define a profiling level for this header. See ProfileLevels.h"
#endif


#if defined PROFILE_LEVEL_ON || defined PROFILE_LEVEL_DEEP
#define PROFILE_SCOPE(name) Timing::InstrumentationData instrumentationData##__LINE__(name)
#define PROFILE_FN Timing::InstrumentationData instrumentationData##__LINE__(__FUNCSIG__)
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FN
#endif

#if defined PROFILE_LEVEL_DEEP
#define PROFILE_SCOPE_DEEP(name) Timing::InstrumentationData instrumentationData##__LINE__(name)
#define PROFILE_FN_DEEP Timing::InstrumentationData instrumentationData##__LINE__(__FUNCSIG__)
#else
#define PROFILE_SCOPE_DEEP(name)
#define PROFILE_FN_DEEP
#endif









