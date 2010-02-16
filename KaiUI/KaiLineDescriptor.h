//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    a helper class that holds formatting
//  info for a single line.
//
//    $Id: KaiLineDescriptor.h,v 1.50 2007-08-12 01:51:12 kostya Exp $
//
//==============================================================================

#pragma once

#include "KaiParagraph.h"

class CT_TextPos;
class CKaiView;

class KAI_API CT_Line
{
friend class CT_Paragraph;
friend class CT_TextPos;

protected:
    CT_Paragraph * pco_Parent_;

    bool b_Hyphenated_;

    int i_FirstToken_;
    int i_FirstTokenOffset_;    // from token start
    int i_FirstTokenScreenLength_;

    int i_NTokens_;

    int i_LastTokenLength_;
    int i_LastTokenScreenLength_;

    int i_TextLength_;
    int i_VisibleTextLength_;

    int i_ScreenLength_;
    int i_UnjustifiedScreenLength_;
    int i_ScreenHeight_;

    double d_PrintScaleFactor_;

    bool b_Valid_;

public:
    CT_Line();
    CT_Line (CT_Paragraph * const pco_parent, 
             int i_firstToken, 
             int i_firstTokenOffset = UNDEFINED,
             int i_firstTokenScreenLength = UNDEFINED);
    ~CT_Line();
    CT_Line (const CT_Line& co_ld);
    void v_Null();
    CT_Line& operator = (const CT_Line& co_ld);
    bool operator < (CT_Line&);
    bool operator == (CT_Line&);    

    int i_NTokens()
    {
        return i_NTokens_;
    }

//    int i_ScreenPos (CT_TextPos& co_textPos, CDC * pco_dc);
    int i_ScreenPosFromParagraphOffset (int i_paragraphOffset, CDC * pco_dc);
    int i_ParagraphOffsetFromScreenPos (int i_screenPos, CDC * pco_dc);
//    bool b_GetAbsoluteOffset (int i_lineOffset, int& i_absOffset);

    bool b_LineOffsetFromParagraphOffset (int i_paragraphOffset, 
                                          int& i_lineOffset);
    bool b_ParagraphOffsetFromLineOffset (int i_lineOffset, 
                                          int& i_paragraphOffset);

    CT_Paragraph * const pco_GetParent() const
    {
        return pco_Parent_;
    }

    void v_Invalidate()
    {
        b_Valid_ = false;
    };

    int i_At();
    int i_GetStart();
    int i_GetTextLength();
    int i_GetTokenOffset (int i_token);
    int i_GetTokenLength (int i_token);
    int i_GetTokenScreenLength (int i_token);
    int i_GetVisibleTextLength();
    int i_GetScreenLength();
    int i_GetUnJustifiedLength();

    int i_GetSpacing();

    int i_GetScreenHeight();
    int i_GetDiacrHeight() const;

    int i_GetLineHeight()        // includes diacr height
    {
        return i_GetScreenHeight() + i_GetDiacrHeight();
    }

    int i_GetRemainingSpace();

    bool b_RightJustify (bool b_isPrinting = false);
    bool b_IsRightJustified();
    bool b_IsLastInParagraph();

    void v_SetStartingFont (CDC *);

    void v_Show (CKaiView * pco_view, CDC * pco_dc, int i_xPos, int i_yPos);

    void v_Print (CKaiView *, CDC *, int i_xPos, int i_yPos);

    int i_ShowToken (CDC *, int i_token, int i_x, int i_y);

    void v_ShowDiacritic (CDC * pco_dc, 
                          int i_x, 
                          int i_y, 
                          int i_diacrToken);

    int i_GetParagraphTermWidth (CDC * pco_dc);

    void v_GetLastPos (CT_TextPos& rco_lastPos);

    void v_Dump() const;

protected:
    void v_Validate_();
    void v_DeepCopy_ (const CT_Line& co_source);
    int i_TokenLength_ (int i_tokenPosInLine);
    int i_TokenScreenLength_ (int i_tokenPosInLine);
    void v_SetLineSelection_ (CKaiView * pco_view, 
                              CDC * pco_dc,  
                              int i_xPos, 
                              int i_yPos);

    bool b_SetLineBackground_ (CKaiView * pco_view, 
                               CDC * pco_dc,  
                               int i_xPos, 
                               int i_yPos,
                               ET_Color eo_color = ec_Yellow);

    struct b_CompareSpaceTokens_ 
    {
        bool operator() (const ST_Token * pst_left, const ST_Token * pst_right) const
        {
            if (pst_left->i_ScreenLength < pst_right->i_ScreenLength)
            {
                return false;
            }
            else
            {
                if (pst_left->i_ScreenLength > pst_right->i_ScreenLength)
                {
                    return true;
                }
            }

            return pst_left->i_ExtraSpace > pst_right->i_ExtraSpace;
        }; 
    };

    struct b_ComparePrintSpaceTokens_ 
    {
        bool operator() (const ST_Token * pst_left, const ST_Token * pst_right) const
        {
            if (pst_left->i_PrintLength < pst_right->i_PrintLength)
            {
                return false;
            }
            else
            {
                if (pst_left->i_PrintLength > pst_right->i_PrintLength)
                {
                    return true;
                }
            }

            return pst_left->i_ExtraPrintSpace > pst_right->i_ExtraPrintSpace;
        }; 
    };

};
