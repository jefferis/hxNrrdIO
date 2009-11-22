/***************************************************************
 *
 * DLL export/import definitions for Windows
 *
 ***************************************************************/
#ifndef NRRDIO_WIN_DLL_API_H
#define NRRDIO_WIN_DLL_API_H

#ifdef _WIN32
#  ifdef NRRDIO_EXPORTS
#     define NRRDIO_API __declspec(dllexport)
#  else
#     define NRRDIO_API __declspec(dllimport)
#  endif
#else
#   define NRRDIO_API 
#endif

#endif
