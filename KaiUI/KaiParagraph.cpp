//==============================================================================
//
//  Copyright (c) Konstantin Bogatyrev, 2001
//  
//  Facility: Kai 1.2
//
//  Module description: implementation of the Paragraph class.
//
//  $Id: KaiParagraph.cpp,v 1.127 2008-06-07 05:04:28 kostya Exp $
//  
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning (disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
//#include "KaiWord.h"
#include "KaiDocDescriptor.h"
#include "KaiFontDescriptor.h"
#include "KaiTextPos.h"
#include "KaiParagraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//  Paragraph construction/destruction/initialization
//
CT_Paragraph::CT_Paragraph()
{
    v_Null();
}

CT_Paragraph::CT_Paragraph (CT_DocDescriptor * pco_doc)
 : pco_ParentDoc_ (pco_doc)
{
    v_Null();
}

CT_Paragraph::CT_Paragraph (const CT_Paragraph& co_paragraph)
{
    v_DeepCopy_ (co_paragraph);
}

CT_Paragraph::~CT_Paragraph()
{
    v_Null();
}

void CT_Paragraph::v_Null()
{
    CT_KaiString::v_Null_();

    pco_Previous_ = NULL;
    pco_Next_ = NULL;    
    b_Current_ = false;
    b_IsLastParagraph_ = false;
    eo_Alignment_ = ec_AlignmentFront;
//    i_PageWidth_ = 0;
    i_MaxLineWidth_ = -1;
    st_Spacing_.eo_Type = ec_LineSpacingAuto;
    st_Spacing_.i_Size = 0;
    i_LeftIndent_ = 0;
    i_RightIndent_ = 0;
    i_Height_ = -1;
    vi_TabStops_.clear();
    pco_Dc_ = NULL;
    v_ClearLines_();
//    v_ClearWords_();
}

CT_Paragraph& CT_Paragraph::operator = (const CT_Paragraph& co_paragraph)
{
    v_DeepCopy_ (co_paragraph);
    return *this;
}

//
// Accessors
//
bool CT_Paragraph::b_GetFontKey (int i_offset, unsigned int& ui_fontKey)
{
    v_Synchronize_();

    vector<ST_Token>::reverse_iterator rit_;
    ST_Token st_token;
    for (rit_ = vo_Tokens_.rbegin(); rit_ != vo_Tokens_.rend(); ++rit_)
    {
        st_token = *rit_;
        if ((st_token.eo_TokenType == ec_TokenFormatting) &&
            (st_token.i_Offset <= i_offset))
        {
            break;
        }
    }
    if (rit_ == vo_Tokens_.rend())
    {
        ERROR_TRACE (_T("No formatting in paragraph"));
        return false;
    }

    ui_fontKey = st_token.ui_FontDescriptor;

    return true;

}    //  b_GetFontKey (...)

CFont * CT_Paragraph::pco_GetWindowsFont (CDC * pco_dc, int i_offset)
{
    v_Synchronize_();

    vector<ST_Token>::reverse_iterator rit_;
    ST_Token st_token;
    for (rit_ = vo_Tokens_.rbegin(); rit_ != vo_Tokens_.rend(); ++rit_)
    {
        st_token = *rit_;
        if ((st_token.eo_TokenType == ec_TokenFormatting) &&
            (st_token.i_Offset <= i_offset))
        {
            break;
        }
    }
    if (rit_ == vo_Tokens_.rend())
    {
        ERROR_TRACE (_T("No formatting in paragraph"));
        return NULL;
    }

    CT_FontDescriptor * pco_fd = NULL;
    try
    {
        pco_fd = pco_ParentDoc_->pco_GetFontDescriptor (st_token.ui_FontDescriptor);
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return NULL;
    }
    ATLASSERT (pco_fd);

    CFont * pco_font = NULL;
    if (pco_dc->IsPrinting())
    {
        pco_font = pco_fd->pco_GetPrintFont (pco_dc);
    }
    else
    {
        pco_font = pco_fd->pco_GetWindowsFont();
    }

    return pco_font;

}    //  pco_GetWindowsFont (...)

int CT_Paragraph::i_GetMaxLineWidth()
{
    if (!pco_Dc_)
    {
        ATLASSERT(0);
        ERROR_TRACE (_T("No DC."));
    }

    int i_maxWidth = pco_ParentDoc_->i_GetPageWidth();

    int i_pixPerInch = pco_Dc_->GetDeviceCaps (LOGPIXELSX);
    return (i_pixPerInch * (i_maxWidth - (i_LeftIndent_ + i_RightIndent_)))/100;
}

int CT_Paragraph::i_GetPageWidth()       // 100ths of inch
{
    return pco_ParentDoc_->i_GetPageWidth();
}


//
// Loading/unloading
//
bool CT_Paragraph::b_Load (CFile& co_file)
{
    v_Null();

    int i_bytesRead = 0;

    try
    {
        CT_KaiString::v_Load (co_file);
        V_LoadItem (co_file, b_IsLastParagraph_);
        V_LoadItem (co_file, eo_Alignment_);
        if ((eo_Alignment_ <= ec_AlignmentFront) || 
         (eo_Alignment_ >= ec_AlignmentBack))
        {
            ERROR_TRACE (_T("eo_Alignment: unrecognized value"));
            return false;
        }

        V_LoadItem (co_file, st_Spacing_.eo_Type);
        V_LoadItem (co_file, st_Spacing_.i_Size);
        V_LoadItem (co_file, i_LeftIndent_);
        V_LoadItem (co_file, i_RightIndent_);
        V_LoadVector (co_file, vi_TabStops_);

    }   //  try
    catch (CT_KaiException& co_ex)
    {
        v_ClearLines_();
        co_ex.v_Report();
        return false;
    }

    return true;

}   // b_Load (...)

bool CT_Paragraph::b_Store (CFile& co_file)
{
    try
    {
        CT_KaiString::v_Store (co_file);
        co_file.Write (&b_IsLastParagraph_, sizeof (bool));
        co_file.Write (&eo_Alignment_, sizeof (ET_Alignment));
//        co_file.Write (&i_PageWidth_, sizeof (int));
        co_file.Write (&st_Spacing_.eo_Type, sizeof (ET_LineSpacingType));
        co_file.Write (&st_Spacing_.i_Size, sizeof (int));
        co_file.Write (&i_LeftIndent_, sizeof (int));
        co_file.Write (&i_RightIndent_, sizeof (int));

        int i_tsSize = vi_TabStops_.size();
        co_file.Write (&i_tsSize, sizeof (int));
        for (int i_ts = 0; i_ts < i_tsSize; ++i_ts)
        {
            co_file.Write (&vi_TabStops_[i_ts], sizeof (int));
        }
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
        return false;
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }

    return true;

}   //  b_Store (...)

bool CT_Paragraph::b_StoreAsPlainText (CFile& co_file)
{
    try
    {
        CT_KaiString::v_StoreAsPlainText (co_file);
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
        return false;
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }

    return true;

}   //  b_StoreAsPlainText (...)

void CT_Paragraph::v_CopyAttributes (const CT_Paragraph& co_source)
{
    this->CT_KaiString::v_CopyAttributes (co_source);
    pco_ParentDoc_ = co_source.pco_ParentDoc_;
    b_IsLastParagraph_ = co_source.b_IsLastParagraph();
    eo_Alignment_ = co_source.eo_Alignment_;
//    i_PageWidth_ = co_source.i_PageWidth_;
    st_Spacing_.eo_Type = co_source.st_Spacing_.eo_Type;
    st_Spacing_.i_Size = co_source.st_Spacing_.i_Size;
    i_LeftIndent_ = co_source.i_LeftIndent_;
    i_RightIndent_ = co_source.i_RightIndent_;
    vi_TabStops_ = co_source.vi_TabStops_;
}

bool CT_Paragraph::b_CreateFDString (unsigned int ui_fd, STL_STRING& str_fd)
{
    if (str_Escape_.empty())
    {
        ERROR_TRACE (_T("Escape chars not defined"));
        return false;
    }
    str_fd = str_Escape_[0];
    str_fd += _T("F");
    str_fd += STR_UIToStr (ui_fd);
    str_fd += str_Escape_[0];

    return true;
}

bool CT_Paragraph::b_CreateDiacrString (const _TCHAR chr_, STL_STRING& str_d)
{
    if (str_Escape_.empty())
    {
        ERROR_TRACE (_T("Escape chars not defined"));
        return false;
    }
    str_d = str_Escape_[0];
    str_d += _T("D");
    str_d += chr_;
    str_d += str_Escape_[0];

    return true;
}

bool CT_Paragraph::b_LinearOffsetToAbsoluteOffset (int i_lOffset, int& ri_aOffset)
{
    ri_aOffset = 0;

    ST_Token st_t;
    int i_wholeTokenVisLength = 0;
    int i_t = 0;
    for (; i_t < i_GetNumOfTokens(); ++i_t)
    {
        st_t = st_GetToken (i_t);
        if (st_t.b_IsLinearText())
        {
            if (i_wholeTokenVisLength + st_t.i_Length > i_lOffset)
            {
                break;
            }
            else
            {
                i_wholeTokenVisLength += st_t.i_Length;
            }
        }
    }
    if (i_t == i_GetNumOfTokens())
    {
        ATLASSERT(0);
        ERROR_TRACE (_T("Visible position past end of last token."));

        return false;
    }

    int i_partialLength = i_lOffset - i_wholeTokenVisLength;
    ri_aOffset = st_t.i_Offset + i_partialLength;
    if (0 == i_partialLength)
    {
        if (i_t > 0)
        {
            const ST_Token& rst_t = rst_GetToken (i_t - 1);
            if (ec_TokenDiacritics == rst_t.eo_TokenType)
            {
                ri_aOffset = rst_t.i_Offset;
            }
        }
    }

    return true;

}   //  b_LinearOffsetToAbsoluteOffset (...)

bool CT_Paragraph::b_AbsoluteOffsetToLinearOffset (int i_aOffset, int& ri_lOffset)
{
    ri_lOffset = 0;

    ST_Token st_t;
    int i_t = 0;
    for (; i_t < i_GetNumOfTokens(); ++i_t)
    {
        st_t = st_GetToken (i_t);
        if (st_t.i_Offset + st_t.i_Length  - 1 < i_aOffset)
        {
            if (st_t.b_IsLinearText())
            {
                ri_lOffset += st_t.i_Length;
            }
        }
        else
        {
            break;
        }
    }
    if (i_t == i_GetNumOfTokens())
    {
        ATLASSERT(0);
        ERROR_TRACE (_T("Visible position past end of last token."));

        return false;
    }
    if (st_t.b_IsLinearText())
    {
        ri_lOffset += i_aOffset - st_t.i_Offset;
    }

    return true;

}   //  b_AbsoluteOffsetToLinearOffset (...)

void CT_Paragraph::v_MergeWithNext()
{
    if (pco_GetNext() == NULL)
    {
        return;
    }

    //
    // Remove paragraph end mark
    //
    erase (length()-1, 1);

    CT_Paragraph * pco_next = pco_GetNext();

    int i_removeAt = pco_next->i_At();
    if (i_removeAt == pco_ParentDoc_->i_NParagraphs())
    {
        ERROR_TRACE (_T("Error removing next paragraph."));
        return;
    }
    pco_ParentDoc_->v_RemoveParagraph (i_removeAt);

    append (*pco_next);
    v_SetLastParagraph (pco_next->b_IsLastParagraph());

    CT_Paragraph * pco_newNext = pco_next->pco_GetNext();
    if (pco_newNext != NULL)
    {
        pco_newNext->v_SetPrevious (this);
    }
    v_SetNext (pco_newNext);
    delete pco_next;

    v_FdCleanup();
    v_Invalidate();

}    //  v_MergeWithNext()

bool CT_Paragraph::b_Delete (CT_TextPos& co_first, 
                             CT_TextPos& co_onePastEnd,
                             STL_STRING& str_deletedText)
{
    if (co_first.pco_GetParagraph() != co_onePastEnd.pco_GetParagraph())
    {
        ERROR_TRACE (_T("Start and end positions must point to the same paragraph."));
        return false;
    }
    if (co_first.pco_GetParagraph() != this)
    {
        ERROR_TRACE (_T("Text not in current paragraph."));
        return false;
    }

    int i_start = co_first.i_GetParagraphOffset();
    int i_end = co_onePastEnd.i_GetParagraphOffset();
    int i_t = i_GetTokenNumFromOffset (i_end);
    STL_STRING str_lastFontDescriptor;
    for (; i_t > 0; --i_t)
    {
        ST_Token& rst_t = vo_Tokens_[i_t];
        if (rst_t.i_Offset < i_start)
        {
            break;
        }
        if (rst_t.eo_TokenType == ec_TokenFormatting) 
        {
            str_lastFontDescriptor = str_GetToken (i_t);
            break;
        }
    }

    str_deletedText = substr (i_start, i_end - i_start);
    erase (i_start, i_end - i_start);
    if (str_lastFontDescriptor.length() > 0)
    {
        insert (i_start, str_lastFontDescriptor);
    }

    v_Synchronize_();

    //
    // Remove font descriptors that are not followed by text
    //
    v_FdCleanup();
    v_Invalidate();

    return true;

}    // b_Delete()

//
// Formatting
//
bool CT_Paragraph::b_Measure_()
{
    if (!pco_Dc_)
    {
        ERROR_TRACE (_T("No device context"));
        return false;
    }

    v_Synchronize_();

    unsigned int ui_curFont = 0;

    for (unsigned int ui_at = 0; ui_at < vo_Tokens_.size(); ++ui_at)
    {
        ST_Token& st_token = vo_Tokens_[ui_at];
//        if (ec_TokenFormatting == st_token.eo_TokenType)
        if (!st_token.b_IsPrintable())
        {
            st_token.i_ScreenHeight = 0;
            st_token.i_ScreenLength = 0;
//            st_token.b_Measured = true;
            if (ec_TokenFormatting == st_token.eo_TokenType)
            {
                ui_curFont = st_token.ui_FontDescriptor;
            }
            st_token.i_ExtraSpace = 0;
            st_token.i_ExtraPrintSpace = 0;

            continue;
        }

        if (!st_token.b_Measured)
        {
            bool b_ = b_MeasureToken (st_token, ui_curFont);
            if (!b_)
            {
                return false;
            }
        }
    }

    return true;

}    // b_Measure_ (...)

bool CT_Paragraph::b_MeasureToken (ST_Token& st_token,
                                   unsigned int ui_font)
{
    if (!pco_Dc_)
    {
        ERROR_TRACE (_T("No device context"));
        return false;
    }

    STL_STRING str_;
    CT_FontDescriptor * pco_fd  = NULL;

    try
    {
        pco_fd = pco_GetFontDescriptor (ui_font);
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }
    ATLASSERT (pco_fd);

    CFont * pco_font = pco_fd->pco_GetWindowsFont();
    if (pco_Dc_->IsPrinting())
    {
        pco_font = pco_fd->pco_GetPrintFont (pco_Dc_);
    }
    else
    {
        pco_font = pco_fd->pco_GetWindowsFont();
    }
    CFont * pco_oldFont = pco_Dc_->SelectObject (pco_font);

    CSize co_size;
    if (st_token.eo_TokenType == ec_TokenDiacritics)
    {
        str_ = substr (st_token.i_Offset + 2, st_token.i_Length - 3);
        TEXTMETRIC st_tm;
        pco_Dc_->GetTextMetrics(&st_tm);
        co_size = pco_Dc_->GetTextExtent (str_.data(), st_token.i_Length - 3);
        co_size.cy -= st_tm.tmAscent;
    }
    else
    {
        str_ = substr (st_token.i_Offset, st_token.i_Length);
        co_size = pco_Dc_->GetTabbedTextExtent (str_.data(),
                                                st_token.i_Length,
                                                vi_TabStops_.size(),
                                                arri_TabStops_);
    }

    if (pco_Dc_->IsPrinting())
    {
        st_token.i_ExtraPrintSpace = 0;
        st_token.i_PrintLength = co_size.cx;
    }
    else
    {
        st_token.i_ExtraSpace = 0;
        st_token.i_ScreenLength = co_size.cx;
        st_token.i_ScreenHeight = co_size.cy;
        st_token.b_Measured = true;
    }

    pco_Dc_->SelectObject (pco_oldFont);

    return true;

}    //  b_MeasureToken (...)

bool CT_Paragraph::b_MeasureString (CDC * pco_dc,
                                    int i_offset, 
                                    int i_length, 
                                    CSize& co_mfcSize)
{
    if (i_offset < 0)
    {
        ERROR_TRACE (_T("Bad offset"));
        return false;
    }

    //
    // Get first text token
    //
    ST_Token st_token = st_GetTokenFromOffset (i_offset);

    //
    // Token --> iterator
    //
    vector<ST_Token>::iterator it_t = ::find (vo_Tokens_.begin(), 
                                              vo_Tokens_.end(), 
                                              st_token);
    if (it_t == vo_Tokens_.end())
    {
        ERROR_TRACE (_T("Token not in vector"));
        return false;
    }

    //
    // Scan backwards to find the font descriptor for the
    // first field
    //
    vector<ST_Token>::iterator it_ft = it_t;
    while (it_ft != vo_Tokens_.begin())
    {
        if ((*it_ft).eo_TokenType == ec_TokenFormatting)
        {
            break;
        }
        --it_ft;
    }

    if ((*it_ft).eo_TokenType != ec_TokenFormatting)
    {
        ERROR_TRACE (_T("No formatting data in paragraph"));
        return false;
    }

    ST_Token st_format = *it_ft;
    CT_FontDescriptor * pco_fd = NULL;
    try
    {
        pco_fd = pco_ParentDoc_->pco_GetFontDescriptor (st_format.ui_FontDescriptor);
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return false;
    }
    ATLASSERT (pco_fd);

    if (!(*it_t).b_IsPrintable())
    {
        ++it_t;
        if (it_t == vo_Tokens_.end())
        {
            ERROR_TRACE (_T("No text token after format token"));
            return false;
        }
    }

    CFont * pco_oldFont = pco_dc->GetCurrentFont();

    co_mfcSize.cx = 0;
    co_mfcSize.cy = 0;
    while (i_length > 0)
    {
        CFont * pco_font = pco_fd->pco_GetWindowsFont();
        pco_dc->SelectObject (pco_font);
        int i_start = max (i_offset, (*it_t).i_Offset);
        int i_charsToMeasure = min (i_length, 
                                    (*it_t).i_Offset + (*it_t).i_Length - i_start);
        if (i_charsToMeasure < 0)
        {
            ERROR_TRACE (_T("Negative length"));
            return false;
        }
        STL_STRING str_ = substr (i_start, i_charsToMeasure);
        CSize co_s = pco_dc->GetTabbedTextExtent (str_.data(),
                                                  i_charsToMeasure,
                                                  vi_TabStops_.size(),
                                                  arri_TabStops_);
        co_mfcSize.cx += co_s.cx;
        co_mfcSize.cy = max (co_mfcSize.cy, co_s.cy);

        i_length -= i_charsToMeasure;
        if (i_length <= 0)
        {
            break;
        }
        
        //
        // Get next token
        //
        ++it_t;
        while ((it_t != vo_Tokens_.end()) &&
               !(*it_t).b_IsLinearText())
        {
            if ((*it_t).eo_TokenType == ec_TokenFormatting)
            {
                try
                {
                    pco_fd = pco_ParentDoc_->pco_GetFontDescriptor ((*it_t).ui_FontDescriptor);
                }
                catch (CT_KaiException& co_ex)
                {
                    co_ex.v_Report();
                    return false;
                }
                ATLASSERT (pco_fd);

                pco_font = pco_fd->pco_GetWindowsFont();
                pco_dc->SelectObject (pco_font);
            }
            ++it_t;
        }

        if (it_t == vo_Tokens_.end())
        {
            if (i_length > 0)
            {
                ERROR_TRACE (_T("String length out of range"));
                return false;
            }
            break;
        }
        if (i_length < 0)
        {
            ERROR_TRACE (_T("Negative string length"));
            return false;
        }
    }

    pco_dc->SelectObject (pco_oldFont);

    return true;

}   // b_MeasureString (...)

void CT_Paragraph::v_FdCleanup()
{
    v_Synchronize_();

    vector<int> vec_removeUs;
    unsigned int ui_lastFD = 0;
    vector<ST_Token>::iterator it_token = vo_Tokens_.begin();
    for (; it_token != vo_Tokens_.end(); ++it_token)
    {
        if ((*it_token).eo_TokenType == ec_TokenFormatting)
        {
            if (it_token - vo_Tokens_.begin() < (int)vo_Tokens_.size() - 1)
            {
                vector<ST_Token>::iterator it_next = it_token + 1;
                if ((*it_next).eo_TokenType == ec_TokenFormatting)
                {
                    vec_removeUs.push_back (it_token - vo_Tokens_.begin());
                    continue;
                }
            }

            if ((*it_token).ui_FontDescriptor == ui_lastFD)
            {
                vec_removeUs.push_back (it_token - vo_Tokens_.begin());
            }
            else
            {
                ui_lastFD = (*it_token).ui_FontDescriptor;
            }
        }
    }    // for (; it_token ...)

    if (vec_removeUs.empty())
    {
        return;
    }

    STL_STRING str_new;
    it_token = vo_Tokens_.begin();
    int i_deleteAt = vec_removeUs[0];
    for (; (!vec_removeUs.empty() && (it_token != vo_Tokens_.end())); ++it_token)
    {
        if (it_token - vo_Tokens_.begin() != i_deleteAt)
        {
            str_new += substr ((*it_token).i_Offset, (*it_token).i_Length);
        }
        else
        {
            pco_GetDocument()->b_UnRegisterFont ((*it_token).ui_FontDescriptor);
            vec_removeUs.erase (vec_removeUs.begin());
            if (vec_removeUs.empty())
            {
                continue;
            }
            i_deleteAt = vec_removeUs[0];
        }
    }
    for (; it_token != vo_Tokens_.end(); ++it_token)
    {
        if (it_token - vo_Tokens_.begin() != i_deleteAt)
        {
            str_new += substr ((*it_token).i_Offset, (*it_token).i_Length);
        }
    }

    assign (str_new);
    v_Synchronize_();
    v_Invalidate();

}    //  void CT_Paragraph::v_FdCleanup()

bool CT_Paragraph::b_Render (CDC * pco_dc)
{
    if (b_Current_)
    {
        return true;
    }

    pco_Dc_ = pco_dc;

    v_Synchronize_();

    //
    //  MFC needs an array, not a vector
    //
    for (unsigned int ui_ts = 0; ui_ts < vi_TabStops_.size(); ++ui_ts)
    {
        arri_TabStops_[ui_ts] = vi_TabStops_[ui_ts];
    }

    bool b_ = b_Measure_();
    if (!b_)
    {
        return false;
    }

    if (CT_KaiString::i_GetNumOfTokens() < 2)
    {
        ERROR_TRACE (_T("Too few tokens in paragraph."));
        pco_Dc_ = NULL;
        return false;
    }

    i_MaxLineWidth_ = i_GetMaxLineWidth();

    if (i_MaxLineWidth_ < 0)
    {
        ERROR_TRACE (_T("Insufficient horizontal space."));
        return false;
    }

    v_ClearLines_();

    ST_Token& st_token = vo_Tokens_[0];
    if (st_token.eo_TokenType != ec_TokenFormatting)
    {
        ERROR_TRACE (_T("Too few tokens in paragraph."));
        pco_Dc_ = NULL;
        return false;
    }

    i_CurrentLine_ = 0;
    i_CurrentToken_ = 0;
    i_CurrentScreenLength_ = 0;
    i_TokensInCompound_ = 0;
    pco_CurrentLine_ = new CT_Line (this, 0);
    bool b_EndOfParagraph_ = false;
    while (!b_EndOfParagraph_ && (i_CurrentToken_ < i_NTokens()))
    {
        bool b_ = b_AddToken_();
        if (!b_)
        {
            pco_ParentDoc_->v_SetUnrenderable();
            return false;
        }

    }   // while (!b_endOfParagraph)
    b_Current_ = true;
    pco_Dc_ = NULL;

    return true;

}   // b_Render (...)

bool CT_Paragraph::b_AddToken_()
{
    ATLASSERT (i_MaxLineWidth_ > 0);

    ST_Token& rst_token = vo_Tokens_[i_CurrentToken_];
    switch (rst_token.eo_TokenType)
    {
        case ec_TokenDiacritics:
        case ec_TokenFormatting:
        case ec_TokenHiddenTag:
        {
            pco_CurrentLine_->i_NTokens_++;
            ++i_CurrentToken_;
            break;
        }

        case ec_TokenSpace:
        case ec_TokenTab: 
        {
            pco_CurrentLine_->i_NTokens_++;
            ++i_CurrentToken_;
            i_CurrentScreenLength_ += rst_token.i_ScreenLength;
            break;
        }

        case ec_TokenText:
        case ec_TokenPunctuation:
        case ec_TokenEndOfParagraph:
        {
            i_CompoundScreenLength_ = rst_token.i_ScreenLength;
            i_TokensInCompound_ = 1;
            for (int i_t = 1;
                 i_t + i_CurrentToken_ < static_cast<int>(vo_Tokens_.size()); 
                 ++i_t)
            {
                ST_Token& rst_nextToken = vo_Tokens_[i_CurrentToken_ + i_TokensInCompound_];
                if ((ec_TokenSpace == rst_nextToken.eo_TokenType) ||
                    (ec_TokenTab == rst_nextToken.eo_TokenType))
                {
                    break;
                }

                if (i_CurrentScreenLength_ + 
                    i_CompoundScreenLength_ + 
                    rst_nextToken.i_ScreenLength > i_MaxLineWidth_)
                {
                    break;
                }

                ++i_TokensInCompound_;
                i_CompoundScreenLength_ += rst_nextToken.i_ScreenLength;
            }

            if (i_CurrentScreenLength_ + i_CompoundScreenLength_ > i_MaxLineWidth_)
            {
                ATLASSERT (i_MaxLineWidth_ > 0);
                if (i_CompoundScreenLength_ > i_MaxLineWidth_)
                {
                    ATLASSERT (1 == i_TokensInCompound_);
                    bool b_ = b_SplitNoHyphen_();
                    if (!b_)
                    {
                        return false;
                    }
                    break;
                }

                if (pco_CurrentLine_->i_NTokens_ == 1)
                {
                    if ((UNDEFINED != pco_CurrentLine_->i_FirstTokenOffset_) && 
                        (pco_CurrentLine_->i_FirstTokenOffset_ > 0))
                    {
                        ST_Token& rst_token = vo_Tokens_[pco_CurrentLine_->i_FirstToken_];
                        pco_CurrentLine_->i_LastTokenLength_ = 
                            rst_token.i_Length - pco_CurrentLine_->i_FirstTokenOffset_;
                        pco_CurrentLine_->i_LastTokenScreenLength_ = 
                                            pco_CurrentLine_->i_FirstTokenScreenLength_;
                    }
                }

                ATLASSERT (i_CurrentScreenLength_ > 0);
                ATLASSERT (pco_CurrentLine_->i_NTokens_ > 0);
                if (eo_Alignment_ == ec_RightJustified) 
                {
                    pco_CurrentLine_->b_RightJustify();
                }

                v_InsertLine();
                if (i_CurrentLine_ >= (int)vpo_Lines_.size())
                {
                    ATLASSERT (i_CurrentLine_ == (int)vpo_Lines_.size());
                    pco_CurrentLine_ = new CT_Line (this, i_CurrentToken_);
                }
                else
                {
                    pco_CurrentLine_ = vpo_Lines_[i_CurrentLine_];
                    pco_CurrentLine_->v_Null();
                    pco_CurrentLine_->i_FirstToken_ = i_CurrentToken_;
                }
                i_CurrentToken_ += i_TokensInCompound_;
                pco_CurrentLine_->i_NTokens_ = i_TokensInCompound_;
                i_CurrentScreenLength_ = i_CompoundScreenLength_;
            }
            else
            {
                pco_CurrentLine_->i_NTokens_ += i_TokensInCompound_;
                i_CurrentToken_ += i_TokensInCompound_;
                i_CurrentScreenLength_ += i_CompoundScreenLength_;
            }

            if (i_CurrentToken_ >= (int)vo_Tokens_.size())
            {
                if (i_CurrentToken_ > (int)vo_Tokens_.size())
                {
                    ATLASSERT(0);
                    ERROR_TRACE (_T("Bad token number"));
                    return false;
                }
                if (ec_TokenEndOfParagraph != vo_Tokens_[i_CurrentToken_-1].eo_TokenType)
                {
                    ATLASSERT(0);
                    ERROR_TRACE (_T("Illegal token type at paragraph end"));
                    return false;
                }
                v_InsertLine();
                b_EndOfParagraph_ = true;
            }
            else
            {
                b_EndOfParagraph_ = false;
                ATLASSERT (i_CurrentToken_ < (int)vo_Tokens_.size());
            }
            break;
        }

        case ec_TokenTypeFront:
        case ec_TokenTypeBack:
        default:
        {
            ERROR_TRACE (_T("Illegal token type."));
            return false;
        }
    }

    return true;

}  // b_AddToken_ (...)

bool CT_Paragraph::b_SplitNoHyphen_()
{
    ST_Token& rst_token = vo_Tokens_[i_CurrentToken_];
    
    int i_remainingScreenLength = rst_token.i_ScreenLength;

    int i_stubOffset = 0;
    int i_stubLength = 0;
    int i_stubScreenLength = 0;

    bool b_ = b_SplitToken_ (rst_token, 
                             rst_token.i_Offset + i_stubOffset, 
                             i_MaxLineWidth_ - i_CurrentScreenLength_,
                             i_stubLength, 
                             i_stubScreenLength);
    if (!b_)
    {
        return false;
    }

    if (i_stubLength > 0)
    {
        pco_CurrentLine_->i_NTokens_++;
        pco_CurrentLine_->i_LastTokenLength_ = i_stubLength;
        pco_CurrentLine_->i_LastTokenScreenLength_ = i_stubScreenLength;
    }
    v_InsertLine();

    i_stubOffset += i_stubLength;

    if (i_CurrentLine_ >= (int)vpo_Lines_.size())
    {
        ATLASSERT (i_CurrentLine_ == (int)vpo_Lines_.size());
        pco_CurrentLine_ = new CT_Line (this, 
                                        i_CurrentToken_, 
                                        i_stubOffset, 
                                        i_stubScreenLength);
    }
    else
    {
        pco_CurrentLine_ = vpo_Lines_[i_CurrentLine_];
        pco_CurrentLine_->v_Null();
        pco_CurrentLine_->i_FirstToken_ = i_CurrentToken_;
        pco_CurrentLine_->i_FirstTokenOffset_ = i_stubOffset;
        pco_CurrentLine_->i_FirstTokenScreenLength_ = i_stubScreenLength;
    }

    i_CurrentScreenLength_ = 0;
    i_remainingScreenLength -= i_stubScreenLength;

    while (i_remainingScreenLength > i_MaxLineWidth_)
    {
        bool b_ = b_SplitToken_ (rst_token, 
                                 rst_token.i_Offset + i_stubOffset, 
                                 i_MaxLineWidth_,
                                 i_stubLength, 
                                 i_stubScreenLength);
        if (!b_)
        {
            return false;
        }

        i_remainingScreenLength -= i_stubScreenLength;

        pco_CurrentLine_->i_NTokens_ = 1;
        pco_CurrentLine_->i_FirstTokenOffset_ = i_stubOffset;
        pco_CurrentLine_->i_FirstTokenScreenLength_ = i_stubScreenLength;
        pco_CurrentLine_->i_LastTokenLength_ = i_stubLength;
        pco_CurrentLine_->i_LastTokenScreenLength_ = i_stubScreenLength;
        
        v_InsertLine();

        i_stubOffset += i_stubLength;

        if (i_CurrentLine_ >= (int)vpo_Lines_.size())
        {
            ATLASSERT (i_CurrentLine_ == (int)vpo_Lines_.size());
            pco_CurrentLine_ = new CT_Line (this, 
                                            i_CurrentToken_, 
                                            i_stubOffset);
        }
        else
        {
            pco_CurrentLine_ = vpo_Lines_[i_CurrentLine_];
            pco_CurrentLine_->v_Null();
            pco_CurrentLine_->i_FirstToken_ = i_CurrentToken_;
            pco_CurrentLine_->i_FirstTokenOffset_ = i_stubOffset;
        }
    }

    if (i_remainingScreenLength == i_MaxLineWidth_)
    {
        pco_CurrentLine_->i_LastTokenLength_ = i_stubLength;
        pco_CurrentLine_->i_LastTokenScreenLength_ = i_remainingScreenLength;
    }

    pco_CurrentLine_->i_NTokens_ = 1;
    pco_CurrentLine_->i_FirstTokenScreenLength_ = i_remainingScreenLength;

    i_CurrentScreenLength_ = i_remainingScreenLength;   
    ++i_CurrentToken_;

    return true;

}   // b_SplitNoHyphen_

bool CT_Paragraph::b_SplitToken_ (const ST_Token& rst_token, 
                                  int i_start,
                                  int i_maxScreenLength,
                                  int& i_length, 
                                  int& i_screenLength)  
{
    CSize co_mfcSize;
    int i_l = rst_token.i_Offset + rst_token.i_Length;
    for (; i_l > i_start; --i_l)
    {
        bool b_ = b_MeasureString (pco_Dc_,
                                   i_start, 
                                   i_l - i_start, 
                                   co_mfcSize);
        if (!b_)
        {
            return false;
        }

        i_length = i_l - i_start;
        i_screenLength = co_mfcSize.cx;
        if (co_mfcSize.cx <= i_maxScreenLength)
        {
            break;
        }
    }

    if (i_l == i_start)
    {
        i_length = 0;
        i_screenLength = 0;
    }

    return true;

}   //  b_SplitToken_ (...)

void CT_Paragraph::v_PartialRender (CDC * pco_dc, 
                                    int i_tokensAdded, 
                                    int i_lastTokenBefore, 
                                    int i_tokensAfter)
{
    bool b_ = b_Measure_();

    int i_firstToken = i_lastTokenBefore + 1;
    std::vector<CT_Line *>::iterator it_l = vpo_Lines_.begin();
    for (; it_l != vpo_Lines_.end(); ++it_l)
    {
        if (((*it_l)->i_FirstToken_ <= i_firstToken) &&
            (i_firstToken < (*it_l)->i_FirstToken_ + (*it_l)->i_NTokens_))
        {
            break;
        }
    }

    //
    // If line starts with a split token, pull back
    //
    while (it_l != vpo_Lines_.begin() &&
           ((*it_l)->i_FirstTokenOffset_ != UNDEFINED) &&
           ((*it_l)->i_FirstTokenOffset_ != 0))
    {
        --it_l;
    }

    if (vpo_Lines_.end() == it_l)
    {
        ATLASSERT(0);
        ERROR_TRACE (_T("Unable to find 1st affected line."));
        return;
    }

    i_CurrentLine_ = (int) (it_l - vpo_Lines_.begin());
    ATLASSERT (i_CurrentLine_ >= 0);

    //
    // Adjust token numbers in all lines following the last affected line
    //
    for (int i_l = i_CurrentLine_ + 1; i_tokensAdded && (i_l < i_NLines()); ++i_l)
    {
        CT_Line * pco_l = vpo_Lines_[i_l];
        pco_l->i_FirstToken_ += i_tokensAdded;
    }

    pco_CurrentLine_ = *it_l;
    i_CurrentToken_ = pco_CurrentLine_->i_FirstToken_;
    pco_CurrentLine_->v_Null();
    i_CurrentScreenLength_ = 0;
    i_TokensInCompound_ = 0;
    bool b_EndOfParagraph_ = false;

    CT_Line co_nextLine;
    if (i_CurrentLine_ < i_NLines() - 1)
    {
        co_nextLine = *(vpo_Lines_[i_CurrentLine_ + 1]);
    }

    while ((i_CurrentToken_ < i_NTokens()))
    {
        int i_prevLine = i_CurrentLine_;
        bool b_ = b_AddToken_();
        if (!b_)
        {
            ATLASSERT(0);
            return;
        }

        if ((i_CurrentLine_ > i_prevLine) &&
            (pco_CurrentLine_->i_FirstToken_ >= i_NTokens() - i_tokensAfter))
        {
            if (co_nextLine.i_FirstToken_ == pco_CurrentLine_->i_FirstToken_)
            {
                *pco_CurrentLine_ = co_nextLine;
                break;
            }
            if (i_CurrentLine_ < i_NLines() - 1)
            {
                co_nextLine = *(vpo_Lines_[i_CurrentLine_ + 1]);
            }
        }

    }   // while (!b_endOfParagraph)


    if ((i_NTokens() == i_CurrentToken_) && (i_NLines() > 1))
    {
        std::vector<CT_Line *>::iterator it_last = vpo_Lines_.begin() + i_NLines() - 1;
        std::vector<CT_Line *>::iterator it_previous = it_last - 1;
        if ((*it_last)->i_FirstToken_ + (*it_last)->i_NTokens_ - 1 == 
            (*it_previous)->i_FirstToken_ + (*it_previous)->i_NTokens_ - 1)
        {
            delete *it_last;
            vpo_Lines_.erase (it_last);
        }
    }

}   //  v_PartialRender (...)

bool CT_Paragraph::b_InsertOrReplaceChar (CDC * pco_dc, 
                                          int i_offset, 
                                          _TUCHAR ui_char, 
                                          bool& b_insert)
{
    ATLASSERT (NULL == pco_Dc_);

    pco_Dc_ = pco_dc;

    int i_lastToken = i_NTokens() - 1;
    ST_Token st_last = vo_Tokens_[i_lastToken];
    while (!st_last.b_IsLinearText())
    {
        ATLASSERT(0);
        ERROR_TRACE (_T("No formatting in paragraph"));
        st_last = st_GetToken (--i_lastToken);
    }

    if ((!b_insert) && (i_offset == st_last.i_Offset))
    {
        b_insert = true;
    }

    int i_lastTokenBefore = 0;
    int i_tokensAfter = 0;
    int i_oldSize = i_NTokens();
    if (!b_insert)
    {
        int i_linearOffset = 0;
        bool b_ = b_AbsoluteOffsetToLinearOffset (i_offset, i_linearOffset);
        if (!b_)
        {
            ATLASSERT(0);
            return false;
        }

        bool b_canOptimize = true;
        b_ = b_DeleteChar_ (i_offset, b_canOptimize, i_lastTokenBefore, i_tokensAfter);
        if (!b_)
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Deletion error. Paragraph re-formatting aborted."));
            return false;
        }

        if (!b_canOptimize)
        {
            v_FdCleanup();
            v_Invalidate();
            bool b_ = b_Render (pco_dc);
            if (!b_)
            {
                return false;
            }
            b_ = b_LinearOffsetToAbsoluteOffset (i_linearOffset, i_offset);
            if (!b_)
            {
                ATLASSERT(0);
                return false;
            }
        }
        else
        {
            v_PartialRender (pco_dc, 
                             i_NTokens() - i_oldSize, 
                             i_lastTokenBefore, 
                             i_tokensAfter);
        }
    }

    i_oldSize = i_NTokens();
    bool b_ = true;
    b_ = CT_KaiString::b_InsertChar_ (i_offset, ui_char, i_lastTokenBefore, i_tokensAfter);
    if (!b_)
    {
        ATLASSERT(0);
        ERROR_TRACE (_T("Insertion error. Paragraph re-formatting aborted."));
        return false;
    }

    v_PartialRender (pco_dc, i_NTokens() - i_oldSize, i_lastTokenBefore, i_tokensAfter);

    pco_Dc_ = NULL;

    return true;
}

bool CT_Paragraph::b_DeleteChar (CDC * pco_dc, CT_TextPos& co_char)
{
    pco_Dc_ = pco_dc;

    if (co_char.pco_GetParagraph()->i_At() != this->i_At())
    {
        ERROR_TRACE (_T("Text not in current paragraph."));
        return false;
    }

    int i_at = co_char.i_GetParagraphOffset();

    int i_token = i_GetTokenNumFromOffset (i_at);
    if (i_token <= 0)
    {
        ATLASSERT (0);
        ERROR_TRACE (_T("Bad token number."));
        return false;
    }

    if (ec_TokenDiacritics == vo_Tokens_[i_token].eo_TokenType)
    {
        ATLASSERT (0);
        ERROR_TRACE (_T("Bad token number."));
        return false;
    }

    int i_oldSize = i_NTokens();
    int i_lastTokenBefore = 0;
    int i_tokensAfter = 0;
    bool b_canOptimize = false;
    bool b_ = CT_KaiString::b_DeleteChar_ (i_at, 
                                           b_canOptimize, 
                                           i_lastTokenBefore, 
                                           i_tokensAfter);
    if (!b_canOptimize)
    {
        v_FdCleanup();
        v_Invalidate();
    }
    else
    {
        v_PartialRender (pco_dc, 
                         i_NTokens() - i_oldSize, 
                         i_lastTokenBefore, 
                         i_tokensAfter);
    }

    pco_Dc_ = NULL;

    return true;

}    // b_DeleteChar()

bool CT_Paragraph::b_ChangeFont (CT_TextPos& co_start,
                                 CT_TextPos& co_onePastEnd,
                                 unsigned int ui_newFont,
                                 ET_FontModifier eo_newStyle,
                                 bool b_add)
{
    CT_TextPos co_top, co_bottom;
    if (co_start <= co_onePastEnd)
    {
        co_top = co_start;
        co_bottom = co_onePastEnd;
    }
    else
    {
        co_top = co_onePastEnd;
        co_bottom = co_start;
    }

    int i_thisParagraph = i_At();

    //
    // Remove all existing FD's from the range
    //
    int i_startOffset = (co_top.i_GetParagraph() == i_thisParagraph) 
                                  ? co_top.i_GetParagraphOffset()
                                  : 0;
    int i_segmentLength = (co_bottom.i_GetParagraph() == i_thisParagraph)
                                  ? co_bottom.i_GetParagraphOffset() - i_startOffset
                                  : length() - i_startOffset;

    int i_firstToken = i_GetTokenNumFromOffset (i_startOffset);

    // Copy preceding tokens, find last relevant font 
    unsigned int ui_lastFd = vo_Tokens_[0].ui_FontDescriptor;
    int i_prior = 0;
    for (; i_prior < i_firstToken; ++i_prior)
    {
        if (vo_Tokens_[i_prior].eo_TokenType == ec_TokenFormatting)
        {
            ui_lastFd = vo_Tokens_[i_prior].ui_FontDescriptor;
        }
    }

    STL_STRING str_newFd;

    bool b_modify = (0 == ui_newFont);
    if (b_modify)
    {
        bool b_ = pco_ParentDoc_->b_ModifyFontDescriptor (ui_lastFd,
                                                          eo_newStyle, 
                                                          b_add, 
                                                          ui_newFont);
        if (!b_)
        {
            ATLASSERT(0);
            return false;
        }
        ATLASSERT (ui_newFont);
    }
    
    bool b_ = b_CreateFDString (ui_newFont, str_newFd);
    if (!b_)
    {
        ATLASSERT(0);
        return false;
    }

    // Save affected tokens
    vector<ST_Token> vec_affectedTokens;

    if ((i_firstToken > 0) && 
        (ec_TokenFormatting == vo_Tokens_[i_firstToken].eo_TokenType))
    {
        i_startOffset = vo_Tokens_[i_firstToken].i_Offset - 1;
        --i_firstToken;
    }

    int i_newSegmentLength = i_segmentLength;
    for (int i_t = i_firstToken; i_t < i_NTokens(); ++i_t)
    {
        ST_Token& st_t = vo_Tokens_[i_t];
//        if ((st_t.i_Offset >= i_startOffset + i_segmentLength) || 
//            (st_t.eo_TokenType != ec_TokenFormatting))
        if (st_t.i_Offset < i_startOffset + i_segmentLength)
        {
            if (st_t.eo_TokenType != ec_TokenFormatting)
            {
                vec_affectedTokens.push_back (st_t);
            }
            else
            {
                i_newSegmentLength -= st_t.i_Length;
                if (b_modify)
                {
                    ui_lastFd = st_t.ui_FontDescriptor;
                    b_ = pco_ParentDoc_->b_ModifyFontDescriptor (st_t.ui_FontDescriptor,
                                                                 eo_newStyle, 
                                                                 b_add, 
                                                                 st_t.ui_FontDescriptor);
                    if (!b_)
                    {
                        return false;
                    }
                    ATLASSERT (st_t.ui_FontDescriptor);
                    vec_affectedTokens.push_back (st_t);
                }
                else
                {
                    ui_lastFd = st_t.ui_FontDescriptor;
                    pco_ParentDoc_->b_UnRegisterFont (st_t.ui_FontDescriptor);
                }
            }
        }
    }       //  for (int i_t = i_firstToken...

    // Reassemble paragraph
    STL_STRING str_new = substr (0, vo_Tokens_[i_firstToken].i_Offset);
    STL_STRING str_fd;
    vector<ST_Token>::iterator it_ = vec_affectedTokens.begin();
    for (; it_ != vec_affectedTokens.end(); ++it_)
    {
        if (ec_TokenFormatting == (*it_).eo_TokenType)
        {
            ATLASSERT ((*it_).ui_FontDescriptor);
            b_ = b_CreateFDString ((*it_).ui_FontDescriptor, str_fd);
            if (!b_)
            {
                ATLASSERT (0);
                return false;
            }
            str_new += str_fd;
            i_newSegmentLength += str_fd.length();
        }
        else
        {
            str_new += substr ((*it_).i_Offset, (*it_).i_Length);
        }
    }

    int i_oldLength = length();
    str_new = str_new.substr (0, i_startOffset) + 
              str_newFd + 
              str_new.substr (i_startOffset, i_newSegmentLength);
    
    if (i_startOffset + i_segmentLength < i_oldLength)
    {
        STL_STRING str_oldFd;
        b_ = b_CreateFDString (ui_lastFd, str_oldFd);
        ATLASSERT (b_);
        str_new.append (str_oldFd + substr (i_startOffset + i_segmentLength));
    }
    else
    {
        ATLASSERT (i_oldLength == i_startOffset + i_segmentLength);
    }

    assign (str_new);

    v_Invalidate();
    v_FdCleanup();

    return true;

}  //  bool CT_Paragraph::v_ChangeFont (...)

// Find a segment, i.e. word, punctuation mark, etc. that contains co_in
bool CT_Paragraph::b_GetTokenPos (const CT_TextPos& co_in, 
                                  CT_TextPos& co_start, 
                                  CT_TextPos& co_end)
{
    int i_pOffset = co_in.i_GetParagraphOffset();
    int i_token = i_GetTokenNumFromOffset (i_pOffset);
    const ST_Token& rst_token = rst_GetToken (i_token);
    co_start.v_SetParagraph (co_in.pco_GetParagraph());
    co_start.v_SetParagraphOffset (rst_token.i_Offset);

    co_end.v_SetParagraph (co_in.pco_GetParagraph());
    int i_length = rst_token.i_Length;
    if (ec_TokenSpace == rst_token.eo_TokenType)
    {
        for (int i_at = i_token+1; i_at < i_NTokens(); ++i_at)
        {
            const ST_Token& rst_next = rst_GetToken (i_at);
            if (ec_TokenSpace == rst_next.eo_TokenType)
            {
                i_length += rst_next.i_Length;
            }
            else
            {
                break;
            }
        }
    }

    co_end.v_SetParagraphOffset (rst_token.i_Offset + i_length);

    return true;

}   //  b_GetTokenPos (...)

void CT_Paragraph::v_ClearLines_()
{
    for (unsigned int ui_at = 0; ui_at < vpo_Lines_.size(); ++ui_at)
    {
        CT_Line * pco_l = vpo_Lines_[ui_at];
        delete pco_l;
    }
    vpo_Lines_.clear();
}

void CT_Paragraph::v_DeepCopy_ (const CT_Paragraph& co_paragraph)
{
    CT_KaiString::v_DeepCopy_ (co_paragraph);
    pco_ParentDoc_ = co_paragraph.pco_ParentDoc_;
    pco_Previous_ = co_paragraph.pco_Previous_;
    pco_Next_ = co_paragraph.pco_Next_;
    b_Current_ = false;                //  don't copy formatting
    b_IsLastParagraph_ = co_paragraph.b_IsLastParagraph();
    eo_Alignment_ = co_paragraph.eo_Alignment_;
//    i_PageWidth_ = co_paragraph.i_PageWidth_;
    st_Spacing_.eo_Type = co_paragraph.st_Spacing_.eo_Type;
    st_Spacing_.i_Size = co_paragraph.st_Spacing_.i_Size;
    i_LeftIndent_ = co_paragraph.i_LeftIndent_;
    i_RightIndent_ = co_paragraph.i_RightIndent_;

    vi_TabStops_ = co_paragraph.vi_TabStops_;

    v_ClearLines_();
}

CT_Line * CT_Paragraph::pco_GetLine (int i_line) const
{
    if (!b_Current_)
    {
        ERROR_TRACE (_T("The paragraph is not formatted."));
        return NULL;
    }
    if ((i_line < 0) || (i_line >= static_cast<int>(vpo_Lines_.size())))
    {
        ERROR_TRACE (_T("Line index out of range."));
        return vpo_Lines_[vpo_Lines_.size()-1];
    }

    return vpo_Lines_[i_line];

}    //  pco_GetLine (...)

int CT_Paragraph::i_GetLine (CT_Line * pco_line)
{
    vector<CT_Line *>::iterator it_l = ::find (vpo_Lines_.begin(), 
                                               vpo_Lines_.end(), 
                                               pco_line);
    if (it_l == vpo_Lines_.end())
    {
        throw CT_KaiException (CT_KaiException::ec_StringFieldOutOfRange,
                               _T("KaiParagraph.cpp"),
                               _T("CT_KaiParagraph"),
                               _T("i_GetLine (...)"),
                               _T("Line not in array"));
    }

    return static_cast<int> (it_l - vpo_Lines_.begin());
}

int CT_Paragraph::i_At()
{
    int i_at = 0;
    CT_Paragraph * pco_p = this;

    while (pco_p->pco_Previous_ != NULL)
    {
        ++i_at;
        pco_p = pco_p->pco_Previous_;
    }

    return i_at;
}

CT_FontDescriptor * CT_Paragraph::pco_GetFontDescriptor (unsigned long ui_key)
{
    CT_DocDescriptor * pco_d = pco_GetDocument();
    ATLASSERT (pco_d);

    CT_FontDescriptor * pco_fd = NULL;
    try
    {
        pco_fd = pco_d->pco_GetFontDescriptor (ui_key);
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return NULL;
    }
    ATLASSERT (pco_fd);

    return pco_fd;
}

int CT_Paragraph::i_Height()
{
    if (!b_Current_)
    {
        throw CT_KaiException (CT_KaiException::ec_ParagraphNotRendered,
                               _T("KaiParagraph.cpp"),
                               _T("CT_KaiParagraph"),
                               _T("i_Height()"),
                               _T("CT_Paragraph::b_Current_ is \'false\' "));
    }
    if (i_Height_ >= 0)
    {
        return i_Height_;
    }

    i_Height_ = 0;

    CT_Line * pco_line = NULL;
    for (unsigned int ui_line = 0; ui_line < vpo_Lines_.size(); ++ui_line)
    {
        pco_line = vpo_Lines_[ui_line];
        i_Height_ += pco_line->i_GetLineHeight();
    }

    return i_Height_;

}  //  i_Height();

//
// Diagnostics
//
void CT_Paragraph::v_Dump()
{
    DUMP_TRACE (_T("-------- CT_Paragraph -----------------------"));

    STL_STRING str_parentDoc = _T("\tpco_ParentDoc_ = ");
    str_parentDoc += STR_UIToStr (reinterpret_cast <int>(pco_ParentDoc_));
    ERROR_TRACE (str_parentDoc);

    STL_STRING str_prev = _T("\tpco_Previous_ = ");
    str_prev += STR_UIToStr (reinterpret_cast <int>(pco_Previous_));
    DUMP_TRACE (str_prev);

    STL_STRING str_next = _T("\tpco_Next_ = ");
    str_next += STR_UIToStr (reinterpret_cast <int>(pco_Next_));
    DUMP_TRACE (str_next);
    DUMP_TRACE (_T("\tstring: "));
    CT_KaiString::v_Dump();

    STL_STRING str_isCurrent = _T("\tb_Current_ = ");
    b_Current_ ? str_isCurrent += _T("true") : str_isCurrent += _T("false");
    DUMP_TRACE (str_isCurrent);

    STL_STRING str_isLastParagraph = _T("\tb_IsLastParagraph_ = "); 
    b_IsLastParagraph_ ? str_isLastParagraph += _T("true") : 
                         str_isLastParagraph += _T("false");
    DUMP_TRACE (str_isLastParagraph);

    STL_STRING str_alignment;
    switch (eo_Alignment_)
    {
        case ec_AlignmentFront:
        {
            str_alignment = _T("ec_AlignmentFront");
            break;
        }
        case ec_Left:
        {
            str_alignment = _T("ec_Left");
            break;
        }
        case ec_Center:
        {
            str_alignment = _T("ec_Center");
            break;
        }
        case ec_Right:
        {
            str_alignment = _T("ec_Right");
            break;
        }
        case ec_RightJustified:
        {
            str_alignment = _T("RightJustified");
            break;
        }
        case ec_AlignmentBack:
        {
            str_alignment = _T("ec_AlignmentBack");
            break;
        }
        default:
        {
            str_alignment = _T("Unknown; numeric value = ");
            str_alignment += STR_UIToStr (static_cast <int>(eo_Alignment_));
        }
    }   
    DUMP_TRACE (CT_KaiString (_T("\teo_Alignment_ = ")) + str_alignment);

//    V_DumpTrace (str_location, CT_KaiString (_T("\ti_PageWidth_ = "))
//                               + STR_IToStr (i_PageWidth_));
    DUMP_TRACE (CT_KaiString (_T("\tst_Spacing_.eo_Type = ")) + 
                STR_IToStr (st_Spacing_.eo_Type));
    DUMP_TRACE (CT_KaiString (_T("\tst_Spacing_.i_Size = ")) + 
                STR_IToStr (st_Spacing_.i_Size));
    DUMP_TRACE (CT_KaiString (_T("\ti_LeftIndent_ = ")) + 
                STR_IToStr (i_LeftIndent_));
    DUMP_TRACE (CT_KaiString (_T("\ti_RightIndent_ = ")) + 
                STR_IToStr (i_RightIndent_));

    DUMP_TRACE (_T("\tvi_TabStops_ = "));
    for (unsigned int ui_ts = 0; ui_ts < vi_TabStops_.size(); ++ui_ts)
    {
        DUMP_TRACE (_T("\t\t") + STR_IToStr (vi_TabStops_[ui_ts]));
    }

    DUMP_TRACE (CT_KaiString (_T("\tvpo_Lines__ =")));
    for (unsigned int ui_l = 0; ui_l < vpo_Lines_.size(); ++ui_l)
    {
        vpo_Lines_[ui_l]->v_Dump();
    }

    DUMP_TRACE (_T("-------- CT_Paragraph: end of dump --------\n"));

}   //  v_Dump()
