/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

/** @file
 *
 * @defgroup TWESER ヘッダファイル
 *
 * JN51XX 用シリアル処理
 */

#ifndef TWESERIAL_JEN_H_
#define TWESERIAL_JEN_H_

#ifdef JENNIC_CHIP

#include "twecommon.h"
#include "tweserial.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 初期化関数 */
void TWETERM_vInitJen(TWE_tsFILE *fp, uint8 u8Port, TWETERM_tsSerDefs *pser); // UART初期化関数 for JN51XX


#ifdef __cplusplus
}
#endif

#endif // JENNIC_CHIP
#endif // TWESERIAL_JEN_H_
