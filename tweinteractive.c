/* Copyright (C) 2019 Mono Wireless Inc. All Rights Reserved.    *
 * Released under MW-SLA-*J,*E (MONO WIRELESS SOFTWARE LICENSE   *
 * AGREEMENT).                                                   */

#include <string.h>
#include "twecommon.h"
#include "twesettings.h"
#include "twesettings_std.h"
#include "twesettings_validator.h"

#include "tweinteractive.h"

#if defined(ARDUINO)
#include <Arduino.h>
#define u32TickCount_ms millis()
#else
extern volatile uint32 u32TickCount_ms; //!< ToCoNet での TickTimer
#endif

// static uint16 u16HoldUpdateScreen; //!< 画面更新のためのカウンタ
static uint16 u16lastupd_ms; //!< 画面更新の最後のアップデートタイミング

//static uint8 u8RevertStatus; //!< 設定の再初期化 0:通常 1:セーブデータ 2:デーブデータ破棄

static TWESERCMD_tsPlus3_Context sSerCmd_P3; //!< + + + を処理するパーサー
static TWEINPSTR_tsInpStr_Context sSerInpStr; //!< １行入力処理を行う

static TWEINTRCT_tsContext sIntrct_Context, *psIntr = &sIntrct_Context; //! インタラクティブモードのコンテキスト

static void(*pf_vProcessInputByte)(TWEINTRCT_tsContext *, int16) = NULL; //!< シリアルパーサー

bool_t (*TWEINTRCT_pfbCbUnHandledCommand)(TWEINTRCT_tsContext *, int16, int16) = NULL; //!< インタラクティブモードの未処理コマンドのコールバック

/*!
 * インタラクティブモードの初期化
 * 
 * \param psFinal             確定設定リスト(tsFinal)
 * \param pSerCmd             シリアルコマンド
 * \param fp                  入出力ストリーム
 * \param pfProcessInputByte  通常のシリアル通常処理用の処理関数（アプリケーション定義）, NULL の場合はインタラクティブモードのみ動作
 */
TWEINTRCT_tsContext* TWEINTRCT_pscInit(TWESTG_tsFinal *psFinal, TWESERCMD_tsSerCmd_Context *pSerCmd, TWE_tsFILE *fp, void *pfProcessInputByte, const TWEINTRCT_tsFuncs *pFuncs) {
	memset(psIntr, 0, sizeof(TWEINTRCT_tsContext));

	psIntr->psFinal = psFinal;
	psIntr->pSerCmd = pSerCmd;
	psIntr->pSerInpStr = &sSerInpStr;
	psIntr->pStream = fp;
	psIntr->pFuncs = pFuncs;
	pf_vProcessInputByte = pfProcessInputByte;

	// メニュー数をカウントする
	while(pFuncs[psIntr->u8screen_max].u8MenuId != 0xFF) {
		psIntr->u8screen_max++;
	}

	// １行入力の初期化
	TWEINPSTR_vInit(&sSerInpStr, psIntr->pStream);

	// +++ 判定用
	memset(&sSerCmd_P3, 0x00, sizeof(TWESERCMD_tsPlus3_Context)); // ＋＋＋判定、ゼロクリアだけでOK

	return psIntr;
}

/*!
 * オプション等の再コンフィグを行う
 * 
 * \param _psIntr
 */
void TWEINTRCT_vReConf(TWEINTRCT_tsContext* _psIntr) {
	if (_psIntr->u8Mode == 1) {
		sSerCmd_P3.bverbose = TRUE;
		TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINTCT_OP_ENTER, psIntr->u8screen, 0, NULL); // インタラクティブモードへの遷移
	}
}

/*!
 * シリアル入力チェック
 * 
 * 本関数は以下のタイミングで呼び出すこと。
 * - 割り込み等が発生したタイミング
 * - 定周期タイマー発生時
 * 
 */
void TWEINTRCT_vHandleSerialInput() {
	// シリアル画面制御のためのカウンタを減算
	{
		uint16 u16t = u32TickCount_ms & 0xffff;
		if ((uint16)(u16t - u16lastupd_ms) >= 16) {
			if (sSerCmd_P3.bverbose && psIntr->u16HoldUpdateScreen) {
				if (!(--psIntr->u16HoldUpdateScreen)) {
					psIntr->pFuncs[psIntr->u8screen].pf_vSerUpdateScreen(psIntr);
				}
			}
			u16lastupd_ms = u16t;
		}
	}

	/* UARTポートの処理 */
	int i16Char;
	while (-1 != (i16Char = TWE_fgetc(psIntr->pStream))) {
		// process
		if (i16Char >= 0 && i16Char <= 0xFF) {
			//DBGOUT(0, "[%02x]", i16Char);
			if (INPSTR_bActive(&sSerInpStr)) {
				// 文字列入力モード
				uint8 u8res = TWEINPSTR_u8InputByte(&sSerInpStr, (uint8)i16Char);

				if (u8res == E_TWEINPUTSTRING_STATE_COMPLETE) {
					psIntr->pFuncs[psIntr->u8screen].pf_vProcessInputString(psIntr, &sSerInpStr);
				}
				else if (u8res == E_TWEINPUTSTRING_STATE_CANCELED) {
					TWE_fprintf(psIntr->pStream, "(canceled)");
					psIntr->u16HoldUpdateScreen = 64;
				}
				continue;
			}

			{
				// コマンド書式の系列解釈、および verbose モードの判定
				uint8 u8res;

				u8res = TWESERCMD_Plus3_u8Parse(&sSerCmd_P3, (uint8)i16Char);

				if (u8res != E_TWESERCMD_PLUS3_EMPTY) {
					if (u8res == E_TWESERCMD_PLUS3_VERBOSE_ON) {
						// verbose モードの判定があった
						psIntr->pFuncs[psIntr->u8screen].pf_vSerUpdateScreen(psIntr);
						TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINTCT_OP_ENTER, psIntr->u8screen, 0, NULL); // インタラクティブモードへの遷移
						continue;
					}

					if (u8res == E_TWESERCMD_PLUS3_VERBOSE_OFF) {
						TWE_fprintf(psIntr->pStream, "!INF EXIT INTERACTIVE MODE."_TWELB);
						TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINTCT_OP_EXIT, psIntr->u8screen | 0x80, 0, NULL);
						continue;
					}
				}
				else {
					; // コマンド解釈モードではない
				}
			}

			if (sSerCmd_P3.bverbose) {
				// Verbose モード=インタラクティブモードのときは、s_vProcessInputByte()にて入力系列を処理する。
				psIntr->pFuncs[psIntr->u8screen].pf_vProcessInputByte(psIntr, i16Char);
			}
			else {
				if (pf_vProcessInputByte) {
					bool_t bCallBack = FALSE;
					if (psIntr->pSerCmd) {
						// SERCMD の解釈を行う
						if(psIntr->u8OptSerCmd & 0x01) TWE_fputc(i16Char, psIntr->pStream); // エコーバック

						uint8 u8stat = psIntr->pSerCmd->u8Parse(psIntr->pSerCmd, (uint8)i16Char);
						if (u8stat & 0x80) {
							bCallBack = TRUE;
						}
					}
					else {
						// pSerCmd が NULL の場合は、毎回コールバックする
						bCallBack = TRUE;
					}

					// コールバック
					if (bCallBack) pf_vProcessInputByte(psIntr, i16Char);
				}
			}
		}
		else {
			break;
		}
	}
}

/*!
 * インタラクティブモードで表示される先頭行を表示する
 * 
 * \param psIntr インタラクティブモードの管理構造体
 * \param u32Op 表示オプション(現時点では未実装）
 */
void TWEINTRCT_vSerHeadLine(TWEINTRCT_tsContext *psIntr, uint32 u32Op) {
	TWE_fputs("\033[2J\033[H", psIntr->pStream); // CLEAR SCREEN

	uint8 strAppName[16] = "NONAME", *pstrAppName = strAppName;
	uint8 strKindName[16] = "", *pstrKindName = strKindName;
	uint8 strOptMsg[32] = "", *pstrOptMsg = strOptMsg;
	uint32 u32Sid = 0x80000000;

	// 先頭行に必要な情報の取り出し
	TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_GET_APPNAME, 0, 0, &pstrAppName);
	TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_GET_KINDNAME, psIntr->psFinal->u8Kind, psIntr->psFinal->u8Slot, &pstrKindName);
	TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINTCT_OP_GET_SID, 0, 0, &u32Sid);
	TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINTCT_OP_GET_OPTMSG, 0, 0, &pstrOptMsg);

	if (u32Op & 0x01) {
		uint8 au8buff[16] = "";

		
		if (pstrKindName && pstrKindName[0] != '\0') {
			TWE_snprintf((char*)au8buff, 16, ":%s", pstrKindName);
		}
		
		TWE_fprintf(psIntr->pStream, "--- %s / %s%s:%d V%d-%02d-%d/SID=%8X",
			psIntr->pFuncs[psIntr->u8screen].pu8MenuString,
			pstrAppName,
			au8buff,
			psIntr->psFinal->u8Slot,   // LID
			(psIntr->psFinal->u32AppVer & 0xFF000000) >> 24,
			(psIntr->psFinal->u32AppVer & 0xFF0000) >> 16,
			(psIntr->psFinal->u32AppVer & 0xFF00) >> 8,
			u32Sid // SID
			);
			
	} else {
		TWE_fprintf(psIntr->pStream, "--- %s / %s V%d-%02d-%d/SID=%8X",
			psIntr->pFuncs[psIntr->u8screen].pu8MenuString,
			pstrAppName,
			(psIntr->psFinal->u32AppVer & 0xFF000000) >> 24,
			(psIntr->psFinal->u32AppVer & 0xFF0000) >> 16,
			(psIntr->psFinal->u32AppVer & 0xFF00) >> 8,
			u32Sid // SID
			);
	}

	if (pstrOptMsg && pstrOptMsg[0] != '\0') {
		TWE_fputc('/', psIntr->pStream);
		TWE_fputs((char*)pstrOptMsg, psIntr->pStream);
	}

	TWE_fputs(" ---"_TWELB, psIntr->pStream);
	TWE_fflush(psIntr->pStream);
}

/*!
 * インタラクティブモードで表示される末尾行を表示する
 * 
 * \param psIntr インタラクティブモードの管理構造体
 * \param u32Op 表示オプション(現時点では未実装）
 */
void TWEINTRCT_vSerFootLine(TWEINTRCT_tsContext *pContext, uint32 u32Op) {
	//TWE_fprintf(psIntr->pStream, "%d/%d", psIntr->u8screen, psIntr->u8screen_max); // TODO
	TWE_fputs("- [Ctrl+L]:Refresh Scr [BS]:Back Menu [!]: Reset System"_TWELB, psIntr->pStream);
}

/*!
 * インタラクティブモードのメニュー操作の共通処理
 * 
 * \param psIntr インタラクティブモードの管理構造体
 * \param u8Key 入力キー
 * \return SUCCESS:処理された FAIL:処理されない
 */
TWE_APIRET TWEINTRCT_u32MenuOpKey(TWEINTRCT_tsContext *pContext, uint8 u8Key) {
	TWE_APIRET ret = TWE_APIRET_FAIL;

	switch(u8Key) {
	case _TWELF: // LF
		ret = TWE_APIRET_SUCCESS_W_VALUE(0);
		break;
    case _TWECR: // CR
	case _TWECLR: // Ctrl+L
		// 画面の書き換え
		ret = TWE_APIRET_SUCCESS_W_VALUE(1);
		break;

	case _TWEBS:
        psIntr->u8screen = 0; // back to the menu
		ret = TWE_APIRET_SUCCESS_W_VALUE(1);
        break;

	case '!': // MODULE RESET
		TWE_fprintf(psIntr->pStream, _TWELB"!!! RESET the system !!!");
		TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_WAIT, 100, 0, NULL);
		TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINRCT_OP_RESET, 0, 0, NULL);
		ret = TWE_APIRET_SUCCESS_W_VALUE(1);
		break;

	case _TWEESC:
		sSerCmd_P3.bverbose = FALSE;
		psIntr->u8Mode = 0;
		TWE_fprintf(psIntr->pStream, "!INF EXIT INTERACTIVE MODE."_TWELB);
		TWEINTRCT_cbu32GenericHandler(psIntr, E_TWEINTCT_OP_EXIT, 0, 0, NULL);
		break;
	}

	return ret;
}

/**
 * @brief メニュー切り替えを行う。同時にアプリケーションにもコールバックで切り替えを伝達する。
 * 
 * 
 * @param pContext 
 * @param u8MenuIdx 
 * @return TWE_APIRET 
 */
TWE_APIRET TWEINTRCT_u32MenuChange(TWEINTRCT_tsContext *psIntr, uint8 u8NewScreen) {
	if (u8NewScreen < psIntr->u8screen_max) {
		psIntr->u8screen = u8NewScreen;
		if (psIntr->pFuncs[psIntr->u8screen].pf_u32ProcessMenuEvent != NULL) {
			TWE_APIRET apiRet = psIntr->pFuncs[psIntr->u8screen].pf_u32ProcessMenuEvent(psIntr, E_TWEINTCT_MENU_EV_LOAD, psIntr->u8screen, 0UL, NULL);
			(void)apiRet;
		}
		return TWE_APIRET_SUCCESS_W_VALUE(u8NewScreen);
	} else {
		return TWE_APIRET_FAIL;
	}
}