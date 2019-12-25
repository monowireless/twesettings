/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */
#pragma once

#if defined(_MSC_VER) // || defined(ARDUINO)

#include "twecommon.h"

#ifdef __cplusplus
extern "C" {
#endif

	extern volatile uint32 u32TickCount_ms;

#ifdef __cplusplus
}
#endif

#endif // _MSC_VER