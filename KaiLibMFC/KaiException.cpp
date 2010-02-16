//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    implementation of a general-purpose exception class.
//
//    $Id: KaiException.cpp,v 1.14 2007-12-17 08:06:02 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning (disable: 4786)

#include "StdAfx.h"
#include "KaiGeneric.h"
#include "KaiException.h"

#using <mscorlib.dll>

using namespace KaiLibMFC;

CT_KaiException::CT_KaiException (const ET_Exception eo_code, 
                                  const std::wstring& str_module,
                                  const std::wstring& str_class,
                                  const std::wstring& str_method,
                                  const std::wstring& str_description) :
eo_NumericCode_ (eo_code),
str_Module_ (str_module),
str_Class_ (str_class),
str_Method_ (str_method),
str_Description_ (str_description)
{
    std::basic_stringstream<_TCHAR> io_;
    io_ << _T("*** KAI EXCEPTION: ") << std::endl
        << _T("              code   = ") << eo_code << std::endl
        << _T("              module = ") << str_module << std::endl
        << _T("              method = ") << str_method << std::endl
        << _T("              class  = ") << str_class << std::endl
        << _T("              description: ") << str_description 
        << _T(".") << std::endl
        << _T("*** END KAI EXCEPTION") << std::endl;
    std::wstring str_ = io_.str();
    #ifdef _DEBUG
        ATLTRACE2 (str_.data());
    #endif
}

CT_KaiException::CT_KaiException (const ET_Exception eo_code,
                                  const _TCHAR * const qcz_module, 
                                  const _TCHAR * const qcz_class,
                                  const _TCHAR * const qcz_method,
                                  const _TCHAR * const qcz_description) :
eo_NumericCode_ (eo_code),
str_Module_ (qcz_module),
str_Class_ (qcz_class),
str_Method_ (qcz_method),
str_Description_ (qcz_description)
{
    std::basic_stringstream<_TCHAR> io_;
    io_ << _T("*** KAI EXCEPTION: ") << std::endl
        << _T("              code   = ") << eo_code << std::endl
        << _T("              module = ") << qcz_module << std::endl
        << _T("              method = ") << qcz_method << std::endl
        << _T("              class  = " )<< qcz_class << std::endl
        << _T("              description: ") << qcz_description 
        << _T(".") << std::endl
        << _T("*** END KAI EXCEPTION") << std::endl;
    std::wstring str_ = io_.str();
    #ifdef _DEBUG
        ATLTRACE2 (str_.data());
    #endif
}

std::wstring CT_KaiException::str_FormatAll()
{
    std::wstring str_out = _T("\nKai Exception: \n");

    str_out += _T("\tNumeric Code = ");
    _TCHAR arrchr_buf[11];
    memset (arrchr_buf, '\0', 11);
    _itot_s (static_cast<int>(eo_NumericCode_), arrchr_buf, 10, 10);
    str_out += std::wstring (arrchr_buf) + _T("\n");
    str_out += _T("\tModule: ") + str_Module_ + _T("\n");
    str_out += _T("\tClass: ") + str_Class_ + _T("\n");
    str_out += _T("\tFunction: ") + str_Method_ + _T("\n");
    str_out += _T("\tCondition: ") + str_Description_ + _T("\n\n");

    return str_out;
}

void CT_KaiException::v_Report()
{
    std::wstring str_out = str_FormatAll();

    std::wstring::size_type size_index = 0;
    while (true)
    {
        std::wstring::size_type size_at = str_out.find (_T('\n'), size_index);
        if (size_at == std::wstring::npos)
        {
            break;
        }
        str_out.insert (size_at, 1, _T('\r'));
        size_index = size_at + 2;
    }

    std::wstring str_timeDate;
    V_CreateTimeStamp (str_timeDate);
    str_out = str_timeDate + std::wstring (_T("\t")) + str_out;
    B_TraceOutput (str_out);
}

void CT_KaiException::v_MessageBox()
{
    AfxMessageBox ((str_FormatAll()).data());
}
