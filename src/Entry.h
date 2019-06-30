//#include "stdafx.h"

//#define PLAYERAI_API __declspec(dllexport)
//EXTERN_C PLAYERAI_API void __stdcall AI(char* mapinfo, char* opestr);
// 忽略这个文件
#pragma once

#include "Logic.h"
#include <iostream>
#include <cstdio>
#include <cstring>

/*#ifdef __cplusplus
#define EXTERNC         extern "C"
#else
#define EXTERNC
#endif

#if defined(__NT__)                   // MS Windows
  #define idaapi            __stdcall
  #define ida_export        idaapi
  #if defined(__X64__) || defined(__NOEXPORT__)
    #define idaman          EXTERNC
  #else
    #define idaman          EXTERNC __declspec(dllexport)
  #endif
  #define ida_local
#elif defined(__APPLE__)				// mac no longer define __UNIX__
  #define idaapi
  #define ida_export        idaapi
  #define idaman          EXTERNC __attribute__((visibility("default")))
  #define ida_local       __attribute__((visibility("hidden")))
#elif defined(__UNIX__)                 // for linux
  #define idaapi 
  #define ida_export        idaapi
  #if __GNUC__ >= 4
    #define idaman          EXTERNC __attribute__ ((visibility("default")))
    #define ida_local       __attribute__((visibility("hidden")))
  #else
    #define idaman          EXTERNC
    #define ida_local
  #endif
#endif*/

//idaman void ida_export AI(char* mapinfo, char* opestr);
void AI(char* mapinfo, char* opestr, int &opestrlen);
