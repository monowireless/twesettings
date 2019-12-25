/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

#ifdef _MSC_VER

#include <stdio.h>
#include <string.h>
#include "twecommon.h"
#include "msc_eep.h"

static bool_t s_bInit = FALSE;
static uint8 s_au8buff[EEPROM_6X_USER_SIZE];
static const char *s_fname = "msc_eep.bin";

static bool_t s_bReadFromFile();
static bool_t s_bWriteToFile();

/*!
 * ファイルからデータを読み出す (MSC EEPROM シミュレート用)
 * ファイルがない場合は 0xFF で初期化したデータをセーブする。
 * 
 * \return TRUE:成功 FALSE:失敗
 */
static bool_t s_bReadFromFile() {
	bool_t bRet = FALSE;
	FILE *fp = NULL;
	fopen_s(&fp, s_fname, "rb");
	if (fp != NULL) {
		fread(s_au8buff, sizeof(uint8), EEPROM_6X_USER_SIZE, fp);
		fclose(fp);
	}
	else {
		memset(s_au8buff, 0xFF, EEPROM_6X_USER_SIZE);
		s_bWriteToFile();
	}

	s_bInit = TRUE;

	return bRet;
}

/*!
 * ファイルにデータを書き出す (MSC EEPROM シミュレート用)
 * 
 * \return TRUE:成功 FALSE:失敗
 */
static bool_t s_bWriteToFile() {
	bool_t bRet = FALSE;
	FILE *fp = NULL;
	fopen_s(&fp, s_fname, "w+b");
	if (fp != NULL) {
		fwrite(s_au8buff, sizeof(uint8), EEPROM_6X_USER_SIZE, fp);
		fclose(fp);
		bRet = TRUE;
	}
	return bRet;
}


/*!
 * EEPROM 読み出し関数 (MSC EEPROM シミュレート用)
 * 
 * \param u16StartAddr
 * \param u16Bytes
 * \param pu8Buffer
 * \return 
 */
bool_t EEP_6x_bRead(uint16  u16StartAddr, uint16 u16Bytes, uint8 *pu8Buffer) {
	bool_t bRet = TRUE;
	if (!s_bInit) s_bReadFromFile(); // 初期化チェック

	memcpy(pu8Buffer, s_au8buff + u16StartAddr, u16Bytes);
	
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
	if (!s_bInit) s_bReadFromFile(); // 初期化チェック

	memcpy(s_au8buff + u16StartAddr, pu8Buffer, u16Bytes);
	bRet = s_bWriteToFile();

	return bRet;
}

#endif