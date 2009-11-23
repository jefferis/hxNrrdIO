/***************************************************************
 *
 * DLL export/import definitions for Windows
 *
 ***************************************************************/
#ifndef HXNRRDIO_WIN_DLL_API_H
#define HXNRRDIO_WIN_DLL_API_H

#ifdef _WIN32
#  ifdef HXNRRDIO_EXPORTS
#     define HXNRRDIO_API __declspec(dllexport)
#  else
#     define HXNRRDIO_API __declspec(dllimport)
#  endif
#else
#   define HXNRRDIO_API 
#endif

#endif
