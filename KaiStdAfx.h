//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: MFC standrad include files.
//
//    $Id: KaiStdAfx.h,v 1.17 2007-03-26 19:04:11 kostya Exp $
//    
//==============================================================================

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#ifndef STRICT
#define STRICT
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER                // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0500        // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500    // Change this to the appropriate value to target Windows 2000 or later.
#endif                        

#ifndef _WIN32_WINDOWS          // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0500 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE            // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0400    // Change this to the appropriate value to target IE 5.0 or later.
#endif

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>        // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>            // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//
// STL stuff
//
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <strstream>

using namespace std;

//
// DLL export/import defines
//
#ifdef KAI_DLL
#define KAI_API __declspec (dllexport)
#define EXP_IMP_TEMPLATE
#elif defined (USE_KAI_DLL)
#define KAI_API __declspec (dllimport)
#define EXP_IMP_TEMPLATE
#else
#define KAI_API 
#define EXP_IMP_TEMPLATE
#endif

//
// Select basic_string type
//
#ifdef _UNICODE
#define STL_STRING wstring
#else
#define STL_STRING string
#endif

//
// Trace macro
//
#define LOCATION(str_arg) STL_STRING str_location (_T(__FUNCTION__));

#ifdef _TRACE        // if trace mode expand macros into function calls
#define START_TRACE V_StartTrace (_T(__FUNCTION__));
#define END_TRACE V_EndTrace (str_location);
#define ERROR_TRACE(str_msg) V_Trace (ec_ErrorTraceMsg, \
                                     _T(__FILE__), \
                                     __LINE__, \
                                     _T(__FUNCTION__), \
                                     str_msg);
#define INFO_TRACE(str_msg) V_Trace (ec_InfoTraceMsg, \
                                    _T(__FILE__), \
                                    __LINE__, \
                                    _T(__FUNCTION__), \
                                    str_msg);
#define DUMP_TRACE(str_arg) V_DumpTrace (str_arg);
#else                // if not _TRACE ignore
#define START_TRACE 
#define END_TRACE
#define ERROR_TRACE(str_arg)
#define INFO_TRACE(str_arg)
#define DUMP_TRACE(str_arg)
#endif

//
// Kai-specific types 
//
#include "KaiString.h" // include first
#include "KaiTypes.h"
#include "KaiException.h"
#include "KaiGeneric.h"
#include <tchar.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
