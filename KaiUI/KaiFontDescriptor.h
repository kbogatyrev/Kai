//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: interface for the font descriptor class.
//
//    $Id: KaiFontDescriptor.h,v 1.19 2007-12-23 19:55:52 kostya Exp $
//
//==============================================================================

#pragma once

using namespace KaiLibMFC;

class CT_FontDescriptor
{

protected:
    //
    // Persistent properties:
    //
    CT_KaiString kstr_Typeface_;
    ET_Charset eo_Charset_;
    unsigned int ui_PointSize_;
    bool b_Bold_;
    bool b_Italic_;
    bool b_Underline_;
    bool b_Strikeout_;
    unsigned int ui_ReferenceCount_;

    //
    // Volatile property:
    //
    CFont * pco_WindowsFont_;
    CFont * pco_PrintFont_;

public:
    CT_FontDescriptor();
    CT_FontDescriptor (const CT_KaiString& kstr_typeface,
                       const ET_Charset& eo_charset,
                       const unsigned int ui_size,
                       const bool b_bold = false,
                       const bool b_italic = false,
                       const bool b_underline = false,
                       const bool b_strikeout = false);
    CT_FontDescriptor (const CT_FontDescriptor& co_fd);
    ~CT_FontDescriptor();
    void v_Null();
    CT_FontDescriptor& operator = (const CT_FontDescriptor& co_fd);
//    bool operator == (const CT_FontDescriptor& co_fd) const;
    bool operator == (CT_FontDescriptor * pco_fd) const;
    bool operator < (const CT_FontDescriptor& co_fd) const;

    bool b_Load (CFile&);
    bool b_Store (CFile&);

    unsigned int ui_Hash();

    bool b_Modify (ET_FontModifier eo_newStyle);
    bool b_IsStylePresent (ET_FontModifier eo_style);

    void v_SetTypeface (const CT_KaiString& kstr_typeface)
    {
        kstr_Typeface_ = kstr_typeface;
    }

    const CT_KaiString& kstr_GetTypeface() const
    {
        return kstr_Typeface_;
    }

    void v_SetCharset (ET_Charset eo_charset)
    {
        eo_Charset_ = eo_charset;
    }

    ET_Charset eo_GetCharset() const
    {
        return eo_Charset_;
    }

    void v_SetPointSize (unsigned int ui_pointSize)
    {
        ui_PointSize_ = ui_pointSize;
    }

    unsigned int ui_GetPointSize() const
    { 
        return ui_PointSize_; 
    }

    void v_SetBold (bool b_bold)
    { 
        b_Bold_ = b_bold; 
    }

    bool b_GetBold() const
    { 
        return b_Bold_; 
    }

    void v_SetItalic (bool b_italic)
    {
        b_Italic_ = b_italic; 
    }
    
    bool b_GetItalic() const
    { 
        return b_Italic_; 
    }
    
    void v_SetUnderline (bool b_underline)
    { 
        b_Underline_ = b_underline; 
    }
    
    bool b_GetUnderLine() const
    { 
        return b_Underline_;
    }
    
    void v_SetStrikeOut (bool b_strikeout)
    { 
        b_Strikeout_ = b_strikeout; 
    }
    
    bool b_GetStrikeOut() const
    { 
        return b_Strikeout_; 
    }
    
    void v_SetReferenceCount (unsigned int ui_referenceCount)
    { 
        ui_ReferenceCount_ = ui_referenceCount; 
    }
    
    unsigned int ui_GetReferenceCount() const
    { 
        return ui_ReferenceCount_; 
    }

    CFont * pco_GetWindowsFont();
    CFont * pco_GetPrintFont (CDC *);

    void v_ResetPrintFont()
    {
        if (pco_PrintFont_)
        {
            delete pco_PrintFont_;
            pco_PrintFont_ = NULL;
        }
    }

    void v_Dump();
};
