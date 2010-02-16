//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2002
//    
//    Facility: Kai 1.2
//
//    Module description:    keystrokes processing - part of the MFC CView class.
//
//    $Id: KaiViewKeyDn.cpp,v 1.75 2007-12-31 07:55:38 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiDoc.h"
#include "KaiView.h"
#include "KaiLineDescriptor.h"
#include "KaiParagraph.h"
#include "KaiUndoStack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CKaiView::v_Char_ (UINT ui_char) 
{
    CDC * pco_screenDC = GetDC();
    ASSERT_VALID (pco_screenDC);

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    CT_UndoStack * pco_us = pco_doc->pco_GetUndoStack();
    pco_doc->v_SetModified();

    bool b_r = true;

    i_SavedTextX_ = -1;
    if (b_DiacrInput_)
    {
        int i_paragraph = pco_CaretPos_->i_GetParagraph();
        CT_Paragraph * pco_paragraph = pco_doc->pco_GetParagraph (i_paragraph);

        int i_offset = pco_CaretPos_->i_GetParagraphOffset();

        _TCHAR chr_ = (_TCHAR)ui_char;
        if (!_istprint (chr_))
        {
            pco_doc->v_ClearModified();
            return;
        }

        pco_us->v_AddSimpleEvent (ec_UndoInsertDiacritic,
                                  i_paragraph,
                                  pco_CaretPos_->i_GetVisibleParagraphOffset(),
                                  1,
                                  false);

        STL_STRING str_diacrSeq;
        b_r = pco_paragraph->b_CreateDiacrString (chr_, str_diacrSeq);
        pco_paragraph->insert (i_offset, str_diacrSeq);
        pco_paragraph->v_Invalidate();
        bool b_result = pco_paragraph->b_Render (pco_screenDC);
        if (!b_result)
        {
            return;
        }

        eo_RefreshMode_ = ec_WholeScreen;
        InvalidateRect (NULL, TRUE);                

//        pco_CaretPos_->v_Invalidate();
        b_r = b_RepositionCaret_();

        ReleaseDC (pco_screenDC);

        b_DiacrInput_ = false;

        return;
    }

    if (!_istprint ((_TCHAR)ui_char))
    {
        if (ui_char == _T('\x01b'))    // ESC
        {
            if (eo_SelectionMode_ != ec_NoSelection)
            {
                v_CancelSelection_();
                bool b_r = b_RepositionCaret_();
            }
        }
        else
        {
            if (eo_SelectionMode_ != ec_NoSelection)
            {
                v_DeleteSelection_();
            }
        }

        if (ui_char == _T('\r'))
        {
            int i_paragraph = pco_CaretPos_->i_GetParagraph();
            CT_Paragraph * pco_paragraph = pco_doc->pco_GetParagraph (i_paragraph);

            int i_offset = pco_CaretPos_->i_GetParagraphOffset();
            int i_visibleOffset = pco_CaretPos_->i_GetVisibleParagraphOffset();

            v_SplitParagraph_ (pco_paragraph, i_offset);
            pco_us->v_AddSimpleEvent (ec_UndoSplitParagraph, 
                                      i_paragraph,
                                      i_visibleOffset,
                                      0,
                                      false);
            pco_CaretPos_->v_Update (pco_GetParagraph (pco_CaretPos_->i_GetParagraph() + 1),
                                     0, 
                                     0);
            pco_CaretPos_->v_ClearPastEndOfLine();

            eo_RefreshMode_ = ec_WholeScreen;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();
            bool b_r = b_RepositionCaret_();
        }
        return;
    }

    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_DeleteSelection_();
    }

    int i_paragraph = pco_CaretPos_->i_GetParagraph();
    CT_Paragraph * pco_paragraph = pco_doc->pco_GetParagraph (i_paragraph);
    int i_offset = pco_CaretPos_->i_GetParagraphOffset();
    int i_visibleOffset = pco_CaretPos_->i_GetVisibleParagraphOffset();

    bool b_canOptimize = true;
    bool b_insert = !b_IsOverstrikeOn_();

    bool b_linkToPrevious = false;
    if (!b_insert)
    {
        int i_t = pco_paragraph->i_GetTokenNumFromOffset (i_offset);
        const ST_Token& rst_t = pco_paragraph->rst_GetToken (i_t);
        if (ec_TokenDiacritics == rst_t.eo_TokenType)
        {
            CT_TextPos co_onePast (*pco_CaretPos_);
            ++co_onePast;
            pco_doc->v_DeleteText (*pco_CaretPos_, co_onePast, false, false);
            pco_paragraph->v_Invalidate();
            bool b_render = pco_paragraph->b_Render (pco_screenDC);
            b_linkToPrevious = true;
            b_insert = true;
        }
    }

    CT_Paragraph co_oldParagraph (*pco_paragraph);
    _TCHAR chr_oldChar = pco_paragraph->at (i_offset);

    b_r = pco_paragraph->b_InsertOrReplaceChar (pco_screenDC, 
                                                i_offset, 
                                                ui_char, 
                                                b_insert);
    if (0 != ui_RequestedFont_)
    {
        CT_TextPos co_start (*pco_CaretPos_);
        co_start.v_SetVisibleParagraphOffset (i_visibleOffset);
        CT_TextPos co_onePastEnd (co_start);
        ++co_onePastEnd;
        v_ChangeTextSegmentFont_ (ui_RequestedFont_, co_start, co_onePastEnd);
    }

    if (b_insert)
    {
        pco_us->v_ClearCurrentEvent();
        pco_us->v_SetEventType (ec_UndoInsertChar);
        pco_us->v_SetParagraph (i_paragraph);
        pco_us->v_SetVisibleOffset (i_visibleOffset);
        pco_us->v_SetLength (1);
    }
    else
    {
        pco_us->v_ClearCurrentEvent();
        pco_us->v_SetEventType (ec_UndoReplaceChar);
        pco_us->v_SetParagraph (i_paragraph);
        pco_us->v_SetVisibleOffset (i_visibleOffset);
        pco_us->v_SetLength (1);
        pco_us->v_SetText (STL_STRING (1, chr_oldChar));        
    }

    if (b_linkToPrevious)
    {
        pco_us->v_LinkToPrevious();
    }
    pco_us->v_AddEvent();

    pco_CaretPos_->v_SetVisibleParagraphOffset (++i_visibleOffset);

    ui_RequestedFont_ = 0;

    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

    b_r = b_RepositionCaret_();

    ReleaseDC (pco_screenDC);

}  //  void CKaiView::v_Char_ (UINT ui_char) 

void CKaiView::v_Home_()
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
    }
    i_SavedTextX_ = -1;

    if (i_HScrollPos_ > 0)
    {
        i_HScrollPos_ = 0;
        eo_RefreshMode_ = ec_WholeScreen;
        InvalidateRect (NULL, TRUE);
        UpdateWindow();
    }

    pco_CaretPos_->v_MoveToStartOfLine();
    bool b_r = b_RepositionCaret_();

}  // void CKaiView::v_Home_()

void CKaiView::v_End_()
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
    }

    i_SavedTextX_ = -1;

    CT_Line * pco_caretLine = pco_GetLine_ (pco_CaretPos_->i_GetParagraph(),
                                           pco_CaretPos_->i_GetLine());
    if (!pco_caretLine->b_IsLastInParagraph())
    {
        pco_CaretPos_->v_MovePastEndOfLine();
    }
    else
    {
        pco_CaretPos_->v_SetLineOffset 
                                (pco_caretLine->i_GetVisibleTextLength() - 1);
    }
    bool b_r = b_RepositionCaret_();


}  // void CKaiView::v_End_()

void CKaiView::v_Prior_()
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
    }

    if (i_SavedTextX_ == -1)
    {
        if (pco_CaretPos_->b_PastEndOfLine())
        {
            i_SavedTextX_ = ec_PastEndOfLine;
        }
        else
        {
            i_SavedTextX_ = pco_CaretPos_->i_GetLineOffset();
        }
    }

    int i_yPos = i_GetYPos_ (*pco_CaretPos_);

    BOOL mfcb_result = SendMessage (WM_VSCROLL, SB_PAGEUP);

    i_yPos = min (st_DisplayWindow_.i_linesShown, i_yPos);

    int i_paragraph = st_DisplayWindow_.i_firstParagraph;
    int i_line = st_DisplayWindow_.i_firstLine;
    bool b_ = true;
    for (int i_ = 0; i_ < i_yPos; ++i_)
    {
        b_ = b_GetNextLine_ (i_paragraph, i_line, i_paragraph, i_line);
        if (!b_)
        {
            break;
        }
    }
    if (!b_)
    {
        return;
    }

    CT_Line * pco_caretLine = pco_GetLine_ (i_paragraph, i_line);
    int i_lineOffset = 0;
    if (i_SavedTextX_ == ec_PastEndOfLine)
    {
        if (!pco_caretLine->b_IsLastInParagraph())
        {
            pco_CaretPos_->v_Update (pco_GetParagraph (i_paragraph), i_line, 1);
            pco_CaretPos_->v_MovePastEndOfLine();
        }
        else
        {
            i_lineOffset = (min (pco_caretLine->i_GetVisibleTextLength() - 1, 
                                 i_SavedTextX_));
            pco_CaretPos_->v_Update (pco_GetParagraph (i_paragraph), 
                                     i_line, 
                                     i_lineOffset);
        }
    }
    else
    {
        i_lineOffset = (min (pco_caretLine->i_GetVisibleTextLength() - 1, 
                             i_SavedTextX_));
        pco_CaretPos_->v_Update (pco_GetParagraph (i_paragraph), 
                                 i_line, 
                                 i_lineOffset);
    }

    bool b_r = b_RepositionCaret_();


}  // void CKaiView::v_Prior_()

void CKaiView::v_Next_()
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
    }

    if (i_SavedTextX_ == -1)
    {
        if (!pco_CaretPos_->b_PastEndOfLine())
        {
            i_SavedTextX_ = pco_CaretPos_->i_GetLineOffset();
        }
        else
        {
            i_SavedTextX_ = ec_PastEndOfLine;
        }
    }

    int i_yPos = i_GetYPos_ (*pco_CaretPos_);

    //
    // Show next page
    //
    BOOL mfcb_ = SendMessage (WM_VSCROLL, SB_PAGEDOWN);

    //
    // Determine caret position
    //
    i_yPos = min (st_DisplayWindow_.i_linesShown-1, i_yPos);
    int i_paragraph = st_DisplayWindow_.i_firstParagraph;
    int i_line = st_DisplayWindow_.i_firstLine;
    bool b_ = true;
    for (int i_ = 0; i_ < i_yPos; ++i_)
    {
        b_ = b_GetNextLine_ (i_paragraph, i_line, i_paragraph, i_line);
        if (!b_)
        {
            break;
        }
    }
    if (!b_)
    {
        return;
    }
    CT_Line * pco_caretLine = pco_GetLine_ (i_paragraph, i_line);
    int i_lineOffset = 0;

    i_lineOffset = min (pco_caretLine->i_GetVisibleTextLength()-1, 
                        i_SavedTextX_);
    pco_CaretPos_->v_Update (pco_GetParagraph (i_paragraph), 
                             i_line, 
                             i_lineOffset);
    if (i_SavedTextX_ == ec_PastEndOfLine)
    {
        if (!pco_caretLine->b_IsLastInParagraph())
        {
            pco_CaretPos_->v_MovePastEndOfLine();
        }
    }

    bool b_r = b_RepositionCaret_();

}  //  void CKaiView::v_Next_()

void CKaiView::v_Left_()
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        eo_SelectionMode_ = ec_NoSelection;
        eo_RefreshMode_ = ec_WholeScreen;
        InvalidateRect (NULL, TRUE);
        if (*pco_SelectionStart_ < *pco_SelectionEnd_) 
        {
            *pco_CaretPos_ = *pco_SelectionStart_;
        }
        else if (*pco_SelectionStart_ > *pco_SelectionEnd_)
        {
            *pco_CaretPos_ = *pco_SelectionEnd_;
        }
        bool b_ = b_RepositionCaret_();
        return;
    }

    pco_CaretPos_->v_ClearPastEndOfLine();
    i_SavedTextX_ = -1;

    if ((pco_CaretPos_->i_GetParagraph() == 0) &&
        (pco_CaretPos_->i_GetParagraphOffset() == 0))
    {
        return;
    }

    int i_yPos = -1;
    if (b_IsVisible_ (*pco_CaretPos_))
    {
        i_yPos = i_GetYPos_ (*pco_CaretPos_);
    }

    int i_oldLine = pco_CaretPos_->i_GetLine();
    int i_oldParagraph = pco_CaretPos_->i_GetParagraph();

    --(*pco_CaretPos_);

    if (i_yPos == 0)
    {
        if ((pco_CaretPos_->i_GetLine() < i_oldLine) ||
            (pco_CaretPos_->i_GetParagraph() < i_oldParagraph))
        {
            v_LineUp_();
        }
    }
    bool b_ = b_RepositionCaret_();

}  // void CKaiView::v_Left_()

void CKaiView::v_Right_()
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
        bool b_ = b_RepositionCaret_();
        return;
    }

    //
    // End of document?
    //
    if (b_IsEndOfDocument (*pco_CaretPos_))
    {
        return;
    }

    i_SavedTextX_ = -1;
    CT_TextPos co_oldPos (*pco_CaretPos_);
    if (co_oldPos.b_PastEndOfLine())
    {
        --co_oldPos;
    }

    int i_yPos = -1;
    if (b_IsVisible_ (co_oldPos))
    {
        i_yPos = i_GetYPos_ (co_oldPos);
    }

    ++(*pco_CaretPos_);

    //
    // I don't have an LineDown() equivalent for horizontal scrolling
    // so why bother
    //
    if (0 == i_HScrollPos_)
    {
        if ((pco_CaretPos_->i_GetParagraph() > co_oldPos.i_GetParagraph()) ||
            (pco_CaretPos_->i_GetLine() > co_oldPos.i_GetLine()))
        {
            if ((i_yPos >= 0) && (i_yPos == st_DisplayWindow_.i_linesShown-1))
            {
                v_LineDown_();
            }
        }
    }
    bool b_ = b_RepositionCaret_();

}  // void CKaiView::v_Right_()

void CKaiView::v_Up_()
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
    }

    if (i_SavedTextX_ == -1)
    {
        if (pco_CaretPos_->b_PastEndOfLine())
        {
            i_SavedTextX_ = ec_PastEndOfLine;
        }
        else
        {
            i_SavedTextX_ = pco_CaretPos_->i_GetLineOffset();
        }
    }

    //
    // If caret is at the beginning of document 
    // make sure it is visible and exit
    //
    if ((pco_CaretPos_->i_GetParagraph() == 0) && 
        (pco_CaretPos_->i_GetLine() == 0))
    {
        if (!b_IsCaretPosVisible_())
        {
            bool b_r = b_RepositionCaret_();
        }
        return;
    }

    //
    // Scroll up if needed
    //
    if (b_IsCaretPosVisible_())
    {
        if (i_GetYPos_ (*pco_CaretPos_) == 0)
        {
            if (b_CaretShown_)
            {
                HideCaret();
                b_CaretShown_ = true;
            }
            v_LineUp_();
        }
    }

    CT_TextPos co_caretPos (*pco_CaretPos_);
    if (co_caretPos.b_PastEndOfLine())
    {
        --co_caretPos;
    }

    int i_paragraph = 0;
    int i_line = 0;
    bool b_ = b_GetPrevLine_ (co_caretPos.i_GetParagraph(),
                              co_caretPos.i_GetLine(),
                              i_paragraph,
                              i_line);
    if (!b_)
    {
        return;
    }

    CT_Line * pco_caretLine = pco_GetLine_ (i_paragraph, i_line);

    //
    // Find line offset
    //
    int i_lineOffset = 0;
    if (i_SavedTextX_ == ec_PastEndOfLine)
    {
        if (!pco_caretLine->b_IsLastInParagraph())
        {
            pco_CaretPos_->v_Update (pco_GetParagraph (i_paragraph), 
                                     i_line, 
                                     1);
            pco_CaretPos_->v_MovePastEndOfLine();
        }
        else
        {
            i_lineOffset = (min (pco_caretLine->i_GetVisibleTextLength() - 1, 
                                 i_SavedTextX_));
            pco_CaretPos_->v_Update (pco_GetParagraph (i_paragraph),
                                    i_line, 
                                    i_lineOffset);
        }
    }
    else
    {
        i_lineOffset = (min (pco_caretLine->i_GetVisibleTextLength() - 1, 
                             i_SavedTextX_));
        pco_CaretPos_->v_Update (pco_GetParagraph (i_paragraph), 
                                 i_line, 
                                 i_lineOffset);
    }

    b_ = b_RepositionCaret_();

}  // void CKaiView::v_Up_()

void CKaiView::v_Down_()
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
    }

    CKaiDoc * pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    if (i_SavedTextX_ == -1)
    {
        if (!pco_CaretPos_->b_PastEndOfLine())
        {
            i_SavedTextX_ = pco_CaretPos_->i_GetLineOffset();
        }
        else
        {
            i_SavedTextX_ = ec_PastEndOfLine;
        }
    }

    CT_Paragraph * pco_caretParagraph = 
     pco_doc->pco_GetParagraph (pco_CaretPos_->i_GetParagraph());
    if ((pco_CaretPos_->i_GetParagraph() == pco_doc->i_NParagraphs()-1) &&
        (pco_CaretPos_->i_GetLine() == pco_caretParagraph->i_NLines()-1))
    {
        if (!b_IsCaretPosVisible_())
        {
            bool b_r = b_RepositionCaret_();
        }
        return;
    }

    if (b_IsCaretPosVisible_())
    {
        if (i_GetYPos_ (*pco_CaretPos_) == 
            st_DisplayWindow_.i_linesShown - 1)
        {
            if (b_CaretShown_)
            {
                HideCaret();
                b_CaretShown_ = true;
            }
            v_LineDown_();
        }
    }

    int i_p = 0;
    int i_l = 0;
    bool b_r = b_GetNextLine_ (pco_CaretPos_->i_GetParagraph(), 
                              pco_CaretPos_->i_GetLine(),
                              i_p,
                              i_l);
    CT_Line * pco_caretLine = pco_GetLine_ (i_p, i_l);
    int i_lineOffset = 0;

    i_lineOffset = min (pco_caretLine->i_GetVisibleTextLength()-1, 
                        i_SavedTextX_);
    pco_CaretPos_->v_Update (pco_GetParagraph (i_p), 
                             i_l, 
                             i_lineOffset, 
                             CT_TextPos::ec_LinePos);
    if (i_SavedTextX_ == ec_PastEndOfLine)
    {
        if (!pco_caretLine->b_IsLastInParagraph())
        {
            pco_CaretPos_->v_MovePastEndOfLine();
        }
    }

    b_r = b_RepositionCaret_();

}  //  void CKaiView::v_Down_()

void CKaiView::v_VkDelete_()
{
    v_DeleteChar_ (false);

}    //  void CKaiView::v_VkDelete_()

void CKaiView::v_VkBackSpace_()
{
    v_DeleteChar_ (true);

}    //    void CKaiView::v_VkBackSpace_()

void CKaiView::v_SplitParagraph_ (CT_Paragraph * pco_paragraph, int i_at)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    unsigned int ui_font = 0;
    bool b_ = pco_paragraph->b_GetFontKey (i_at-1, ui_font);
    if (!b_)
    {
        return;
    }
    STL_STRING str_fd;
    CT_Paragraph * pco_rightPar = new CT_Paragraph();
    pco_rightPar->v_CopyAttributes (*pco_paragraph);
    b_ = pco_rightPar->b_CreateFDString (ui_font, str_fd);
    pco_rightPar->assign (str_fd);
    pco_rightPar->v_SetPrevious (pco_paragraph);
    pco_rightPar->v_SetNext (pco_paragraph->pco_GetNext());
    pco_paragraph->v_SetNext (pco_rightPar);
    if (!pco_rightPar->b_IsLastParagraph())
    {
        pco_rightPar->pco_GetNext()->v_SetPrevious (pco_rightPar);
    }
    pco_rightPar->append (pco_paragraph->substr (i_at));
    pco_paragraph->assign (pco_paragraph->substr (0, i_at));
    STL_STRING str_endMark = pco_paragraph->str_GetEndOfParagraphChars();
    pco_paragraph->append (str_endMark);
    pco_paragraph->v_Invalidate();
    CClientDC co_dc (this);
    b_ = pco_paragraph->b_Render (&co_dc);
    pco_doc->v_AddParagraph (pco_rightPar, pco_paragraph->i_At());
    pco_rightPar->v_Invalidate();
    b_ = pco_rightPar->b_Render (&co_dc);
//    pco_CaretPos_->v_Update 
//        (pco_GetParagraph (pco_CaretPos_->i_GetParagraph()+1), 
//         0, 
//         0);
//    pco_CaretPos_->v_ClearPastEndOfLine();
//    eo_RefreshMode_ = ec_WholeScreen;
//    InvalidateRect (NULL, TRUE);
//    UpdateWindow();
//    b_ = b_RepositionCaret_();

}    //  void CKaiView::v_SplitParagraph_ (...)
