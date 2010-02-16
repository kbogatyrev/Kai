//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    generic helpers.
//
//    $Id: KaiGeneric.cpp,v 1.51 2009-01-18 02:32:08 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include <stdio.h>
#include <math.h>
#include <typeinfo>
#include "StdAfx.h"
#include "KaiException.h"
#include "KaiGeneric.h"

using namespace System;
using namespace Microsoft::Win32;

#using <mscorlib.dll>

using namespace KaiLibMFC;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// Generic helpers
//
int KaiLibMFC::I_Round (double d_arg)
{
    double d_intPart;
    double d_fractionalPart = modf (d_arg, &d_intPart);
    int i_result = 0;
    if (d_fractionalPart > .5)
    {
        i_result = (int) ceil (d_arg);
    }
    else
    {
        i_result = (int) floor (d_arg);
    }
    
    return i_result;
}

std::wstring KaiLibMFC::STR_IToStr (int i_num)
{
    _TCHAR arrchr_buf[11];
    memset (arrchr_buf, '\0', 11);
    _itot_s (i_num, arrchr_buf, 11, 10);
    return std::wstring (arrchr_buf);
}

std::wstring KaiLibMFC::STR_UIToStr (unsigned int ui_num)
{
    _TCHAR arrchr_buf[11];
    memset (arrchr_buf, '\0', 11);
    _i64tot_s (static_cast <__int64>(ui_num), arrchr_buf, 11, 10);
    return std::wstring (arrchr_buf);
}

unsigned int KaiLibMFC::ui_STRToUI (const std::wstring& str_)
{
    std::basic_istringstream<_TCHAR> io_fd (str_);
    unsigned int ui_;
    io_fd >> ui_;

    return ui_;
}

int KaiLibMFC::i_STRToI (const std::wstring& str_)
{
    std::basic_istringstream<_TCHAR> io_fd (str_);
    int i_;
    io_fd >> i_;

    return i_;
}

void KaiLibMFC::v_RemoveCrLf (std::wstring& str_)
{
    unsigned int ui_pos = 0;
    do
    {
        ui_pos = (unsigned int)str_.find (_T('\r'));
        if (std::wstring::npos == ui_pos)
        {
            continue;
        }

        str_[ui_pos] = _T('\xB6');
        if ((ui_pos < str_.length()-1) && (_T('\n') == str_[ui_pos+1]))
        {
            str_.erase (ui_pos+1, 1);
        }

    } while (std::wstring::npos != ui_pos);
}

std::wstring KaiLibMFC::STR_LoadResourceString (unsigned int ui_resourceID)
{
    CString cstr;
    std::wstring str_text;
    BOOL mfcb_result = cstr.LoadString (ui_resourceID);
    if (mfcb_result)
    {
        str_text = LPCTSTR (cstr);
    }
    else
    {
        str_text = _T("");
    }

    return str_text;
}

bool KaiLibMFC::B_FormatWindowsMessage (const DWORD dw_msgCode, std::wstring& str_msg)
{
    DWORD dw_flags = FORMAT_MESSAGE_ALLOCATE_BUFFER;
    dw_flags |= FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    void * v_msgText = NULL;
    DWORD dw_length = FormatMessage (dw_flags,
                                     0,  // ignored
                                     dw_msgCode,
                                     MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                                     (LPTSTR)&v_msgText,
                                     0,  // min size to allocate
                                     NULL);  // no inserts
    bool b_result;
    if (dw_length == 0)
    {
        DWORD dw_error = GetLastError();
        str_msg = _T("Unknown error.");
        b_result = false;        
    }
    else
    {
        str_msg = (_TCHAR *)v_msgText;
        b_result = true;
    }
    
    LocalFree (v_msgText);

    return b_result;

}    // B_FormatWindowsMessage

CT_KaiString KaiLibMFC::KSTR_GetLocalTime()
{
    SYSTEMTIME st_time;    

    GetLocalTime (&st_time);    

    CT_KaiString kstr_time (_T(""), _T("/"));
    _TCHAR chr_num[6];
    chr_num[5] = '\0';

    _itot_s (st_time.wYear, chr_num, 5, 10);
    kstr_time = chr_num;
    kstr_time += _T("/");
    _itot_s (st_time.wMonth, chr_num, 3, 10);
    kstr_time += chr_num;
    kstr_time += _T("/");
    _itot_s (st_time.wDay, chr_num, 3, 10);
    kstr_time += chr_num;
    kstr_time += _T("/");
    _itot_s (st_time.wHour, chr_num, 3, 10);
    kstr_time += chr_num;
    kstr_time += _T("/");
    _itot_s (st_time.wMinute, chr_num, 3, 10);
    kstr_time += chr_num;
    kstr_time += _T("/");
    _itot_s (st_time.wSecond, chr_num, 3, 10);
    kstr_time += chr_num;
    kstr_time += _T("/");
    _itot_s (st_time.wMilliseconds, chr_num, 4, 10);
    kstr_time += chr_num;

    return kstr_time;

}    //  KSTR_GetLocalTime()

std::wstring KaiLibMFC::STR_GetCharsetName (const ET_Charset eo_charset)
{
    std::wstring str_charset;
    switch (eo_charset)
    {
        case ec_ANSICharset:
        {
            str_charset = _T("ec_ANSICharset");
            break;
        }
        case ec_BalticCharset:
        {
            str_charset = _T("ec_BalticCharset");
            break;
        }
        case ec_ChineseBig5Charset:
        {
            str_charset = _T("ec_ChineseBig5Charset");
            break;
        }
        case ec_DefaultCharset:
        {
            str_charset = _T("ec_DefaultCharset");
            break;
        }
        case ec_EastEuropeCharset:
        {
            str_charset = _T("ec_EastEuropeCharset");
            break;
        }
        case ec_GB2312Charset:
        {
            str_charset = _T("ec_GB2312Charset");
            break;
        }
        case ec_GreekCharset:
        {
            str_charset = _T("ec_GreekCharset");
            break;
        }
        case ec_HangulCharset:
        {
            str_charset = _T("ec_HangulCharset");
            break;
        }
        case ec_MacCharset:
        {
            str_charset = _T("ec_MacCharset");
            break;
        }
        case ec_OEMCharset:
        {
            str_charset = _T("ec_OEMCharset");
            break;
        }
        case ec_RussianCharset:
        {
            str_charset = _T("ec_RussianCharset");
            break;
        }
        case ec_ShiftjisCharset:
        {
            str_charset = _T("ec_ShiftjisCharset");
            break;
        }
        case ec_SymbolCharset:
        {
            str_charset = _T("ec_SymbolCharset");
            break;
        }
        case ec_TurkishCharset:
        {
            str_charset = _T("ec_TurkishCharset");
            break;
        }
        case ec_JohabCharset:
        {
            str_charset = _T("ec_JohabCharset");
            break;
        }
        case ec_HebrewCharset:
        {
            str_charset = _T("ec_HebrewCharset");
            break;
        }
        case ec_ArabicCharset:
        {
            str_charset = _T("ec_ArabicCharset");
            break;
        }
        case ec_ThaiCharset:
        {
            str_charset = _T("ec_ThaiCharset");
            break;
        }
        default:
        {
            str_charset = _T("Unknown; numeric value = ");
            str_charset += STR_UIToStr (static_cast <unsigned int>
             (eo_charset));
        }
    }    // switch (...)

    return str_charset;

}  // STR_GetCharsetName


//
// Returns true if *all* characters in the 2nd argument are punctuiation marks
//
bool KaiLibMFC::B_IsPunctuationMark (const ET_Charset eo_charset, const std::wstring& str_chars)
{
    std::wstring str_pmSet;

    //
    // Primitive version
    //
    switch (eo_charset)
    {
        case ec_ANSICharset:
        case ec_DefaultCharset:
        {
            str_pmSet = _T(".,:;?!-=+()[]{}<>|`");
            str_pmSet += _T('\\');
            str_pmSet += _T('\"');
            str_pmSet += _T('\'');
            break;
        }
        case ec_RussianCharset:
        {
            str_pmSet = _T("..:;?!-=+()");  // *** NOT ALL -- TBD ***
            break;
        }
        default:
        {
            std::wstring str_msg = _T("Bad charset id ");
            str_msg += STR_UIToStr (static_cast <unsigned int>(eo_charset));
            str_msg += _T(".");
            AfxMessageBox (str_msg.data());
        }
    }
        
    if (str_chars.find_first_not_of (str_pmSet) == std::wstring::npos)
    {
        return true;
    }
    else
    {
        return false;
    }

}  //  bool B_IsPunctuation (...)

//
// Load/store a single value of a built-in type
//
template AFX_EXT_CLASS void KaiLibMFC::V_LoadItem <bool> (CFile&, bool&);
template AFX_EXT_CLASS void KaiLibMFC::V_LoadItem <int> (CFile&, int&);
template AFX_EXT_CLASS void KaiLibMFC::V_LoadItem <unsigned int> (CFile&, unsigned int&);
template AFX_EXT_CLASS void KaiLibMFC::V_LoadItem <unsigned long> (CFile&, unsigned long&);
template AFX_EXT_CLASS void KaiLibMFC::V_LoadItem <ET_Alignment> (CFile&, ET_Alignment&);
template AFX_EXT_CLASS void KaiLibMFC::V_LoadItem <ET_Charset> (CFile&, ET_Charset&);
template AFX_EXT_CLASS void KaiLibMFC::V_LoadItem <ET_ItemType> (CFile&, ET_ItemType&);
//EXP_IMP_TEMPLATE template AFX_EXT_CLASS void V_LoadItem <ET_CompoundType> (CFile&, ET_CompoundType&);
template AFX_EXT_CLASS void KaiLibMFC::V_LoadItem <ET_LineSpacingType> (CFile&, ET_LineSpacingType&);
template <class t_item>
void KaiLibMFC::V_LoadItem (CFile& co_file, t_item& co_item)
{
    unsigned int ui_bytesRead = 0;

    try
    {
        ui_bytesRead = co_file.Read (&co_item, sizeof (t_item));
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
    }

    if (ui_bytesRead < (unsigned int)sizeof (t_item))
    {
        throw CT_KaiException (CT_KaiException::ec_LoadError,
                               _T("KaiGeneric.cpp"),
                               _T(""),
                               _T(__FUNCTION__),
                               _T("Item size less than expected"));
    }

}  // V_LoadItem

template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <bool> (CFile&, bool&);
template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <int> (CFile&, int&);
template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <unsigned int> (CFile&, unsigned int&);
template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <unsigned long> (CFile&, unsigned long&);
template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <ET_Alignment> (CFile&, ET_Alignment&);
template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <ET_Charset> (CFile&, ET_Charset&);
template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <ET_ItemType> (CFile&, ET_ItemType&);
//EXP_IMP_TEMPLATE template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <ET_CompoundType> (CFile&, ET_CompoundType&);
template AFX_EXT_CLASS void KaiLibMFC::V_StoreItem <ET_LineSpacingType> (CFile&, ET_LineSpacingType&);
template <class t_item>
void KaiLibMFC::V_StoreItem (CFile& co_file, t_item& co_item)
{
    try
    {
        unsigned int ui_size = sizeof (t_item); 
//        co_file.Write (&ui_size, sizeof (unsigned int));
        co_file.Write (&co_item, sizeof (t_item));
    }
    catch (CFileException * pco_ex)
    {
        pco_ex->Delete();
        throw CT_KaiException (CT_KaiException::ec_LoadError,
                               _T("KaiGeneric.cpp"),
                               _T(""),
                               _T(__FUNCTION__),
                               _T("I/O error"));
    }

}  // V_StoreItem

//EXP_IMP_TEMPLATE template class AFX_EXT_CLASS std::vector<int>;
template AFX_EXT_CLASS void KaiLibMFC::V_LoadVector (CFile&, std::vector<int>&);
template <class t_item>
void KaiLibMFC::V_LoadVector (CFile& co_file, std::vector<t_item>& vo_vector)
{
    unsigned int ui_items = 0;
    V_LoadItem (co_file, ui_items);

    unsigned int ui_bytesRead = 0;
    for (unsigned int ui_at = 0; ui_at < ui_items; ++ui_at)
    {
        t_item t_value;
        try
        {
            ui_bytesRead = co_file.Read (&t_value, sizeof (t_item));
        }
        catch (CFileException * pco_ex)
        {
            V_ReportMFCFileException (*pco_ex, READ_ERROR);
        }

        if (ui_bytesRead != (unsigned int)sizeof (t_item))
        {            
            throw CT_KaiException (CT_KaiException::ec_LoadError,
                                   _T("KaiGeneric.cpp"),
                                   _T(""),
                                   _T("V_LoadVector"),
                                   _T("Bad item size"));
        }
        vo_vector.insert (vo_vector.end(), t_value);
    }

}  // V_LoadVector

template AFX_EXT_CLASS void KaiLibMFC::V_StoreVector (CFile&, std::vector<int>&);
template <class t_item>
void V_StoreVector (CFile& co_file, std::vector<t_item>& vo_vector)
{
    try
    {
        co_file.Write (&vo_vector.size(), sizeof (unsigned int));
        for (unsigned int ui_at = 0; ui_at < vo_vector.size(); ++ui_at)
        {
            co_file.Write (&vo_vector[ui_at], sizeof (t_item));
        }
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
    }

}  // V_StoreVector

void KaiLibMFC::V_LoadStlString (CFile& co_file, std::wstring& str_)
{
    unsigned int ui_bytesRead = 0;
    LPTSTR qz_buffer = NULL;
    int i_length = 0;

    // Each string is stored as < i_length > < 'i_length' of characters >
    // without the trailing null character
    try
    {
        V_LoadItem (co_file, i_length);
        if ((i_length <= 0) || (i_length >= static_cast<int>(str_.max_size())))
        {
            throw CT_KaiException (CT_KaiException::ec_IllegalValue,
                                   _T("KaiGeneric.cpp"),
                                   _T("none"),
                                   _T(__FUNCTION__),
                                   _T("Illegal string length"));
            return;
        }

        qz_buffer = new _TCHAR[i_length+1];
        memset (qz_buffer, 0, sizeof(_TCHAR)*(i_length + 1));
        ui_bytesRead = co_file.Read (qz_buffer, (sizeof(_TCHAR))*i_length);
        if (ui_bytesRead < (sizeof(_TCHAR))*i_length)
        {
            delete[] qz_buffer;
            throw CT_KaiException (CT_KaiException::ec_IncorrectLength,
                                   _T("KaiGeneric.cpp"),
                                   _T("none"),
                                   _T(__FUNCTION__),
                                   _T("Size less than expected"));
            return;
        }
    }
    catch (CFileException * pco_ex)
    {
        delete[] qz_buffer;
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
        return;
    }

    str_.assign (qz_buffer);

    delete[] qz_buffer;

}    //  V_LoadStlString (...)

void KaiLibMFC::V_StoreStlString (CFile& co_file, std::wstring& str_)
{
    try
    {
        int i_length = (int)str_.length();
        V_StoreItem (co_file, i_length);
        co_file.Write (str_.data(), sizeof(_TCHAR)*i_length);
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
    }

}    //  V_StoreStlString (...)

/*
//void V_DumpTrace (const std::wstring& str_location, const std::wstring& str_msg)
void KaiLibMFC::V_DumpTrace (const std::wstring& str_msg)
{
    CT_AppDescriptor * pco_app = (static_cast <CKaiApp *> (AfxGetApp()))
     ->pco_KaiApp;
    if (!pco_app)
    {
        return;    
    }
    bool b_ = pco_app->b_WriteTraceMsg (ec_DumpTraceMsg, _T(""), str_msg);
}
*/

void KaiLibMFC::V_ReportMFCFileException (CFileException& co_ex,
                                          unsigned int ui_id) // default = READ_ERROR
{
    std::wstring str_out = STR_LoadResourceString (ui_id);
    if (str_out.length() < 1)
    {
        str_out = _T("I/O error");
    }
    str_out += _T(": ");
    std::wstring str_errorDescription;
    B_FormatWindowsMessage (co_ex.m_lOsError, str_errorDescription);
    str_out += str_errorDescription;
    str_out += _T("\n");
    str_out += _T("File name = ");
    str_out += (LPCTSTR)co_ex.m_strFileName;
    str_out += _T("\n\n");

    co_ex.Delete();

    ERROR_TRACE (str_out.data());

//    throw CT_KaiException (CT_KaiException::ec_LoadError,
//                           _T("KaiGeneric.cpp"),
//                           _T(""),
//                           _T(__FUNCTION__),
//                           str_out);

}    //  V_ReportMFCFileException

//
// Registry access
//
bool KaiLibMFC::B_SetConfigValue (const std::wstring& str_param, 
                                  const std::wstring& str_value)
{
#pragma push_macro ("new")
#undef new

    std::wstring str_subKey (_T(""));
    std::wstring str_valueName(_T(""));
    B_MakeKeyString (str_param, str_subKey, str_valueName);
    
    try
    {
        RegistryKey ^ pco_hklm = Registry::LocalMachine;
        RegistryKey ^ pco_subKey = pco_hklm->CreateSubKey (gcnew String (str_subKey.data()));
        pco_subKey->SetValue (gcnew String (str_valueName.data()), 
                              gcnew String (str_value.data()));
    }
    catch (Exception ^ ex_)
    {
        CString cstr_ (_T("Registry write error: "));
        cstr_ += CString (ex_->Message);
        cstr_ += _T("\nKey: ");
        cstr_ += str_subKey.data();
        cstr_ += _T("\nValue: ");
        cstr_ += str_value.data();

        AfxMessageBox (cstr_);
        ATLASSERT(0);
        return false;
    }
    return true;

#pragma pop_macro ("new")

}    //  b_SetStringConfigValue

bool KaiLibMFC::B_GetConfigValue (const std::wstring& str_param, std::wstring& str_value)
{
#pragma push_macro ("new")
#undef new

    std::wstring str_subKey (_T(""));
    std::wstring str_valueName(_T(""));

    bool b_result = B_MakeKeyString (str_param, str_subKey, str_valueName);
    if (!b_result)
    {
        return false;
    }

    try
    {
        RegistryKey ^ pco_hklm = Registry::LocalMachine;
        String ^ csstr_subKey = gcnew String (str_subKey.data());
        RegistryKey ^ pco_subKey = pco_hklm->OpenSubKey (csstr_subKey);
        String ^ csstr_valueName = gcnew String (str_valueName.data());
        RegistryValueKind co_valueKind = pco_subKey->GetValueKind (csstr_valueName);
        if (RegistryValueKind::String != co_valueKind)
        {
            AfxMessageBox (_T("Bad Registry value type; string expected"));
            ATLASSERT(0);
            return false;
        }

        String ^ csstr_value = (String ^)pco_subKey->GetValue (csstr_valueName);
        if (csstr_value->Length >= 0)
        {
            CString cstr_ (csstr_value);
            str_value = cstr_.GetBuffer();
        }
        else
        {
            ATLASSERT(0);
            AfxMessageBox (_T("No value"));
        }

        return true;
    }
    catch (Exception ^ ex_)
    {
        CString cstr_ (_T("Registry read error: "));
        cstr_ += CString (ex_->Message);
        cstr_ += _T("\nKey: ");
        cstr_ += str_subKey.data();
        AfxMessageBox (cstr_);

        ATLASSERT(0);
        return false;
    }

#pragma pop_macro ("new")

}   //  B_GetConfigValue()

bool KaiLibMFC::B_GetConfigValue (const std::wstring& str_param, bool& b_value)
{
#pragma push_macro ("new")
#undef new

    std::wstring str_subKey (_T(""));
    std::wstring str_valueName(_T(""));

    bool b_result = B_MakeKeyString (str_param, str_subKey, str_valueName);
    if (!b_result)
    {
        return false;
    }

    try
    {
        RegistryKey ^ pco_hklm = Registry::LocalMachine;
        String ^ csstr_subKey = gcnew String (str_subKey.data());
        RegistryKey ^ pco_subKey = pco_hklm->OpenSubKey (csstr_subKey);
        String ^ csstr_valueName = gcnew String (str_valueName.data());
        RegistryValueKind co_valueKind = pco_subKey->GetValueKind (csstr_valueName);
        if (RegistryValueKind::Binary != co_valueKind)
        {
            ATLASSERT(0);
            AfxMessageBox (_T("Bad Registry value type; binary expected"));
            return false;
        }

        array<Byte>^ arr_value = (array<Byte> ^)pco_subKey->GetValue (csstr_valueName);
        if (arr_value->Length >= 0)
        {
            if (0 == arr_value[arr_value->Length-1])
            {
                b_value = false;
            }
            else
            {
                b_value = true;
            }
        }
        else
        {
            ATLASSERT(0);
            AfxMessageBox (_T("No value, assume \'false\'"));
            b_value = false;
        }

        return true;
    }
    catch (Exception ^ ex_)
    {
        CString cstr_ (_T("Registry read error: "));
        cstr_ += CString (ex_->Message);
        cstr_ += _T("\nKey: ");
        cstr_ += str_subKey.data();

        AfxMessageBox (cstr_);
        ATLASSERT(0);
        return false;

    }

#pragma pop_macro ("new")

}   //  B_GetConfigValue()

bool KaiLibMFC::B_GetConfigValue (const std::wstring& str_param, int& i_value)
{
#pragma push_macro ("new")
#undef new

    std::wstring str_subKey (_T(""));
    std::wstring str_valueName(_T(""));

    bool b_result = B_MakeKeyString (str_param, str_subKey, str_valueName);
    if (!b_result)
    {
        return false;
    }

    try
    {
        RegistryKey ^ pco_hklm = Registry::LocalMachine;
        String ^ csstr_subKey = gcnew String (str_subKey.data());
        RegistryKey ^ pco_subKey = pco_hklm->OpenSubKey (csstr_subKey);
        String ^ csstr_valueName = gcnew String (str_valueName.data());
        RegistryValueKind co_valueKind = pco_subKey->GetValueKind (csstr_valueName);
        if (RegistryValueKind::DWord != co_valueKind)
        {
            ATLASSERT(0);
            AfxMessageBox (_T("Bad Registry value type; DWord expected"));
            return false;
        }

        System::Int32 ^ i32_value = (System::Int32)pco_subKey->GetValue (csstr_valueName);
        i_value = *i32_value;
        return true;
    }
    catch (Exception ^ ex_)
    {
        CString cstr_ (_T("Registry read error: "));
        cstr_ += CString (ex_->Message);
        cstr_ += _T("\nKey: ");
        cstr_ += str_subKey.data();

        AfxMessageBox (cstr_);
        ATLASSERT(0);
        return false;

    }

#pragma pop_macro ("new")

}   // B_GetConfigValue()

bool KaiLibMFC::B_GetConfigValue (const std::wstring& str_param, unsigned int& ui_value)
{
#pragma push_macro ("new")
#undef new

    std::wstring str_subKey (_T(""));
    std::wstring str_valueName(_T(""));

    bool b_result = B_MakeKeyString (str_param, str_subKey, str_valueName);
    if (!b_result)
    {
        return false;
    }

    try
    {
        RegistryKey ^ pco_hklm = Registry::LocalMachine;
        String ^ csstr_subKey = gcnew String (str_subKey.data());
        RegistryKey ^ pco_subKey = pco_hklm->OpenSubKey (csstr_subKey);
        String ^ csstr_valueName = gcnew String (str_valueName.data());
        RegistryValueKind co_valueKind = pco_subKey->GetValueKind (csstr_valueName);
        if (RegistryValueKind::DWord != co_valueKind)
        {
            ATLASSERT(0);
            AfxMessageBox (_T("Bad Registry value type; DWord expected"));
            return false;
        }

        System::Int32 ^ i32_value = (System::Int32)pco_subKey->GetValue (csstr_valueName);
        ui_value = *i32_value;
        return true;
    }
    catch (Exception ^ ex_)
    {
        CString cstr_ (_T("Registry read error: "));
        cstr_ += CString (ex_->Message);
        cstr_ += _T("\nKey: ");
        cstr_ += str_subKey.data();

        AfxMessageBox (cstr_);
        ATLASSERT(0);
        return false;
    }

#pragma pop_macro ("new")

}   // B_GetConfigValue()

void KaiLibMFC::V_CreateTimeStamp (std::wstring& str_timeStamp)
{
    TCHAR pchr_date[32];
    TCHAR pchr_time[32];
    SYSTEMTIME st_time;
    GetLocalTime (&st_time);
    GetDateFormat (LOCALE_USER_DEFAULT, 0, &st_time, _T("yyyy-MM-dd"), pchr_date, 32);
    GetTimeFormat (LOCALE_USER_DEFAULT, 0, &st_time, _T("HH:mm:ss"), pchr_time, 32);

    CString cstr_date (pchr_date);
    CString cstr_time (pchr_time);

    CString cstr_milliseconds;
    cstr_milliseconds.Format (_T(".%03d"), st_time.wMilliseconds);
    cstr_time += cstr_milliseconds;

    str_timeStamp = std::wstring (cstr_date) + 
                    std::wstring (_T("\t")) + 
                    std::wstring (cstr_time) + 
                    std::wstring (_T("\t"));
}

bool KaiLibMFC::B_WriteTraceMsg (ET_TraceMsgType eo_type, 
                                 const std::wstring& str_file, 
                                 int i_line, 
                                 const std::wstring& str_function, 
                                 const std::wstring& str_msgText)
{
    //
    // Assemble the message
    //
    TCHAR pchr_date[32];
    TCHAR pchr_time[32];
    SYSTEMTIME st_time;
    GetLocalTime (&st_time);
    GetDateFormat (LOCALE_USER_DEFAULT, 0, &st_time, _T("yyyy-MM-dd"), pchr_date, 32);
    GetTimeFormat (LOCALE_USER_DEFAULT, 0, &st_time, _T("HH:mm:ss"), pchr_time, 32);

    CString cstr_date (pchr_date);
    CString cstr_time (pchr_time);

    CString cstr_milliseconds;
    cstr_milliseconds.Format (_T(".%03d"), st_time.wMilliseconds);
    cstr_time += cstr_milliseconds;

    bool b_return = false;

    bool b_errorTrace = false;
    b_return = B_GetConfigValue (std::wstring (_T("Trace.Error Trace")), b_errorTrace);
    if (!b_return)
    {
        b_errorTrace = true;
    }

    bool b_infoTrace = false;
    b_return = B_GetConfigValue (std::wstring (_T("Trace.Info Trace")), b_infoTrace);
    if (!b_return)
    {
        b_infoTrace = false;
    }

    CString cstr_type;
    switch (eo_type)
    {
        case ec_ErrorTraceMsg:
        {
            if (!b_errorTrace)
            {
                return true;
            }
            cstr_type = _T("ERROR");
            break;
        }
        case ec_InfoTraceMsg:
        {
            if (!b_infoTrace)
            {
                return true;
            }
            cstr_type = _T("INFO");
            break;
        }
        case ec_DebugMsg:
        {
            cstr_type = _T("DEBUG");
            break;
        }
        case ec_DumpTraceMsg:
        {
            b_return = B_TraceOutput (std::wstring (_T("\t")) + str_msgText 
             + std::wstring (_T("\r\n")));
            return true;
        }
        default:
        {
            B_TraceOutput (std::wstring (_T("b_WriteTraceMsg: unknown nessage type")));
            return false;
        }

    }    // switch (eo_type)

    CString cstr_text;
    cstr_text.Format (_T("%s\t%s\t%s\t%7d\t%s\t%s\t%s"), 
                      cstr_date,
                      cstr_time,
                      str_file.c_str(),
                      i_line,
                      _T("ERROR"),
                      str_function.c_str(),
                      str_msgText.c_str());

    b_return = B_TraceOutput (std::wstring(cstr_text));

    //
    // Also send to debugger window:
    //
    std::wstring str_out (_T(" *** "));
    str_out += str_function;
    str_out += _T(": ");
    str_out += str_msgText;
    str_out += _T("\n");
    #ifdef _DEBUG
        ATLTRACE2 (str_out.c_str());
    #endif

    return b_return;

}    // bool B_WriteTraceMsgEx (...)

//
// Convert a dotted-style param name into registry key & value pair
//
bool KaiLibMFC::B_MakeKeyString (const std::wstring& str_paramName,
                                 std::wstring& str_regKey,
                                 std::wstring& str_regValueName)
{
    V_GetRegistryPath (str_regKey);

    CT_KaiString kstr_paramName (str_paramName, _T("."));
    if (kstr_paramName.i_GetNumOfFields() < 2) // key + value name
    {
        ATLASSERT(0);
        AfxMessageBox (_T("Less than two tokens in param name"));
        return false;
    }

    int i_token = 0;
    for (i_token = 0; 
         i_token < (kstr_paramName.i_GetNumOfFields()-1);
         ++i_token)
    {
        str_regKey += _T('\\');
        str_regKey += kstr_paramName.str_GetField (i_token);
    }
    str_regValueName = kstr_paramName.str_GetField (i_token);

    return true;
}

//
// Tracing may not be available... Errors are reported only if manadatory data are missing
//
bool KaiLibMFC::B_GetVersionInfo (const std::wstring& str_module, ST_VersionInfo& st_versionInfo)
{
    TCHAR sz_filePath[MAX_PATH];

    DWORD dw_length = GetModuleFileName (GetModuleHandle (str_module.data()), sz_filePath, MAX_PATH);
    if (0 == dw_length)
    {
        AfxMessageBox (_T("B_GetVersionInfo(): GetModuleFileName() failed."));
        return false;
    }
    
    DWORD dw_ignore = 0;
    DWORD dw_versionInfoSize = GetFileVersionInfoSize (sz_filePath, &dw_ignore);
    if (0 == dw_versionInfoSize)
    {
        AfxMessageBox (_T("B_GetVersionInfo(): GetFileVersionInfoSize() failed."));
        return false;
    }

    BYTE * byte_versionInfo = NULL;
    ATLTRY (byte_versionInfo = new BYTE[dw_versionInfoSize]);
    if (NULL == byte_versionInfo)
    {
        return false;
    }

    BOOL ui_ret = GetFileVersionInfo (sz_filePath, 0, dw_versionInfoSize, byte_versionInfo);
    if (0 == ui_ret)
    {
        AfxMessageBox (_T("B_GetVersionInfo(): GetFileVersionInfo() failed."));
        delete[] byte_versionInfo;
        return false;
    }

    struct st_languageInfo 
    {
        WORD w_language;
        WORD w_codePage;

    } * pst_languageInfo; 

    UINT ui_infoSize = 0;
    
    // Read the list of languages and code pages.
    ui_ret = VerQueryValue (byte_versionInfo, 
                            _T ("\\VarFileInfo\\Translation"),
                            (LPVOID *)&pst_languageInfo,
                            &ui_infoSize);
    if (0 == ui_ret)
    {
        AfxMessageBox (_T("B_GetVersionInfo(): VerQueryValue() failed getting language data."));
        delete[] byte_versionInfo;
        return false;
    }

    if (ui_infoSize < sizeof (st_languageInfo))
    {
        AfxMessageBox (_T("B_GetVersionInfo(): VerQueryValue() failed getting language data."));
        delete[] byte_versionInfo;
        return false;
    }

    CString cstr_langCodePage;
    cstr_langCodePage.Format (_T("%04x%04x"), 
                              pst_languageInfo[0].w_language,
                              pst_languageInfo[0].w_codePage);

    CString cstr_versionBlock;
    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\Comments"), cstr_langCodePage);
    
    LPCTSTR sz_comments;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_comments,
                            &ui_infoSize);
    if ((0 != ui_ret) || (0 != ui_infoSize))
    {
        st_versionInfo.str_Comments = std::wstring (sz_comments);
    }

    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\CompanyName"), cstr_langCodePage);    
    LPCTSTR sz_companyName;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_companyName,
                            &ui_infoSize);
    if ((0 == ui_ret) || (0 == ui_infoSize))
    {
        AfxMessageBox 
            (_T("B_GetVersionInfo(): VerQueryValue() failed; unable to retrieve company name."));
        st_versionInfo.str_CompanyName = _T("Ling Ling San Software");
    }
    else
    {
        st_versionInfo.str_CompanyName = std::wstring (sz_companyName);
    }

    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\FileDescription"), cstr_langCodePage);
    LPCTSTR sz_fileDescription;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_fileDescription,
                            &ui_infoSize);
    if ((0 != ui_ret) || (0 != ui_infoSize))
    {
        st_versionInfo.str_FileDescription = std::wstring (sz_fileDescription);
    }

    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\InternalName"), cstr_langCodePage);
    LPCTSTR sz_internalName;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_internalName,
                            &ui_infoSize);
    if ((0 != ui_ret) || (0 != ui_infoSize))
    {
        st_versionInfo.str_InternalName = std::wstring (sz_internalName);
    }

    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\LegalCopyright"), cstr_langCodePage);
    LPCTSTR sz_legalCopyright;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_legalCopyright,
                            &ui_infoSize);
    if ((0 != ui_ret) || (0 != ui_infoSize))
    {
        st_versionInfo.str_LegalCopyright = std::wstring (sz_legalCopyright);
    }

    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\LegalTrademarks"), cstr_langCodePage);
    LPCTSTR sz_legalTrademarks;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_legalTrademarks,
                            &ui_infoSize);
    if ((0 != ui_ret) || (0 != ui_infoSize))
    {
        st_versionInfo.str_LegalTrademarks = std::wstring (sz_legalTrademarks);
    }

    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\OriginalFilename"), cstr_langCodePage);
    LPCTSTR sz_originalFilename;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_originalFilename,
                            &ui_infoSize);
    if ((0 != ui_ret) || (0 != ui_infoSize))
    {
        st_versionInfo.str_OriginalFilename = std::wstring (sz_originalFilename);
    }

    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\ProductName"), cstr_langCodePage);
    LPCTSTR sz_productName;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_productName,
                            &ui_infoSize);
    if ((0 == ui_ret) || (0 == ui_infoSize))
    {
        AfxMessageBox 
            (_T("B_GetVersionInfo(): VerQueryValue() failed; unable to retrieve product name."));
        st_versionInfo.str_ProductName = _T("Kai");
    }
    else
    {
        st_versionInfo.str_ProductName = std::wstring (sz_productName);
    }

    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\SpecialBuild"), cstr_langCodePage);
    LPCTSTR sz_specialBuild;
    ui_ret = VerQueryValue (byte_versionInfo, 
                            cstr_versionBlock.GetBuffer(),
                            (LPVOID *)&sz_specialBuild,
                            &ui_infoSize);
    if ((0 != ui_ret) || (0 != ui_infoSize))
    {
        st_versionInfo.str_SpecialBuild = std::wstring (sz_specialBuild);
    }

//    cstr_versionBlock.Format (_T("\\StringFileInfo\\%s\\ProductVersion"), cstr_langCodePage);
    void * pv_fixedFileInfo = NULL;
    unsigned int ui_bufLen = 0;
    ui_ret = VerQueryValue (byte_versionInfo,
                            _T("\\"),
                            &pv_fixedFileInfo,
                            &ui_bufLen);
    if (ui_ret == 0)
    {
        AfxMessageBox 
            (_T("B_GetVersionInfo(): VerQueryValue() failed; unable to retrieve product version."));
    }
    else
    {
        VS_FIXEDFILEINFO * pst_versionInfo = static_cast<VS_FIXEDFILEINFO *>(pv_fixedFileInfo);

        st_versionInfo.st_Build.ui_major1 = HIWORD (pst_versionInfo->dwProductVersionMS);
        st_versionInfo.st_Build.ui_minor1 = LOWORD (pst_versionInfo->dwProductVersionMS);
        st_versionInfo.st_Build.ui_major2 = HIWORD (pst_versionInfo->dwProductVersionLS);
        st_versionInfo.st_Build.ui_minor2 = LOWORD (pst_versionInfo->dwProductVersionLS);
    }

    delete[] byte_versionInfo;

    return true;

}   //  B_GetVersionInfo()

void KaiLibMFC::V_GetRegistryPath (std::wstring& str_regPath)
{
    str_regPath = _T("SOFTWARE\\Ling Ling San Software\\Kai"); //fallback string

    ST_VersionInfo st_versionInfo;
    bool b_ret = B_GetVersionInfo (_T("KaiLibMFC"), st_versionInfo);
    if (!b_ret)
    {
        return;
    }

    str_regPath = std::wstring (_T("SOFTWARE\\"));
    str_regPath += std::wstring (st_versionInfo.str_CompanyName);
    str_regPath += std::wstring (_T("\\"));
    str_regPath += std::wstring (st_versionInfo.str_ProductName);

}   //  V_GetRegistryPath()

bool KaiLibMFC::B_TraceOutput (const std::wstring& str_msg)
{
    bool b_return = false;

    std::wstring str_logPath;
    b_return = B_GetAppDataPath (str_logPath);
    if (!b_return)
    {
        return false;
    }

    str_logPath += _T("Trace\\KaiLog.txt");

    HANDLE h_file = CreateFile (str_logPath.data(),
                                GENERIC_WRITE,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_ALWAYS,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
    if (INVALID_HANDLE_VALUE == h_file)
    {
        DWORD dw_error = GetLastError();
        std::wstring str_out (_T("Error creating log file."));
        str_out += _T(":\n");
        std::wstring str_errorDescription;
        B_FormatWindowsMessage (dw_error, str_errorDescription);
        str_out += str_errorDescription;
        AfxMessageBox (str_out.data(), MB_ICONINFORMATION | MB_OK);
        ATLASSERT(0);
        return false;
    }
    else
    {
        V_CheckLogFileSize (h_file);
        DWORD dw_return = SetFilePointer (h_file, 0, NULL, FILE_END);
        if (INVALID_SET_FILE_POINTER == dw_return)
        {
            AfxMessageBox (_T("Error setting file pointer."), MB_ICONEXCLAMATION | MB_OK);
            ATLASSERT(0);
        }
        std::wstring str_out = str_msg + _T("\r\n");
        DWORD dw_bytesWritten = 0;
        BOOL ui_return = WriteFile (h_file,
                                    str_out.data(),
                                    (DWORD)sizeof (_TCHAR) * str_out.length(),
                                    &dw_bytesWritten,
                                    NULL);

        CloseHandle (h_file);

        return (TRUE == ui_return);
    }
}    //  B_TraceOutput

void KaiLibMFC::V_CheckLogFileSize (HANDLE h_file)
{
    if (!h_file)
    {
        ATLASSERT(0);
        return;
    }

    LARGE_INTEGER li_size;
    BOOL ui_return = GetFileSizeEx (h_file, &li_size);
    if (!ui_return)
    {
        AfxMessageBox (_T("Unable to obtain log file size."), MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    
    if (li_size.LowPart < 1048576)
    {
        return;
    }

    std::wstring str_dirPath;
    bool b_return = B_GetAppDataPath (str_dirPath);
    if (!b_return)
    {
        return;
    }

    std::wstring str_logFilePath = str_dirPath + std::wstring (_T("Trace\\KaiLog.txt"));
    std::wstring str_archivePath = str_dirPath + std::wstring (_T("Trace\\KaiLogArchive.txt"));
    ui_return = CopyFile (str_logFilePath.data(), 
                          str_archivePath.data(), 
                          FALSE); // overrite existing
    if (!ui_return)
    {
        AfxMessageBox (_T("Unable to archive existing log."), MB_ICONEXCLAMATION | MB_OK);
        ATLASSERT(0);
        return;
    }

    DWORD dw_return = SetFilePointer (h_file, 0, NULL, FILE_BEGIN);
    if (INVALID_SET_FILE_POINTER == dw_return)
    {
        AfxMessageBox (_T("Unable to delete old log."), MB_ICONEXCLAMATION | MB_OK);
        ATLASSERT(0);
        return;
    }

    ui_return = SetEndOfFile (h_file);
    if (!ui_return)
    {
        AfxMessageBox (_T("Unable to reset end-of-file on the log file."),
                       MB_ICONEXCLAMATION | MB_OK);
        ATLASSERT(0);
    }

}   //  V_CheckLogFileSize();

bool KaiLibMFC::B_GetAppDataPath (std::wstring& str_path)
{
    TCHAR pchr_path[MAX_PATH + 1] = {0};
    HRESULT h_r = SHGetFolderPath (NULL, CSIDL_LOCAL_APPDATA, NULL, 0, pchr_path);
    if (S_OK != h_r)
    {
        AfxMessageBox (_T("Unable to obtain log folder path."), 
                       MB_ICONEXCLAMATION | MB_OK);
        ATLASSERT(0);
        return false;
    }

    str_path = pchr_path;

    ST_VersionInfo st_versionInfo;
    bool b_ret = B_GetVersionInfo (_T("KaiLibMFC"), st_versionInfo);
    if (b_ret)
    {
        str_path += _T("\\") + st_versionInfo.str_CompanyName + _T("\\");
    }
    else
    {
        AfxMessageBox (_T("Unable to obtain company name; using default."), 
                       MB_ICONEXCLAMATION | MB_OK);
        ATLASSERT(0);
        str_path = pchr_path + std::wstring (_T("\\Ling Ling San Software\\"));
    }

    return true;
}

//
// Wrapper for C# libraries
//
void KaiLibMFC::LogError (LPCTSTR sz_fileName, int i_line, LPCTSTR sz_method, LPCTSTR sz_message)
{
    B_WriteTraceMsg (ec_ErrorTraceMsg, 
                     sz_fileName, 
                     i_line, 
                     sz_method, 
                     sz_message);
}

TCHAR * KaiLibMFC::GetRegistryPath()
{
    std::wstring str_regPath;
    V_GetRegistryPath (str_regPath);
    TCHAR * comstring_ = (TCHAR *)CoTaskMemAlloc (sizeof (TCHAR)*(str_regPath.length()+1));
    _tcscpy_s (comstring_, str_regPath.length()+1, str_regPath.c_str());
    return comstring_;
}
