#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#if _MSC_VER && !__INTEL_COMPILER
#define WINDOWS
#endif

#if defined(__clang__)
#define CLANG
#endif

// forceinline
#if defined(WINDOWS)
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE __attribute__((always_inline))
#endif

// C
#include <cstdio>
#include <cstring>
#include <cassert>
#include <ctime>
#include <cinttypes>
#define _USE_MATH_DEFINES
#include <cmath>

// C++
#include <fstream>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <functional>
#include <random>
#include <array>
#include <valarray>
#include <unordered_map>
#include <deque>
#include <sstream>
#include <iomanip>
#include <random>
#include <thread>
#include <typeindex>
#include <shared_mutex>
#include <atomic>

#if defined(WINDOWS)
// windows
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <windowsx.h>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <timeapi.h>
#include <sal.h>
#include <Shlwapi.h>
#include <xmllite.h>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib, "xmllite.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

// SIMD
#if defined(CLANG)
#include <x86intrin.h>
#else
#include <mmintrin.h>
#include <intrin.h>
#endif


// Third party
#include "tinyxml2.h"

// macro
#define PI float(M_PI)
#define INV_PI float(M_1_PI)

// 
#define DEG2RAD(deg) ((deg) / 180.0f * PI)

// 波長
#define WAVE_LENGTH_D 0.58930f
#define WAVE_LENGTH_d 0.58756f
#define WAVE_LENGTH_F 0.4861f
#define WAVE_LENGTH_C 0.6563f
#define WAVE_LENGTH_e 0.5461f
#define WAVE_LENGTH_F_DASH 0.4880f
#define WAVE_LENGTH_C_DASH 0.6439f

#if defined(NO_ASSERT)
#define AL_ASSERT_DEBUG(expr)
#define AL_VALID(expr) expr
#else

#if defined(WINDOWS)
#define AL_ASSERT_DEBUG(expr) if(!(expr)){ printf("%s %s %d\n",#expr, __FILE__, __LINE__); OutputDebugString(#expr);  DebugBreak(); }
#else
#define AL_ASSERT_DEBUG(expr) if(!(expr)){ printf("%s %s %d\n",#expr, __FILE__, __LINE__); }
#endif
#define AL_VALID(expr) AL_ASSERT_DEBUG(expr)
#endif

#if defined(WINDOWS)
#define AL_ASSERT_ALWAYS(expr) if(!(expr)){ printf("%s %s %d\n",#expr, __FILE__, __LINE__); OutputDebugString(#expr);  DebugBreak(); }
#else
#define AL_ASSERT_ALWAYS(expr) if(!(expr)){ printf("%s %s %d\n",#expr, __FILE__, __LINE__); __builtin_trap(); }
#endif

// aligin
#if defined(WINDOWS)
#define ALIGN(bit) __declspec(align(bit)) 
#else
#define ALIGN(bit) alignas(bit)
#endif
#define ALIGN16 ALIGN(16)
#define ALIGN32 ALIGN(32)
#define ALIGN64 ALIGN(64)
#define ALIGN128 ALIGN(128)

// inline
#if defined(WINDOWS)
#define AL_FORCEINLINE __forceinline
#define INLINE inline
#else
#define AL_FORCEINLINE inline
#define INLINE inline
#endif

// 
#pragma warning(disable:4201) // 無名の構造体または共用体
#pragma warning(disable:4324) // 構造体がパッドされた
#pragma warning(disable:4127) // 条件式が定数
#pragma warning(disable:4503) // chaiscript対策


// SALの代替をここにかいておく
#if !defined(WINDOWS)
#define _In_reads_(expr)
#define _In_
#define _Out_
#define _Inout_
#endif

// TLS
#if defined(WINDOWS)
#define AL_TLS __declspec(thread)
#else
#define AL_TLS __thread
#endif

