//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: implementation of the line descriptor helper class.
//
//    $Id: KaiLineDescriptor.cpp,v 1.94 2007-12-20 01:44:47 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning (disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiParagraph.h"
#include "KaiFontDescriptor.h"
#include "KaiLineDescriptor.h"
#include "KaiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CT_Line::CT_Line()
{
    v_Null();
}

CT_Line::CT_Line (CT_Paragraph * const pco_parent, 
                  int i_firstToken, 
                  int i_firstTokenOffset,
                  int i_firstTokenScreenLength) :
pco_Parent_ (pco_parent), 
i_FirstToken_ (i_firstToken), 
i_FirstTokenOffset_ (i_firstTokenOffset),
i_FirstTokenScreenLength_ (i_firstTokenScreenLength)
{
    b_Hyphenated_ = false;
    i_NTokens_ = 0;
    i_LastTokenLength_ = UNDEFINED;
    i_LastTokenScreenLength_ = UNDEFINED;
    i_TextLength_ = 0;
    i_VisibleTextLength_ = 0;
    i_ScreenLength_ = UNDEFINED;
    i_UnjustifiedScreenLength_ = UNDEFINED;
    i_ScreenHeight_ = 0;
    d_PrintScaleFactor_ = 0.0;

    b_Valid_ = false;
}

CT_Line::CT_Line (const CT_Line& co_ld)
{
    v_DeepCopy_ (co_ld);
}

void CT_Line::v_Null()
{
    b_Hyphenated_ = false;
    i_NTokens_ = 0;
    i_FirstTokenOffset_ = UNDEFINED;
    i_FirstTokenScreenLength_ = UNDEFINED;
    i_LastTokenLength_ = UNDEFINED;
    i_LastTokenScreenLength_ = UNDEFINED;
    i_TextLength_ = 0;
    i_VisibleTextLength_ = 0;
    i_ScreenLength_ = UNDEFINED;
    i_UnjustifiedScreenLength_ = UNDEFINED;
    i_ScreenHeight_ = 0;
    d_PrintScaleFactor_ = 0.0;

    b_Valid_ = false;
}

CT_Line::~CT_Line()
{}

CT_Line& CT_Line::operator = (const CT_Line& co_ld)
{
    v_DeepCopy_ (co_ld);

    return *this;
}

//
// Two operators needed to export STL thingies
//
bool CT_Line::operator == (CT_Line& co_ld)
{
    bool b_parent = (pco_Parent_ == co_ld.pco_Parent_);
    bool b_isHyphenated = (b_Hyphenated_ == co_ld.b_Hyphenated_);

    return (b_parent && 
            b_isHyphenated && 
            (i_FirstToken_ == co_ld.i_FirstToken_) &&
            (i_GetStart() == co_ld.i_GetStart()) &&
            (i_NTokens_ == co_ld.i_NTokens_) &&
            (i_LastTokenLength_ == co_ld.i_LastTokenLength_));
}

bool CT_Line::operator < (CT_Line& co_ld)
{
    return i_GetStart() < co_ld.i_GetStart();
}

//
// Find horizontal position
//
int CT_Line::i_ScreenPosFromParagraphOffset (int i_paragraphOffset, 
                                             CDC * pco_dc)
{
    //
    // Measure tokens to the left
    //
    int i_screenLength = 0;
    int i_lastTokenOffset = 0;
    int i_lastTokenLength = 0;
    int i_token = 0;
    for (; i_token < i_NTokens_; ++i_token)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_token];
        if (!rst_t.b_IsLinearText())
        {
            continue;
        }

        i_lastTokenOffset = i_GetTokenOffset (i_token);
        i_lastTokenLength = i_GetTokenLength (i_token);
        if (i_lastTokenOffset + i_lastTokenLength > i_paragraphOffset)
        {
            break;
        }

        i_screenLength += i_GetTokenScreenLength (i_token);
        if (b_IsRightJustified())
        {
            i_screenLength += rst_t.i_ExtraSpace;
        }
    }

    if (i_lastTokenOffset + i_lastTokenLength == i_paragraphOffset)
    {
        return i_screenLength;
    }

    if (i_token == i_NTokens_)
    {
        return i_screenLength;
    }

    ATLASSERT (i_token < i_NTokens_);

    ST_Token& rst_token = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_token];

    //
    // Count all non-format linear characters from start of
    // token to paragraph offset 
    //
    if (!rst_token.b_IsLinearText())
    {
        ATLASSERT(0);
        ERROR_TRACE (_T("Illegal token type."));
        return i_screenLength;
    }

    int i_textLength = i_paragraphOffset - i_lastTokenOffset;
    switch (rst_token.eo_TokenType)
    {
        case ec_TokenText:
        case ec_TokenEndOfParagraph:
        {
            CSize co_s;
            bool b_ = pco_Parent_->b_MeasureString (pco_dc, 
                                                    i_lastTokenOffset,
                                                    i_textLength,
                                                    co_s);
            if (!b_)
            {
                return 0;
            }
            i_screenLength += co_s.cx;

            break;
        }
        case ec_TokenSpace:
        case ec_TokenTab:
        {
            int i_tokenScreenLength = i_GetTokenScreenLength (i_token);
            if (b_IsRightJustified())
            {
                i_tokenScreenLength += rst_token.i_ExtraSpace;
            }
            double fl_lengthPerChar = i_tokenScreenLength/i_GetTokenLength (i_token);
            i_screenLength += I_Round (fl_lengthPerChar * i_textLength);
            
            break;
        }

//        case ec_TokenDiacritics:
//        {
//            // *** TBD ***
//            break;
//        }

    } // switch...

    return i_screenLength;

}    //  int CKaiView::i_ScreenPos (CT_TextPos& co_textPos)

int CT_Line::i_ParagraphOffsetFromScreenPos (int i_screenPos, CDC * pco_dc)
{
    int i_screenLength = 0;
    int i_token = 0;
    for (; i_token < i_NTokens_; ++i_token)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_token];
        if (!rst_t.b_IsLinearText())
        {
            continue;
        }
        int i_tokenScreenLength = i_GetTokenScreenLength (i_token);
        if (i_screenLength + i_tokenScreenLength > i_screenPos)
        {
            break;
        }

        i_screenLength += i_tokenScreenLength;
        if (b_IsRightJustified())
        {
            i_screenLength += rst_t.i_ExtraSpace;
        }

    }   // for (; i_token < i_NTokens_...)

    if (i_NTokens_ == i_token)
    {
        return i_GetTextLength();
    }

    int i_partialTokenLength = 0;
    ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_token];
    switch (rst_t.eo_TokenType)
    {
        case ec_TokenText:
        case ec_TokenPunctuation:
        {
            int i_oldCX = 0;
            for (i_partialTokenLength = 1; 
                 i_partialTokenLength <= rst_t.i_GetLinearTextLength(); 
                 ++i_partialTokenLength)
            {
                CSize co_s;
                bool b_ = pco_Parent_->b_MeasureString (pco_dc, 
                                                        i_GetTokenOffset(i_token),
                                                        i_partialTokenLength,
                                                        co_s);
                if (!b_)
                {
                    return false;
                }
                double fl_threshold = (co_s.cx - i_oldCX)/2;
                if (i_screenLength + co_s.cx > i_screenPos)
                {
                    if (i_screenLength + i_oldCX + fl_threshold > i_screenPos)
                    {
                        --i_partialTokenLength;
                    }
                    break;
                }
                i_oldCX = co_s.cx;
            }

            break;
        }
        case ec_TokenSpace:
        {
            int i_tokenLength = i_GetTokenScreenLength (i_token);
            if (b_IsRightJustified())
            {
                i_tokenLength += rst_t.i_ExtraSpace;
            }
            double fl_lengthPerSpace = i_tokenLength/i_GetTokenLength (i_token);
            i_partialTokenLength = 
                        I_Round ((i_screenPos-i_screenLength)/fl_lengthPerSpace);
            i_screenLength += I_Round (fl_lengthPerSpace*i_partialTokenLength);
            
            break;
        }
        case ec_TokenTab:
        {
            int i_tokenLength = i_GetTokenScreenLength (i_token);
            double fl_lengthPerTab = i_tokenLength/i_GetTokenLength (i_token);
            i_partialTokenLength = 
             I_Round ((i_screenPos-i_screenLength)/fl_lengthPerTab);
            i_screenLength += I_Round (fl_lengthPerTab*i_partialTokenLength);

            break;
        }
        case ec_TokenEndOfParagraph:
        {
            i_partialTokenLength = 0;
            break;
        }
        default:
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Bad token type."));
            return i_GetTokenOffset (i_token) + i_partialTokenLength;
        }


    } // switch...

    int i_offset = i_GetTokenOffset (i_token) + i_partialTokenLength;
    if (0 == i_partialTokenLength)
    {
        if (0 < i_token)
        {
            ST_Token& rst_prev = pco_Parent_->vo_Tokens_[i_token-1];
            if (ec_TokenDiacritics == rst_prev.eo_TokenType)
            {
                i_offset = rst_prev.i_Offset;
            }
        }
    }
    return i_offset;

//    return i_GetTokenOffset (i_token) + i_partialTokenLength;

}   //  i_ParagraphOffsetFromScreenPos (...)

bool CT_Line::b_ParagraphOffsetFromLineOffset (int i_lineOffset, 
                                               int& i_paragraphOffset)
{
    if (i_lineOffset >= i_GetVisibleTextLength())
    {
        ATLASSERT (i_lineOffset == i_VisibleTextLength_);
        i_paragraphOffset = i_TextLength_;
        return true;
    }

    i_paragraphOffset = 0;

    //
    // Count all complete tokens on the left
    //
    int i_wholeTokenVisibleLength = 0;
    int i_partialLength = 0;
    int i_t = 0;
    for (; i_t < i_NTokens_; ++i_t)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_t];
        if (!rst_t.b_IsLinearText())
        {
            continue;
        }

        int i_tokenLength = i_GetTokenLength (i_t);
        if (i_wholeTokenVisibleLength + i_tokenLength > i_lineOffset)
        {
             i_partialLength = i_lineOffset - i_wholeTokenVisibleLength;
             break;
        }

        i_wholeTokenVisibleLength += i_tokenLength;
//        i_paragraphOffset += i_GetTokenOffset (i_t) + i_tokenLength;
    }
    if (i_NTokens_ == i_t)
    {
        ERROR_TRACE (_T("Token out of range."));
        return false;
    }

    i_paragraphOffset = i_GetTokenOffset (i_t) + i_partialLength;

    if (0 == i_partialLength)
    {
        if (0 < i_t)
        {
            ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_t-1];
            if (ec_TokenDiacritics == rst_t.eo_TokenType)
            {
                i_paragraphOffset = rst_t.i_Offset;
            }
        }
    }

    return true;

}    //  b_ParagraphOffsetFromLineOffset

bool CT_Line::b_LineOffsetFromParagraphOffset (int i_paragraphOffset, 
                                               int& i_lineOffset)
{
    if (!b_Valid_)
    {
        v_Validate_();
    }

    i_lineOffset = 0;
    int i_t = 0;
    for (; i_t < i_NTokens_; ++i_t)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_t];
        if (i_GetTokenOffset (i_t) + i_GetTokenLength (i_t) > i_paragraphOffset)
        {
            break;
        }
        
        if (rst_t.b_IsLinearText())
        {
            i_lineOffset += i_GetTokenLength (i_t);
        }
    }

    i_lineOffset += i_paragraphOffset - i_GetTokenOffset (i_t);

    return true;

}   //  b_LineOffsetFromParagraphOffset (...)

int CT_Line::i_At()
{
    return pco_Parent_->i_GetLine (this);
}

int CT_Line::i_GetStart()
{
    if (!b_Valid_)
    {
        v_Validate_();
    }

    ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_];
    return rst_t.i_Offset + i_FirstTokenOffset_;
}

int CT_Line::i_GetTextLength()
{
    if (!b_Valid_)
    {
        v_Validate_();
    }

    return i_TextLength_;

}   //  i_GetTextLength()

int CT_Line::i_GetTokenOffset (int i_token)
{
    if (!b_Valid_)
    {
        v_Validate_();
    }

    int i_offset = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_token].i_Offset;
    if (0 == i_token)
    {
        return i_offset + i_FirstTokenOffset_;
    }

    return i_offset;
}

int CT_Line::i_GetTokenLength (int i_token)
{
    if (!b_Valid_)
    {
        v_Validate_();
    }

    if (i_NTokens_ - 1 == i_token)
    {
        return i_LastTokenLength_;
    }

    ST_Token& rst_token = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_token];
    if (0 == i_token)
    {
        return rst_token.i_Length - i_FirstTokenOffset_;
    }
    
    return rst_token.i_Length;
}

int CT_Line::i_GetTokenScreenLength (int i_token)
{
    if (!b_Valid_)
    {
        v_Validate_();
    }

    if (0 == i_token)
    {
        return i_FirstTokenScreenLength_;
    }
    
    if (i_NTokens_ - 1 == i_token)
    {
        return i_LastTokenScreenLength_;
    }

    ST_Token& rst_token = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_token];
    return rst_token.i_ScreenLength;
}

int CT_Line::i_GetVisibleTextLength()
{
    if (!b_Valid_)
    {
        v_Validate_();
    }
    return i_VisibleTextLength_;
}

int CT_Line::i_GetScreenLength()
{
    if (UNDEFINED != i_ScreenLength_)
    {
        return i_ScreenLength_;
    }

    i_ScreenLength_ = 0;
    for (int i_t = 0; i_t < i_NTokens_; ++i_t)
    {
        ST_Token& rst_token = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_t];
        if (!rst_token.b_IsLinearText())
        {
            continue;
        }
        i_ScreenLength_ += i_GetTokenScreenLength (i_t);
        if (this->b_IsRightJustified())
        {
            i_ScreenLength_ += rst_token.i_ExtraSpace;
        }
    }

    return i_ScreenLength_;

}   //  i_GetScreenLength()

int CT_Line::i_GetUnJustifiedLength()
{
    if (UNDEFINED != i_UnjustifiedScreenLength_)
    {
        return i_UnjustifiedScreenLength_;
    }

    int i_length = 0;

    for (int i_t = 0; i_t < i_NTokens_; ++i_t)
    {
        i_length += i_GetTokenScreenLength (i_t);
    }
    return i_length;
}

int CT_Line::i_GetSpacing()
{
    int i_spacing = 0;

    ET_LineSpacingType eo_type = pco_Parent_->eo_GetSpacingType();
    switch (eo_type)
    {
        case ec_LineSpacingAuto:
        {
            i_spacing = (int)((double)i_GetLineHeight() * .2);
            break;
        }
        case ec_LineSpacingLines:
        {
            i_spacing = (int)((double)i_GetLineHeight() * 
                              ((double)pco_Parent_->i_GetSpacingSize()/100 - 1));
            break;
        }
        case ec_LineSpacingPoints:
        {
            i_spacing = pco_Parent_->i_GetSpacingSize()/10;
            break;
        }
        case ec_LineSpacingFront:
        case ec_LineSpacingBack:
        default:
        {
            ERROR_TRACE (_T("Bad spacing type."));
            i_spacing = (int)((double) i_GetLineHeight() * 1.2);
        }
    }

    return i_spacing;

}   //  i_GetSpacing()

int CT_Line::i_GetScreenHeight()
{
    if (i_ScreenHeight_ > 0)
    {
        return i_ScreenHeight_;
    }

    i_ScreenHeight_ = 0;

    for (int i_t = 0; i_t < i_NTokens_; ++i_t)
    {
        ST_Token& rst_token = pco_Parent_->vo_Tokens_ [i_FirstToken_ + i_t];
        i_ScreenHeight_ = max (i_ScreenHeight_, rst_token.i_ScreenHeight);
    }
    return i_ScreenHeight_;
}

int CT_Line::i_GetDiacrHeight() const
{
    // *** STUB ***
    return 0;
}

int CT_Line::i_GetRemainingSpace()
{
    int i_remSpace = pco_Parent_->i_GetMaxLineWidth();

    for (int i_t = 0; i_t < i_NTokens_; ++i_t)
    {
        i_remSpace -= i_GetTokenScreenLength (i_t);
    }

    return i_remSpace;
}

//
// Needed for qsort in the func below
//
int compare (const void * arg1, const void * arg2)
{
    const int * pi_ind1 = static_cast <const int *>(arg1);
    const int * pi_ind2 = static_cast <const int *>(arg2);

    if (pi_ind1[1] == pi_ind2[1])
    {
        return 0;
    }    
    if (pi_ind1[1] > pi_ind1[1])
    {
        return -1;
    }
    else
    {    
        return 1;
    }
}

bool CT_Line::b_RightJustify (bool b_isPrinting)
{
    if (pco_Parent_->eo_Alignment_ != ec_RightJustified)
    {
        return true;
    }

//    ATLASSERT (!(i_GetStart() + i_GetTextLength() > 
//                 static_cast<int>(pco_Parent_->length())));
    
    if (i_GetStart() + i_GetTextLength() >= static_cast<int>(pco_Parent_->length()))
    {
        return true;
    }

    int i_lastTab = i_NTokens_ - 1;
    for (; i_lastTab >= 0; --i_lastTab)
    {
        ST_Token& rst_token = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_lastTab];
        if (ec_TokenTab == rst_token.eo_TokenType)
        {
            break;
        }
    }
    
    int i_lastTextToken = i_NTokens_ - 1;
    for (; i_lastTextToken >= 0; --i_lastTextToken)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_lastTextToken];
        if ((ec_TokenText == rst_t.eo_TokenType) ||
            (ec_TokenPunctuation == rst_t.eo_TokenType))
        {
            break;
        }
    }

    //
    // Count the total number of spaces to the right of the last tab
    //
    vector<ST_Token *> vpo_spaceTokens;
    vpo_spaceTokens.reserve (i_NTokens_);
    int i_t = i_lastTab + 1;
    for (; i_t < i_lastTextToken; ++i_t)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_t];
        if (b_isPrinting)
        {
            rst_t.i_ExtraPrintSpace = 0;
        }
        else
        {
            rst_t.i_ExtraSpace = 0;
        }
        if (ec_TokenSpace == rst_t.eo_TokenType)
        {
            vpo_spaceTokens.push_back (&rst_t);
        }
    }

    int i_spaceTokens = vpo_spaceTokens.size();

    int i_spaceToAdd = pco_Parent_->i_GetMaxLineWidth();
    if (b_isPrinting)
    {
        i_spaceToAdd = I_Round (i_spaceToAdd * 1.05);
    }

    for (i_t = 0; i_t <= i_lastTextToken; ++i_t)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_t];
        if (ec_TokenDiacritics != rst_t.eo_TokenType)
        {
            i_spaceToAdd -= b_isPrinting ? rst_t.i_PrintLength
                                         : rst_t.i_ScreenLength;
        }
    }

    random_shuffle (vpo_spaceTokens.begin(), vpo_spaceTokens.end());
    while ((i_spaceToAdd > 0) && (i_spaceTokens > 0))
    {
        if (b_isPrinting)
        {
            make_heap (vpo_spaceTokens.begin(), 
                       vpo_spaceTokens.end(), 
                       b_ComparePrintSpaceTokens_());
        }
        else
        {
            make_heap (vpo_spaceTokens.begin(), 
                       vpo_spaceTokens.end(), 
                       b_CompareSpaceTokens_());
        }
        for (int i_st = 0; (i_st < i_spaceTokens) && (i_spaceToAdd > 0); ++i_st)
        {
            ST_Token * pst_head = *(vpo_spaceTokens.begin());
            if (b_isPrinting)
            {
                pst_head->i_ExtraPrintSpace++;
            }
            else
            {
                pst_head->i_ExtraSpace++;
            }

            --i_spaceToAdd;

            //if (b_isPrinting)
            //{
            //    pop_heap (vpo_spaceTokens.begin(), 
            //              vpo_spaceTokens.end(), 
            //              b_ComparePrintSpaceTokens_());
            //}
            //else
            //{
            //    pop_heap (vpo_spaceTokens.begin(), 
            //              vpo_spaceTokens.end(), 
            //              b_CompareSpaceTokens_());
            //}
        }
    }

    return true;

}    //  bool CT_Line::b_RightJustify()

bool CT_Line::b_IsRightJustified()
{
    CT_Paragraph * pco_p = pco_GetParent();

    bool b_justify = ((pco_p->eo_GetAlignment() == ec_RightJustified) &&
     (i_GetStart() + i_GetTextLength() < static_cast<int>(pco_p->length())));

    return b_justify;
}


bool CT_Line::b_IsLastInParagraph()
{ 
    bool b_ = i_GetStart() + i_GetTextLength() == pco_Parent_->length();
    return b_; 
}

void CT_Line::v_SetStartingFont (CDC * pco_dc)
{
    CFont * pco_startFont = pco_Parent_->pco_GetWindowsFont (pco_dc, i_GetStart());
    CFont * pco_oldFont = pco_dc->SelectObject (pco_startFont);
}

void CT_Line::v_Show (CKaiView * pco_view, CDC * pco_dc, int i_xPos, int i_yPos)
{
    pco_dc->SetBkMode (TRANSPARENT);

    int i_pixPerInch = pco_dc->GetDeviceCaps (LOGPIXELSX);
    i_xPos += (pco_dc->GetDeviceCaps (LOGPIXELSX) * 
              pco_Parent_->i_GetLeftIndent())/100;
    int i_screenOffset = i_xPos;
    int i_y = i_yPos + i_GetLineHeight();

    if (pco_view->b_EnumHighlights())
    {
        b_SetLineBackground_ (pco_view, pco_dc, i_xPos, i_yPos);
    }

    for (int i_t = 0; i_t < i_NTokens_; ++i_t)
    {
        i_screenOffset += i_ShowToken (pco_dc, i_t, i_screenOffset, i_y);
    }

    if (pco_view->b_SelectionOn())
    {
        v_SetLineSelection_ (pco_view, pco_dc, i_xPos, i_yPos);
    }

    pco_dc->SetBkMode (OPAQUE);

}  // v_Show (...)

void CT_Line::v_Print (CKaiView * pco_view, CDC * pco_dc, int i_xPos, int i_yPos)
{
    pco_dc->SetBkMode (TRANSPARENT);

    //
    // Justification done at time of rendering does not work for
    // printing. We'll have to re-calculate extra space and store it
    // temporarily with each compound.
    //
    if (!pco_dc->IsPrinting())
    {
        ERROR_TRACE (_T("Bad print DC"));
        return;
    }
    int i_y = i_yPos + i_GetLineHeight();

    if (b_IsRightJustified())
    {
        pco_Parent_->pco_Dc_ = pco_dc;

        unsigned int ui_font = 0;
        bool b_ = pco_Parent_->b_GetFontKey (0, ui_font);
        for (int i_t = 0; i_t < i_NTokens_; ++i_t)
        {
            ST_Token& rst_token = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_t];
            rst_token.i_PrintLength = 0;
            rst_token.i_ExtraPrintSpace = 0;
            if (rst_token.b_IsLinearText())
            {
                pco_Parent_->b_MeasureToken (rst_token, ui_font);
            }
            if (ec_TokenFormatting == rst_token.eo_TokenType)
            {
                ui_font = rst_token.ui_FontDescriptor;
            }
        }
        bool b_isPrinting = true;
        b_RightJustify (b_isPrinting);
    }

    int i_offset = i_xPos;
    for (int i_t = 0; i_t < i_NTokens_; ++i_t)
    {
        ST_Token& rst_token = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_t];
        if (0 == i_t)
        {
            CFont * pco_font = pco_Parent_->pco_GetWindowsFont (pco_dc, 
                                                                rst_token.i_Offset);
            CFont * pco_oldFont = pco_dc->SelectObject (pco_font);
        }
        i_offset += i_ShowToken (pco_dc, i_t, i_offset, i_y);
//        if (b_IsRightJustified())
//        {
//            i_offset += rst_token.i_ExtraPrintSpace;
//        }

        pco_Parent_->pco_Dc_ = NULL;
    }

}  // v_Print (...)

int CT_Line::i_ShowToken (CDC * pco_dc, int i_token, int i_x, int i_y)
{
    int * pi_tabs = const_cast<int *>(pco_Parent_->pi_GetTabStops());
    int i_outLength = 0;

    const ST_Token& rst_token = pco_Parent_->vo_Tokens_ [i_FirstToken_ + i_token];
    switch (rst_token.eo_TokenType)
    {
        case ec_TokenFormatting:
        {
            CT_FontDescriptor * pco_fd = 
                pco_Parent_->pco_GetFontDescriptor (rst_token.ui_FontDescriptor);
            CFont * pco_textFont = NULL;
            if (pco_dc->IsPrinting())
            {
                pco_textFont = pco_fd->pco_GetPrintFont (pco_dc);
            }
            else
            {
                pco_textFont = pco_fd->pco_GetWindowsFont();
            }
            CFont * pco_oldFont = pco_dc->SelectObject (pco_textFont);

            break;
        }

        case ec_TokenDiacritics:
        {
            v_ShowDiacritic (pco_dc, i_x, i_y, i_token);
            break;
        }

        case ec_TokenText:
        case ec_TokenPunctuation:
        case ec_TokenEndOfParagraph:
        {
            int i_offset = i_GetTokenOffset (i_token);
            LPCTSTR pchr_out = pco_Parent_->c_str();
            CSize co_s = pco_dc->TabbedTextOut (i_x, 
                                                i_y, 
                                                &pchr_out[i_offset],
                                                i_GetTokenLength (i_token),
                                                pco_Parent_->i_NTabs(),
                                                pi_tabs,
                                                i_x);
            i_outLength = co_s.cx;
            break;
        }

        case ec_TokenSpace:
        case ec_TokenTab:
        {
            LPCTSTR pchr_out = pco_Parent_->c_str();
            int i_offset = i_GetTokenOffset (i_token);
            CSize co_s = pco_dc->GetTabbedTextExtent (&pchr_out[i_offset], 
                                                      i_GetTokenLength (i_token),
                                                      pco_Parent_->i_NTabs(), 
                                                      pi_tabs);
            i_outLength = co_s.cx;
            if (b_IsRightJustified())
            {
                if (pco_dc->IsPrinting())
                {
                    i_outLength += rst_token.i_ExtraPrintSpace;
                }
                else
                {
                    i_outLength += rst_token.i_ExtraSpace;
                }
            }            

            break;
        }

        default:
        {
            ERROR_TRACE (_T("Unknown token type."));
        }

    }    //  switch

    return i_outLength;

}    //  int CT_Line::i_ShowToken (...)

void CT_Line::v_ShowDiacritic (CDC * pco_dc, 
                               int i_x, 
                               int i_y, 
                               int i_diacrToken)
{
    ST_Token& rst_diacrToken = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_diacrToken];
    STL_STRING str_diacr = pco_Parent_->substr (rst_diacrToken.i_Offset + 2, 
                                                rst_diacrToken.i_Length - 3);

    STL_STRING str_next;
    int i_at = i_FirstToken_ + i_diacrToken + 1;
    for (; i_at < pco_Parent_->i_NTokens(); ++i_at)
    {
        ST_Token& rst_next = pco_Parent_->vo_Tokens_[i_at];
        if (rst_next.eo_TokenType != ec_TokenDiacritics)
        {
            str_next = pco_Parent_->substr (rst_next.i_Offset, rst_next.i_Length);
            break;
        }
    }
    if (i_at == pco_Parent_->i_NTokens())
    {
        ERROR_TRACE (_T("No text token after diacritic."));
    }

    int i_vScreenOffset = 0;
    int i_hScreenOffset = 0;
    TEXTMETRIC st_tm;
    pco_dc->GetTextMetrics (&st_tm);
    if (str_diacr.length() > 1)
    {
//        i_vScreenOffset = st_tm.tmAscent;
    }
    else
    {
        GLYPHMETRICS st_gm;
        MAT2 st_m2;    // required but not used
        DWORD dw_ = pco_dc->GetGlyphOutline (str_next[0], 
                                             GGO_METRICS, 
                                             &st_gm, 
                                             0, 
                                             NULL, 
                                             &st_m2);

        CSize co_ext = pco_dc->GetTextExtent (str_next.data(), 1);
        i_hScreenOffset = co_ext.cx;

    }

    unsigned int ui_textFont = 0;
    for (int i_t = i_FirstToken_ + i_diacrToken - 1; i_t >= 0; --i_t)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_t];
        if (rst_t.eo_TokenType == ec_TokenFormatting)
        {
            ui_textFont = rst_t.ui_FontDescriptor;
            break;
        }
    }

    if (ui_textFont == 0)
    {
        ERROR_TRACE (_T("No formatting data in paragraph"));
        return;
    }

    CT_FontDescriptor * pco_fd = pco_Parent_->pco_GetFontDescriptor (ui_textFont);
    CFont * pco_textFont = NULL;
    if (pco_dc->IsPrinting())
    {
        pco_textFont = pco_fd->pco_GetPrintFont (pco_dc);
    }
    else
    {
        pco_textFont = pco_fd->pco_GetWindowsFont();
    }
    LOGFONT st_lf;
    pco_textFont->GetLogFont (&st_lf);
    st_lf.lfHeight = pco_fd->ui_GetPointSize() * 10;

    STL_STRING str_diacrFont;
    
    bool b_r = B_GetConfigValue (STL_STRING (_T("Default Font.FontForDiacritics")), 
                                 str_diacrFont);
    if (!b_r)
    {
        str_diacrFont = _T("Actus");
    }

    _tcscpy_s (st_lf.lfFaceName, LF_FACESIZE, str_diacrFont.data());
    st_lf.lfCharSet = DEFAULT_CHARSET;

    CFont co_diacrFont;
    BOOL ui_ = co_diacrFont.CreatePointFontIndirect (&st_lf, pco_dc);
    if (!ui_)
    {
        ERROR_TRACE (_T("Error creating diacritics font."));
        return;
    }
    CFont * pco_oldFont = pco_dc->SelectObject (&co_diacrFont);

    //if (str_diacr.length() == 1)
    //{
    //    CSize co_ext = pco_dc->GetTextExtent (str_diacr.data(), 1);
    //}

    pco_dc->TextOut (i_x + i_hScreenOffset, 
                     i_y + i_vScreenOffset,
                     str_diacr.data(),
                     str_diacr.length());

    pco_dc->SelectObject (pco_oldFont);

}    // void CT_Line::v_ShowDiacritic (...)

int CT_Line::i_GetParagraphTermWidth (CDC * pco_dc)
{
    CFont * pco_mfcFont = NULL;

//    if (i_GetTextLength() > 0)
//    {                
        pco_mfcFont = pco_Parent_->pco_GetWindowsFont (pco_dc, 
                                                       i_GetTextLength() - 1);
//    }
//    else
//    {
//        unsigned long ui_key = pco_Parent_->ul_GetFirstFd();
//        CT_FontDescriptor * pco_fd = pco_Parent_->
//                                        pco_GetFontDescriptor (ui_key);
//        pco_mfcFont = pco_fd->pco_GetWindowsFont (pco_dc);
//    }

    CFont * pco_oldFont = pco_dc->SelectObject (pco_mfcFont);
    TEXTMETRIC st_tm;
    BOOL mfcb_ = ::GetTextMetrics (pco_dc->GetSafeHdc(), &st_tm);
    if (!mfcb_)
    {
        ERROR_TRACE (_T("Error obtaining font info."));
        return 0;
    }
    pco_dc->SelectObject (pco_oldFont);
    return 2*st_tm.tmAveCharWidth;

}  //  i_GetParagraphTermWidth (...)

void CT_Line::v_GetLastPos (CT_TextPos& rco_lastPos)
{
    rco_lastPos.v_SetParagraph (pco_Parent_);
    rco_lastPos.v_SetLine (this->i_At());
    rco_lastPos.v_SetParagraphOffset (i_GetStart() + i_GetTextLength() - 1);
}

//
// Private
//
void CT_Line::v_Validate_()
{
    if (b_Valid_)
    {
        return;
    }

    b_Valid_ = true;

    ST_Token& rst_firstToken = pco_Parent_->vo_Tokens_[i_FirstToken_];
    ST_Token& rst_lastToken = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_NTokens_ - 1];
    if (UNDEFINED == i_FirstTokenOffset_)
    {
        ATLASSERT (UNDEFINED == i_FirstTokenScreenLength_);
        i_FirstTokenOffset_ = 0;
        i_FirstTokenScreenLength_ = rst_firstToken.i_ScreenLength;
    }

    if (UNDEFINED == i_LastTokenLength_)
    {
        ATLASSERT (UNDEFINED == i_LastTokenScreenLength_);
        i_LastTokenLength_ = rst_lastToken.i_Length;
        i_LastTokenScreenLength_ = rst_lastToken.i_ScreenLength;
    }

    i_TextLength_ = 0;
    for (int i_t = 0; i_t < i_NTokens_; ++i_t)
    {
        i_TextLength_ += i_GetTokenLength (i_t);
    }

    i_VisibleTextLength_ = 0;

    for (int i_t = 0; i_t < i_NTokens_; ++i_t)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_FirstToken_ + i_t];
        if (rst_t.b_IsLinearText())
        {
            i_VisibleTextLength_ += i_GetTokenLength (i_t);
        }
    }

}   //  v_Validate_()

void CT_Line::v_DeepCopy_ (const CT_Line& co_source)
{
    pco_Parent_ = co_source.pco_Parent_;
    b_Hyphenated_ = co_source.b_Hyphenated_;

    i_FirstToken_ = co_source.i_FirstToken_;
    i_FirstTokenOffset_ = co_source.i_FirstTokenOffset_;
    i_FirstTokenScreenLength_ = co_source.i_FirstTokenScreenLength_;
    i_NTokens_ = co_source.i_NTokens_;
    i_LastTokenLength_ = co_source.i_LastTokenLength_;
    i_LastTokenScreenLength_ = co_source.i_LastTokenScreenLength_;
    i_TextLength_ = co_source.i_TextLength_;
    i_VisibleTextLength_ = co_source.i_VisibleTextLength_;
    i_ScreenLength_ = co_source.i_ScreenLength_;
    i_ScreenHeight_ = co_source.i_ScreenHeight_;
    d_PrintScaleFactor_ = co_source.d_PrintScaleFactor_;
}

int CT_Line::i_TokenLength_ (int i_token)
{
    ATLASSERT (i_token >= 0);
    ATLASSERT (i_token < static_cast<int>(pco_Parent_->vo_Tokens_.size()));

    if (!b_Valid_)
    {
        v_Validate_();
    }

    if (0 == i_token)
    {
        ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_token + i_FirstToken_];
        ATLASSERT (rst_t.i_Length > i_FirstTokenOffset_);
        return rst_t.i_Length - i_FirstTokenOffset_;
    }

    if (pco_Parent_->vo_Tokens_.size() - 1 == i_token)
    {
        return i_LastTokenLength_;
    }

    ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_token + i_FirstToken_];
    return rst_t.i_Length;

}   //  i_TokenLength_ (...)

int CT_Line::i_TokenScreenLength_ (int i_token)
{
    ATLASSERT (i_token >= 0);
    ATLASSERT (i_token < static_cast<int>(pco_Parent_->vo_Tokens_.size()));

    if (!b_Valid_)
    {
        v_Validate_();
    }

    if (0 == i_token)
    {
        return i_FirstTokenScreenLength_;
    }

    if (pco_Parent_->vo_Tokens_.size() - 1 == i_token)
    {
        return i_LastTokenScreenLength_;
    }

    ST_Token& rst_t = pco_Parent_->vo_Tokens_[i_token + i_FirstToken_];
    return rst_t.i_ScreenLength;

}   // i_TokenScreenLength_ (...)

void CT_Line::v_SetLineSelection_ (CKaiView * pco_view, 
                                   CDC * pco_dc, 
                                   int i_xPos, 
                                   int i_yPos)
{
    int i_line = pco_Parent_->i_GetLine (this);
    CT_TextPos co_lineStart (pco_Parent_, i_line, 0);
    CT_TextPos co_lineEnd (pco_Parent_, 
                           i_line, 
                           i_GetVisibleTextLength() - 1);

    CT_TextPos co_selectionTop, co_selectionBottom;
    if (pco_view->co_GetSelectionStart() < pco_view->co_GetSelectionEnd())
    {
        co_selectionTop = pco_view->co_GetSelectionStart();
        co_selectionBottom = pco_view->co_GetSelectionEnd();
    }
    else
    {
        co_selectionTop = pco_view->co_GetSelectionEnd();
        co_selectionBottom = pco_view->co_GetSelectionStart();
    }

    //
    // Line is not affected by selection
    //
    if ((co_selectionBottom < co_lineStart) ||
        (co_selectionTop > co_lineEnd))
    {
        return;
    }

    CT_TextPos& co_lineSelectionStart = 
        (co_selectionTop > co_lineStart)
        ? co_selectionTop
        : co_lineStart;

    CT_TextPos& co_lineSelectionEnd = (co_selectionBottom < co_lineEnd)
                                    ? co_selectionBottom
                                    : co_lineEnd;
    CPoint co_topLeftPt;
//    co_topLeftPt.x = i_xPos + i_ScreenPos (co_lineSelectionStart, pco_dc);
    co_topLeftPt.x = 
        i_xPos + 
        i_ScreenPosFromParagraphOffset (co_lineSelectionStart.i_GetParagraphOffset(), pco_dc);
    co_topLeftPt.y = i_yPos;

    CPoint co_bottomRightPt;
    if (co_lineEnd < co_selectionBottom)
    {
        co_bottomRightPt.x = i_xPos + i_GetScreenLength();
    }
    else
    {
        co_bottomRightPt.x = 
            i_xPos + 
            i_ScreenPosFromParagraphOffset (co_lineSelectionEnd.i_GetParagraphOffset(), 
                                            pco_dc);
    }
    co_bottomRightPt.y = i_yPos + 
                         i_GetLineHeight() + 
//                         pco_Parent_->i_GetSpacing();
                         i_GetSpacing();

    pco_dc->InvertRect (CRect (co_topLeftPt, co_bottomRightPt));

}    // v_SetLineSelection_ (...)

bool CT_Line::b_SetLineBackground_ (CKaiView * pco_view, 
                                    CDC * pco_dc,  
                                    int i_xPos, 
                                    int i_yPos,
                                    ET_Color eo_color)
{
    int i_line = pco_Parent_->i_GetLine (this);
    CT_TextPos co_lineStart (pco_Parent_, i_line, 0);
    CT_TextPos co_lineEnd (pco_Parent_, 
                           i_line, 
                           i_GetVisibleTextLength() - 1);

    bool b_next = true;
    while (b_next)
    {
        CT_TextPos * pco_start = NULL;
        CT_TextPos * pco_end = NULL;
        b_next = pco_view->b_GetNextHighlight (pco_start, pco_end);
        if (!b_next)
        {
            return false;
        }

        if (pco_start->pco_GetParagraph()->i_At() < pco_Parent_->i_At())
        {
            continue;
        }

        if (pco_start->pco_GetParagraph()->i_At() > pco_Parent_->i_At())
        {
            return false;
        }

        if (*pco_start > *pco_end)
        {
            ERROR_TRACE (_T("Wrong order."));
            return false;
        }

        if (*pco_end < co_lineStart) 
        {
            continue;
        }

        if (*pco_start > co_lineEnd)
        {
            return false;
        }

        CT_TextPos& co_hlStart = (*pco_start > co_lineStart) ? *pco_start
                                                             : co_lineStart;
        CPoint co_topLeftPt;
        co_topLeftPt.x = 
            i_xPos + 
            i_ScreenPosFromParagraphOffset (co_hlStart.i_GetParagraphOffset(), 
                                            pco_dc);
        co_topLeftPt.y = i_yPos;

        CPoint co_bottomRightPt;
        if (co_lineEnd < *pco_end)
        {
            co_bottomRightPt.x = i_xPos + i_GetScreenLength();
        }
        else
        {
            co_bottomRightPt.x = 
                i_xPos + 
                i_ScreenPosFromParagraphOffset (pco_end->i_GetParagraphOffset(), 
                                                pco_dc);
        }

        co_bottomRightPt.y = i_yPos + 
                             i_GetLineHeight() + 
//                             pco_Parent_->i_GetSpacing();
                             i_GetSpacing();

        pco_dc->FillSolidRect (CRect (co_topLeftPt, co_bottomRightPt), ec_Yellow);

    }    // while (...)

    return b_next;

}    // b_SetLineBackground_ (...)

//
// --------------------------- object dump -----------------------------
//
void CT_Line::v_Dump() const
{
    DUMP_TRACE (_T("-------- CT_Line ------------------"));
    STL_STRING str_value = b_Hyphenated_ ? _T("true") : _T("false");
    STL_STRING str_text = _T("\tb_Hyphenated_ = ") + str_value;
    DUMP_TRACE (str_text);
    DUMP_TRACE (str_text);
    DUMP_TRACE (CT_KaiString (_T("\ti_FirstToken_ = ")) + STR_IToStr (i_FirstToken_));
    DUMP_TRACE (CT_KaiString (_T("\ti_NTokens_ = ")) + STR_IToStr (i_NTokens_));
    DUMP_TRACE (_T("-------- CT_Line: end of dump -----"));

}    //  v_Dump()
