/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

 /** @file
  *
  * @defgroup TWESTGS ヘッダファイル
  *
  */

#ifndef TWESETTINGS_VLD_H_
#define TWESETTINGS_VLD_H_

#include "twecommon.h"
#include "twesettings.h"

#ifdef __cplusplus
extern "C" {
#endif


#define TWESTGS_VLD_OP_VALIDATE 0x01
#define TWESTGS_VLD_OP_CUSTDISP 0x02

#define TWESTGS_VLD_MAX_CUSTOMSTR_BUFF 32 //! 終端文字列を含む

TWE_APIRET TWESTGS_VLD_u32MinMax(struct _TWESTG_sElement* pMe, TWESTG_tuDatum* pDatum, uint16 u16OpId, TWE_tsBuffer *pBuf);
TWE_APIRET TWESTGS_VLD_u32AppId(struct _TWESTG_sElement* pMe, TWESTG_tuDatum* pDatum, uint16 u16OpId, TWE_tsBuffer* pBuf);
TWE_APIRET TWESTGS_VLD_u32ChList(struct _TWESTG_sElement* pMe, TWESTG_tuDatum* pDatum, uint16 u16OpId, TWE_tsBuffer* pBuf);
TWE_APIRET TWESTGS_VLD_u32UartOpt(struct _TWESTG_sElement* pMe, TWESTG_tuDatum* pDatum, uint16 u16OpId, TWE_tsBuffer* pBuf);
TWE_APIRET TWESTGS_VLD_u32UartBaudOpt(struct _TWESTG_sElement* pMe, TWESTG_tuDatum* pDatum, uint16 u16OpId, TWE_tsBuffer* pBuf);


#ifdef __cplusplus
}
#endif



#endif