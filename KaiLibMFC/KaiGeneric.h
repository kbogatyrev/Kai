//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: header file for generic helpers.
//
//    $Id: KaiGeneric.h,v 1.32 2009-01-11 23:53:55 kostya Exp $
//
//==============================================================================

#pragma once

#include "StdAfx.h"
#include "KaiString.h"

namespace KaiLibMFC
{

#define READ_ERROR                      1001
#define READ_ERROR_VERSION              1002
#define READ_ERROR_WATERMARK            1003
#define WRITE_ERROR                     1004
#define IMPORT_ERROR_SEEK               1011
#define IMPORT_ERROR_READ               1014
#define REG_ERROR_OPENKEY               1021
#define REG_ERROR_ENUMKEY               1022
#define REG_ERROR_QUERYKEY              1023
#define REG_ERROR_SETVALUE              1024
#define REG_ERROR_CREATEKEY             1025
#define REG_ERROR_KEY_EXISTS            1026

struct ST_Build
{
    unsigned int ui_major1;
    unsigned int ui_minor1;
    unsigned int ui_major2;
    unsigned int ui_minor2;

    ST_Build()
    {
        ui_major1 = 1;
        ui_minor1 = 0;
        ui_major2 = 0;
        ui_minor2 = 0;
    }
};

struct ST_VersionInfo
{
    std::wstring str_Comments;
    std::wstring str_CompanyName;
    std::wstring str_FileDescription;
    std::wstring str_InternalName;
    std::wstring str_LegalCopyright;
    std::wstring str_LegalTrademarks;
    std::wstring str_OriginalFilename;
    std::wstring str_PrivateBuild;
    std::wstring str_ProductName;
    std::wstring str_SpecialBuild;

    ST_Build st_Build;

    ST_VersionInfo()
    {
        str_CompanyName = _T("Ling Ling San Software");
        str_ProductName = _T("Kai");
    }
};

//
// Trace mode identifiers
//
enum ET_TraceMsgType
{
    ec_TraceMsgTypeFront    = 1400,
    ec_ErrorTraceMsg,
    ec_DebugMsg,
    ec_InfoTraceMsg,
    ec_DumpTraceMsg,
    ec_TraceMsgTypeBack        = ec_DumpTraceMsg+1
};

AFX_EXT_CLASS int I_Round (double d_value);

AFX_EXT_CLASS std::wstring STR_IToStr (int i_num);

AFX_EXT_CLASS std::wstring STR_UIToStr (unsigned int ui_num);

AFX_EXT_CLASS unsigned int ui_STRToUI (const std::wstring& str_);

AFX_EXT_CLASS int i_STRToI (const std::wstring& str_);

AFX_EXT_CLASS void v_RemoveCrLf (std::wstring& str_);

AFX_EXT_CLASS std::wstring STR_LoadResourceString (unsigned int ui_resourceID);

AFX_EXT_CLASS bool B_FormatWindowsMessage (const DWORD dw_msgCode, std::wstring& str_msg);

AFX_EXT_CLASS CT_KaiString KSTR_GetLocalTime();

AFX_EXT_CLASS std::wstring STR_GetCharsetName (const ET_Charset);

AFX_EXT_CLASS bool B_IsPunctuationMark (const ET_Charset, const std::wstring&);

template <class t_item>
AFX_EXT_CLASS void V_LoadItem (CFile&, t_item&);

template <class t_item>
AFX_EXT_CLASS void V_StoreItem (CFile&, t_item&);

template <class t_item>
AFX_EXT_CLASS void V_LoadVector (CFile&, std::vector<t_item>&);

template <class t_item>
AFX_EXT_CLASS void V_StoreVector (CFile&, std::vector<t_item>&);

AFX_EXT_CLASS void V_LoadStlString (CFile&, std::wstring&);
AFX_EXT_CLASS void V_StoreStlString (CFile&, std::wstring&);

AFX_EXT_CLASS void V_ReportMFCFileException (CFileException& co_ex,
                                             unsigned int ui_id = READ_ERROR);

AFX_EXT_CLASS bool B_SetConfigValue (const std::wstring& str_param, 
                                     const std::wstring& str_value);

AFX_EXT_CLASS bool B_GetConfigValue (const std::wstring& str_param, std::wstring& str_value);
AFX_EXT_CLASS bool B_GetConfigValue (const std::wstring& str_param, bool& b_value);
AFX_EXT_CLASS bool B_GetConfigValue (const std::wstring& str_param, int& i_value);
AFX_EXT_CLASS bool B_GetConfigValue (const std::wstring& str_param, unsigned int& ui_value);

AFX_EXT_CLASS void V_CreateTimeStamp (std::wstring& str_timeStamp); 

//AFX_EXT_CLASS bool B_CreateLogFile();
AFX_EXT_CLASS bool B_WriteTraceMsg (ET_TraceMsgType eo_type, 
                                    const std::wstring& str_file, 
                                    int i_line, 
                                    const std::wstring& str_function, 
                                    const std::wstring& str_msgText);
AFX_EXT_CLASS bool B_MakeKeyString (const std::wstring& str_paramName,
                                    std::wstring& str_regKey,
                                    std::wstring& str_regValueName);
AFX_EXT_CLASS bool B_GetVersionInfo (const std::wstring& str_module, 
                                     ST_VersionInfo& st_versionInfo);
AFX_EXT_CLASS void V_GetRegistryPath (std::wstring& str_regPath);
AFX_EXT_CLASS bool B_TraceOutput (const std::wstring& str_msg);
AFX_EXT_CLASS void V_CheckLogFileSize (HANDLE h_file);
AFX_EXT_CLASS bool B_GetAppDataPath (std::wstring& str_path);

//
// Wrappers accessible from C# libraries
//
extern "C" AFX_EXT_CLASS void LogError (LPCTSTR sz_fileName, int i_line, LPCTSTR sz_method, LPCTSTR sz_message);
extern "C" AFX_EXT_CLASS TCHAR * GetRegistryPath();

}   //  namespace KaiLibMFC

