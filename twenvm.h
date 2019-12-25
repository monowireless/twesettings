/* Copyright (C) 2017 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

/** @file
 *
 * @defgroup EEPROM EEPROMメモリの読み書き関数群
 * EEPROM への読み書き関数
 */

#ifndef TWENVM_H_
#define TWENVM_H_

#if defined(JENNIC_CHIP_NAME)
#include <jendefs.h>
#include <eeprom_6x.h>
#include <jendefs.h>
#include <AppHardwareApi.h>
#elif defined(_MSC_VER)
#include "msc_eep.h"
#endif
#include "twecommon.h"

#ifdef __cplusplus
extern "C" {
#endif

bool_t TWENVM_bRead(TWE_tsBuffer *pBuff, uint8 u8sector);
bool_t TWENVM_bWrite(TWE_tsBuffer *pBuff, uint8 u8sector);
bool_t TWENVM_bErase(uint8 u8sector);
bool_t TWENVM_bReadSectorRaw(uint8 *u8buff, uint8 u8sector, uint8 u8len);


#ifdef __cplusplus
}
#endif


#endif /* TWENVM_H_ */
