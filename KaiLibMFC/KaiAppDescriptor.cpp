//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:.
//
//    $Id: KaiAppDescriptor.cpp,v 1.23 2007-12-17 08:05:06 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiAppDescriptor.h"
#include "KaiGeneric.h"
#include <typeinfo.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace KaiLibMFC;
using namespace std;

CT_AppDescriptor::CT_AppDescriptor()
{
    st_Version_.str_major = _T("");
    st_Version_.str_minor = _T("");
    st_Version_.ui_major = 0;
    st_Version_.ui_minor = 0;
}

CT_AppDescriptor::~CT_AppDescriptor()
{
    std::wstring str_endMsg;
    V_CreateTimeStamp (str_endMsg);
    str_endMsg += _T("\t\t*** Kai is terminating ***\r\n\r\n");
    B_TraceOutput (str_endMsg);

    v_Null_();
}

void CT_AppDescriptor::v_Null_()
{
    st_Version_.str_major = _T("");
    st_Version_.str_minor = _T("");
    st_Version_.ui_major = 0;
    st_Version_.ui_minor = 0;

    ui_ClipboardFormat_ = 0;
}

void CT_AppDescriptor::v_Init()
{
    //
    // Call first so we can access Registry
    //
    bool b_sv = b_SetVersion_();
    if (!b_sv)
    {
         return;
    }

    //
    // Register clipboard format
    //
    ui_ClipboardFormat_ = RegisterClipboardFormat (_T("CF_KAI"));

    //
    // Write log message
    //
    std::wstring str_startMsg;
    V_CreateTimeStamp (str_startMsg);
    str_startMsg += _T("\t\t*** Kai initializing ***\r\n");
    B_TraceOutput (str_startMsg);

}   // v_Init();

void CT_AppDescriptor::v_GetVersion (unsigned int& ui_major, 
                                     unsigned int& ui_minor)
{
    ui_major = st_Version_.ui_major;
    ui_minor = st_Version_.ui_minor;
}

void CT_AppDescriptor::v_GetVersion (std::wstring& str_major, 
                                     std::wstring& str_minor)
{
    str_major = st_Version_.str_major;
    str_minor = st_Version_.str_minor;
}

bool CT_AppDescriptor::b_SetVersion_()
{
    //
    // Tracing is not enabled yet...
    //
    DWORD dw_dummy = 0;
    DWORD dw_size = GetFileVersionInfoSize (_T("Kai.exe"), &dw_dummy);
    if (dw_size == 0)
    {
        DWORD dw_errCode = GetLastError();
        std::wstring str_msg;
        bool b_fwmResult = B_FormatWindowsMessage (dw_errCode, str_msg);
        // no tracing, so pop a message box
        str_msg = _T("Error accessing version data: ") + str_msg;
        AfxMessageBox (str_msg.data(), MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    _TCHAR * pchr_versionData = new _TCHAR [dw_size];
    void * pv_versionData = static_cast <void *> (pchr_versionData);
    BOOL mfcb_gfviReturn = GetFileVersionInfo (_T("Kai.exe"),
                                               0,    // ignored
                                               dw_size,
                                               pv_versionData);
    if (!mfcb_gfviReturn)
    {
        DWORD dw_errCode = GetLastError();
        std::wstring str_msg;
        bool b_fwmResult = B_FormatWindowsMessage (dw_errCode, str_msg);
        // ditto
        str_msg = _T("Error accessing version data: ") + str_msg;
        AfxMessageBox (str_msg.data(), MB_ICONEXCLAMATION | MB_OK);
        delete[] pchr_versionData;
        return false;
    }

    void * pv_fixedFileInfo = NULL;
    unsigned int ui_bufLen = 0;
    BOOL mfcb_vqvReturn = VerQueryValue (pv_versionData,
                                         _T("\\"),
                                         &pv_fixedFileInfo,
                                         &ui_bufLen);
    if (mfcb_vqvReturn == 0)
    {
        delete[] pchr_versionData;
        return false;
    }
 
    VS_FIXEDFILEINFO * pst_versionInfo = 
        static_cast<VS_FIXEDFILEINFO *>(pv_fixedFileInfo);

    st_Version_.ui_major = pst_versionInfo->dwProductVersionMS;
    st_Version_.ui_minor = pst_versionInfo->dwProductVersionLS;
    st_Version_.str_major = STR_UIToStr (st_Version_.ui_major);
    st_Version_.str_minor = STR_UIToStr (st_Version_.ui_minor);

    delete[] pchr_versionData;

    return true;

}    //  b_SetVersion (...)
