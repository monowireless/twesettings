/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

#pragma once

/** @file
 *
 * @defgroup TWESER ヘッダファイル
 *
 * シリアル等の入出力の抽象定義を行う。
 */
#include <stdarg.h>

#include "twecommon.h"
#include "tweserial.h"
#include "../printf/printf.h"

#ifdef __cplusplus
extern "C" {
#endif

int TWE_fprintf(TWE_tsFILE *fp, const char *format, ...);
int TWE_vfprintf(TWE_tsFILE *fp, const char *format, va_list va);
#define TWE_snprintf(out,siz,fmt,...) snprintf_(out,siz,fmt,__VA_ARGS__)
// int TWE_snprintf(char *out, size_t siz, const char *format, ...);

#ifdef __cplusplus
}
#endif

