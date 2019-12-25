/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

#include <string.h>
#include "twecommon.h"
#include "twesettings.h"
#include "twesettings_std.h"
#include "twesettings_validator.h"

#include "tweinteractive.h"

// if debugging, ...
#define DEBUGMSG
// #define DEBUGMSG(...) TWE_fprintf(psIntr->pStream, __VA_ARGS__) 

/*!
 * インタラクティブモードで画面クリアと設定の全表示を行う
 */
void TWEINTCT_vSerUpdateScreen_settings(TWEINTRCT_tsContext *psIntr) {
    TWEINTRCT_vSerHeadLine(psIntr, 0x01UL);

	int i;
	for (i = 0; i < psIntr->psFinal->u8DataCount; i++) {
		uint8 u8mark = ' ';
		if (psIntr->psFinal->u8Opt & TWESTG_FINAL_OPT_REVERTED_TO_DEFAULT) u8mark = '*';
		else if (psIntr->psFinal->asDatum[i].u8Stat & TWESTG_DATASTAT_MODIFIED_MASK) u8mark = '*';
		else if (psIntr->psFinal->asDatum[i].u8Stat & TWESTG_DATASTAT_SAVED_MASK) u8mark = '$';
		else if (psIntr->psFinal->asDatum[i].u8Stat & TWESTG_DATASTAT_SAVED_HIGHER_MASK) u8mark = '^';
		
		TWE_fprintf(psIntr->pStream, " %c:%c(%3s=",
			psIntr->psFinal->apEle[i]->sFormat.u8ShortcutKey,
			u8mark,
			psIntr->psFinal->apEle[i]->sStr.strLabel
		);

		if (psIntr->psFinal->apEle[i]->sFormat.u8Format & E_TWEINPUTSTRING_DATATYPE_CUSTOM_DISP_MASK) {
			uint8 au8buf[TWESTGS_VLD_MAX_CUSTOMSTR_BUFF];
			TWE_tsBuffer sBuf;
			TWE_vInit_tsBuffer(&sBuf, au8buf, 0, TWESTGS_VLD_MAX_CUSTOMSTR_BUFF);
			
			psIntr->psFinal->apEle[i]->sValidate.fpOp(psIntr->psFinal->apEle[i], &psIntr->psFinal->asDatum[i].uDatum, TWESTGS_VLD_OP_CUSTDISP, &sBuf);
			TWE_fprintf(psIntr->pStream, "%-10s", sBuf.pu8buff);
		} else {
			uint32 u32Val = TWESTG_u32GetU32FrUDatum(psIntr->psFinal->asDatum[i].u8Type, &psIntr->psFinal->asDatum[i].uDatum);

			switch (psIntr->psFinal->apEle[i]->sFormat.u8Format) {
			case E_TWEINPUTSTRING_DATATYPE_DEC:
				TWE_fprintf(psIntr->pStream, "%10d", u32Val);
				break;
			case E_TWEINPUTSTRING_DATATYPE_DEC_DECA:
				TWE_fprintf(psIntr->pStream, "%%9d0", u32Val);
				break;
			case E_TWEINPUTSTRING_DATATYPE_DEC_HECTO:
				TWE_fprintf(psIntr->pStream, "%8d00", u32Val);
				break;
			case E_TWEINPUTSTRING_DATATYPE_HEX: 
				_TWESC(TRUE) {
					uint8 fmt[16];
					uint8 len = psIntr->psFinal->asDatum[i].u8Len * 2;
					if (len > 8) len = 8;
					uint8 pad = 8 - len;

					TWE_snprintf((char*)fmt, 16, "%%%ds0x%%0%dX", pad, len);
					TWE_fprintf(psIntr->pStream, (const char*)fmt, "", u32Val);
				}
				break;
			default:
				// エラー処理は省略
				break;
			}
		}
#if 0 // DEBUGMSG // TODO DEBUG
		TWE_fprintf(psIntr->pStream, "<%02X>",
			psIntr->psFinal->asDatum[i].u8Stat,
			psIntr->psFinal->apEle[i]->sStr.strLabel
		); // デバッグ表示
#endif

		TWE_fprintf(psIntr->pStream, ") %s"_TWELB, psIntr->psFinal->apEle[i]->sStr.strName);
		TWE_fflush(psIntr->pStream);
	}
	TWE_fputs("   */modified $/saved ^/slot0 saved"_TWELB, psIntr->pStream);
	
	if (psIntr->psFinal->u8Opt & TWESTG_FINAL_OPT_SAVED) {
		TWE_fputs("!!! Saved, Please RESET to take effect."_TWELB, psIntr->pStream);
	}
	if (psIntr->psFinal->u8Opt & TWESTG_FINAL_OPT_REVERTED_TO_DEFAULT) {
		TWE_fputs("!!! Reverted, Please SAVE and RESET to take effect."_TWELB, psIntr->pStream);
	}

	TWE_fputs(_TWELB"- [S]:Save settings [R]:Revert settings"_TWELB, psIntr->pStream);

    TWEINTRCT_vSerFootLine(psIntr, 0); // フッター行の表示


	TWE_fflush(psIntr->pStream);
}

/*!
 * インタラクティブモードでの１バイトコマンド入力を処理する。
 * 
 * \param u8Byte 入力バイト
 */
void TWEINTCT_vProcessInputByte_settings(TWEINTRCT_tsContext *psIntr, uint8 u8Byte) {
	//static uint8 u8lastbyte = 0xFF;
	bool_t bInhibitUpdate = TRUE;
	bool_t bHandled = FALSE;
    TWE_APIRET apiRet;
	bool_t bChangeKindSlot = FALSE;

	if (u8Byte == _TWELF) return; // LF は無視
	
	psIntr->u16HoldUpdateScreen = 0;

	switch (u8Byte) {
	//case _TWELF: // LF
    case _TWECR: // CR
	case _TWECLR: // Ctrl+L
		// 画面の書き換え
		bHandled = TRUE;
		break;

    case _TWEBS:
		TWEINTRCT_u32MenuChange(psIntr, 0); // back to root menu
		bHandled = TRUE;
        break;

	case 'S': // Save
		TWE_fprintf(psIntr->pStream, _TWELB"!!! Settings SAVED. !!!"_TWELB"Note: Please RESET to take effects.");
		
		TWESTG_u32SaveDataToAppStrg(psIntr->psFinal, psIntr->psFinal->u8Kind, psIntr->psFinal->u8Slot, psIntr->psFinal->u32AppId, psIntr->psFinal->sVer.u8Set, 0UL);

		psIntr->psFinal->u8Opt |= TWESTG_FINAL_OPT_SAVED; // SAVE フラグ
		if (psIntr->psFinal->u8Opt & TWESTG_FINAL_OPT_REVERTED_TO_DEFAULT) {
			psIntr->psFinal->u8Opt &= ~TWESTG_FINAL_OPT_REVERTED_TO_DEFAULT;
		}

		// TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_WAIT, 3000, 0, NULL);
		psIntr->u16HoldUpdateScreen = 96;
		
		bHandled = TRUE;
		break;

	case 'R': // Revert Settings
		_TWESC(TRUE) {
			// bool_t bRevertAll = FALSE;
			
#if 0
			// R により Save 済みと、クリアをトグルする
			if (u8lastbyte == 'R') {
				bRevertAll = TRUE;
				u8Byte = 0xFF;
				TWE_fprintf(psIntr->pStream, _TWELB"*** REVERT to DEFAULT settings. ***" _TWELB"Note: Please SAVE and RESET to take effects.");
			}
			else {
				TWE_fprintf(psIntr->pStream, _TWELB"*** REVERT to SAVED settings. ***");
			}
			TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_REVERT, bRevertAll, 0, NULL);
			TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_WAIT, 3000, 0, NULL);
#endif
			TWE_fprintf(psIntr->pStream, _TWELB"!!! REVERT Settins. !!!"_TWELB"Note: Please RESET to take effects.");
			TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_REVERT, TRUE, 0, NULL);
			psIntr->psFinal->u8Opt |= TWESTG_FINAL_OPT_REVERTED_TO_DEFAULT; // REVERT フラグ

			psIntr->u16HoldUpdateScreen = 96;

			bHandled = TRUE;
		}
		break;

	case '>': // NEXT SLOT (TODO)
		apiRet = TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_CHANGE_KIND_SLOT, 0xFF, 0x0100, NULL);
		if (TWE_APIRET_IS_SUCCESS(apiRet)) bChangeKindSlot = TRUE;
		bHandled = TRUE;
		break;

	case '<': // PREV SLOT (TODO)
		apiRet = TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_CHANGE_KIND_SLOT, 0xFF, 0xFF00, NULL);
		if (TWE_APIRET_IS_SUCCESS(apiRet)) bChangeKindSlot = TRUE;
		bHandled = TRUE;
		break;

	case ')': // NEXT KIND (TODO)
		apiRet = TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_CHANGE_KIND_SLOT, 0x0100, 0xFF, NULL);
		if (TWE_APIRET_IS_SUCCESS(apiRet)) bChangeKindSlot = TRUE;
		bHandled = TRUE;
		break;

	case '(': // PREV KIND (TODO)
		apiRet = TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_CHANGE_KIND_SLOT, 0xFF00, 0xFF, NULL);
		if (TWE_APIRET_IS_SUCCESS(apiRet)) bChangeKindSlot = TRUE;
		bHandled = TRUE;
		break;

	case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7': // DIRECT SLOT SELECTION
		apiRet = TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_CHANGE_KIND_SLOT, 0xFF, u8Byte - '0', NULL);
		if (TWE_APIRET_IS_SUCCESS(apiRet)) bChangeKindSlot = TRUE;
		psIntr->u16HoldUpdateScreen = 5;
		bHandled = TRUE;
		break;
	
	default:
        apiRet = TWEINTRCT_u32MenuOpKey(psIntr, u8Byte);
		if (TWE_APIRET_IS_SUCCESS(apiRet)) {
			bHandled = TRUE;
		}
		break;
	}

	// KIND/SLOT に変更があった(戻り値のkind/slotを保存)
	if (bChangeKindSlot) {
		psIntr->psFinal->u8Slot = TWE_B(apiRet, 0);
		psIntr->psFinal->u8Kind = TWE_B(apiRet, 8);
		//DEBUGMSG("<CHANGE %d:%d>", psIntr->psFinal->u8Kind, psIntr->psFinal->u8Slot);
	}

	// 画面を更新する
	if (bHandled) {
		if (psIntr->u16HoldUpdateScreen == 0) psIntr->u16HoldUpdateScreen = 5;
		bInhibitUpdate = FALSE;
	}

	// 設定項目かどうかを確認する
	if (!bHandled) {
		int i;
		for (i = 0; i < psIntr->psFinal->u8DataCount && psIntr->psFinal->apEle[i] != NULL; i++) {
			if (u8Byte == psIntr->psFinal->au8ShortcutKeys[i]) {
				// 設定入力に入る
				TWESTG_tsElement *pE = psIntr->psFinal->apEle[i];
				//TWESTG_tsDatum *pD = &(psIntr->psFinal->asDatum[i]);

				TWE_fprintf(psIntr->pStream, _TWELB _TWELB "--- Set %s ---", pE->sStr.strName);

				if (pE->sStr.strDesc[0] != '\0') {
					TWE_fprintf(psIntr->pStream, _TWELB"%s", pE->sStr.strDesc);
				}

				TWE_fprintf(psIntr->pStream, _TWELB"Input: ", pE->sStr.strLabel);
				TWEINPSTR_vStart(psIntr->pSerInpStr, pE->sFormat.u8Format & E_TWEINPUTSTRING_DATATYPE_STANDARD_MASK, pE->sFormat.u8InputMaxLength, i);

				bHandled = TRUE;
				break;
			}
		}
	}

	// 処理されなかったキーはコールバックで処理
	// int i = TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_UNHANDLED_CHAR, u8Byte, u8lastbyte, NULL);
	
	// 処理されない場合
	if (!bHandled) {
		bInhibitUpdate = FALSE;
	}

	// 最後の処理
	//u8lastbyte = u8Byte; // 2キー入力を行うための直前のストロークの保存

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
void TWEINTCT_vProcessInputString_settings(TWEINTRCT_tsContext *psIntr, TWEINPSTR_tsInpStr_Context *pContext) {
	//uint8 *pu8str = pContext->au8Data;
	//uint8 u8idx = pContext->u8Idx;

	int i = pContext->u32Opt;
	if (i < psIntr->psFinal->u8DataCount && pContext->u8Idx > 0) {
		TWESTG_tsElement *pE = psIntr->psFinal->apEle[i];
		TWESTG_tsDatum *pDat = &(psIntr->psFinal->asDatum[i]);

		// 文字列からデータ型への変換
		TWE_tsBuffer sBuf;
		TWE_vInit_tsBuffer(&sBuf, pContext->au8Data, pContext->u8Idx, pContext->u8MaxLen);
		
		TWE_APIRET apiRet = pE->sValidate.fpOp(pE, &pDat->uDatum, TWESTGS_VLD_OP_VALIDATE, &sBuf); // データ変換とチェック

		if (TWE_APIRET_IS_SUCCESS(apiRet)) { // データがチェックされた＝pDtに書き込まれている
			pDat->u8Stat |= (TWESTG_DATASTAT_SAVED_MASK | TWESTG_DATASTAT_MODIFIED_MASK); // セーブデータがある場合のマスクを設定
		}
		else {
			TWE_fputs("(invalid data)", psIntr->pStream);
		}
	}
	else {
		TWE_fputs("(canceled)", psIntr->pStream);
	}

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
TWE_APIRET TWEINTCT_u32ProcessMenuEvent_settings(TWEINTRCT_tsContext *psIntr, uint32 u32Op, uint32 u32Arg1, uint32 u32Arg2, void *vpArg) {
	TWE_APIRET ret = TWE_APIRET_FAIL;
	TWE_APIRET apiRet = TWE_APIRET_FAIL;

	switch (u32Op) {
		case E_TWEINTCT_MENU_EV_LOAD:
			/// ロード処理
			// ロード処理の戻り値は、設定された Kind と Id 値
			// アプリケーションは、最初にロードすべき Kind/Slot を設定の上、ロード処理をすます。
			// ロード完了後に psIntr->psFinal をロード済みの構造体にセットする。
			// 戻り値の FF00 マスク：KIND、00FF マスク：SLOT
			apiRet = TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINTCT_MENU_EV_LOAD, psIntr->u8screen, 0, NULL); // メニューへの遷移（初期化）をアプリケーションに伝える
			if (TWE_APIRET_IS_SUCCESS(apiRet)) {
				psIntr->psFinal->u8Slot = TWE_B(apiRet, 0);
				psIntr->psFinal->u8Kind = TWE_B(apiRet, 8);

				ret = TWE_APIRET_SUCCESS;
			}
		break;

		default:
		break;
	}

	return ret;
}