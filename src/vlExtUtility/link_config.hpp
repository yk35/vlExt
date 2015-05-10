#pragma once

#if defined (_WIN32) && defined(VLEXT_DYNAMIC_LINKING)
  #if defined(VLEXT_EXPORTS)
    #define  VLEXT_EXPORT __declspec(dllexport)
  #else
    #define  VLEXT_EXPORT __declspec(dllimport)
  #endif /* MyLibrary_EXPORTS */
#else /* defined (_WIN32) */
 #define VLEXT_EXPORT
#endif