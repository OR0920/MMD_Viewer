#ifndef _DEBUG_MESSAGE_H_
#define _DEBUG_MESSAGE_H_

#include"ToString.h"
#include<cassert>

#ifdef _DEBUG
#include<iostream>
#include<bitset>
#define SET_JAPANESE_ENABLE std::wcout.imbue(std::locale("Japanese", std::locale::ctype))
// �R���\�[���o�̓}�N��
// �V���v���Ȉ�s�o��
#define DebugMessage(x) std::cout << x << std::endl
#define DebugMessageError(x) DebugMessage("ERROR : " << x)
#define DebugMessageWarning(x) DebugMessage("WARNING : " << x)
// �֐��G���[���b�Z�[�W
#define DebugMessageFunctionError(funcName, at) DebugMessageError(ToString(funcName) << " at " << ToString(at) << " is Failed !")
// ���C�h�����p
#define DebugMessageWide(x) std::wcout << reinterpret_cast<const wchar_t*>(x); std::wcout.clear(); std::cout << std::endl
// ���s
#define DebugMessageNewLine() std::cout << std::endl
// �����p�����[�^���o�́@unsigned �Ȃǂɂ��Ή�
#define DebugOutParamI(p) std::cout << ToString(p) << " = " << static_cast<int>(p) << std::endl
// ���̑��̃p�����[�^�o�͗p
#define DebugOutParam(p) std::cout << ToString(p) << " = " << p << std::endl
// �o�C�i���o�́@������dig�Ŏw�肷��
#define DebugOutParamBin(p, dig) std::cout << ToString(p) << " = " <<  std::bitset<dig>(p) << std::endl
// �o�C�i���o�́@16�i��
#define DebugOutParamHex(p) std::cout << ToString(p) << " = " << std::hex << p << std::dec << std::endl
// ������\���A�ϐ����ƈꏏ��
#define DebugOutString(s) if (s != nullptr) {std::cout << ToString(s) << " = " << s << std::endl;}
// ���C�h������\��
#define DebugOutStringWide(w)if(w != nullptr) {std::wcout << ToString(w) << " = " << reinterpret_cast<const wchar_t*>(w); std::wcout.clear(); std::cout << std::endl;}
// �z��̓Y�����ƈꏏ�ɏo�͂���
#define DebugOutArray(a, id) std::cout << ToString(a) << "[" << id << "] = " << a[id] << std::endl
// �z��A�����p
#define DebugOutArrayI(a, id) std::cout << ToString(a) << "[" << id << "] = " << static_cast<int>(a[id]) << std::endl
// �z��A�o�C�i���p
#define DebugOutArrayBin(a, id, dig) std::cout << ToString(a) << "[" << id << "] = " << std::bitset<dig>(a[id]) << std::endl
// �z��̓Y�����A�͈͊O�`�F�b�N
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
