//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2002
//    
//    Facility: Kai 1.2
//
//    Module description:    helper functions - part of the MFC CView class.
//
//    $Id: KaiViewHelpers.cpp,v 1.88 2008-11-19 07:38:11 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include <afxole.h>
#include "Kai.h"
#include "KaiMainFrm.h"
#include "KaiDoc.h"
#include "KaiView.h"
#include "KaiParagraph.h"
#include "KaiLineDescriptor.h"
#include "KaiUndoStack.h"
#include "KaiParagraphFormatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CKaiView::i_LinesInDocument_()
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();

    int i_lines = 0;
    for (unsigned int ui_p = 0; ui_p < vpo_paragraphs.size(); ++ui_p)
    {
        CT_Paragraph * pco_p = vpo_paragraphs[ui_p];
        i_lines += pco_p->i_NLines();
    }

    return i_lines;
}

CT_Paragraph * const CKaiView::pco_GetParagraph (int i_paragraph)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();
    CT_Paragraph * pco_p = NULL;
    if (!vpo_paragraphs.empty())
    {
        pco_p = vpo_paragraphs[i_paragraph];
    }
    return pco_p;
}

bool CKaiView::b_GetLineFromYPos_ (int i_yPos, CT_Line *& pco_line)
{
    if (i_yPos >= st_DisplayWindow_.i_linesShown)
    {
        ERROR_TRACE (_T("Invalid line number."));
        return false;
    }

    int i_paragraph = st_DisplayWindow_.i_firstParagraph;
    int i_line = st_DisplayWindow_.i_firstLine;

    for (int i_ = 0; i_ < i_yPos; ++i_)
    {
        bool b_ = b_GetNextLine_ (i_paragraph, i_line, i_paragraph, i_line);
        if (!b_)
        {
            return false;
        }
    }

    pco_line = pco_GetLine_ (i_paragraph, i_line);

    return true;
}

CT_Line * CKaiView::pco_GetLine_ (int i_paragraph, int i_line)
{
    CT_Paragraph * pco_p = pco_GetParagraph (i_paragraph);
    CT_Line * pco_l = pco_p->pco_GetLine (i_line);

    return pco_l;
}

bool CKaiView::b_GetNextLine_ (int i_curParagraph,
                               int i_curLine,
                               int& i_paragraph, 
                               int& i_line)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();

    if (i_curParagraph < 0 || i_curParagraph >= static_cast<int>(vpo_paragraphs.size()))
    {
        ERROR_TRACE (_T("Invalid paragraph number."));
        return false;
    }
    CT_Paragraph * pco_paragraph = vpo_paragraphs[i_curParagraph];
    if (i_curLine < 0 || i_curLine >= pco_paragraph->i_NLines())
    {
        ERROR_TRACE (_T("Invalid line number."));
        return false;
    }

    if (i_curLine < pco_paragraph->i_NLines()-1)
    {
        i_paragraph = i_curParagraph;
        i_line = i_curLine + 1;
        return true;
    }
    else
    {
        if (i_curParagraph < static_cast<int>(vpo_paragraphs.size()-1))
        {
            i_paragraph = i_curParagraph + 1;
            i_line = 0;
            return true;
        }
        else
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Paragraph number too big."));
            i_paragraph = -1;
            i_line = -1;
            return false;
        }
    }

}    //    b_GetNextLine_ (...)

bool CKaiView::b_GetPrevLine_ (int i_curParagraph,
                               int i_curLine,
                               int& i_prevParagraph, 
                               int& i_prevLine)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();

    if (i_curParagraph < 0 || i_curParagraph >= static_cast<int>(vpo_paragraphs.size()))
    {
        ERROR_TRACE (_T("Invalid paragraph number."));
        return false;
    }
    CT_Paragraph * pco_paragraph = vpo_paragraphs[i_curParagraph];
    if (i_curLine < 0 || i_curLine >= pco_paragraph->i_NLines())
    {
        ERROR_TRACE (_T("Invalid line number."));
        return false;
    }

    if (i_curLine > 0)
    {
        i_prevParagraph = i_curParagraph;
        i_prevLine = i_curLine - 1;
        return true;
    }
    else
    {
        if (i_curParagraph > 0)
        {
            i_prevParagraph = i_curParagraph - 1;
            pco_paragraph = vpo_paragraphs[i_prevParagraph];
            if (pco_paragraph->i_NLines() < 1)
            {
                ERROR_TRACE (_T("Empty or unformatted paragraph."));
                pco_paragraph->b_Render (GetDC());
//                i_prevParagraph = -1;
//                i_prevLine = -1;
//                return false;
            }
            i_prevLine = pco_paragraph->i_NLines()-1;
            return true;
        }
        else
        {
            i_prevParagraph = 0;
            i_prevLine = 0;
            return false;
        }
    }

}    //    b_GetPrevLine_ (...)

bool CKaiView::b_ScreenToTextPos_ (CPoint co_point, CT_TextPos * pco_textPos)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    CDC * pco_screenDC = GetDC();

    int i_paragraph = st_DisplayWindow_.i_firstParagraph;
    int i_line = st_DisplayWindow_.i_firstLine;
    CT_Line * pco_line = pco_GetLine_ (i_paragraph, i_line);

    int i_height = st_DisplayWindow_.co_topLeft.cy;
    for (int i_yPos = 0; i_yPos < st_DisplayWindow_.i_linesShown-1; ++i_yPos)
    {
        CT_Paragraph * pco_paragraph = pco_line->pco_GetParent();
        if (i_height + pco_line->i_GetLineHeight() + pco_line->i_GetSpacing() >= 
            co_point.y)
        {
            break;
        }
        i_height += pco_line->i_GetLineHeight() + pco_line->i_GetSpacing();
        if (!b_IsLastLine_ (i_paragraph, i_line))
        {
            bool b_ = b_GetNextLine_ (i_paragraph, i_line, i_paragraph, i_line);
            if (!b_)
            {
                return false;
            }
            pco_line = pco_GetLine_ (i_paragraph, i_line);
        }
    }

    //
    // Horizontal pos.
    //
    i_SavedTextX_ = -1;
    pco_textPos->v_ClearPastEndOfLine();

    co_point.x += i_HScrollPos_;
    if (0 == i_HScrollPos_)
    {
        co_point.x -= st_DisplayWindow_.co_topLeft.cx;
    }

    int i_pixPerInch = pco_screenDC->GetDeviceCaps (LOGPIXELSX);
    co_point.x -= (i_pixPerInch * 
                   pco_GetParagraph (i_paragraph)->i_GetLeftIndent())/100;

    int i_xScreenPos = max (0, co_point.x);
    int i_lineLength = pco_line->i_GetScreenLength();
    if (i_xScreenPos >= i_lineLength)
    {
        if (!pco_line->b_IsLastInParagraph())
        {
            pco_textPos->v_Update (pco_GetParagraph (i_paragraph), i_line, 0);
            pco_textPos->v_MovePastEndOfLine();
        }
        else
        {
            pco_textPos->v_Update (pco_GetParagraph (i_paragraph), 
                                   i_line, 
                                   pco_line->i_GetVisibleTextLength() - 1);
        }
        return true;
    }

    int i_pOffset = pco_line->i_ParagraphOffsetFromScreenPos (i_xScreenPos, 
                                                              pco_screenDC);
    ReleaseDC (pco_screenDC);
    //pco_textPos->v_Update (pco_GetParagraph (i_paragraph),
    //                       i_line,
    //                       i_pOffset);
    pco_textPos->v_SetParagraph (pco_GetParagraph (i_paragraph));
    pco_textPos->v_SetParagraphOffset (i_pOffset);

    return true;

}   // b_ScreenToTextPos_ (...)

bool CKaiView::b_TextPosToClientPos_ (CT_TextPos * pco_textPos, 
                                      CSize& co_screenPos)
{
    CT_TextPos co_textPos (*pco_textPos);

    int i_linesShown = st_DisplayWindow_.i_linesShown;
    int i_topParagraph = st_DisplayWindow_.i_firstParagraph;
    int i_topLine = st_DisplayWindow_.i_firstLine;
    
    //
    // If the bottom of the previous line is visible, go one line back
    //
    if (st_DisplayWindow_.co_topLeft.cy > 0)
    {
        bool b_ = b_GetPrevLine_ (i_topParagraph, 
                                  i_topLine, 
                                  i_topParagraph, 
                                  i_topLine);
        if (!b_)
        {
            ERROR_TRACE 
                (_T("No previous line while st_DisplayWindow_.co_topLeft.cy > 0."));
            return false;
        }
        ++i_linesShown;
    }

    CT_TextPos co_top;
    co_top.v_Update (pco_GetParagraph (i_topParagraph), 
                     i_topLine, 
                     0);
    if (co_textPos < co_top)
    {
        co_screenPos.cx = -1;
        co_screenPos.cy = -1;
        return false;
    }

    int i_bottomParagraph = i_topParagraph;
    int i_bottomLine = i_topLine;
    for (int i_ = 0; i_ < i_linesShown; ++i_)
    {
        if (!b_IsLastLine_ (i_bottomParagraph, i_bottomLine))
        {
            bool b_ = b_GetNextLine_ (i_bottomParagraph, 
                                      i_bottomLine,
                                      i_bottomParagraph,
                                      i_bottomLine);
            if (!b_)
            {
                return false;
            }
        }
    }

    //
    // One more line may be partially visible below 
    // the last fully visible one
    //
    if (!b_IsLastLine_ (i_bottomParagraph, i_bottomLine))
    {
        ++i_linesShown;
    }

    CT_Line * pco_bottomLine = pco_GetLine_ (i_bottomParagraph, 
                                            i_bottomLine);
    CT_TextPos co_bottom;
    co_bottom.v_Update (pco_GetParagraph (i_bottomParagraph), 
                        i_bottomLine, 
                        pco_bottomLine->i_GetVisibleTextLength() - 1);
    if (co_textPos > co_bottom)
    {
        co_screenPos.cx = -1;
        co_screenPos.cy = -1;
        return false;
    }

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();

    //
    // Find the Y pos
    //
    int i_yTextPos = 0;
    for (int i_p = i_topParagraph; i_p < co_textPos.i_GetParagraph(); ++ i_p)
    {
        CT_Paragraph * pco_p = vpo_paragraphs[i_p];
        i_yTextPos += pco_p->i_NLines();
    }

    i_yTextPos += co_textPos.i_GetLine() - i_topLine;
    int i_yScreenPos = 0;
    int i_paragraph = i_topParagraph;
    int i_line = i_topLine;
    CT_Line * pco_line = pco_GetLine_ (i_paragraph, i_line);
    CT_Paragraph * pco_paragraph = pco_line->pco_GetParent();
    int i_height = st_DisplayWindow_.co_topLeft.cy;
    if (i_height > 0)
    {
        i_height -= pco_line->i_GetLineHeight() + pco_line->i_GetSpacing();
    }
    i_yTextPos = min (i_yTextPos, i_linesShown-1);

    for (int i_at = 0; i_at < i_yTextPos; ++i_at)
    {
        i_height += pco_line->i_GetLineHeight() + pco_line->i_GetSpacing();
        bool b_ = b_GetNextLine_ (i_paragraph, i_line, i_paragraph, i_line);
        if (!b_)
        {
            break;
        }
        pco_line = pco_GetLine_ (i_paragraph, i_line);
        pco_paragraph = pco_line->pco_GetParent();
    }

    co_screenPos.cy = i_height;

    //
    // X pos
    //
    CDC * pco_dc = GetDC();
    co_screenPos.cx = 
        pco_line->i_ScreenPosFromParagraphOffset (pco_textPos->i_GetParagraphOffset(), 
                                                  pco_dc);
    co_screenPos.cx -= i_HScrollPos_;
    co_screenPos.cx += st_DisplayWindow_.co_topLeft.cx;
    int i_pixPerInch = pco_dc->GetDeviceCaps (LOGPIXELSX);
    co_screenPos.cx += (i_pixPerInch * 
                        pco_line->pco_GetParent()->i_GetLeftIndent())/100;
    ReleaseDC (pco_dc);

    return true;

}    //    b_TextPosToClientPos_ (...)

int CKaiView::i_GetYPos_ (CT_TextPos& co_pos)
{
    CT_TextPos co_textPos (co_pos);
    if (co_textPos.b_PastEndOfLine())
    {
        --co_textPos;
    }

    int i_p = st_DisplayWindow_.i_firstParagraph;
    int i_l = st_DisplayWindow_.i_firstLine;
    int i_yPos = 0;
    for (; i_yPos < st_DisplayWindow_.i_linesShown; ++i_yPos)
    {
        if ((i_p == co_pos.i_GetParagraph()) && 
            (i_l == co_pos.i_GetLine()))
        {
            break;
        }
        bool b_ = b_GetNextLine_ (i_p, i_l, i_p, i_l);
    }

    return i_yPos;

}    //    i_GetYPos_ (...)

void CKaiView::v_MakeVisible_ (CT_TextPos& co_pos)
{
    st_DisplayWindow_.i_firstParagraph = co_pos.i_GetParagraph();
    st_DisplayWindow_.i_firstLine = co_pos.i_GetLine();
    st_DisplayWindow_.co_topLeft.cy = 0;
    
    CRect co_clientRect;
    GetClientRect (&co_clientRect);

    if (!((0 == st_DisplayWindow_.i_firstParagraph) && 
          (0 == st_DisplayWindow_.i_firstLine)) &&
        st_DisplayWindow_.b_bottom)
    {
        int i_spaceLeft = co_clientRect.Height() - i_GetTotalHeight_();
        int i_p = 0;
        int i_l = 0;
        CT_Paragraph * pco_p = pco_GetParagraph (0);
        CT_Line * pco_l = pco_GetLine_ (0, 0);
        for (int i_ = 0; i_spaceLeft > 0; ++i_)
        {
            i_spaceLeft -= pco_l->i_GetLineHeight();
            i_spaceLeft -= pco_l->i_GetSpacing();
            if ((i_p == st_DisplayWindow_.i_firstParagraph) &&
                (i_l == st_DisplayWindow_.i_firstLine))
            {
                break;
            }

            if (b_IsLastLine_ (i_p, i_l))
            {
                break;
            }

            bool b_ = b_GetNextLine_ (i_p, i_l, i_p, i_l);
            if (!b_)
            {
                ERROR_TRACE (_T("Next line expected."));
                break;
            }
            pco_p = pco_GetParagraph (i_p);
            pco_l = pco_GetLine_ (i_p, i_l);
        }
        if ((i_spaceLeft > 0) && 
            (i_p == st_DisplayWindow_.i_firstParagraph) && 
            (i_l == st_DisplayWindow_.i_firstLine))
        {
            st_DisplayWindow_.i_firstParagraph = 0;
            st_DisplayWindow_.i_firstLine = 0;
        }
    }

    i_VScrollPos_ = i_CalculateVScrollPos_ (co_pos);

    CT_Line * pco_line = pco_GetLine_ (co_pos.i_GetParagraph(), 
                                      co_pos.i_GetLine());

    CClientDC co_dc (this);
    int i_x = 
        pco_line->i_ScreenPosFromParagraphOffset (co_pos.i_GetParagraphOffset(), 
                                                  &co_dc);

    while (i_x < i_HScrollPos_)
    {
        i_HScrollPos_ -= i_HScrollStep_;
    }

    while (i_x - i_HScrollPos_ > co_clientRect.Width())
    {
        i_HScrollPos_ += i_HScrollStep_;
    }
        
    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

    v_AdjustScrollbars_();

}

bool CKaiView::b_RepositionCaret_()
{
    if (st_DisplayWindow_.i_linesShown < 1)
    {
        return true;
    }

    //
    // Make sure caret pos is inside display window
    //
    if (!b_IsCaretPosVisible_())
    {
        v_MakeVisible_ (*pco_CaretPos_);
    }

    CSize co_caretScreenPos;
    
    bool b_ = b_TextPosToClientPos_ (pco_CaretPos_, co_caretScreenPos);
    if (!b_)
    {
        return false;
    }

    if (b_CaretShown_)
    {
        HideCaret();
    }
    BOOL mfcb_ = DestroyCaret();

    CT_TextPos co_caretPos (*pco_CaretPos_);
    CT_Line * pco_caretLine = pco_GetLine_ (co_caretPos.i_GetParagraph(), co_caretPos.i_GetLine());
    if (co_caretPos.b_PastEndOfLine())
    {
        if (pco_caretLine->b_IsLastInParagraph())
        {
            co_caretPos.v_ClearPastEndOfLine();
            co_caretPos.v_SetLineOffset (pco_caretLine->i_GetVisibleTextLength() - 1);
        }
        else
        {
//            ++co_caretPos;
        }
    }

    CT_Paragraph * pco_caretParagraph = pco_caretLine->pco_GetParent();
    CDC * pco_screenDC = GetDC();
    CFont * pco_font = pco_caretParagraph->pco_GetWindowsFont (pco_screenDC,
                                                               co_caretPos.i_GetParagraphOffset());
    CFont * pco_oldFont = pco_screenDC->SelectObject (pco_font);
    TEXTMETRIC st_tm;
    UINT ui_return = pco_screenDC->GetTextMetrics (&st_tm);
    int i_width = max (2, st_tm.tmAveCharWidth/5);
    CreateSolidCaret (i_width, st_tm.tmHeight);
    pco_screenDC->SelectObject (pco_oldFont);

    CPoint co_cp (co_caretScreenPos);
    co_cp.x -= max (1, st_tm.tmAveCharWidth/100);
    co_cp.y += pco_caretLine->i_GetLineHeight() - 
               st_tm.tmHeight + 
               st_tm.tmDescent;
    SetCaretPos (co_cp);
    ShowCaret();
    b_CaretShown_ = true;
    ReleaseDC (pco_screenDC);

    return true;

}  //    b_RepositionCaret_()

int CKaiView::i_CalculateVScrollPos_ (CT_TextPos& co_anchor)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();

    int i_lines = 0;
    unsigned int ui_anchorParagraph = static_cast<unsigned int>(co_anchor.i_GetParagraph());
    for (unsigned int ui_p = 0; ui_p < vpo_paragraphs.size(); ++ui_p)
    {
        if (ui_p >= ui_anchorParagraph)
        {
            break;
        }
        CT_Paragraph * pco_p = vpo_paragraphs[ui_p];
        i_lines += pco_p->i_NLines();
    }

    i_lines += co_anchor.i_GetLine();
    int i_scrollPos = (i_lines * GetScrollLimit(SB_VERT))/i_LinesInDocument_();

    return i_scrollPos;

}    //   i_CalculateVScrollPos_()

bool CKaiView::b_IsCaretPosVisible_()
{
    CT_TextPos co_pos (*pco_CaretPos_);

    //
    // If caret pos has been moved to the end of line CaretPos will be 
    // pointing to the next character which may be below client rectangle
    //
    const CT_Line * pco_caretLine = 
            pco_GetLine_ (co_pos.i_GetParagraph(), co_pos.i_GetLine());
    if (co_pos.b_PastEndOfLine())
    {
        --co_pos;
    }

    return b_IsVisible_ (co_pos);

}    //    b_IsCaretPosVisible_()

bool CKaiView::b_IsVisible_ (CT_TextPos& co_pos)
{
    if (co_pos.i_GetParagraph() < st_DisplayWindow_.i_firstParagraph)
    {
        return false;
    }
    if ((co_pos.i_GetParagraph() == st_DisplayWindow_.i_firstParagraph) 
        && (co_pos.i_GetLine() < st_DisplayWindow_.i_firstLine))
    {
        return false;
    }

    int i_lastParagraph = st_DisplayWindow_.i_firstParagraph;
    b_CountVisibleLines_();
    int i_lastLine = st_DisplayWindow_.i_firstLine;
    for (int i_ = 0; i_ < st_DisplayWindow_.i_linesShown-1; ++ i_)
    {
        bool b_ = b_GetNextLine_ (i_lastParagraph, 
                                  i_lastLine, 
                                  i_lastParagraph, 
                                  i_lastLine);
        if (!b_)
        {
            return false;
        }
    }

    if (co_pos.i_GetParagraph() > i_lastParagraph)
    {
        return false;
    }
    if ((co_pos.i_GetParagraph() == i_lastParagraph) && 
        (co_pos.i_GetLine() > i_lastLine))
    {
        return false;
    }

    CT_Line * pco_line = pco_GetLine_ (co_pos.i_GetParagraph(), 
                                      co_pos.i_GetLine());

    CClientDC co_dc (this);
    int i_x = 
        pco_line->i_ScreenPosFromParagraphOffset (co_pos.i_GetParagraphOffset(), 
                                                  &co_dc);
    i_x -= i_HScrollPos_;
    if (0 == i_HScrollPos_)
    {
        i_x += st_DisplayWindow_.co_topLeft.cx;
    }

    CRect co_clientRect;
    GetClientRect (&co_clientRect);
    if ((i_x < 0) || i_x > co_clientRect.Width())
    {
        return false;
    }

    return true;

}    //    b_IsVisible_ (...)

bool CKaiView::b_CountVisibleLines_()
{
    CRect co_rect;
    GetClientRect (&co_rect);
    co_rect.top = st_DisplayWindow_.co_topLeft.cy;
    int i_maxHeight = co_rect.bottom - co_rect.top;

    int i_paragraph = st_DisplayWindow_.i_firstParagraph;
    int i_line = st_DisplayWindow_.i_firstLine;
    int i_height = 0;
    int i_visibleLines = 0;
    for (; i_height < i_maxHeight; ++i_visibleLines)
    {
        CT_Line * pco_line = pco_GetLine_ (i_paragraph, i_line);
        i_height += pco_line->i_GetLineHeight();
        i_height += pco_line->i_GetSpacing();
        if (b_IsLastLine_ (i_paragraph, i_line))
        {
            st_DisplayWindow_.i_linesShown = i_visibleLines + 1;
            st_DisplayWindow_.b_bottom = true;        
            return true;
        }
        else
        {
            bool b_ = b_GetNextLine_ (i_paragraph, i_line, i_paragraph, i_line);
            if (!b_)
            {
                return false;
            }
        }
    }

    st_DisplayWindow_.i_linesShown = i_visibleLines;
    st_DisplayWindow_.b_bottom = false;

    return true;

}    //    b_CountVisibleLines_()

int CKaiView::i_GetTotalHeight_()
{
    int i_height = 0;

    int i_paragraph = st_DisplayWindow_.i_firstParagraph;
    int i_line = st_DisplayWindow_.i_firstLine;

    CT_Line * pco_line = pco_GetLine_ (i_paragraph, i_line);

    for (int i_ = 0; i_ < st_DisplayWindow_.i_linesShown; ++i_)
    {
        i_height += pco_line->i_GetLineHeight();
        i_height += pco_line->i_GetSpacing();
        if (b_IsLastLine_ (i_paragraph, i_line))
        {
            break;
        }
        bool b_ = b_GetNextLine_ (i_paragraph, i_line, i_paragraph, i_line);
        if (!b_)
        {
            break;
        }
    }

    return i_height;

}    //    i_GetTotalHeight_()

int CKaiView::i_GetDocumentHeight_()
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();

    int i_height = 0;
    for (unsigned int ui_p = 0; ui_p < vpo_paragraphs.size(); ++ui_p)
    {
        CT_Paragraph * pco_p = vpo_paragraphs[ui_p];
        i_height += pco_p->i_Height();
    }

    return i_height;

}    //    i_GetDocumentHeight_()

bool CKaiView::b_IsLastLine_ (int i_paragraph, int i_line)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    CT_Paragraph * pco_paragraph = pco_doc->pco_GetParagraph (i_paragraph);
    if ((i_paragraph == pco_doc->i_NParagraphs()-1) 
        && (i_line == pco_paragraph->i_NLines()-1))    
    {
        return true;
    }
    return false;

}    //  bool CKaiView::b_IsLastLine_ (...)

bool CKaiView::b_IsEndOfDocument (CT_TextPos& co_textPos)
{
    if (b_IsLastLine_ (co_textPos.i_GetParagraph(), 
                       co_textPos.i_GetLine()))
    {
        CT_Paragraph * pco_paragraph = 
            pco_GetParagraph (co_textPos.i_GetParagraph());
        if (co_textPos.i_GetParagraphOffset() == pco_paragraph->length() - 1)
        {
            return true;
        }
    }
    return false;
}

bool CKaiView::b_IsOverstrikeOn_()
{
    CMainFrame * pco_mainFrameWnd = 
    static_cast<CMainFrame *>(GetParent()->GetParent()->GetParent());
     static_cast<CMainFrame *>(GetParent());

    return pco_mainFrameWnd->b_GetOverstrikeStatus();
}

bool CKaiView::b_LineExists_ (int i_paragraph, int i_line)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    if (i_paragraph >= pco_doc->i_NParagraphs())
    {
        return false;
    }

    CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (i_paragraph);
    if (i_line >= pco_p->i_NLines())
    {
        return false;
    }

    return true;

}    //  b_LineExists_ (...)

void CKaiView::v_UndoSingleEvent_ (CT_UndoStack * pco_us)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    int i_paragraph = pco_us->i_GetParagraph();
    CT_Paragraph * pco_paragraph = pco_doc->pco_GetParagraph (i_paragraph);
    CT_TextPos co_at;
    co_at.v_SetParagraph (pco_paragraph);
    co_at.v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());
    int i_offset = co_at.i_GetParagraphOffset();

    CClientDC co_dc (this);

    switch (pco_us->eo_GetEventType())
    {
        case ec_UndoInsertChar:
        {
            pco_paragraph->b_DeleteChar (&co_dc, co_at);
            pco_CaretPos_->v_SetParagraph (pco_paragraph);
            pco_CaretPos_->v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());

            break;
        }

        case ec_UndoReplaceChar:
        {
            STL_STRING str_char = pco_us->str_GetText();
            if (str_char.length() != 1)
            {
                ATLASSERT(0);
                ERROR_TRACE (_T("Invalid undo text data."));
                return;
            }
            _TUCHAR ui_char = str_char[0];

            // We'll do it in two steps to avoid problems with font changes
            pco_paragraph->b_DeleteChar (&co_dc, co_at);
            int i_t = pco_paragraph->i_GetTokenNumFromOffset (i_offset);
            ST_Token st_t = pco_paragraph->st_GetToken (i_t);
            while (!st_t.b_IsPrintable() && (i_t < pco_paragraph->i_NTokens()))
            {
                st_t = pco_paragraph->st_GetToken (++i_t);
                i_offset = st_t.i_Offset;
            }

            co_at.v_SetVisibleParagraphOffset (pco_us->i_GetStartVisibleOffset());            

            pco_paragraph->insert (pco_paragraph->begin() + co_at.i_GetParagraphOffset(), 
                                   ui_char);

            pco_CaretPos_->v_SetParagraph (pco_paragraph);
            pco_CaretPos_->v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());
            pco_paragraph->v_Invalidate();
            bool b_render = pco_paragraph->b_Render (&co_dc);
            pco_CaretPos_->v_SetParagraph (pco_paragraph);
            pco_CaretPos_->v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());

            break;
        }

        case ec_UndoInsertText:
        {
            CT_TextPos co_start;
            pco_paragraph = pco_doc->pco_GetParagraph (i_paragraph);
            co_start.v_SetParagraph (pco_paragraph);
            co_start.v_SetVisibleParagraphOffset (pco_us->i_GetStartVisibleOffset());
            CT_TextPos co_end;
            co_end.v_SetParagraph (pco_doc->pco_GetParagraph (pco_us->i_GetEndParagraph()));
            co_end.v_SetVisibleParagraphOffset (pco_us->i_GetEndVisibleOffset());
            pco_doc->v_DeleteText (co_start, co_end);
            pco_paragraph = pco_doc->pco_GetParagraph (i_paragraph);
            pco_paragraph->b_Render (&co_dc);
            pco_CaretPos_->v_SetParagraph (pco_paragraph);
            pco_CaretPos_->v_SetVisibleParagraphOffset (pco_us->i_GetStartVisibleOffset());

            break;
        }

        case ec_UndoDeleteChar:
        {
            STL_STRING str_char = pco_us->str_GetText();
            if (str_char.length() != 1)
            {
                ATLASSERT(0);
                ERROR_TRACE (_T("Invalid undo text data."));
                return;
            }
            _TUCHAR ui_char = str_char[0];

            int i_t = pco_paragraph->i_GetTokenNumFromOffset (i_offset);
            ST_Token st_t = pco_paragraph->st_GetToken (i_t);
            while (!st_t.b_IsPrintable() && (i_t < pco_paragraph->i_NTokens()))
            {
                st_t = pco_paragraph->st_GetToken (++i_t);
                i_offset = st_t.i_Offset;
            }

            co_at.v_SetVisibleParagraphOffset (pco_us->i_GetStartVisibleOffset());            

            pco_paragraph->insert (pco_paragraph->begin() + co_at.i_GetParagraphOffset(), 
                                   ui_char);

            pco_CaretPos_->v_SetParagraph (pco_paragraph);
            pco_CaretPos_->v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());
            pco_paragraph->v_Invalidate();
            bool b_render = pco_paragraph->b_Render (&co_dc);
            pco_CaretPos_->v_SetParagraph (pco_paragraph);
            pco_CaretPos_->v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());

            break;
        }

        case ec_UndoDeleteText:
        case ec_UndoChangeFont:
        case ec_UndoParagraphFormatting:
        {
            int i_insertAt = pco_us->i_GetParagraph();
            CT_Paragraph * pco_old = pco_doc->pco_GetParagraph (i_insertAt);
            CT_Paragraph * pco_previous = pco_old->pco_GetPrevious();
            CT_Paragraph * pco_next = NULL;
            if ((pco_us->eo_GetEventType() == ec_UndoChangeFont) || 
                (pco_us->eo_GetEventType() == ec_UndoParagraphFormatting))
            {
                pco_next = (pco_doc->pco_GetParagraph (pco_us->i_GetEndParagraph()))->pco_GetNext();
            }
            else
            {
                pco_next = pco_old->pco_GetNext();
            }

            pco_doc->v_RemoveParagraph (i_insertAt);
            delete pco_old;

            if ((pco_us->eo_GetEventType() == ec_UndoChangeFont) || 
                (pco_us->eo_GetEventType() == ec_UndoParagraphFormatting))
            {
                for (int i_p = i_insertAt + 1; 
                     i_p <= pco_us->i_GetEndParagraph();
                     ++i_p)
                {
                    pco_old = pco_doc->pco_GetParagraph (i_insertAt);
                    pco_doc->v_RemoveParagraph (i_insertAt);
                    delete pco_old;
                }
            }

            CT_Paragraph * pco_current = pco_us->pco_PopParagraph();
            ATLASSERT (pco_current);
            if (!pco_current)
            {
                ERROR_TRACE (_T("Paragraph stack empty."));
                return;
            }

            i_insertAt--;
            while (pco_current)
            {
                pco_current->v_SetPrevious (pco_previous);
                if (pco_previous)
                {
                    pco_previous->v_SetNext (pco_current);
                }
                pco_current->v_Invalidate();
                pco_current->b_Render (&co_dc);
                pco_doc->v_AddParagraph (pco_current, i_insertAt++);
                pco_previous = pco_current;
                pco_current = pco_us->pco_PopParagraph();
            }
            
            ATLASSERT (pco_previous);
            pco_previous->v_SetNext (pco_next);
            if (pco_next)
            {
                pco_next->v_SetPrevious (pco_previous);
            }

            if (pco_us->b_Selection())
            {
                CT_TextPos co_top;
                co_top.v_SetParagraph (pco_doc->pco_GetParagraph (pco_us->i_GetParagraph()));
                co_top.v_SetVisibleParagraphOffset (pco_us->i_GetStartVisibleOffset());
                co_top.v_Invalidate (CT_TextPos::ec_ParagraphPos);

                CT_TextPos co_bottom;
                co_bottom.v_SetParagraph (pco_doc->pco_GetParagraph (pco_us->i_GetEndParagraph()));
                co_bottom.v_SetVisibleParagraphOffset (pco_us->i_GetEndVisibleOffset());
                co_bottom.v_Invalidate (CT_TextPos::ec_ParagraphPos);

                if (pco_us->b_IsCaretAtStart())
                {
                    *pco_SelectionStart_ = co_bottom;
                    *pco_SelectionEnd_ = co_top;
                }
                else
                {
                    *pco_SelectionStart_ = co_top;
                    *pco_SelectionEnd_ = co_bottom;
                }
            }
            else
            {
                pco_CaretPos_->v_SetParagraph (pco_doc->pco_GetParagraph (i_insertAt));
                pco_CaretPos_->v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());
                pco_CaretPos_->v_Invalidate (CT_TextPos::ec_ParagraphPos);
            }

            break;
        }

        case ec_UndoInsertDiacritic:
        {
            int i_token = pco_paragraph->i_GetTokenNumFromOffset (i_offset);
            pco_paragraph->b_DeleteToken (i_token);
            pco_paragraph->v_Invalidate();
            bool b_ = pco_paragraph->b_Render (&co_dc);
            ATLASSERT(b_);
            co_at.v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());
            i_offset = co_at.i_GetParagraphOffset();
            pco_CaretPos_->v_SetParagraph (pco_paragraph);
            pco_CaretPos_->v_SetParagraphOffset (i_offset);

            break;
        }

        case ec_UndoDeleteDiacritic:
        {
            STL_STRING str_d = pco_us->str_GetText();
             STL_STRING str_diacrSeq;
            bool b_ = pco_paragraph->b_CreateDiacrString (str_d[0], str_diacrSeq);
            CT_KaiString str_new = pco_paragraph->substr (0, i_offset);
            str_new += str_diacrSeq;
            str_new += pco_paragraph->substr (i_offset);
            pco_paragraph->v_Invalidate();
            b_ = pco_paragraph->b_Render (&co_dc);
            ATLASSERT(b_);

            break;
        }

        case ec_UndoMergeParagraphs:
        {
            STL_STRING str_add = pco_us->str_GetInitialFragment();
            v_SplitParagraph_ (pco_paragraph, i_offset);
            STL_STRING str_initial = pco_paragraph->substr (0, i_offset);
            STL_STRING str_final = pco_paragraph->substr (i_offset);
            pco_paragraph->assign (str_initial + str_add + str_final);
            pco_paragraph->v_Invalidate();
            pco_paragraph->b_Render (&co_dc);
            if (pco_us->b_Selection())
            {
                pco_SelectionStart_->v_SetParagraph (pco_paragraph);
                pco_SelectionStart_->v_SetVisibleParagraphOffset (pco_us->i_GetVisibleOffset());
                pco_SelectionStart_->v_Invalidate (CT_TextPos::ec_ParagraphPos);
                pco_SelectionEnd_->v_SetParagraph (pco_paragraph->pco_GetNext());
                pco_SelectionEnd_->v_SetVisibleParagraphOffset (0);
                pco_SelectionEnd_->v_Invalidate (CT_TextPos::ec_ParagraphPos);
            }
            else
            {
                pco_CaretPos_->v_SetParagraph (pco_paragraph);
                pco_CaretPos_->v_SetParagraphOffset (i_offset);
            }

            break;
        }

        case ec_UndoSplitParagraph:
        {
            pco_paragraph->v_MergeWithNext();
            pco_paragraph->v_Invalidate();
            bool b_render = pco_paragraph->b_Render (&co_dc);
            pco_CaretPos_->v_SetParagraph (pco_paragraph);
            pco_CaretPos_->v_SetParagraphOffset (i_offset);

            break;
        }

        default:
        {
            ERROR_TRACE (_T("Unknown undo event"));
        }
    }

    if (pco_us->b_Selection())
    {
        eo_SelectionMode_ = ec_StaticSelection;
    }
    else
    {
        eo_SelectionMode_ = ec_NoSelection;
    }

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

}        // v_UndoSingleEvent_ (...)

void CKaiView::v_DeleteChar_ (bool b_backSpace)
{
    CKaiDoc * pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_DeleteSelection_();
    }
    else
    {
        CT_UndoStack * pco_undoStack = pco_doc->pco_GetUndoStack();
        if (b_backSpace && pco_CaretPos_->b_FirstPos())
        {
            return;
        }

        if (b_backSpace)
        {
            --(*pco_CaretPos_);
        }

        CClientDC co_dc (this);
        int i_offset = pco_CaretPos_->i_GetVisibleParagraphOffset();
        CT_Paragraph * pco_paragraph = pco_doc->pco_GetParagraph (pco_CaretPos_->i_GetParagraph());
        if (pco_CaretPos_->i_GetParagraphOffset() == pco_paragraph->length()-1)
        {
            if (pco_CaretPos_->i_GetParagraph() == pco_doc->i_NParagraphs()-1)
            {
                return;
            }
            else
            {
                pco_paragraph->v_MergeWithNext();
    
                pco_undoStack->v_ClearCurrentEvent();
                pco_undoStack->v_SetEventType (ec_UndoMergeParagraphs);
                pco_undoStack->v_SetParagraph (pco_CaretPos_->i_GetParagraph());
                pco_undoStack->v_SetVisibleOffset (pco_CaretPos_->i_GetVisibleParagraphOffset());
                pco_undoStack->v_AddEvent();

                pco_paragraph->v_Invalidate();
                bool b_render = pco_paragraph->b_Render (&co_dc);
                pco_CaretPos_->v_SetParagraph (pco_paragraph);
                pco_CaretPos_->v_SetVisibleParagraphOffset (i_offset);
                pco_CaretPos_->v_Invalidate (CT_TextPos::ec_ParagraphPos);
            }
        }
        else
        {
            int i_t = pco_paragraph->i_GetTokenNumFromOffset (pco_CaretPos_->i_GetParagraphOffset());
            const ST_Token& rst_t = pco_paragraph->rst_GetToken (i_t);
            if (ec_TokenDiacritics == rst_t.eo_TokenType)
            {
                CT_TextPos co_onePast (*pco_CaretPos_);
                ++co_onePast;
                pco_doc->v_DeleteText (*pco_CaretPos_, co_onePast, false, false);
                pco_paragraph->v_Invalidate();
                bool b_render = pco_paragraph->b_Render (&co_dc);
                pco_CaretPos_->v_Invalidate (CT_TextPos::ec_LinePos);
            }
            else
            {
                STL_STRING str_chr = pco_paragraph->substr (pco_CaretPos_->i_GetParagraphOffset(), 1);

                pco_undoStack->v_ClearCurrentEvent();
                pco_undoStack->v_SetEventType (ec_UndoDeleteChar);
                pco_undoStack->v_SetParagraph (pco_paragraph->i_At());
                pco_undoStack->v_SetStartVisibleOffset (pco_CaretPos_->i_GetVisibleParagraphOffset());
                pco_undoStack->v_SetLength (1);
                pco_undoStack->v_SetText (str_chr);
                pco_undoStack->v_AddEvent();

                bool b_delete = pco_paragraph->b_DeleteChar (&co_dc, *pco_CaretPos_);
                                                        // optimized version; 
                                                        // caret pos may be invalid
                bool b_render = pco_paragraph->b_Render (&co_dc);
                pco_CaretPos_->v_SetParagraph (pco_paragraph);
                pco_CaretPos_->v_SetVisibleParagraphOffset (i_offset);
                pco_CaretPos_->v_Invalidate (CT_TextPos::ec_ParagraphPos);
            }
        }

        if (!b_IsCaretPosVisible_())
        {
            v_MakeVisible_ (*pco_CaretPos_);
        }
        eo_RefreshMode_ = ec_WholeScreen;
        InvalidateRect (NULL, TRUE);
        UpdateWindow();
        bool b_reposition = b_RepositionCaret_();
        pco_CaretPos_->v_ClearPastEndOfLine();
    }

    pco_doc->v_SetModified();

}    //  v_DeleteChar_ (bool b_backSpace)

int CKaiView::i_GetDocHeightAboveClient_()
{
    CKaiDoc * pco_doc = GetDocument_();

    int i_height = 0;
    for (int i_p = 0; i_p < st_DisplayWindow_.i_firstParagraph; ++i_p)
    {

        CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (i_p);
        i_height += pco_p->i_Height();
    }

    CT_Paragraph * pco_p = 
                pco_doc->pco_GetParagraph (st_DisplayWindow_.i_firstParagraph);
    for (int i_l =0; i_l < st_DisplayWindow_.i_firstLine; ++i_l)
    {
        CT_Line * pco_l = pco_p->pco_GetLine (i_l);
        i_height += pco_l->i_GetLineHeight();
    }

    return i_height;

}    //  i_GetDocHeightAboveClient_()

void CKaiView::v_LineFromScrollPos_ (int& i_paragraph, int& i_line)
{
    SCROLLINFO st_scrollInfo;
    GetScrollInfo (SB_VERT, &st_scrollInfo, SIF_ALL);

    CRect co_client;
    GetClientRect (&co_client);

    CKaiDoc * pco_doc = GetDocument_();

    int i_height = 0;
    CT_Paragraph * pco_p = NULL;
    i_paragraph = 0;
    for (; i_paragraph < pco_doc->i_NParagraphs(); ++i_paragraph)
    {
        pco_p = pco_doc->pco_GetParagraph (i_paragraph);
        if (i_height + pco_p->i_Height() > st_scrollInfo.nTrackPos)
        {
            break;
        }
        i_height += pco_p->i_Height();
    }

    i_line = 0;
    for (; i_line < pco_p->i_NLines(); ++i_line)
    {
        CT_Line * pco_l = pco_p->pco_GetLine (i_line);
        if ((i_height + pco_l->i_GetLineHeight()) > st_scrollInfo.nTrackPos)
        {
            break;
        }
        i_height += pco_l->i_GetLineHeight();
    }

}    //  v_LineFromScrollPos_ (...)

void CKaiView::v_GetTextFrame_ (CRect& co_rect)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    CT_DocDescriptor * pco_docDescriptor = pco_GetDocDescriptor_();
    
    GetClientRect (&co_rect);
    co_rect.right = i_GetDisplayWidthInPixels_();
    co_rect.right -= i_HScrollPos_;
}

CT_DocDescriptor * CKaiView::pco_GetDocDescriptor_()
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    return pco_doc->pco_GetDocDescriptor();
}

void CKaiView::v_Paste_()
{
    bool b_replaceSelection = false;
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_DeleteSelection_ (false);
        b_replaceSelection = true;
    }

    COleDataObject co_do;
    BOOL ui_cbData = co_do.AttachClipboard();
    if (!ui_cbData)
    {
        ERROR_TRACE (_T("AttachClipboard failed"));
        return;
    }

    UINT ui_cpFormat = ((CKaiApp *)AfxGetApp())->ui_GetClipboardFormat();
    ui_cbData = co_do.IsDataAvailable (((CKaiApp *)AfxGetApp())->ui_GetClipboardFormat());
    if (!ui_cbData)
    {

#ifdef _UNICODE
        ui_cpFormat = CF_UNICODETEXT;
        ui_cbData = co_do.IsDataAvailable (ui_cpFormat);
#endif
        if (!ui_cbData)
        {
            ui_cpFormat = CF_TEXT;
            ui_cbData = co_do.IsDataAvailable (ui_cpFormat);
        }        
    }

    if (!ui_cbData)
    {
        return;
    }

    CT_UndoStack * pco_us = GetDocument_()->pco_GetUndoStack();
    pco_us->v_SetEventType (ec_UndoInsertText);

    CFile * pco_cf = co_do.GetFileData (ui_cpFormat);
    ULONGLONG ull_filelength = pco_cf->GetLength();
    unsigned int ui_bufSize = static_cast<unsigned int>(ull_filelength/sizeof (_TCHAR)) + 1;
    if (ui_bufSize > MAX_BUF_LENGTH)
    {
        ERROR_TRACE (_T("Clipboard size too big."));
        AfxMessageBox (_T ("The text you are trying to paste is too long."));
        return;
    }
    _TCHAR * pco_buf = new _TCHAR [ui_bufSize];
    memset (pco_buf, 0, ui_bufSize);
    UINT nBytesRead = pco_cf->Read (pco_buf, ui_bufSize * sizeof (_TCHAR));
    STL_STRING str_insert (pco_buf);
    delete[] pco_buf;

    v_RemoveCrLf (str_insert);

    CT_DocDescriptor * pco_doc = pco_CaretPos_->pco_GetParagraph()-> pco_GetDocument();

    int i_caretParagraph = pco_CaretPos_->i_GetParagraph();
    int i_visibleOffset = pco_CaretPos_->i_GetVisibleParagraphOffset();
    int i_paragraphsBelow = pco_doc->i_NParagraphs() - i_caretParagraph - 1;

    pco_doc->v_InsertText (pco_CaretPos_, CT_KaiString (str_insert), b_replaceSelection);

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

    pco_CaretPos_->v_Invalidate (CT_TextPos::ec_ParagraphPos);

    bool b_r = b_RepositionCaret_();

    delete pco_cf;

}   //  v_Paste_()

void CKaiView::v_ChangeParagraphProperties_()
{
    CT_UndoStack * pco_us = GetDocument_()->pco_GetUndoStack();
    pco_us->v_SetEventType (ec_UndoParagraphFormatting);

    CT_Paragraph * pco_p = NULL;
    int i_paragraphsAffected = 0;
    if (ec_NoSelection == eo_SelectionMode_)
    {
        pco_p = pco_CaretPos_->pco_GetParagraph();
        i_paragraphsAffected = 1;

        pco_us->v_SetSelection (false);
        pco_us->v_SetParagraph (pco_CaretPos_->i_GetParagraph());
        pco_us->v_SetEndParagraph (pco_CaretPos_->i_GetParagraph());
        pco_us->v_PushParagraph (pco_p);
    }
    else
    {
        CT_UndoStack * pco_us = GetDocument_()->pco_GetUndoStack();
        pco_us->v_SetSelection (true);

        CT_TextPos co_top, co_bottom;
        if (*pco_SelectionStart_ <= *pco_SelectionEnd_)
        {
            co_top = *pco_SelectionStart_;
            co_bottom = *pco_SelectionEnd_;
            pco_p = pco_SelectionStart_->pco_GetParagraph();
            i_paragraphsAffected = pco_SelectionEnd_->pco_GetParagraph()->i_At();
        }
        else
        {
            co_top = *pco_SelectionEnd_;
            co_bottom = *pco_SelectionStart_;
            pco_p = pco_SelectionEnd_->pco_GetParagraph();
            i_paragraphsAffected = pco_SelectionStart_->pco_GetParagraph()->i_At();
        }
        i_paragraphsAffected -= pco_p->i_At();
        ++i_paragraphsAffected;

        pco_us->v_SetParagraph (co_top.i_GetParagraph());
        pco_us->v_SetEndParagraph (co_bottom.i_GetParagraph());
        pco_us->v_SetStartVisibleOffset (co_top.i_GetVisibleParagraphOffset());
        pco_us->v_SetEndVisibleOffset (co_bottom.i_GetVisibleParagraphOffset());
//            pco_us->v_SetCaretAtStart (co_end < co_start);
            
        int i_p = co_top.i_GetParagraph();
        for (; i_p <= co_bottom.i_GetParagraph(); ++i_p)
        {
            pco_us->v_PushParagraph (pco_GetParagraph (i_p));
        }
    }       // multiple paragraphs

    pco_us->v_AddEvent();

    ST_Spacing st_oldSpacing (pco_p->rst_GetSpacing());
    ET_Alignment eo_oldAlignment = pco_p->eo_GetAlignment();
    int i_oldLeftIndent = pco_p->i_GetLeftIndent();
    int i_oldRightIndent = pco_p->i_GetRightIndent();
    
    CT_ParagraphFormatDlg co_dlg (pco_p, (CWnd *)this);
    INT_PTR n_ret = co_dlg.DoModal();
    if (IDOK != n_ret)
    {
        return;
    }

    bool b_rerender = false;
    for (int i_ = 0; i_ < i_paragraphsAffected; ++i_)
    {
        pco_p->v_SetSpacing (co_dlg.eo_CurrentSpacingMode(), 
                             co_dlg.i_CurrentSpacingSize());
        pco_p->v_SetAlignment (co_dlg.eo_CurrentAlignment());
        pco_p->v_SetLeftIndent (co_dlg.i_CurrentLeftIndent());
        pco_p->v_SetRigthIndent (co_dlg.i_CurrentRightIndent());

        if ((pco_p->rst_GetSpacing() != st_oldSpacing)  || 
            (pco_p->eo_GetAlignment() != eo_oldAlignment) ||
            (pco_p->i_GetLeftIndent() != i_oldLeftIndent) ||
            (pco_p->i_GetRightIndent() != i_oldRightIndent))
        {
            b_rerender = true;
            pco_p->v_Invalidate();
            CClientDC co_dc (this);
            pco_p->b_Render (&co_dc);
        }

        if (i_ < i_paragraphsAffected - 1)
        {
            if (!pco_p->pco_GetNext())
            {
                ATLASSERT(0);
                ERROR_TRACE (_T("Next paragraph expected"));
                return;
            }
            pco_p = pco_p->pco_GetNext();
        }

        CKaiDoc * pco_doc = GetDocument_();
        ASSERT_VALID (pco_doc);
        pco_doc->SetModifiedFlag();
    }

    if (b_rerender)
    {
//        st_DisplayWindow_.v_Reset (*pco_CaretPos_);
        eo_RefreshMode_ = ec_WholeScreen;
        InvalidateRect (NULL, TRUE);
        UpdateWindow();
    }

}   //  v_ChangeParagraphProperties_()

void CKaiView::v_Paginate_ (CDC * pco_dc)
{
    if (b_Paginated_)
    {
        return;
    }

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    double d_coeff = static_cast<double>(pco_dc->GetDeviceCaps (LOGPIXELSY)) / 
                            static_cast<double> (i_LogPixelsY_);
    d_coeff *= 1.1;

    bool b_noMorePages = false;
    int i_pageHeight = pco_dc->GetDeviceCaps (PHYSICALHEIGHT) - 
            pco_dc->GetDeviceCaps (LOGPIXELSY);
    int i_y = pco_dc->GetDeviceCaps (LOGPIXELSY)/2;

    vec_PrintPages_.clear();
    ST_PRINT_PAGE st_page (0, 0, 0);
    for (int i_p = 0; i_p < pco_doc->i_NParagraphs(); ++i_p)
    {
        CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (i_p);
        for (int i_l = 0; i_l < pco_p->i_NLines(); ++i_l)
        {
            CT_Line * pco_l = pco_GetLine_ (i_p, i_l);
            int i_lHeight = pco_l->i_GetLineHeight() + pco_l->i_GetSpacing();
            i_lHeight = static_cast<int> (d_coeff * i_lHeight);
            if (i_y + i_lHeight >= i_pageHeight)
            {
                vec_PrintPages_.push_back (st_page);
                st_page.i_FirstParagraph = i_p;
                st_page.i_FirstLine = i_l;
                st_page.i_Lines = 0;
                i_y = pco_dc->GetDeviceCaps (LOGPIXELSY)/2;
            }
            i_y += i_lHeight;
            ++st_page.i_Lines;
        }
    }
    vec_PrintPages_.push_back (st_page);
    b_Paginated_ = true;

}   //  v_Paginate_ (...)

int CKaiView::i_GetDisplayWidthInPixels_()
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    int i_pixPerInch = pco_MemDC_->GetDeviceCaps (LOGPIXELSX);
    return (pco_doc->pco_GetDocDescriptor()->i_GetPageWidth() * i_pixPerInch)/100;
}

void CKaiView::v_ChangeTextSegmentFont_ (unsigned int ui_fontKey, 
                                         CT_TextPos& co_start,
                                         CT_TextPos& co_end)
{
    // Order start/end positions
    CT_TextPos co_top, co_bottom;
    if (co_start <= co_end)
    {
        co_top = co_start;
        co_bottom = co_end;
    }
    else
    {
        co_top = co_end;
        co_bottom = co_start;
    }

    CT_Paragraph * pco_p = co_top.pco_GetParagraph();
    for (int i_p = co_top.i_GetParagraph(); 
         i_p <= co_bottom.i_GetParagraph(); 
         ++i_p)
    {
        bool b_ = pco_p->b_ChangeFont (co_top, co_bottom, ui_fontKey);
        if (!b_)
        {
            return;
        }
        pco_p->b_Render (GetDC());
        pco_p = pco_p->pco_GetNext();
    }

}  //  void CKaiView::v_ChangeTextSegmentFont_ (...)

void CKaiView::v_HandleOnFontModify (ET_FontModifier eo_newStyle)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    bool b_ = true;
    if (ec_NoSelection == eo_SelectionMode_)
    {
        //
        // No selection: use modified font for future editing
        //
        unsigned int ui_currentFont = 0;
        if (0 == ui_RequestedFont_)
        {
            CT_Paragraph * pco_p = pco_CaretPos_->pco_GetParagraph();
            int i_offset = pco_CaretPos_->i_GetParagraphOffset();
            int i_t = pco_p->i_GetTokenNumFromOffset (i_offset);
            
            ST_Token st_t = pco_p->st_GetToken (i_t);
            
            // If inserting at font boundary, pull back
            if ((i_t > 1) && (i_offset == st_t.i_Offset))
            {
                st_t = pco_p->st_GetToken (i_t - 1);
                if (ec_TokenFormatting == st_t.eo_TokenType)
                {
                    st_t = pco_p->st_GetToken (i_t - 2);
                    i_offset = st_t.i_Offset;
                }
                ATLASSERT (ec_TokenFormatting != st_t.eo_TokenType);
                ATLASSERT (0 < i_offset);
            }
            
            b_ = pco_p->b_GetFontKey (i_offset, ui_currentFont);
            if (!b_)
            {
                return;
            }
        }
        else
        {
            ui_currentFont = ui_RequestedFont_;
        }
        
        CT_FontDescriptor * pco_fd = pco_doc->pco_GetFontDescriptor (ui_currentFont);
        ATLASSERT (NULL != pco_fd);

        b_ = pco_doc->b_ModifyFontDescriptor (ui_currentFont, 
                                              eo_newStyle,
                                              !pco_fd->b_IsStylePresent (eo_newStyle),
                                              ui_RequestedFont_);
        if (!b_)
        {
            ATLASSERT(0);
            return;
        }
    }
    else
    {
        //
        // Modify selected text
        //
        CT_TextPos co_top, co_bottom;
        if (*pco_SelectionStart_ <= *pco_SelectionEnd_)
        {
            co_top = *pco_SelectionStart_;
            co_bottom = *pco_SelectionEnd_;
        }
        else
        {
            co_top = *pco_SelectionEnd_;
            co_bottom = *pco_SelectionStart_;
        }

        // Add event to undo stack
        CT_UndoStack * pco_us = GetDocument_()->pco_GetUndoStack();
        pco_us->v_SetEventType (ec_UndoChangeFont);
        pco_us->v_SetSelection (true);

        pco_us->v_SetParagraph (co_top.i_GetParagraph());
        pco_us->v_SetOffset (co_top.i_GetParagraphOffset());
        pco_us->v_SetStartVisibleOffset (co_top.i_GetVisibleParagraphOffset());
        pco_us->v_SetEndParagraph (co_bottom.i_GetParagraph());
        pco_us->v_SetEndVisibleOffset (co_bottom.i_GetVisibleParagraphOffset());
        pco_us->v_SetCaretAtStart (*pco_SelectionEnd_ < *pco_SelectionStart_);

        int i_p = co_top.i_GetParagraph();
        for (; i_p <= co_bottom.i_GetParagraph(); ++i_p)
        {
            pco_us->v_PushParagraph (pco_GetParagraph (i_p));
        }

        pco_us->v_AddEvent();

        CT_Paragraph * pco_p = co_top.pco_GetParagraph();
        unsigned int ui_lastFd = 0;
        bool b_ = pco_p->b_GetFontKey (co_top.i_GetParagraphOffset(), ui_lastFd);
        if (!b_)
        {
            return;
        }
        ATLASSERT (0 != ui_lastFd);

        CT_FontDescriptor * pco_last = pco_p->pco_GetFontDescriptor (ui_lastFd);
        if (!pco_last)
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Unable to find starting font descriptor."));
            return;
        }

        for (int i_p = co_top.i_GetParagraph(); 
             i_p <= co_bottom.i_GetParagraph(); 
             ++i_p)
        {
            bool b_ = pco_p->b_ChangeFont (co_top, 
                                           co_bottom, 
                                           0, 
                                           eo_newStyle, 
                                           !pco_last->b_IsStylePresent (eo_newStyle));
            if (!b_)
            {
                return;
            }

            pco_p->b_Render (GetDC());
            pco_p = pco_p->pco_GetNext();
        }

        pco_SelectionStart_->v_Invalidate();
        pco_SelectionEnd_->v_Invalidate();
        eo_RefreshMode_ = ec_WholeScreen;
        InvalidateRect (NULL, TRUE);
        UpdateWindow();
        pco_doc->SetModifiedFlag();

    }   //  selection

}   //  v_HandleOnFontModify()
