//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:.
//
//    $Id: KaiAppDescriptor.h,v 1.14 2007-12-17 08:15:29 kostya Exp $
//    
//==============================================================================

#pragma once

#include "KaiString.h"

namespace KaiLibMFC
{

#define MISC_ERROR_BAD_TIME_STR         1101
#define MISC_ERROR_LOGFILE_CREATE       1102

struct ST_KaiVersion
{
    unsigned int ui_major;
    unsigned int ui_minor;
	std::wstring str_major;
    std::wstring str_minor;
};


class AFX_EXT_CLASS CT_AppDescriptor
{
protected:
    ST_KaiVersion st_Version_;
    unsigned int ui_ClipboardFormat_;

    void v_Null_();

    bool b_SetVersion_();
    bool b_CreateLogFile_();

public:
    bool b_TraceOutput (const std::wstring& str_msg);

public:
    CT_AppDescriptor();
    ~CT_AppDescriptor();

    void v_Init();
    unsigned int ui_GetClipboardFormat()
    {
        return ui_ClipboardFormat_;
    }

    void v_GetVersion (unsigned int& ui_major, unsigned int& ui_minor);
    void v_GetVersion (std::wstring& str_major, std::wstring& str_minor);

};   //  CT_AppDescriptor

}   //  namespace KaiLibMFC
