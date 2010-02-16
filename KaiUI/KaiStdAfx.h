//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: MFC standrad include files.
//
//    $Id: KaiStdAfx.h,v 1.21 2008-02-19 21:50:05 kostya Exp $
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
#include <afxpriv.h>        	// required for WM_SETMESSAGESTRING; from ProgStatusBar

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

#define KAI_API 
#define EXP_IMP_TEMPLATE

//
// Select basic_string type
//
#ifdef _UNICODE
#define STL_STRING wstring
#else
#endif

//
// Kai-specific types 
//
#include "KaiString.h" // include first
#include "KaiTypes.h"
#include "KaiException.h"
#include "KaiGeneric.h"
#include <tchar.h>

//
// Progress bar
//
#define KAIWM_PROGRESS WM_USER + 1

using namespace KaiLibMFC;

//
// Common controls style
//
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
