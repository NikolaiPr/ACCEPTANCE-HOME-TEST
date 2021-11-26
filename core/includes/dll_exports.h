/** @file  dll_exports.h */
#pragma once

#ifdef CORE_EXPORTS
#define CORE_EXPORT __declspec( dllexport )
#define CORE_EXTERN
#else
#define CORE_EXPORT __declspec( dllimport )
#define CORE_EXTERN extern
#endif