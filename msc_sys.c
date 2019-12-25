/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

#if defined(_MSC_VER) //|| defined(ARDUINO)

#include "twecommon.h"
volatile uint32 u32TickCount_ms = 0; //! for TWENET compat code.

#endif // _MSC_VER