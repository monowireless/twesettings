/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

#ifndef TWEINTERACTIVE_DEFMENUS_H_
#define TWEINTERACTIVE_DEFMENUS_H_

#include "twecommon.h"
#include "tweinteractive.h"
#include "tweinputstring.h"

#ifdef __cplusplus
extern "C" {
#endif

void TWEINTCT_vSerUpdateScreen_defmenus(TWEINTRCT_tsContext *psIntr);
void TWEINTCT_vProcessInputByte_defmenus(TWEINTRCT_tsContext *psIntr, uint8 u8Byte);
void TWEINTCT_vProcessInputString_defmenus(TWEINTRCT_tsContext *psIntr, TWEINPSTR_tsInpStr_Context *pContext);
TWE_APIRET TWEINTCT_u32ProcessMenuEvent_defmenus(TWEINTRCT_tsContext *pContext, uint32 u32Op, uint32 u32Arg1, uint32 u32Arg2, void *vpArg);

#ifdef __cplusplus
}
#endif


#endif