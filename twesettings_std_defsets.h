/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

 /** @file
  *
  * @defgroup TWESETTINGS_STD_DEFSETS ヘッダファイル
  * 
  * 
  * 
  */

#ifndef TWESETTINGS_STD_DEFSETS_H_
#define TWESETTINGS_STD_DEFSETS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	E_TWESTG_DEFSETS_APPID = 1,
	E_TWESTG_DEFSETS_LOGICALID,
	E_TWESTG_DEFSETS_CHANNEL,
	E_TWESTG_DEFSETS_CHANNELS_3,
	E_TWESTG_DEFSETS_POWER_N_RETRY,
	E_TWESTG_DEFSETS_OPTBITS,
	E_TWESTG_DEFSETS_UARTBAUD,
	
	E_TWESTG_DEFSETS_VOID = 0xFF,
} teTWESTG_STD_DEFSETS;

extern const uint8 TWESTG_DEFCUST_U8_SLOT_NUMBER; //! スロットの最大数 [0..TWESTG_DEFCUST_U8_SLOT_NUMBER]

extern const TWESTG_tsElement TWESTG_DEFSETS_BASE[];
extern const TWESTG_tsElement TWESTG_DEFSETS_SLOT[][2];
extern const uint8 TWESTG_DEFCUST_SLOT[][4];
extern const uint8 TWESTG_DEFCUST_REMOVE_CHAN3[];
extern const uint8 TWESTG_DEFCUST_REMOVE_CHAN1[];


#ifdef __cplusplus
}
#endif


#endif