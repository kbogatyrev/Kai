//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    MFC application header.
//
//    $Id: Kai.h,v 1.13 2009-01-11 23:55:24 kostya Exp $
//    
//==============================================================================

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
    #error include 'KaiStdAfx.h' before including this file for PCH
#endif

#include "KaiResource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CKaiApp:
// See Kai.cpp for the implementation of this class
//

//
// Kai specific
//
//class CT_AppDescriptor;
#define MISC_ERROR_BAD_TIME_STR         1101
#define MISC_ERROR_LOGFILE_CREATE       1102

class CKaiApp : public CWinApp
{
public:
    CKaiApp();

    //
    // Kai specific
    //
protected:
    KaiLibMFC::ST_VersionInfo st_Version_;
    unsigned int ui_ClipboardFormat_;

    void v_Null_();
    bool b_CreateLogFile_();

public:
    unsigned int ui_GetClipboardFormat()
    {
        return ui_ClipboardFormat_;
    }

    void v_GetVersion (unsigned int& ui_major1, 
                       unsigned int& ui_minor1,
                       unsigned int& ui_major2, 
                       unsigned int& ui_minor2);
    void v_GetVersion (std::wstring& str_major1, 
                       std::wstring& str_minor1,
                       std::wstring& str_major2, 
                       std::wstring& str_minor2);
    void v_GetVersionInfo (KaiLibMFC::ST_VersionInfo& st_version)
    {
        st_version = st_Version_;
    }

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CKaiApp)
    public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
    //}}AFX_VIRTUAL

    COleTemplateServer m_server;

// Implementation
    //{{AFX_MSG(CKaiApp)
    afx_msg void OnAppAbout();
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    virtual BOOL OnIdle(LONG lCount);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
