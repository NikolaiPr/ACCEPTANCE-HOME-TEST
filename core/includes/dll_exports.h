#pragma once

#ifdef CORE_EXPORTS
#define CORE_EXPORT __declspec( dllexport )
#else
#define CORE_EXPORT __declspec( dllimport )
#endif