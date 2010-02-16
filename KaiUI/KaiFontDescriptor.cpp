//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: implementation of the internal font descriptor class.
//
//    $Id: KaiFontDescriptor.cpp,v 1.32 2007-12-23 19:54:40 kostya Exp $
//
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning (disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "kai_hash.h"
#include "KaiFontDescriptor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern "C"
//{
//unsigned long hash (unsigned long * pul_key, 
//                    unsigned long ui_length, 
//                    unsigned long ui_initval);
//}

CT_FontDescriptor::CT_FontDescriptor()
{
    pco_WindowsFont_ = NULL;
    pco_PrintFont_ = NULL;
    v_Null();
}

CT_FontDescriptor::CT_FontDescriptor (const CT_KaiString& kstr_typeface,
                                      const ET_Charset& eo_charset,
                                      const unsigned int ui_size,
                                      const bool b_bold,
                                      const bool b_italic,
                                      const bool b_underline,
                                      const bool b_strikeout)
  : kstr_Typeface_ (kstr_typeface),
    eo_Charset_ (eo_charset),
    ui_PointSize_ (ui_size),
    b_Bold_ (b_bold),
    b_Italic_ (b_italic),
    b_Underline_ (b_underline),
    b_Strikeout_ (b_strikeout),
    ui_ReferenceCount_ (0),
    pco_WindowsFont_ (NULL),
    pco_PrintFont_ (NULL)
{
}

CT_FontDescriptor::CT_FontDescriptor (const CT_FontDescriptor& co_fd)
{
    kstr_Typeface_ = co_fd.kstr_Typeface_;
    eo_Charset_ = co_fd.eo_Charset_;
    ui_PointSize_ = co_fd.ui_PointSize_;
    b_Bold_ = co_fd.b_Bold_;
    b_Italic_ = co_fd.b_Italic_;
    b_Underline_ = co_fd.b_Underline_;
    b_Strikeout_ = co_fd.b_Strikeout_;
    ui_ReferenceCount_ = co_fd.ui_ReferenceCount_;
    pco_WindowsFont_ = co_fd.pco_WindowsFont_;
    pco_PrintFont_ = co_fd.pco_PrintFont_;
}

CT_FontDescriptor::~CT_FontDescriptor()
{
    v_Null();
}

void CT_FontDescriptor::v_Null()
{
    kstr_Typeface_ = _T("");
    eo_Charset_ = ec_CharsetFront;
    ui_PointSize_ = 0;
    b_Bold_ = false;
    b_Italic_ = false;
    b_Underline_ = false;
    b_Strikeout_ = false;
    ui_ReferenceCount_ = 0;
    if (pco_WindowsFont_)
    {
        delete pco_WindowsFont_;
        pco_WindowsFont_ = NULL;
    }
    if (pco_PrintFont_)
    {
        delete pco_PrintFont_;
        pco_PrintFont_ = NULL;
    }
}

CT_FontDescriptor& CT_FontDescriptor::operator = 
                                       (const CT_FontDescriptor& co_fd)
{
    kstr_Typeface_ = co_fd.kstr_Typeface_;
    eo_Charset_ = co_fd.eo_Charset_;
    ui_PointSize_ = co_fd.ui_PointSize_;
    b_Bold_ = co_fd.b_Bold_;
    b_Italic_ = co_fd.b_Italic_;
    b_Underline_ = co_fd.b_Underline_;
    b_Strikeout_ = co_fd.b_Strikeout_;
    ui_ReferenceCount_ = co_fd.ui_ReferenceCount_;
    pco_WindowsFont_ = co_fd.pco_WindowsFont_;
    pco_PrintFont_ = co_fd.pco_PrintFont_;

    return *this;
}
/*
bool CT_FontDescriptor::operator == (const CT_FontDescriptor& co_fd) const
{
    if (kstr_Typeface_ != co_fd.kstr_Typeface_)
    {
        INFO_TRACE ("kstr_Typeface_ != co_fd.kstr_Typeface_")
        return false;
    }
    if (eo_Charset_ != co_fd.eo_Charset_)
    {
        INFO_TRACE ("eo_Charset_ != co_fd.eo_Charset_")
        return false;
    }    
    if (ui_PointSize_ != co_fd.ui_PointSize_)
    {
        INFO_TRACE ("ui_PointSize_ != co_fd.ui_PointSize_")
        return false;
    }
    if (b_Bold_ != co_fd.b_Bold_)
    {
        INFO_TRACE ("b_Bold_ != co_fd.b_Bold_")
        return false;
    }
    if (b_Italic_ != co_fd.b_Italic_)
    {
        INFO_TRACE ("!b_Italic_ != co_fd.b_Italic_")
        return false;
    }
    if (b_Underline_ != co_fd.b_Underline_)
    {
        INFO_TRACE ("b_Underline_ != co_fd.b_Underline_")
        return false;
    }
    if (b_Strikeout_ != co_fd.b_Strikeout_)
    {
        INFO_TRACE ("!b_StrikeOut_ && co_fd.b_Strikeout_")
        return false;
    }

    return true;

}  //  bool CT_FontDescriptor::operator == (CT_FontDescriptor& co_fd)
*/
bool CT_FontDescriptor::operator == (CT_FontDescriptor * pco_fd) const
{
    if (kstr_Typeface_ != pco_fd->kstr_Typeface_)
    {
        INFO_TRACE (_T("kstr_Typeface_ != pco_fd->kstr_Typeface_"))
        return false;
    }
    if (eo_Charset_ != pco_fd->eo_Charset_)
    {
        INFO_TRACE (_T("eo_Charset_ != pco_fd->eo_Charset_"))
        return false;
    }    
    if (ui_PointSize_ != pco_fd->ui_PointSize_)
    {
        INFO_TRACE (_T("ui_PointSize_ != pco_fd->ui_PointSize_"))
        return false;
    }
    if (b_Bold_ != pco_fd->b_Bold_)
    {
        INFO_TRACE (_T("b_Bold_ != pco_fd->b_Bold_"))
        return false;
    }
    if (b_Italic_ != pco_fd->b_Italic_)
    {
        INFO_TRACE (_T("!b_Italic_ != pco_fd->b_Italic_"))
        return false;
    }
    if (b_Underline_ != pco_fd->b_Underline_)
    {
        INFO_TRACE (_T("b_Underline_ != pco_pd->b_Underline_"))
        return false;
    }
    if (b_Strikeout_ != pco_fd->b_Strikeout_)
    {
        INFO_TRACE (_T("!b_StrikeOut_ && pco_fd->b_Strikeout_"))
        return false;
    }

    return true;

}  //  bool CT_FontDescriptor::operator == (CT_FontDescriptor& co_fd)

bool CT_FontDescriptor::operator < (const CT_FontDescriptor& co_fd) const
{
    if (kstr_Typeface_ < co_fd.kstr_Typeface_)
    {
        INFO_TRACE (_T("kstr_Typeface_ < co_fd.kstr_Typeface_"))
        return true;
    }
    if (kstr_Typeface_ > co_fd.kstr_Typeface_)
    {
        INFO_TRACE (_T("kstr_Typeface_ > co_fd.kstr_Typeface_"))
        return false;
    }

    if (eo_Charset_ < co_fd.eo_Charset_)
    {
        INFO_TRACE (_T("eo_Charset_ < co_fd.eo_Charset_"))
        return true;
    }
    if (eo_Charset_ > co_fd.eo_Charset_)
    {
        INFO_TRACE (_T("eo_Charset_ > co_fd.eo_Charset_"))
        return false;
    }
    
    if (ui_PointSize_ < co_fd.ui_PointSize_)
    {
        INFO_TRACE (_T("ui_PointSize_ < co_fd.ui_PointSize_"))
        return true;
    }
    if (ui_PointSize_ > co_fd.ui_PointSize_)
    {
        INFO_TRACE (_T("ui_PointSize_ > co_fd.ui_PointSize_"))
        return false;
    }

    if (!b_Bold_ && co_fd.b_Bold_)
    {
        INFO_TRACE (_T("!b_Bold_ && co_fd.b_Bold_"))
        return true;
    }
    if (b_Bold_ && !co_fd.b_Bold_)
    {
        INFO_TRACE (_T("b_Bold_ && !co_fd.b_Bold_"))
        return false;
    }

    if (!b_Italic_ && co_fd.b_Italic_)
    {
        INFO_TRACE (_T("!b_Italic_ && co_fd.b_Italic_"))
        return true;
    }
    if (b_Italic_ && !co_fd.b_Italic_)
    {
        INFO_TRACE (_T("b_Italic_ && !co_fd.b_Italic_"))
        return false;
    }

    if (!b_Underline_ && co_fd.b_Underline_)
    {
        INFO_TRACE (_T("!b_Underline_ && co_fd.b_Underline_"))
        return true;
    }
    if (b_Underline_ && !co_fd.b_Underline_)
    {
        INFO_TRACE (_T("b_Underline_ && !co_fd.b_Underline_"))
        return false;
    }

    if (!b_Strikeout_ && co_fd.b_Strikeout_)
    {
        INFO_TRACE (_T("!b_Strikeout_ && co_fd.b_Strikeout_"))
        return true;
    }
    if (b_Strikeout_ && !co_fd.b_Strikeout_)
    {
        INFO_TRACE (_T("b_Strikeout_ && !co_fd.b_Strikeout_"))
        return false;
    }

    return false;

}  //  bool CT_FontDescriptor::operator < (CT_FontDescriptor& co_fd)

bool CT_FontDescriptor::b_Load (CFile& co_inFile)
{
    unsigned int ui_bytesRead = 0;
    try
    {
        //
        //  kstr_Typeface_
        //
        V_LoadStlString (co_inFile, kstr_Typeface_);
        V_LoadItem (co_inFile, eo_Charset_);
        V_LoadItem (co_inFile, ui_PointSize_);
        V_LoadItem (co_inFile, b_Bold_);
        V_LoadItem (co_inFile, b_Italic_);
        V_LoadItem (co_inFile, b_Underline_);
        V_LoadItem (co_inFile, b_Strikeout_);
        V_LoadItem (co_inFile, ui_ReferenceCount_);
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }

    return true;

}    //  b_Load (...)

bool CT_FontDescriptor::b_Store (CFile& co_outFile)
{
    try
    {
        V_StoreStlString (co_outFile, kstr_Typeface_);
        co_outFile.Write (&eo_Charset_, sizeof (ET_Charset));
        co_outFile.Write (&ui_PointSize_, sizeof (unsigned int));
        co_outFile.Write (&b_Bold_, sizeof (bool));
        co_outFile.Write (&b_Italic_, sizeof (bool));
        co_outFile.Write (&b_Underline_, sizeof (bool));
        co_outFile.Write (&b_Strikeout_, sizeof (bool));
        co_outFile.Write (&ui_ReferenceCount_, sizeof (unsigned int));
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, WRITE_ERROR);
        return false;
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }

    return true;

}    // b_Store (...)

CFont * CT_FontDescriptor::pco_GetWindowsFont()
{ 
    if (pco_WindowsFont_ == NULL)
    {
        LOGFONT st_logFont;

        st_logFont.lfHeight = ui_PointSize_ * 10;    
        st_logFont.lfWidth = 0;
        st_logFont.lfEscapement = 0; 
        st_logFont.lfOrientation = 0; 
        st_logFont.lfWeight = b_Bold_ ? FW_BOLD : FW_NORMAL; 
        st_logFont.lfItalic = b_Italic_;
        st_logFont.lfUnderline = b_Underline_; 
        st_logFont.lfStrikeOut = b_Strikeout_; 
        st_logFont.lfCharSet = eo_Charset_; 
        st_logFont.lfOutPrecision = OUT_DEFAULT_PRECIS; 
        st_logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS; 
        st_logFont.lfQuality = DEFAULT_QUALITY; 
        st_logFont.lfPitchAndFamily = DEFAULT_PITCH;

        _tcscpy_s (st_logFont.lfFaceName, LF_FACESIZE, kstr_Typeface_.c_str());
 
        pco_WindowsFont_ = new CFont;
        BOOL mfcb_result = pco_WindowsFont_->CreatePointFontIndirect (&st_logFont);
        if (!mfcb_result)
        {
            delete pco_WindowsFont_;
            pco_WindowsFont_ = NULL;
        }

    }
    return pco_WindowsFont_; 

}    // pco_GetWindowsFont()

CFont * CT_FontDescriptor::pco_GetPrintFont (CDC * pco_printDc)
{
    if (pco_PrintFont_ == NULL)
    {
        LOGFONT st_logFont;

        st_logFont.lfHeight = ui_PointSize_ * 10;    
        st_logFont.lfWidth = 0;
        st_logFont.lfEscapement = 0; 
        st_logFont.lfOrientation = 0; 
        st_logFont.lfWeight = b_Bold_ ? FW_BOLD : FW_NORMAL; 
        st_logFont.lfItalic = b_Italic_;
        st_logFont.lfUnderline = b_Underline_; 
        st_logFont.lfStrikeOut = b_Strikeout_; 
        st_logFont.lfCharSet = eo_Charset_; 
        st_logFont.lfOutPrecision = OUT_DEFAULT_PRECIS; 
        st_logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS; 
        st_logFont.lfQuality = DEFAULT_QUALITY; 
        st_logFont.lfPitchAndFamily = DEFAULT_PITCH; 
        _tcscpy_s (st_logFont.lfFaceName, LF_FACESIZE, kstr_Typeface_.c_str());
 
        pco_PrintFont_ = new CFont;
        BOOL mfcb_result = 
                pco_PrintFont_->CreatePointFontIndirect (&st_logFont, 
                                                         pco_printDc);
        if (!mfcb_result)
        {
            delete pco_PrintFont_;
            pco_PrintFont_ = NULL;
        }
    }
    return pco_PrintFont_; 

}   // pco_GetPrintFont (CDC * pco_dc)

unsigned int CT_FontDescriptor::ui_Hash()
{
    struct ST_SourceData
    {
        _TCHAR arrchr_name[100];
        unsigned int ui_charset;
        unsigned int ui_size;
        bool b_bold;
        bool b_italic;
        bool b_underline;
        bool b_strikeout;

    } st_sourceData;

    memset (&st_sourceData, 0, sizeof(ST_SourceData));
    _tcscpy_s (st_sourceData.arrchr_name, LF_FACESIZE, kstr_GetTypeface().data());
    st_sourceData.ui_charset = (unsigned int)eo_Charset_;
    st_sourceData.ui_size = ui_PointSize_;
    st_sourceData.b_bold = b_Bold_;
    st_sourceData.b_italic = b_Italic_;
    st_sourceData.b_underline = b_Underline_;
    st_sourceData.b_strikeout = b_Strikeout_;

    unsigned long ui_v = KaiLibMFC::hash ((unsigned char *)&st_sourceData, 
                                          sizeof (ST_SourceData), 
                                          14563284);
    return ui_v;
}

bool CT_FontDescriptor::b_IsStylePresent (ET_FontModifier eo_style)
{
    switch (eo_style)
    {
        case ec_Bold:
        {
            return b_Bold_;
        }
        case ec_Italic:
        {
            return b_Italic_;
        }
        case ec_Underline:
        {
            return b_Underline_;
        }
        default:
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Unknown font style."));
            return false;
        }
    }
}

bool CT_FontDescriptor::b_Modify (ET_FontModifier eo_newStyle)
{
    bool b_add = true;
    switch (eo_newStyle)
    {
        case ec_Bold:
        {
            b_Bold_ = !b_Bold_;
            break;
        }
        case ec_Italic:
        {
            b_Italic_ = !b_Italic_;
            break;
        }
        case ec_Underline:
        {
            b_Underline_ = !b_Underline_;
            break;
        }
        default:
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Unknown font style."));
            return false;
        }
    }

    return true;

}   // b_Modify()

void CT_FontDescriptor::v_Dump()
{
    DUMP_TRACE (_T("-------- CT_FontDescriptor ------------------"));
    DUMP_TRACE (STL_STRING (_T("\tkstr_Typeface_ = ")) + kstr_Typeface_);
    STL_STRING str_charset = STR_GetCharsetName (eo_Charset_);
    DUMP_TRACE (STL_STRING (_T("\teo_Charset_ = ")) + str_charset);
    DUMP_TRACE (STL_STRING (_T("\tui_PointSize_ = ")) + STR_UIToStr (ui_PointSize_));
    DUMP_TRACE (STL_STRING (_T("\tb_Bold_ = ")) + 
                (b_Bold_ ? STL_STRING (_T("true")) 
                         : STL_STRING (_T("false"))));
    DUMP_TRACE (STL_STRING (_T("\tb_Italic_ = ")) + 
                (b_Italic_ ? STL_STRING (_T("true")) 
                           : STL_STRING (_T("false"))));
    DUMP_TRACE (STL_STRING (_T("\tb_Underline_ = ")) + 
                (b_Underline_ ? STL_STRING (_T("true"))
                              : STL_STRING (_T("false"))));
    DUMP_TRACE (STL_STRING (_T("\tb_Strikeout_ = ")) +
                (b_Strikeout_ ? STL_STRING (_T("true"))
                              : STL_STRING (_T("false"))));
    DUMP_TRACE (STL_STRING (_T("\tui_ReferenceCount_ = ")) + STR_UIToStr (ui_ReferenceCount_));

    DUMP_TRACE (_T("-------- CT_FontDescriptor: end of dump -----"));

}    // v_Dump (...)
