/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */
#pragma once

#ifdef _MSC_VER

// MSCデバッグ目的：TWENET用EEPROM APIのダミー

#include "twecommon.h"

// just simulate eeprom on JN51XX.
#define EEPROM_6X_SEGMENT_SIZE  (64)
#define EEPROM_6X_USER_SEGMENTS (60)

 /* Values derived from configurable parameters */
#define EEPROM_6X_USER_SIZE      (EEPROM_6X_SEGMENT_SIZE * EEPROM_6X_USER_SEGMENTS)

bool_t EEP_6x_bRead(uint16  u16StartAddr, uint16 u16Bytes, uint8 *pu8Buffer);
bool_t EEP_6x_bWrite(uint16 u16StartAddr, uint16 u16Bytes, uint8 *pu8Buffer);

#endif