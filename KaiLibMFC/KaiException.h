//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: general-purpose exceprion class.
//
//    $Id: KaiException.h,v 1.19 2007-12-17 08:16:50 kostya Exp $
//    
//==============================================================================

#pragma once

namespace KaiLibMFC
{

class AFX_EXT_CLASS CT_KaiException
{

public:
    enum ET_Exception;

protected:
    ET_Exception eo_NumericCode_;
    std::wstring str_Module_;
    std::wstring str_Class_;
    std::wstring str_Method_;
    std::wstring str_Description_;

//
// Numeric codes
//
public:
    enum ET_Exception
    {
        ec_ExceptionFront =    10001,
        ec_IndexOutOfBounds,
        ec_StringFieldOutOfRange,
        ec_FontIndexOutOfBounds,
        ec_WordIndexOutOfBounds,
        ec_LineIndexOutOfBounds,
        ec_ParagraphNotRendered,
        ec_ParagraphNotFound,
        ec_OutOfMemory,
        ec_LoadError,
        ec_StoreError,
        ec_IllegalValue,
        ec_UnknownValue,
        ec_IncorrectLength,
        ec_IncorrectOffset,
        ec_BadConfigParamName,
        ec_BadRegDataType,
        ec_BadFontDescriptor,
        ec_FileError,
        ec_ExceptionBack  =    10099
    };

//
// C-tors and d-tors
//
public:
    CT_KaiException (const ET_Exception eo_code, 
                     const std::wstring& str_module,
                     const std::wstring& str_class,
                     const std::wstring& str_method,
                     const std::wstring& str_description);

    CT_KaiException (const ET_Exception eo_code,
                     const _TCHAR * const qcz_module, 
                     const _TCHAR * const qcz_class,
                     const _TCHAR * const qcz_method,
                     const _TCHAR * const qcz_description);

    ~CT_KaiException() {};

protected:
    CT_KaiException() {};    // disabled

public:
    void v_Report();
    void v_MessageBox();
    std::wstring str_FormatAll();

    ET_Exception eo_GetNumericCode()
    {
        return eo_NumericCode_;
    }
    std::wstring str_GetModuleName()
    {
        return str_Module_;
    }
    std::wstring str_GetClassName()
    {
        return str_Class_;
    }
    std::wstring str_GetMethodName()
    {
        return str_Method_;
    }
    std::wstring str_GetDescriptionName()
    {
        return str_Description_;
    }
};

}   //  namespace KaiLibMFC
