//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2002
//    
//    Facility: Kai 1.2
//
//    Module description:    drawing functions - part of the MFC CView class.
//
//    $Id: KaiViewDraw.cpp,v 1.64 2008-02-17 00:51:22 kostya Exp $
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
#include "KaiParagraph.h"
#include "KaiLineDescriptor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CKaiView::b_RenderScreen_ (CDC * pco_screenDC)
{
    b_Rendered_ = false;

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    CT_DocDescriptor * pco_docDescriptor = pco_doc->pco_GetDocDescriptor();

    if (pco_docDescriptor->b_IsUnrenderable())
    {
        return false;
    }

    if (pco_screenDC->IsPrinting())
    {
        ERROR_TRACE (_T("*** Bad DC"));
        return false;
    }

    v_AdjustScrollbars_();

    CRect co_clientRect;
    GetClientRect (&co_clientRect);

    co_clientRect.right = i_GetDisplayWidthInPixels_();

    CBitmap * pco_newBMP = new CBitmap;
    BOOL ui_bitmap = pco_newBMP->CreateCompatibleBitmap (pco_screenDC, 
                                                         co_clientRect.right-co_clientRect.left,
                                                         co_clientRect.bottom-co_clientRect.top);
    CBitmap * pco_oldBMP = pco_MemDC_->SelectObject (pco_newBMP);
    if (!b_InitialUpdate_)
    {
        delete pco_oldBMP;
    }

    //
    // Set text alignment
    //
    pco_MemDC_->SetTextAlign (TA_LEFT | TA_BASELINE);

    //
    // Erase the background.
    //
    pco_MemDC_->FillSolidRect (&co_clientRect, GetSysColor (COLOR_WINDOW));

    RECT st_rect;
    GetClientRect (&st_rect);
    st_rect.top = st_DisplayWindow_.co_topLeft.cy;
    int i_vSpaceLeft = st_rect.bottom - st_rect.top;
    st_DisplayWindow_.i_linesShown = 0;
    bool b_enoughSpace = true;
    CSize co_origin (st_DisplayWindow_.co_topLeft.cx,
                     st_DisplayWindow_.co_topLeft.cy);
    if (co_origin.cy > 0)
    {
        //
        // Show bottom of preceding line
        //
        if ((st_DisplayWindow_.i_firstParagraph == 0) && 
            (st_DisplayWindow_.i_firstLine == 0))
        {
            ERROR_TRACE (_T("Bad vertical offset"));
        }

        int i_p = 0;
        int i_l = 0;
        bool b_r = b_GetPrevLine_ (st_DisplayWindow_.i_firstParagraph,
                                   st_DisplayWindow_.i_firstLine,
                                   i_p, 
                                   i_l);

        CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (i_p);
        pco_p->b_Render (pco_MemDC_);
        CT_Line * pco_line = pco_GetLine_ (i_p, i_l);
        co_origin.cy = st_DisplayWindow_.co_topLeft.cy
                        - pco_line->i_GetLineHeight() 
                        - pco_line->i_GetSpacing();
        pco_line->v_SetStartingFont (pco_MemDC_);
        pco_line->v_Show (this, pco_MemDC_, co_origin.cx, co_origin.cy);
        co_origin.cy += pco_line->i_GetLineHeight() + pco_line->i_GetSpacing();
    }

    int i_line = st_DisplayWindow_.i_firstLine;
    for (int i_p = st_DisplayWindow_.i_firstParagraph;
        (i_p < pco_doc->i_NParagraphs() && b_enoughSpace);
        ++i_p) 
    {
        CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (i_p);
        bool b_ = pco_p->b_Render (pco_MemDC_);
        if (!b_)
        {
            return false;
        }
        int i_at = i_line;
        for (; i_at < pco_p->i_NLines(); ++i_at)
        {
            CT_Line * pco_line = pco_GetLine_ (i_p, i_at);
            if ((i_p == st_DisplayWindow_.i_firstParagraph) && 
                (i_at == st_DisplayWindow_.i_firstLine))
            {
                pco_line->v_SetStartingFont (pco_MemDC_);
            }
            if (pco_line->i_GetLineHeight() > i_vSpaceLeft)
            {
                b_enoughSpace = false;
            }
            pco_line->v_Show (this, pco_MemDC_, co_origin.cx, co_origin.cy);
            if (!b_enoughSpace)
            {
                break;
            }
            ++st_DisplayWindow_.i_linesShown;
            co_origin.cy += pco_line->i_GetLineHeight() + pco_line->i_GetSpacing();
            i_vSpaceLeft -= pco_line->i_GetLineHeight() + pco_line->i_GetSpacing();                    
        }
        if ((i_p == pco_doc->i_NParagraphs() - 1) && (i_at == pco_p->i_NLines()))
        {
            st_DisplayWindow_.b_bottom = true;
        }
        else
        {
            i_line = 0;
        }

    }  // for (int i_par ...

    b_Rendered_ = true;

    if (ec_WholeScreenNoRepaint == eo_RefreshMode_)
    {
        return true;
    }

    if (b_InitialUpdate_)
    {
        pco_CaretPos_->v_Update (pco_GetParagraph (0), 0, 0);
        b_RepositionCaret_();
        b_InitialUpdate_ = false;
        i_LogPixelsX_ = pco_screenDC->GetDeviceCaps (LOGPIXELSX);
        i_LogPixelsY_ = pco_screenDC->GetDeviceCaps (LOGPIXELSY);
    }

    v_AdjustScrollbars_();

    return true;

}   // b_RenderScreen_()

void CKaiView::v_Print_ (CDC * pco_dc, CPrintInfo * pst_info)
{
    if (!pco_dc->IsPrinting())
    {
        ERROR_TRACE (_T("*** Bad print DC"));
        return;
    }

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    double d_scaleFactor = static_cast<double>(pco_dc->GetDeviceCaps (LOGPIXELSY)) / 
                           static_cast<double> (i_LogPixelsY_);
    d_scaleFactor *= 1.1;

    bool b_noMorePages = false;
    int i_pageHeight = pco_dc->GetDeviceCaps (PHYSICALHEIGHT) - 
                       pco_dc->GetDeviceCaps (LOGPIXELSY);

    pst_info->SetMaxPage (vec_PrintPages_.size());
    pst_info->m_pPD->m_pd.nToPage = vec_PrintPages_.size();

    if (pst_info->m_nCurPage > vec_PrintPages_.size())
    {
        return;
    }

    int i_page = pst_info->m_nCurPage - 1;
    int i_p = (vec_PrintPages_[i_page]).i_FirstParagraph;
    int i_l = (vec_PrintPages_[i_page]).i_FirstLine;
    int i_y = pco_dc->GetDeviceCaps (LOGPIXELSY)/2;
    for (int i_lines = 0; 
         i_lines < (vec_PrintPages_[i_page]).i_Lines; 
         ++i_lines)
    {
        CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (i_p);
        int i_x = (pco_dc->GetDeviceCaps (LOGPIXELSX) * pco_p->i_GetLeftIndent())/100;
        CT_Line * pco_l = pco_GetLine_ (i_p, i_l);
        int i_lHeight = pco_l->i_GetLineHeight() + pco_l->i_GetSpacing();
        i_lHeight = static_cast<int> (d_scaleFactor * i_lHeight);
        pco_l->v_Print (this, pco_dc, i_x, i_y);
        i_y += i_lHeight;

        if (i_lines < (vec_PrintPages_[i_page]).i_Lines - 1)
        {
            bool b_ = b_GetNextLine_ (i_p, i_l, i_p, i_l);
            if (!b_)
            {
                break;
            }
        }
    }

    if (pst_info->m_nCurPage == vec_PrintPages_.size() - 1)
    {
        pst_info->m_pPD->m_pd.nMaxPage = pst_info->m_nCurPage;
        pst_info->m_bContinuePrinting = false;
    }

}   //  void CKaiView::v_Print_ (CDC * pco_dc)

void CKaiView::v_ScrollToTop_()
{
    i_SavedTextX_ = -1;
    st_DisplayWindow_.i_firstParagraph = 0;
    st_DisplayWindow_.i_firstLine = 0;
    st_DisplayWindow_.i_linesShown = 0;
    st_DisplayWindow_.b_bottom = false;
    st_DisplayWindow_.co_topLeft.cy = 0;
    i_VScrollPos_ = 0;
    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

    pco_CaretPos_->v_Update (pco_GetParagraph (0), 0, 0);
    
    if (eo_SelectionMode_ == ec_NoSelection)
    {
        bool b_r = b_RepositionCaret_();
    }

}   //  v_ScrollToTop()

void CKaiView::v_ScrollToBottom_()
{
    i_SavedTextX_ = -1;
    i_VScrollPos_ = i_LinesInDocument_()-1;

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();
    int i_p = vpo_paragraphs.size()-1;
    CT_Paragraph * pco_p = vpo_paragraphs[i_p];
    int i_l = pco_p->i_NLines()-1;
    CT_Line * pco_caretLine = pco_p->pco_GetLine (i_l);
    pco_CaretPos_->v_Update (pco_GetParagraph (i_p), 
                             i_l, 
                             pco_caretLine->i_GetVisibleTextLength()-1);
    CT_Line * pco_line = pco_caretLine;
    CRect co_clientRect (0, 0, 0, 0);
    GetClientRect (&co_clientRect);
    int i_spaceLeft = co_clientRect.bottom - co_clientRect.top;
    i_spaceLeft -= pco_line->i_GetLineHeight();
    int i_linesOnScreen = 0;
    while ((i_spaceLeft > 0) && (i_l >= 0))
    {
        i_spaceLeft -= pco_line->i_GetSpacing();
        bool b_r = b_GetPrevLine_ (i_p, i_l, i_p, i_l);
        pco_p = vpo_paragraphs[i_p];
        pco_line = pco_p->pco_GetLine (i_l);
        i_spaceLeft -= pco_line->i_GetLineHeight();
        ++i_linesOnScreen;
        if (!b_r)
        {
            break;
        }
    }
    if (i_l < 0)
    {
        i_p = i_l = 0;
    }
    else
    {
        if (!(i_p == 0) && (i_l == 0))
        {
            bool b_r = b_GetNextLine_ (i_p, i_l, i_p, i_l);
        }
    }

    st_DisplayWindow_.i_firstParagraph = i_p;
    st_DisplayWindow_.i_firstLine = i_l;

    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        bool b_r = b_RepositionCaret_();
    }

}   //  v_ScrollToBottom_()

void CKaiView::v_LineUp_()
{
    if ((st_DisplayWindow_.i_firstParagraph == 0) && 
        (st_DisplayWindow_.i_firstLine == 0))
    {
        return;
    }

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();

    //
    // Get the line preceding the first fully displayed line
    //
    int i_prevPar = 0;
    int i_prevLine = 0;
    bool b_ = b_GetPrevLine_ (st_DisplayWindow_.i_firstParagraph,
                              st_DisplayWindow_.i_firstLine,
                              i_prevPar, 
                              i_prevLine);
    CT_Paragraph * pco_p = pco_GetParagraph (i_prevPar);

    CT_Line * pco_newTopLine = pco_p->pco_GetLine (i_prevLine);

    //
    // Find how much of the previous line is displayed already &
    // scroll the display window for the remainder of line height
    //
    int i_scrollHeight = pco_newTopLine->i_GetLineHeight() + 
                         pco_newTopLine->i_GetSpacing() - 
                         st_DisplayWindow_.co_topLeft.cy;


    CRect co_updateRect;
    GetClientRect (&co_updateRect);
    co_updateRect.right = i_GetDisplayWidthInPixels_();
    BOOL mfcb_ = pco_MemDC_->ScrollDC (0, 
                                       i_scrollHeight, 
                                       &co_updateRect, 
                                       &co_updateRect, 
                                       NULL, 
                                       NULL);
    ValidateRect (NULL);   // don't send WM_PAINT
    co_updateRect.bottom = pco_newTopLine->i_GetLineHeight() + 
                           pco_newTopLine->i_GetSpacing();
    st_DisplayWindow_.i_firstParagraph = i_prevPar;
    st_DisplayWindow_.i_firstLine = i_prevLine;
    CSize co_origin (st_DisplayWindow_.co_topLeft.cx,
                     st_DisplayWindow_.co_topLeft.cy = 0);
    co_origin.cx = st_DisplayWindow_.co_topLeft.cx;

    //
    // Erase the background and display the line
    //
    pco_MemDC_->FillSolidRect (&co_updateRect, GetSysColor (COLOR_WINDOW));
    pco_newTopLine->v_SetStartingFont (pco_MemDC_);
    pco_newTopLine->v_Show (this, pco_MemDC_, co_origin.cx, co_origin.cy);

    st_DisplayWindow_.b_bottom = false;

    b_ = b_CountVisibleLines_();

    v_RefreshScreen_();
    v_AdjustScrollbars_();

}   // void CKaiView::v_LineUp_()

//
//  Show the line following the last fully visible line
//
void CKaiView::v_LineDown_()
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    if (b_IsLastLine_ (st_DisplayWindow_.i_firstParagraph, 
                       st_DisplayWindow_.i_firstLine))
    {
        return;
    }

    //
    // Calculate compound height of fully displayed lines
    //
    bool b_nextLineExists = true;

    int i_lastParagraph = 0;
    int i_lastLine = 0;
    int i_p = st_DisplayWindow_.i_firstParagraph;
    int i_l = st_DisplayWindow_.i_firstLine;
    CT_Line * pco_bottomLine = pco_GetLine_ (i_p, i_l);
    int i_totalHeight = st_DisplayWindow_.co_topLeft.cy;

    for (int i_ = 0; i_ < st_DisplayWindow_.i_linesShown; ++i_)
    {
        i_totalHeight += pco_bottomLine->i_GetLineHeight() + 
                         pco_bottomLine->i_GetSpacing();
        if (b_IsLastLine_ (i_p, i_l))
        {
            b_nextLineExists = false;
        }
        else
        {
            bool b_ = b_GetNextLine_ (i_p, i_l, i_p, i_l);
            if (b_)
            {
                pco_bottomLine = pco_GetLine_ (i_p, i_l);
                i_lastParagraph = i_p;
                i_lastLine = i_l;
            }
            else
            {
                //
                // Error -- this is not supposed to happen
                //
                b_nextLineExists = false;
                break;
            }
        }
    }

    CRect co_updateRect;
    GetClientRect (&co_updateRect);
    co_updateRect.right = i_GetDisplayWidthInPixels_();

    if (!b_nextLineExists)
    {
        //
        // Last line - scroll by the height of the top line + spacing
        //
        int i_scrollHeight = st_DisplayWindow_.co_topLeft.cy;
        CT_Line * pco_l = pco_GetLine_ (st_DisplayWindow_.i_firstParagraph, 
                                        st_DisplayWindow_.i_firstLine);
        i_scrollHeight += pco_l->i_GetLineHeight() + pco_l->i_GetSpacing();
        BOOL mfcb_ = pco_MemDC_->ScrollDC (0, 
                                           -1*i_scrollHeight, 
                                           &co_updateRect, 
                                           &co_updateRect, 
                                           NULL, 
                                           NULL);
        co_updateRect.left = 0;
        co_updateRect.top = co_updateRect.bottom - i_scrollHeight;
        pco_MemDC_->FillSolidRect (&co_updateRect, GetSysColor (COLOR_WINDOW));
        v_RefreshScreen_();

        //
        // Assume 2nd line exists (or we wouldn't get here, see above)
        //
        bool b_ = b_GetNextLine_ (st_DisplayWindow_.i_firstParagraph, 
                                  st_DisplayWindow_.i_firstLine,
                                  st_DisplayWindow_.i_firstParagraph,
                                  st_DisplayWindow_.i_firstLine);
        if (!b_)
        {
            return;
        }
        st_DisplayWindow_.co_topLeft.cy = 0;
        --st_DisplayWindow_.i_linesShown;
        st_DisplayWindow_.b_bottom = true;

        b_ = b_CountVisibleLines_();

        return;

    }  //  if (!b_nextLineExists)

    //
    // Inner line - scroll to make next line fully visible
    //
    int i_remainingSpace = co_updateRect.bottom - co_updateRect.top - 
     i_totalHeight;     // -- how much of the next line is below disp. window

    if (i_remainingSpace < 0)
    {
        ERROR_TRACE (_T("Negative scroll offset"));
    }

    int i_scrollHeight = pco_bottomLine->i_GetLineHeight() + 
     pco_bottomLine->i_GetSpacing() - i_remainingSpace;

    BOOL mfcb_ = pco_MemDC_->ScrollDC (0, 
                                       -1*i_scrollHeight, 
                                       &co_updateRect, 
                                       &co_updateRect, 
                                       NULL, 
                                       NULL);
    ValidateRect (NULL);    // don't send WM_PAINT

    //
    // Erase the background.
    //
    co_updateRect.top = co_updateRect.bottom - pco_bottomLine->i_GetLineHeight()
     - pco_bottomLine->i_GetSpacing();
//   - pco_bottomLine->pco_GetParent()->i_GetSpacing();

    pco_MemDC_->FillSolidRect (&co_updateRect, GetSysColor (COLOR_WINDOW));

    co_updateRect.top = i_totalHeight - i_scrollHeight;
    pco_bottomLine->v_SetStartingFont (pco_MemDC_);
    pco_bottomLine->v_Show (this, 
                            pco_MemDC_, 
                            st_DisplayWindow_.co_topLeft.cx, 
                            co_updateRect.top);

    i_p = st_DisplayWindow_.i_firstParagraph;
    i_l = st_DisplayWindow_.i_firstLine;
    CT_Line * pco_topLine = pco_GetLine_ (i_p, i_l);
    int i_spaceOut = i_scrollHeight - st_DisplayWindow_.co_topLeft.cy;

    int i_remove = 0;
    for (; 
         (i_spaceOut > 0) && (i_remove < st_DisplayWindow_.i_linesShown); 
         ++i_remove)
    {
        i_spaceOut -= pco_topLine->i_GetLineHeight() + pco_topLine->i_GetSpacing();
        bool b_r = b_GetNextLine_ (i_p, i_l, i_p, i_l);
        if (!b_r)
        {
            break;
        }
        pco_topLine = pco_GetLine_ (i_p, i_l);
    }

    st_DisplayWindow_.i_firstParagraph = i_p;
    st_DisplayWindow_.i_firstLine = i_l;
    st_DisplayWindow_.i_linesShown -= i_remove - 1;
    st_DisplayWindow_.co_topLeft.cy = -1 * i_spaceOut;

    bool b_ = b_CountVisibleLines_();

    //
    // Have to repaint previous line as selection may be affected
    //
    int i_nextToLastParagraph = 0;
    int i_nextToLastLine = 0;
    bool b_r = b_GetPrevLine_ (i_lastParagraph, 
                               i_lastLine, 
                               i_nextToLastParagraph, 
                               i_nextToLastLine);
    if (!b_r)
    {
        return;
    }
    CT_Line * pco_nextToLastLine = pco_GetLine_ (i_nextToLastParagraph, 
                                                 i_nextToLastLine);
    int i_oldTop = co_updateRect.top;
    co_updateRect.top = co_updateRect.top - 
                         pco_nextToLastLine->i_GetLineHeight()- 
                         pco_nextToLastLine->i_GetSpacing();
    co_updateRect.bottom = i_oldTop;
    pco_MemDC_->FillSolidRect (&co_updateRect, GetSysColor (COLOR_WINDOW));
    pco_nextToLastLine->v_SetStartingFont (pco_MemDC_);
    pco_nextToLastLine->v_Show (this, 
                                pco_MemDC_, 
                                st_DisplayWindow_.co_topLeft.cx, 
                                co_updateRect.top);
    v_RefreshScreen_();

}   // void CKaiView::v_LineDown()

void CKaiView::v_PageDown_()
{
    if (st_DisplayWindow_.b_bottom)
    {
        return;
    }

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    int i_p = st_DisplayWindow_.i_firstParagraph;
    int i_l = st_DisplayWindow_.i_firstLine;

    for (int i_ = 0; i_ < st_DisplayWindow_.i_linesShown; ++i_)
    {
        bool b_r = b_GetNextLine_ (i_p, i_l, i_p, i_l);
        if (!b_r)
        {
            i_p = 0;
            i_l = 0; // we need some error trace here
            break;
        }
    }

    st_DisplayWindow_.i_firstParagraph = i_p;
    st_DisplayWindow_.i_firstLine = i_l;
    st_DisplayWindow_.co_topLeft.cy = 0;
    
}   // v_PageDown_()

void CKaiView::v_PageUp_()
{
    st_DisplayWindow_.b_bottom = false;

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *> vpo_paragraphs = pco_doc->vpo_GetParagraphs();

    int i_p = st_DisplayWindow_.i_firstParagraph;
    int i_l = st_DisplayWindow_.i_firstLine;

    bool b_r = b_GetPrevLine_ (i_p, i_l, i_p, i_l);
    if (!b_r)
    {
        return;
    }

    CT_Paragraph * pco_p = vpo_paragraphs[i_p];
    CT_Line * pco_l = pco_p->pco_GetLine (i_l);
    
    CRect co_clientRect (0, 0, 0, 0);
    GetClientRect (&co_clientRect);
    int i_spaceLeft = co_clientRect.bottom - co_clientRect.top;
    i_spaceLeft -= pco_l->i_GetLineHeight();
    bool b_top = false;
    while ((i_spaceLeft > 0) && (!b_top))
    {
        i_spaceLeft -= pco_l->i_GetSpacing();
        b_top = !b_GetPrevLine_ (i_p, i_l, i_p, i_l);
        if (!b_top)
        {
            pco_p = vpo_paragraphs[i_p];
            pco_l = pco_p->pco_GetLine (i_l);
            i_spaceLeft -= pco_l->i_GetLineHeight();
        }
    }
    if (b_top)
    {
        i_p = i_l = 0;
    }
    else
    {
        bool b_r = b_GetNextLine_ (i_p, i_l, i_p, i_l);
    }

    st_DisplayWindow_.i_firstParagraph = i_p;
    st_DisplayWindow_.i_firstLine = i_l;
    st_DisplayWindow_.co_topLeft.cy = 0;

}   // void CKaiView::v_PageUp_()

void CKaiView::v_RefreshScreen_()
{
    CDC * pco_screenDC = GetDC();

    CRect co_clientRect;
    GetClientRect (&co_clientRect);

    CRect co_textFrame;
    v_GetTextFrame_ (co_textFrame);

    if (co_clientRect.Width() > co_textFrame.Width())
    {
        CRect co_fillRect;
        co_fillRect.SubtractRect (&co_clientRect, &co_textFrame);
        pco_screenDC->FillSolidRect (&co_fillRect, 
                                     GetSysColor (COLOR_APPWORKSPACE));
    }

    //
    // Copy memory DC to the screen DC
    //
    BOOL B_ = pco_screenDC->BitBlt (0, 
                                    0,
                                    co_clientRect.right, 
                                    co_clientRect.bottom,
                                    pco_MemDC_,
                                    i_HScrollPos_, 
                                    0,
                                    SRCCOPY);
    if (!B_)
    {
        ERROR_TRACE (_T("BitBlt failed"));
    }

    ReleaseDC (pco_screenDC);

}   // CKaiView::v_RefreshScreen_()

void CKaiView::v_AdjustScrollbars_()
{
    CKaiDoc * pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);    
    CT_DocDescriptor * pco_docDescriptor = pco_doc->pco_GetDocDescriptor();

    int i_height = i_GetDocumentHeight_();
    CRect co_clientRect;
    GetClientRect (&co_clientRect);

    SCROLLINFO st_scrollInfo;
    GetScrollInfo (SB_HORZ, &st_scrollInfo, SIF_ALL);
    GetClientRect (&co_clientRect);
    st_scrollInfo.nPage = min (co_clientRect.Width(), 
                               i_GetDisplayWidthInPixels_());
    st_scrollInfo.nPos = i_HScrollPos_;
    st_scrollInfo.nMin = 0;
    st_scrollInfo.nMax = i_GetDisplayWidthInPixels_();
    SetScrollInfo (SB_HORZ, &st_scrollInfo, TRUE);

    GetScrollInfo (SB_VERT, &st_scrollInfo, SIF_ALL);
    st_scrollInfo.nPage = co_clientRect.Height();
    st_scrollInfo.nPos = i_GetDocHeightAboveClient_();
    st_scrollInfo.nMin = 0;
    st_scrollInfo.nMax = max (co_clientRect.Height(), 
                              i_height);
    st_scrollInfo.nTrackPos = 0;    // ignored
    SetScrollInfo (SB_VERT, &st_scrollInfo, TRUE);

}   //  v_AdjustScrollbars_()

void CKaiView::v_Track_ (int i_xPos)
{
    if ((i_xPos <= 0) || (abs (i_xPos - i_TrackerPos_) < 5))
    {
        return;
    }

    CRect co_clientRect;
    GetClientRect (&co_clientRect);

    CClientDC co_dc (this);
    if (i_TrackerPos_ > 0)
    {
        CRect co_textFrame;
        v_GetTextFrame_ (co_textFrame);
        if (i_TrackerPos_ < co_textFrame.Width())
        {
            BOOL B_ = co_dc.BitBlt (i_TrackerPos_,
                                    co_clientRect.top,
                                    1, 
                                    co_clientRect.bottom,
                                    pco_MemDC_,
                                    i_TrackerPos_,
                                    co_clientRect.top,
                                    SRCCOPY);
        }
        else
        {
            CPen co_p (PS_SOLID, 1, GetSysColor (COLOR_APPWORKSPACE));
            co_dc.SelectObject (&co_p);
            co_dc.MoveTo (i_TrackerPos_, co_clientRect.top);
            co_dc.LineTo (i_TrackerPos_, co_clientRect.bottom);
        }
    }
     
    CPen co_p (PS_DOT, 1, RGB (0,0,0));
    co_dc.SelectObject (&co_p);
    co_clientRect.right = i_xPos;
    co_dc.MoveTo (i_xPos, co_clientRect.top);
    co_dc.LineTo (i_xPos, co_clientRect.bottom);

    i_TrackerPos_ = i_xPos;

}   //  v_Track_ (...)
