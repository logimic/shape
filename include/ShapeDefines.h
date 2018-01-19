/**
 * Copyright 2018 Logimic,s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

// -----------------------------------------------------------------------
// Compiler version
// -----------------------------------------------------------------------

// Based on: http://sourceforge.net/p/predef/wiki/VersionNormalization/

// This can then be used like this:
// #if defined(PREDEF_COMPILER_GNUC) && (PREDEF_COMPILER_GNUC >= PREDEF_VERSION(4, 6, 0))
// // Do something for GNU C/C++ compiler version 4.6.0 or newer
// #endif

// Version is 0xVVRRPPPP
#define PREDEF_VERSION(v,r,p) (((v) << 24) + ((r) << 16) + (p))

// Normalize GCC version
#if defined(__GNUC__)
# if defined(__GNUC_PATCHLEVEL__)
#  define PREDEF_COMPILER_GNUC PREDEF_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
# else
#  define PREDEF_COMPILER_GNUC PREDEF_VERSION(__GNUC__, __GNUC_MINOR__, 0)
# endif
# define SHAPE_PREDEF_COMPILER PREDEF_COMPILER_GNUC
#endif

// Normalize Visual Studio version
#if defined(_MSC_FULL_VER)
# define PREDEF_COMPILER_MSC PREDEF_VERSION(_MSC_FULL_VER / 1000000, (_MSC_FULL_VER % 1000000) / 10000, _MSC_FULL_VER % 10000)
# define SHAPE_PREDEF_COMPILER PREDEF_COMPILER_MSC
#else
# if defined(_MSC_VER)
#  define PREDEF_COMPILER_MSC CDETECT_MKVER(_MSC_VER / 100, _MSC_VER % 100, 0)
# endif
#endif

// Normalize Clang
#if defined(__clang__)
# if defined SHAPE_PREDEF_COMPILER
#   undef SHAPE_PREDEF_COMPILER
# endif
# if defined(__clang_patchlevel__)
#   define PREDEF_COMPILER_CLANG PREDEF_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
# else
#   define PREDEF_COMPILER_CLANG PREDEF_VERSION(__clang_major__, __clang_minor__, 0)
# endif
#   define SHAPE_PREDEF_COMPILER PREDEF_COMPILER_CLANG
#endif

#ifndef SHAPE_PREDEF_COMPILER
#error Unsupported compiler - add an identification to ShapeCompilerVersion.h
#endif

//-------------------------------------------------------------------
// Platform
//-------------------------------------------------------------------

#if defined(__APPLE__)
#   define SHAPE_PLATFORM_APPLE
#endif

#if defined(__linux__)
#   define SHAPE_PLATFORM_POSIX
#endif

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#   define SHAPE_PLATFORM_WINDOWS
#else
#   define SHAPE_PLATFORM_POSIX
#endif

//-------------------------------------------------------------------------
// ABI Visibility
//-------------------------------------------------------------------------

#ifdef SHAPE_PLATFORM_WINDOWS
#   define LIB_EXPORT_SHARED __declspec(dllexport)
#   define LIB_IMPORT_SHARED __declspec(dllimport)
#else // POSIX
#   define LIB_EXPORT_SHARED __attribute__ ((visibility ("default")))
#   define LIB_IMPORT_SHARED __attribute__ ((visibility ("default")))
#endif

#ifdef SHAPE_STATIC_LIBS
#   define SHAPE_ABI_EXPORT
#   define SHAPE_ABI_IMPORT
#else
#   define SHAPE_ABI_EXPORT LIB_EXPORT_SHARED
#   define SHAPE_ABI_IMPORT LIB_IMPORT_SHARED
#endif

#ifdef SHAPE_PLATFORM_WINDOWS
#ifdef _DEBUG
#define shape_new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define shape_new new
#endif
#else
#define shape_new new
#endif
