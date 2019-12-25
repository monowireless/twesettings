/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

#if defined(ARDUINO) && defined(ESP32)

#include "twecommon.h"
#include "esp32_eep.h"

#include <M5Stack.h>
#include <EEPROM.h>

static bool_t s_bInit = FALSE;

/*!
 * EEPROM 初期関数
 */
void EEP_6x_vInit() {
	EEPROM.begin(EEPROM_6X_USER_SIZE);

	s_bInit = TRUE;
}


/*!
 * EEPROM 読み出し関数 (MSC EEPROM シミュレート用)
 * 
 * \param u16StartAddr
 * \param u16Bytes
 * \param pu8Buffer
 * \return 
 */
bool_t EEP_6x_bRead(uint16 u16StartAddr, uint16 u16Bytes, uint8 *pu8Buffer) {
	bool_t bRet = TRUE;
	
	int i;
	for (i = 0; i < u16Bytes; i++) {
		pu8Buffer[i] = EEPROM.read(u16StartAddr + i);
	}

	return bRet;
}

/*!
 * EEPROM 書き込み関数 (MSC EEPROM シミュレート用)
 * 
 * \param u16StartAddr
 * \param u16Bytes
 * \param pu8Buffer
 * \return 
 */
bool_t EEP_6x_bWrite(uint16 u16StartAddr, uint16 u16Bytes, uint8 *pu8Buffer) {
	bool_t bRet = TRUE;

	int i;
	for (i = 0; i < u16Bytes; i++) {
		EEPROM.write(u16StartAddr + i, pu8Buffer[i]);
	}

	return EEPROM.commit() ? TRUE : FALSE;
}

#endif