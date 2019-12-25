/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

#include <string.h>
#include "twecommon.h"
#include "twesettings.h"
#include "twesettings_std.h"
#include "twesettings_validator.h"

#include "tweinteractive.h"

/*!
 * インタラクティブモードで画面クリアと設定の全表示を行う
 */
void TWEINTCT_vSerUpdateScreen_defmenus(TWEINTRCT_tsContext *psIntr) {
	TWEINTRCT_vSerHeadLine(psIntr, 0UL);

	int i = 1;
	while(psIntr->pFuncs[i].u8MenuId != 0xFF) {
		TWE_fprintf(psIntr->pStream, " %d: %s"_TWELB, i, psIntr->pFuncs[i].pu8MenuString);
		TWE_fflush(psIntr->pStream);
		i++;
	}
	TWEINTRCT_vSerFootLine(psIntr, 0); // フッター行の表示
	TWE_fflush(psIntr->pStream);
}

/*!
 * インタラクティブモードでの１バイトコマンド入力を処理する。
 * 
 * \param u8Byte 入力バイト
 */
void TWEINTCT_vProcessInputByte_defmenus(TWEINTRCT_tsContext *psIntr, uint8 u8Byte) {
	bool_t bInhibitUpdate = TRUE;
	bool_t bHandled = FALSE;

	psIntr->u16HoldUpdateScreen = 0;

	TWE_APIRET apiRet;

	switch (u8Byte) {
	case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7': // DIRECT SLOT SELECTION
		{
			uint8 u8n = u8Byte - '0';

			if(u8n > 0 && u8n < psIntr->u8screen_max) {
				TWEINTRCT_u32MenuChange(psIntr, u8n);
				bHandled = TRUE;
			}
		}
		break;
	
	default:
		apiRet = TWEINTRCT_u32MenuOpKey(psIntr, u8Byte);
		if (TWE_APIRET_IS_SUCCESS(apiRet)) {
			bHandled = TRUE;
		}
		break;
	}

	// 画面を更新する
	if (bHandled) {
		if (psIntr->u16HoldUpdateScreen == 0) psIntr->u16HoldUpdateScreen = 5;
		bInhibitUpdate = FALSE;
	}

	// 処理されなかったキーはコールバックで処理
	//int i = TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_UNHANDLED_CHAR, u8Byte, u8lastbyte, NULL);
	//if (i) bHandled = TRUE;

	// 処理されない場合
	if (!bHandled) {
		psIntr->u16HoldUpdateScreen = 5;
		bInhibitUpdate = FALSE;
	}

	// 画面更新しない
	if (bInhibitUpdate) {
		psIntr->u16HoldUpdateScreen = 0;
	}
}

/*!
 * 入力処理(TWEINPSTR)完了時の処理
 * 
 * 入力された文字列をバリデータ(TWESTGS_VLD_*)に渡し、要素データ(tuDatum)への変換と
 * データのチェックを行う。
 * 
 * \param pContext 入力処理(TWEINPSTR)
 */
void TWEINTCT_vProcessInputString_defmenus(TWEINTRCT_tsContext *psIntr, TWEINPSTR_tsInpStr_Context *pContext) {
	// 結果出力
	TWE_fputs(_TWELB, psIntr->pStream);

	// 一定時間待って画面を再描画
	psIntr->u16HoldUpdateScreen = 96; // 約1.5sec
}

/**
 * @brief ルートメニュー
 * 
 * @param pContext 
 * @param u32Op 
 * @param u32Arg1 
 * @param u32Arg2 
 * @param vpArg 
 * @return TWE_APIRET 
 */
TWE_APIRET TWEINTCT_u32ProcessMenuEvent_defmenus(TWEINTRCT_tsContext *psIntr, uint32 u32Op, uint32 u32Arg1, uint32 u32Arg2, void *vpArg) {
	TWE_APIRET ret = TWE_APIRET_FAIL;

	switch (u32Op) {
		case E_TWEINTCT_MENU_EV_LOAD:
			TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINTCT_MENU_EV_LOAD, psIntr->u8screen, 0, NULL); // メニューへの遷移（初期化）をアプリケーションに伝える
			ret = TWE_APIRET_SUCCESS;
		break;

		default:
		break;

	}

	return ret;
}