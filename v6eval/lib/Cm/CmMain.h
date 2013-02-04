/*
 * Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 * Yokogawa Electric Corporation, YDC Corporation,
 * IPA (Information-technology Promotion Agency, Japan).
 * All rights reserved.
 * 
 * Redistribution and use of this software in source and binary forms, with 
 * or without modification, are permitted provided that the following 
 * conditions and disclaimer are agreed and accepted by the user:
 * 
 * 1. Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the names of the copyrighters, the name of the project which 
 * is related to this software (hereinafter referred to as "project") nor 
 * the names of the contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
 * 
 * 4. No merchantable use may be permitted without prior written 
 * notification to the copyrighters. However, using this software for the 
 * purpose of testing or evaluating any products including merchantable 
 * products may be permitted without any notification to the copyrighters.
 * 
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHTERS, THE PROJECT AND 
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING 
 * BUT NOT LIMITED THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.  IN NO EVENT SHALL THE 
 * COPYRIGHTERS, THE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT,STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $TAHI: v6eval/lib/Cm/CmMain.h,v 1.9 2009/08/27 00:10:04 akisada Exp $
 */
#ifndef _Cm_CmMain_h_
#define _Cm_CmMain_h_	1
/* Interface Definition */
#include "CmTypes.h"
#include "BtList.h"
typedef void (*exitHandler)(int);
//======================================================================
// CmMain: プログラムの開始および終了処理
// C/C++のプログラムはmain()関数で処理が開始されるexit()関数で処理を終了する
//----------------------------------------------------------------------
// (1) 情報の提供
// CmMainは、main()に渡された環境を保持する、以下の情報を提供する
//	起動時の引数
//	起動時の環境変数
//	起動コマンド名
//	起動時間/起動プログラム名/起動ユーザ名等障害解析情報
// なお、上記障害解析情報はcore解析時にwhatで参照可能となる
// システムに共通な値として必要な情報も提供する
//	プロセスがオープンできるファイルの最大値
//	ホスト名
// なお、必要に応じて他のシステム情報も追加可能である
// また、環境変数なども共通に取り扱える様にするのが望ましい
//----------------------------------------------------------------------
// (2) 終了処理
// 終了処理関数の登録および自動呼び出し
//	終了処理が必要な場合、関数の登録を行う
//	例) 共用メモリの解放...
// システム障害時の自動起動
//----------------------------------------------------------------------
// (3) シグナル処理
// プロセスを停止するすべてのシグナルは捕捉され、障害発生時刻を表示する
//----------------------------------------------------------------------
// (4) 使用方法
// アプリケーションのメイン関数では、void applicationMain(CmMain*)の
// 型に従いアプリケーションメインの記述を行う。
// 記述を容易にするためapplMain()マクロを用意してある。
//----------------------------------------------------------------------
// (5) 記述例
// #include "CmMain.h"
// void callMeWhenExit(int i) {
//	...}
// someFunction() {
//	...
//	exit(1);}				// exit関数の使用は制限される
// applMain() {
//	atexit(callMeWhenExit);	// 複数登録可能
//	int argc=main->argc();
//	STR *argv=main->argv();
//	...
//	}
//----------------------------------------------------------------------
// (6) 障害児解析情報の例
// % what core | grep START
// 96/06/19 12:20:29 t1array STARTED by tamura@alps on
// pty/ttyvc:tamura from dog:0.0
// 起動時刻 プログラム名 STARTED by 起動ユーザ@起動マシン on
// ttyname:ログインユーザ名 from リモートマシン名
//----------------------------------------------------------------------
struct CmMain {
private:
static	STR applicationName_;		// 起動コマンド名
static	char catchStart_[256];		// 障害解析情報
	int argCount_;			// 起動引数の数
	STR *orgArgs_;			// 起動引数
	STR *saveArgs_;			// 起動引数の複写
	STR *saveEnvp_;			// 環境変数
static	CmMain* instance_;
public:
static	CmMain* instance();
	~CmMain();
	CmMain(int argc,STR *argv,STR *envp);
//----------------------------------------------------------------------
// 起動時情報
	int argc() const;
	STR*argv() const;
	STR applicationName(STR pgm=0);
	const STR command() const;
	const STR* const saveArguments() const;
	const STR catchEyeStart() const;
static	void setDbgFlags(CSTR);
//----------------------------------------------------------------------
// システム情報
	const int getdtablesize();
	const STR myHostname();
//----------------------------------------------------------------------
// 終了処理
	void restart();
private:
//----------------------------------------------------------------------
// 内部関数
	void save(int);
	void makeCatch2Eye(STR);
	void makeCatchEye(const STR);
	const STR *saveArgs() const;
	const STR *saveEnvp() const;
};
extern char dbgFlags[];
extern uint32_t logLevel;
extern bool DoHexDump;
#define applMain() void applicationMain(CmMain* main) 
//----------------------------------------------------------------------
// inline関数
inline	CmMain* CmMain::instance() {return instance_;}
inline	int CmMain::argc() const {return argCount_;}
inline	STR*CmMain::argv() const {return orgArgs_;}
inline	const STR CmMain::catchEyeStart() const	{return catchStart_;}
inline	const STR CmMain::command() const 	{return saveArgs_[0];}
inline	const STR*CmMain::saveArgs() const 	{return saveArgs_;}
inline	const STR*CmMain::saveEnvp() const 	{return saveEnvp_;}
inline	const STR* const CmMain::saveArguments() const 	{return saveArgs_;}

#ifndef DBGFLAGS
// avoid warning of compilation
//     gcc version 3.3.1 [FreeBSD]
//     warning: array subscript has type `char'
#define DBGFLAGS(c)	(dbgFlags[(int)(c)])
#endif	// DBGFLAGS
#endif
