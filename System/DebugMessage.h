#ifndef _DEBUG_MESSAGE_H_
#define _DEBUG_MESSAGE_H_

#include"ToString.h"
#include<cassert>

#ifdef _DEBUG
#include<iostream>
#include<bitset>
#define SET_JAPANESE_ENABLE std::wcout.imbue(std::locale("Japanese", std::locale::ctype))
// コンソール出力マクロ
// シンプルな一行出力
#define DebugMessage(x) std::cout << x << std::endl
#define DebugMessageError(x) DebugMessage("ERROR : " << x)
#define DebugMessageWarning(x) DebugMessage("WARNING : " << x)
// 関数エラーメッセージ
#define DebugMessageFunctionError(funcName, at) DebugMessageError(ToString(funcName) << " at " << ToString(at) << " is Failed !")
// ワイド文字用
#define DebugMessageWide(x) std::wcout << reinterpret_cast<const wchar_t*>(x); std::wcout.clear(); std::cout << std::endl
// 改行
#define DebugMessageNewLine() std::cout << std::endl
// 整数パラメータを出力　unsigned などにも対応
#define DebugOutParamI(p) std::cout << ToString(p) << " = " << static_cast<int>(p) << std::endl
// その他のパラメータ出力用
#define DebugOutParam(p) std::cout << ToString(p) << " = " << p << std::endl
// バイナリ出力　桁数をdigで指定する
#define DebugOutParamBin(p, dig) std::cout << ToString(p) << " = " <<  std::bitset<dig>(p) << std::endl
// バイナリ出力　16進数
#define DebugOutParamHex(p) std::cout << ToString(p) << " = " << std::hex << p << std::dec << std::endl
// 文字列表示、変数名と一緒に
#define DebugOutString(s) if (s != nullptr) {std::cout << ToString(s) << " = " << s << std::endl;}
// ワイド文字列表示
#define DebugOutStringWide(w)if(w != nullptr) {std::wcout << ToString(w) << " = " << reinterpret_cast<const wchar_t*>(w); std::wcout.clear(); std::cout << std::endl;}
// 配列の添え字と一緒に出力する
#define DebugOutArray(a, id) std::cout << ToString(a) << "[" << id << "] = " << a[id] << std::endl
// 配列、整数用
#define DebugOutArrayI(a, id) std::cout << ToString(a) << "[" << id << "] = " << static_cast<int>(a[id]) << std::endl
// 配列、バイナリ用
#define DebugOutArrayBin(a, id, dig) std::cout << ToString(a) << "[" << id << "] = " << std::bitset<dig>(a[id]) << std::endl
// 配列の添え字、範囲外チェック
#define IS_OUT_OF_RANGE(name, id, arrayCount)\
if(id < 0 || static_cast<unsigned>(arrayCount) <= static_cast<unsigned>(id))\
{\
	DebugMessage(ToString(name) << "[" << static_cast<int>(id) << "] is Out of range ! ");\
	assert(false && "Array Out Of Range ! ");\
}
#else
#define SET_JAPANESE_ENABLE 
#define DebugMessage(x) 
#define DebugMessageError(x) 
#define DebugMessageWarning(x) 
#define DebugMessageFunctionError(funcName, at) 
#define DebugMessageWide(x) 
#define DebugMessageNewLine() 
#define DebugOutParamI(p) 
#define DebugOutParam(p) 
#define DebugOutParamBin(p, dig) 
#define DebugOutParamHex(p) 
#define DebugOutString(s) 
#define DebugOutStringWide(w)
#define DebugOutArray(a, id) 
#define DebugOutArrayI(a, id) 
#define DebugOutArrayBin(a, id, dig) 
#define IS_OUT_OF_RANGE(name, id, arrayCount)

#endif // _DEBUG

#endif // !_DEBUG_MESSAGE_H_
