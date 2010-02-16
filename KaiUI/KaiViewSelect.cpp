//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2002
//    
//    Facility: Kai 1.2
//
//    Module description:    text selection, part of the MFC CView class.
//
//    $Id: KaiViewSelect.cpp,v 1.81 2007-12-25 07:56:50 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include <afxole.h>
#include <afxadv.h>
#include "Kai.h"
#include "KaiGeneric.h"
#include "KaiParagraph.h"
#include "KaiLineDescriptor.h"
#include "KaiDoc.h"
#include "KaiDocDescriptor.h"
#include "KaiUndoStack.h"
#include "KaiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CKaiView::b_SetSelection_ (CT_TextPos * pco_from, CT_TextPos * pco_to)
{
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        ERROR_TRACE (_T("Selection exists."));
        return false;
    }

    HideCaret();
    b_CaretShown_ = false;
    *pco_SelectionStart_ = *pco_from;
    *pco_SelectionEnd_ = *pco_to;
    eo_SelectionMode_ = ec_StaticSelection;
    b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
    v_RefreshScreen_();

    return true;

}  // v_SetSelection (...)

void CKaiView::v_SetMouseSelection_ (CT_TextPos * pco_to)
{
    CT_TextPos co_newSelectionStart;
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        if (*pco_CaretPos_ == *pco_to)
        {
            return;
        }
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = *pco_CaretPos_;
        co_newSelectionStart = *pco_SelectionStart_;
        eo_SelectionMode_ = ec_StaticSelection;
    }
    else
    {
        co_newSelectionStart = *pco_SelectionEnd_;
        if (co_newSelectionStart == *pco_to)
        {
            return;
        }
    }
    *pco_SelectionEnd_ = *pco_to;

    b_InvertSelection_ (co_newSelectionStart, *pco_SelectionEnd_);
    v_RefreshScreen_();

}  // v_SetMouseSelection_ (...)

void CKaiView::v_CancelSelection_()
{
    *pco_CaretPos_ = *pco_SelectionEnd_;
    eo_SelectionMode_ = ec_NoSelection;
    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

}    //    void CKaiView::v_CancelSelection_()

bool CKaiView::b_InvertSelection_ (CT_TextPos& co_start, CT_TextPos& co_end)
{
    CT_TextPos co_selectionTop, co_selectionBottom;
    if (co_start < co_end)
    {
        co_selectionTop = co_start;
        co_selectionBottom = co_end;
    }
    else
    {
        co_selectionTop = co_end;
        co_selectionBottom = co_start;
    }

    //
    // Visible?
    //
    int i_paragraph = st_DisplayWindow_.i_firstParagraph;
    int i_line = st_DisplayWindow_.i_firstLine;
    int i_yPos = 0;
    CT_Line * pco_line = pco_GetLine_ (i_paragraph, i_line);
    if (st_DisplayWindow_.co_topLeft.cy > 0)
    {
        bool b_ = b_GetPrevLine_ (i_paragraph,
                                  i_line,
                                  i_paragraph,
                                  i_line);
        if (!b_)
        {
            return false;
        }

        pco_line = pco_GetLine_ (i_paragraph, i_line);
        i_yPos = st_DisplayWindow_.co_topLeft.cy -
                       (pco_line->i_GetLineHeight() +
                        pco_line->i_GetSpacing());
    }

    CT_TextPos co_clientTop (pco_GetParagraph (i_paragraph),
                             i_line,
                             0);
    if (co_selectionTop < co_clientTop)
    {
        if (co_selectionBottom < co_clientTop)
        {
            return true;
        }
        co_selectionTop = co_clientTop;
    }
    CT_TextPos co_lineEnd;
    pco_line->v_GetLastPos (co_lineEnd);

    int i_linesShown = st_DisplayWindow_.i_linesShown + 1;
                    // including a possible partially visible line at the bottom
    if (st_DisplayWindow_.co_topLeft.cy > 0)
    {
        ++i_linesShown;
    }

    int i_lineNum = 0;
    for (; 
        (i_lineNum < i_linesShown) && (co_lineEnd < co_selectionTop);
        ++ i_lineNum)
    {
        i_yPos += pco_line->i_GetLineHeight() + pco_line->i_GetSpacing();
        bool b_ = b_GetNextLine_ (i_paragraph,
                                  i_line,
                                  i_paragraph,
                                  i_line);
        if (!b_)
        {
            return false;
        }
        pco_line = pco_GetLine_ (i_paragraph, i_line);
        pco_line->v_GetLastPos (co_lineEnd);
    }

    int i_lineStart = 0;
    pco_line->b_ParagraphOffsetFromLineOffset (0, i_lineStart);
    CFont * pco_startFont = pco_line->pco_GetParent()->pco_GetWindowsFont (pco_MemDC_, 
                                                                           i_lineStart);
    CFont * pco_oldFont = pco_MemDC_->SelectObject (pco_startFont);

    for (; i_lineNum < i_linesShown; ++i_lineNum)
    {
        //
        // Erase background & show line
        //
        CRect co_lineRect;
        GetClientRect (&co_lineRect);
        co_lineRect.top = i_yPos;
        co_lineRect.bottom = i_yPos + 
                             pco_line->i_GetLineHeight() + 
                             pco_line->i_GetSpacing();
        pco_MemDC_->FillSolidRect (&co_lineRect, GetSysColor (COLOR_WINDOW));
        pco_line->v_Show (this, 
                          pco_MemDC_, 
                          st_DisplayWindow_.co_topLeft.cx, 
                          i_yPos);
        i_yPos += pco_line->i_GetLineHeight() + pco_line->i_GetSpacing();
        if ((i_paragraph == co_selectionBottom.i_GetParagraph()) &&
            (i_line == co_selectionBottom.i_GetLine()))
        {
            break;
        }
        if (b_IsLastLine_ (i_paragraph, i_line))
        {
            break;
        }
        bool b_ = b_GetNextLine_ (i_paragraph,
                                  i_line,
                                  i_paragraph,
                                  i_line);
        if (!b_)
        {
            return false;
        }
        pco_line = pco_GetLine_ (i_paragraph, i_line);
    }

    return true;

}    //  bool CKaiView::b_InvertSelection_()

void CKaiView::v_ScrollAndSelect_ (ET_Direction eo_direction)
{
    CRect co_selectionRect;
    MSG    st_msg;
    CPoint co_point;
    GetCursorPos (&co_point);
    ScreenToClient (&co_point);

    bool b_continue = true;
    while (b_continue)
    {
        PeekMessage (&st_msg, 0, 0, 0, PM_REMOVE);
        if (st_msg.message == WM_LBUTTONUP)
        {
            b_continue = false;
            ReleaseCapture();
        }
        else
        {
            GetCursorPos (&co_point);
            ScreenToClient (&co_point);
            CRect co_rectClient;
            GetClientRect (&co_rectClient);
            if ((co_point.y > co_rectClient.top) &&
                (co_point.y < co_rectClient.bottom))  // need more precise pos.?
            {
                //
                //  Mouse back inside client rect
                //
                CT_TextPos co_selectTo;
                bool b_ = b_ScreenToTextPos_ (co_point, &co_selectTo);
                if (!b_)
                {
                    return;
                }
                b_continue = false;
                eo_SelectionMode_ = ec_StaticSelection;
                v_SetMouseSelection_ (&co_selectTo);

            }  // if inside client rect
        }

        if (b_continue)
        {
            if (eo_direction == ec_DirectionDown)
            {
                v_LineDown_();
                int i_lastParagraph = st_DisplayWindow_.i_firstParagraph;
                int i_lastLine = st_DisplayWindow_.i_firstLine;
                for (int i_ = 0; i_ < st_DisplayWindow_.i_linesShown-1; ++i_)
                {
                    bool b_ = b_GetNextLine_ (i_lastParagraph, 
                                              i_lastLine, 
                                              i_lastParagraph, 
                                              i_lastLine);
                    if (!b_)
                    {
                        return;
                    }
                }
                CT_Line * pco_bottomLine = pco_GetLine_ (i_lastParagraph,
                                                         i_lastLine);
                CT_TextPos co_pos, 
                           co_selectFrom, 
                           co_selectTo;
                bool b_ = b_ScreenToTextPos_ (co_point, &co_pos);
                if (!b_)
                {
                    return;
                }

                co_selectFrom.v_Update (pco_GetParagraph (i_lastParagraph), 
                                        i_lastLine, 
                                        0);
                int i_lineOffset = 
                                min (pco_bottomLine->i_GetVisibleTextLength(),
                                     co_pos.i_GetLineOffset());
                if (pco_bottomLine->b_IsLastInParagraph())
                {
                    i_lineOffset = 
                            min (i_lineOffset, 
                                 pco_bottomLine->i_GetVisibleTextLength() - 1);
                }
                co_selectTo.v_Update (pco_GetParagraph (i_lastParagraph), 
                                      i_lastLine, 
                                      i_lineOffset);
                                          
                v_SetMouseSelection_ (&co_selectTo);
            }
            else
            {
                if (eo_direction == ec_DirectionUp)
                {
                    v_LineUp_();
                    CT_TextPos co_selectTo;
                    bool b_ = b_ScreenToTextPos_ (co_point, &co_selectTo);
                    if (!b_)
                    {
                        return;
                    }
                    v_SetMouseSelection_ (&co_selectTo);
                }
            }
        }

    } // while (b_continue)

    eo_SelectionMode_ = ec_StaticSelection;

}    //  void CKaiView::v_ScrollAndSelect_ (...)

void CKaiView::v_SelectTextField_ (const CT_TextPos& co_pos)
{
    CT_Paragraph * pco_p = co_pos.pco_GetParagraph();

    bool b_ = pco_p->b_GetTokenPos (co_pos, *pco_SelectionStart_, *pco_SelectionEnd_);
    if (!b_)
    {
        return;
    }

    eo_SelectionMode_ = ec_StaticSelection;
    HideCaret();
    b_CaretShown_ = false;
    b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
    v_RefreshScreen_();

}   //  v_SelectTextField_

//
// Keyboard selection
//
void CKaiView::OnSelectLeft()
{
    CT_TextPos co_selectFrom;
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        co_selectFrom = *pco_CaretPos_;
    }
    else
    {
        co_selectFrom = *pco_SelectionEnd_;
    }

    //
    // If the last selected (or caret) position has been scrolled away:
    //
    if (!b_IsVisible_ (co_selectFrom))
    {
        v_MakeVisible_ (co_selectFrom);
    }

    //
    // Don't go beyond initial pos.
    //
    if (co_selectFrom.b_FirstPos())
    {
        return;
    }

    //
    // Move selection one pos to the left and scroll if 
    // we moved beyond fist visible pos.
    //
    int i_yPos = i_GetYPos_ (co_selectFrom);
    int i_curLine = co_selectFrom.i_GetLine();
    bool b_result = true;

    CT_TextPos co_selectTo (co_selectFrom);
    --co_selectTo;

    if (co_selectTo.i_GetLine() < i_curLine)
    {
        if (i_yPos == 0)
        {
            v_LineUp_();
        }
    }

    //
    // Update selection
    //
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = co_selectFrom;
        *pco_SelectionEnd_ = co_selectTo;
        b_InvertSelection_ (co_selectFrom, co_selectTo);
        v_RefreshScreen_();

    }
    else
    {
        //
        // Cancel selection and restore caret if we are back where
        // selection started
        //
        if (co_selectTo == *pco_SelectionStart_)
        {
            v_CancelSelection_();
            *pco_CaretPos_ = co_selectTo;
            bool b_ = b_RepositionCaret_();
        }
        else
        {
            *pco_SelectionEnd_ = co_selectTo;
            b_InvertSelection_ (co_selectFrom, co_selectTo);
            v_RefreshScreen_();
        }
    }

    i_SavedTextX_ = pco_SelectionEnd_->i_GetLineOffset();

}  //    void CKaiView::OnSelectLeft()

void CKaiView::OnSelectRight()
{
    CT_TextPos co_startPos;
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        co_startPos = *pco_CaretPos_;
    }
    else
    {
        co_startPos = *pco_SelectionEnd_;
    }

    if (b_IsEndOfDocument (co_startPos))
    {
        return;
    }

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = co_startPos;
    }

    CT_TextPos co_endPos (co_startPos);
    ++co_endPos;
    if ((co_endPos.i_GetParagraph() > co_startPos.i_GetParagraph()) ||
        (co_endPos.i_GetLine() > co_startPos.i_GetLine()))
    {
        int i_yPos = i_GetYPos_ (co_startPos);
        if (i_yPos == st_DisplayWindow_.i_linesShown - 1)
        {
            v_LineDown_();
        }
    }
    if (co_endPos == *pco_SelectionStart_)
    {
        v_CancelSelection_();
        *pco_CaretPos_ = co_endPos;
        bool b_ = b_RepositionCaret_();
    }
    else
    {
        *pco_SelectionEnd_ = co_endPos;
        b_InvertSelection_ (co_startPos, co_endPos);
        v_RefreshScreen_();
    }

    i_SavedTextX_ = pco_SelectionEnd_->i_GetLineOffset();

}  //    void CKaiView::OnSelectRight()

void CKaiView::OnSelectUp()
{
    CKaiDoc* pco_doc = GetDocument_();
//    ASSERT_VALID (pco_doc); -- doesn't compile -- why??

    CT_TextPos co_startPos;
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        co_startPos = *pco_CaretPos_;
        if (i_SavedTextX_ == -1)
        {
            i_SavedTextX_ = co_startPos.i_GetLineOffset();
        }
    }
    else
    {
        co_startPos = *pco_SelectionEnd_;
    }
    if (!b_IsVisible_ (co_startPos))
    {
        v_MakeVisible_ (co_startPos);
    }
    if (i_GetYPos_ (co_startPos) == 0)
    {
        v_LineUp_();
    }
    if ((co_startPos.i_GetParagraph() == 0) && 
        (co_startPos.i_GetLine() == 0))
    {
        return;
    }

    int i_paragraph = 0;
    int i_line = 0;
    bool b_r = b_GetPrevLine_ (co_startPos.i_GetParagraph(),
                               co_startPos.i_GetLine(),
                               i_paragraph,
                               i_line);
    CT_Line * pco_curLine = pco_GetLine_ (i_paragraph, i_line);
    int i_lineOffset = 0;
    if (i_SavedTextX_ == ec_PastEndOfLine)
    {
        i_lineOffset = pco_curLine->i_GetVisibleTextLength();
    }
    else
    {
        i_lineOffset = min (pco_curLine->i_GetVisibleTextLength()-1, 
                            i_SavedTextX_);
        i_lineOffset = max (i_lineOffset, 0);
    }
    CT_TextPos co_endPos;
    co_endPos.v_Update (pco_GetParagraph (i_paragraph), i_line, i_lineOffset);

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = co_startPos;
        *pco_SelectionEnd_ = co_endPos;
        bool b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
        v_RefreshScreen_();
    }
    else
    {
        if (co_endPos == *pco_SelectionStart_)
        {
            v_CancelSelection_();
            *pco_CaretPos_ = co_endPos;
            bool b_ = b_RepositionCaret_();
        }
        else
        {
            *pco_SelectionEnd_ = co_endPos;
            bool b_ = b_InvertSelection_ (co_startPos, co_endPos);
            v_RefreshScreen_();
        }
    }

}  // void CKaiView::v_OnSelectUp()

void CKaiView::OnSelectDown()
{
    CKaiDoc* pco_doc = GetDocument_();

    CT_TextPos co_startPos;
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        co_startPos = *pco_CaretPos_;
        if (i_SavedTextX_ == -1)
        {
            i_SavedTextX_ = co_startPos.i_GetLineOffset();
        }
    }
    else
    {
        co_startPos = *pco_SelectionEnd_;
    }
    if (!b_IsVisible_ (co_startPos))
    {
        v_MakeVisible_ (co_startPos);
    }
    if (b_IsLastLine_ (co_startPos.i_GetParagraph(), 
                       co_startPos.i_GetLine()))
    {
        return;
    }

    if (i_GetYPos_ (co_startPos) == st_DisplayWindow_.i_linesShown - 1)
    {
        v_LineDown_();
    }

    int i_paragraph = 0;
    int i_line = 0;
    bool b_r = b_GetNextLine_ (co_startPos.i_GetParagraph(),
                               co_startPos.i_GetLine(),
                               i_paragraph,
                               i_line);
    CT_Line * pco_curLine = pco_GetLine_ (i_paragraph, i_line);
    int i_lineOffset = 0;
    if (i_SavedTextX_ == ec_PastEndOfLine)
    {
        i_lineOffset = pco_curLine->i_GetVisibleTextLength();
    }
    else
    {
        i_lineOffset = min (pco_curLine->i_GetVisibleTextLength() - 1, 
                            i_SavedTextX_);
        i_lineOffset = max (i_lineOffset, 0);
    }
    CT_TextPos co_endPos;
    co_endPos.v_Update (pco_GetParagraph (i_paragraph), i_line, i_lineOffset);

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = co_startPos;
        *pco_SelectionEnd_ = co_endPos;
        bool b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
        v_RefreshScreen_();
    }
    else
    {
        if (co_endPos == *pco_SelectionStart_)
        {
            v_CancelSelection_();
            *pco_CaretPos_ = co_endPos;
            bool b_ = b_RepositionCaret_();
        }
        else
        {
            *pco_SelectionEnd_ = co_endPos;
            bool b_ = b_InvertSelection_ (co_startPos, co_endPos);
            v_RefreshScreen_();
        }
    }

}    //  void CKaiView::OnSelectDown()

void CKaiView::OnSelectToHome()
{
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        CT_Line * pco_caretLine = pco_GetLine_ (pco_CaretPos_->i_GetParagraph(),
                                               pco_CaretPos_->i_GetLine());
        CT_TextPos co_selectTo = *pco_CaretPos_;
        co_selectTo.v_MoveToStartOfLine();
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = *pco_CaretPos_;
        *pco_SelectionEnd_ = co_selectTo;
        bool b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
        v_RefreshScreen_();
    }
    else
    {
        CT_Line * pco_caretLine = 
                pco_GetLine_ (pco_SelectionEnd_->i_GetParagraph(),
                               pco_SelectionEnd_->i_GetLine());
        CT_TextPos co_selectTo = *pco_SelectionEnd_;
        co_selectTo.v_MoveToStartOfLine();
        if (co_selectTo == *pco_SelectionStart_)
        {
            v_CancelSelection_();
            *pco_CaretPos_ = co_selectTo;
            bool b_ = b_RepositionCaret_();
        }
        else
        {
            *pco_SelectionEnd_ = co_selectTo;
            bool b_ = b_InvertSelection_ (*pco_SelectionEnd_, co_selectTo);
            v_RefreshScreen_();
        }
    }

    b_CaretAtLineEnd_ = false;

}    //  void CKaiView::OnSelectToHome()

void CKaiView::OnSelectToEnd()
{
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        CT_Line * pco_caretLine = pco_GetLine_ (pco_CaretPos_->i_GetParagraph(),
                                               pco_CaretPos_->i_GetLine());
        CT_TextPos co_selectTo = *pco_CaretPos_;
        if (!pco_caretLine->b_IsLastInParagraph())
        {
            co_selectTo.v_MovePastEndOfLine();
        }
        else
        {
            co_selectTo.v_SetLineOffset 
                            (pco_caretLine->i_GetVisibleTextLength() - 1);
            ++co_selectTo;
        }
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = *pco_CaretPos_;
        *pco_SelectionEnd_ = co_selectTo;
        bool b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
        v_RefreshScreen_();
    }
    else
    {
        CT_Line * pco_curLine = 
                    pco_GetLine_ (pco_SelectionEnd_->i_GetParagraph(),
                                   pco_SelectionEnd_->i_GetLine());
        CT_TextPos co_selectTo = *pco_SelectionEnd_;
        if (!pco_curLine->b_IsLastInParagraph())
        {
            co_selectTo.v_MovePastEndOfLine();
        }
        else
        {
            co_selectTo.v_SetLineOffset 
                                (pco_curLine->i_GetVisibleTextLength() - 1);
            ++co_selectTo;
        }
        if (co_selectTo == *pco_SelectionStart_)
        {
            v_CancelSelection_();
            *pco_CaretPos_ = co_selectTo;
            bool b_ = b_RepositionCaret_();
        }
        else
        {
            CT_TextPos co_selectFrom = *pco_SelectionEnd_;
            *pco_SelectionEnd_ = co_selectTo;
            bool b_ = b_InvertSelection_ (co_selectFrom, co_selectTo);
            v_RefreshScreen_();
        }
    }

}    //  void CKaiView::OnSelectToEnd()

void CKaiView::OnSelectToTop()
{
    CT_TextPos co_selectTo (pco_GetParagraph (0), 0, 0);
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = *pco_CaretPos_;
        *pco_SelectionEnd_ = co_selectTo;
        bool b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
    }
    else
    {
        if (co_selectTo == *pco_SelectionStart_)
        {
            v_CancelSelection_();
            *pco_CaretPos_ = co_selectTo;
            bool b_ = b_RepositionCaret_();
        }
        else
        {
            CT_TextPos co_selectFrom = *pco_SelectionEnd_;
            *pco_SelectionEnd_ = co_selectTo;
            bool b_ = b_InvertSelection_ (co_selectFrom, co_selectTo);
        }
    }

    i_SavedTextX_ = -1;
    BOOL mfcb_ = SendMessage (WM_VSCROLL, SB_TOP);

}    // void CKaiView::OnSelectToTop()

void CKaiView::OnSelectToBottom() 
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    CT_Paragraph * pco_lastParagraph = 
                        pco_GetParagraph (pco_doc->i_NParagraphs() - 1);
    CT_TextPos co_selectTo (pco_lastParagraph, 
                            pco_lastParagraph->length()-1);
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
        *pco_SelectionStart_ = *pco_CaretPos_;
        *pco_SelectionEnd_ = co_selectTo;
        bool b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
    }
    else
    {
        if (co_selectTo == *pco_SelectionStart_)
        {
            v_CancelSelection_();
            *pco_CaretPos_ = co_selectTo;
            bool b_ = b_RepositionCaret_();
        }
        else
        {
            CT_TextPos co_selectFrom = *pco_SelectionEnd_;
            *pco_SelectionEnd_ = co_selectTo;
            bool b_ = b_InvertSelection_ (co_selectFrom, co_selectTo);
        }
    }

    i_SavedTextX_ = -1;
    BOOL mfcb_ = SendMessage (WM_VSCROLL, SB_BOTTOM);

}  //   void CKaiView::OnSelectToBottom() 

void CKaiView::OnEditSelectAll()
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    CT_TextPos co_start;
    co_start.v_SetParagraph (pco_GetParagraph (0));
    co_start.v_SetVisibleParagraphOffset (0);
    CT_Paragraph * pco_lastParagraph = 
                        pco_GetParagraph (pco_doc->i_NParagraphs() - 1);
    CT_TextPos co_end (pco_lastParagraph, 
                       pco_lastParagraph->length()-1);

    *pco_SelectionStart_ = co_start;
    *pco_SelectionEnd_ = co_end;

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        eo_SelectionMode_ = ec_StaticSelection;
        HideCaret();
        b_CaretShown_ = false;
    }

    bool b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
    v_RefreshScreen_();

}    //  void CKaiView::OnSelectAll()

void CKaiView::v_CopySelection_()
{
    CT_TextPos co_first, co_last;
    if (*pco_SelectionEnd_ > *pco_SelectionStart_)
    {
        co_first = *pco_SelectionStart_;
        co_last = *pco_SelectionEnd_;
    }
    else
    {
        co_first = *pco_SelectionEnd_;
        co_last = *pco_SelectionStart_;
    }

    STL_STRING str_text;
    HGLOBAL h_mem;
    vector<CT_Paragraph *> vec_copy;
    for (int i_ = co_first.i_GetParagraph(); 
         i_ <= co_last.i_GetParagraph(); 
         ++i_)
    {
        CT_Paragraph * pco_p = new CT_Paragraph (*pco_GetParagraph (i_));
        if ((i_ == co_first.i_GetParagraph()) && 
            (co_first.i_GetVisibleParagraphOffset() > 0))
        {
            ST_Token st_start = 
                pco_p->st_GetTokenFromOffset (co_first.i_GetParagraphOffset());
            STL_STRING str_new 
                (pco_p->substr (co_first.i_GetParagraphOffset()));
            int i_t = pco_p->i_GetTokenNum (st_start);
            ST_Token st_format;
            for (; i_t >= 0; --i_t)
            {
                st_format = pco_p->st_GetToken (i_t);
                if (st_format.eo_TokenType == ec_TokenFormatting)
                {
                    break;
                }
            }
            if (i_t < 0)
            {
                ERROR_TRACE (_T("Can't find formatting."));
            }
            else
            {
                str_new = pco_p->substr (st_format.i_Offset, 
                                         st_format.i_Length) +
                          str_new;
            }
            pco_p->assign (str_new);
        }

        if (i_ == co_last.i_GetParagraph())
        {
            if (0 == co_last.i_GetVisibleParagraphOffset())
            {
                delete pco_p;
                continue;
            }
            int i_length = co_last.i_GetParagraphOffset();
            if (co_first.i_GetParagraph() == co_last.i_GetParagraph())
            {
                i_length -= co_first.i_GetVisibleParagraphOffset();
            }
            pco_p->assign (pco_p->substr (0, i_length));
        }

        vec_copy.push_back (pco_p);
        for (int i_t = 0; i_t < pco_p->i_NTokens(); ++i_t)
        {
            ST_Token st_t = pco_p->st_GetToken (i_t);
            switch (st_t.eo_TokenType)
            {
                case ec_TokenText:
                case ec_TokenPunctuation:
                case ec_TokenSpace:
                case ec_TokenTab:
                {
                    str_text += pco_p->substr (st_t.i_Offset, st_t.i_Length);
                    break;
                }
                case ec_TokenEndOfParagraph:
                {
                    str_text += _T("\r\n");
                    break;
                }
                case ec_TokenDiacritics:
                case ec_TokenFormatting:
                case ec_TokenHiddenTag:
                {
                    break;
                }
                case ec_TokenTypeBack:
                case ec_TokenTypeFront:
                default:
                {
                    ERROR_TRACE (_T("Illegal token type"));
                    return;
                }
            }
        }    //  for (int i_t = 0; i_t < pco_p->i_NTokens(); ++i_t)
    }

    CSharedFile    co_sf (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
    vector<CT_Paragraph *>::iterator it_p = vec_copy.begin();
    for (; it_p != vec_copy.end(); ++it_p)
    {
        #ifdef _UNICODE
        co_sf.Write ((*it_p)->data(), 2*(*it_p)->length());
        #else
        co_sf.Write ((*it_p)->data(), (*it_p)->length());
        #endif
        delete (*it_p);
    }
    h_mem = co_sf.Detach();
    if (!h_mem)
    {
        ERROR_TRACE (_T("No formatting in paragraph"));
        return;
    }
    COleDataSource * pco_ds = new COleDataSource;
    pco_ds->Empty();
    pco_ds->CacheGlobalData (((CKaiApp *)AfxGetApp())->ui_GetClipboardFormat(), h_mem);
    CSharedFile    co_sf1 (GMEM_MOVEABLE | GMEM_DDESHARE | GMEM_ZEROINIT);
    #ifdef _UNICODE
    co_sf1.Write (str_text.data(), 2*str_text.length());
    #else
    co_sf1.Write (str_text.data(), str_text.length());
    #endif
    h_mem = co_sf1.Detach();
    if (!h_mem)
    {
        ERROR_TRACE (_T("No formatting in paragraph"));
        return;
    }

    #ifdef _UNICODE
    pco_ds->CacheGlobalData (CF_UNICODETEXT, h_mem);
    #else
    pco_ds->CacheGlobalData (CF_TEXT, h_mem);
    #endif
    pco_ds->SetClipboard();

}    //  void CKaiView::v_CopySelection_()

void CKaiView::v_DeleteSelection_ (bool b_repaint)
{
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        ERROR_TRACE (_T("No selection."));
        return;
    }

    CKaiDoc * pco_doc = GetDocument_();

    CT_TextPos co_first, co_last;
    if (*pco_SelectionEnd_ > *pco_SelectionStart_)
    {
        co_first = *pco_SelectionStart_;
        co_last = *pco_SelectionEnd_;
    }
    else
    {
        co_first = *pco_SelectionEnd_;
        co_last = *pco_SelectionStart_;
    }

    int i_caretPar = co_first.pco_GetParagraph()->i_At();
    int i_caretVisibleOffset = co_first.i_GetVisibleParagraphOffset();

    pco_doc->v_DeleteText (co_first, 
                           co_last,
                           *pco_SelectionEnd_ < *pco_SelectionStart_,
                           true);

    CClientDC co_dc (this);
    eo_SelectionMode_ = ec_NoSelection;
    pco_CaretPos_->v_SetParagraph (pco_doc->pco_GetParagraph (i_caretPar));
    pco_CaretPos_->v_SetVisibleParagraphOffset (i_caretVisibleOffset);
    pco_CaretPos_->v_Invalidate (CT_TextPos::ec_ParagraphPos);
    pco_CaretPos_->pco_GetParagraph()->b_Render (&co_dc);
    if (!b_LineExists_ (st_DisplayWindow_.i_firstParagraph, 
                        st_DisplayWindow_.i_firstLine))
    {
        int i_p = pco_CaretPos_->i_GetParagraph();
        int i_l = pco_CaretPos_->i_GetLine();
        bool b_lineExists = true;
        for (int i_ = 0; (i_ < 3) && (b_lineExists); ++i_)
        {
            b_lineExists = b_GetPrevLine_ (i_p, i_l, i_p, i_l);
        }
        st_DisplayWindow_.i_firstParagraph = i_p;
        st_DisplayWindow_.i_firstLine = i_l;
        st_DisplayWindow_.co_topLeft.cy = 0;
    }

    if (!b_repaint)
    {
        return;
    }

    if (!b_IsVisible_ (*pco_CaretPos_))
    {
        st_DisplayWindow_.i_firstParagraph = pco_CaretPos_->i_GetParagraph();
        st_DisplayWindow_.i_firstLine = pco_CaretPos_->i_GetLine();
        st_DisplayWindow_.co_topLeft.cy = 0;
    }

    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

    bool b_ = b_RepositionCaret_();

}    //  void CKaiView::v_DeleteSelection_()

void CKaiView::v_ReplaceSelection_ (const STL_STRING& str_replaceWith)
{
    if (str_replaceWith.empty())
    {
        ERROR_TRACE (_T("Empty replace string."));
        return;
    }

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        ERROR_TRACE (_T("Selection expected."));
        return;
    }

    v_DeleteSelection_ (false);

    CT_DocDescriptor * pco_doc = GetDocument_()->pco_GetDocDescriptor();

    int i_caretParagraph = pco_CaretPos_->i_GetParagraph();
    int i_visibleOffset = pco_CaretPos_->i_GetVisibleParagraphOffset();
    int i_paragraphsBelow = pco_doc->i_NParagraphs() - i_caretParagraph - 1;

    pco_doc->v_InsertText (pco_CaretPos_, 
                           CT_KaiString (str_replaceWith), 
                           true);

    int i_paragraphsAffected = pco_doc->i_NParagraphs() - 
                                 i_caretParagraph - 
                                 i_paragraphsBelow;

    CClientDC co_dc (this);
    for (int i_ = 0; i_ < i_paragraphsAffected; ++i_)
    {
        CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (i_caretParagraph + i_);
        pco_p->b_Render (&co_dc);
    }
    
    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

}    //  v_ReplaceSelection_ (...)
