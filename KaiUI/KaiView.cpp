//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    implementation of MFC CView class.
//
//    $Id: KaiView.cpp,v 1.141 2008/11/19 07:39:28 kostya Exp $
//
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include <afxole.h>
#include "Kai.h"
#include "KaiDoc.h"
#include "KaiView.h"
#include "KaiParagraph.h"
#include "KaiLineDescriptor.h"
#include "KaiChildFrm.h"
#include "KaiMainFrm.h"
#include "KaiUndoStack.h"
#include "KaiFindDialog.h"
#include "KaiDocDescriptor.h"
#include "KaiSearch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKaiView

IMPLEMENT_DYNCREATE(CKaiView, CView)

static UINT WM_FIND = ::RegisterWindowMessage (_T("Kai_FindDialogMsg"));
static UINT WM_REPLACE = ::RegisterWindowMessage (_T("Kai_ReplaceDialogMsg"));

BEGIN_MESSAGE_MAP(CKaiView, CView)
    //{{AFX_MSG_MAP(CKaiView)
    ON_WM_SIZE()
    ON_WM_VSCROLL()
    ON_WM_ERASEBKGND()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_LBUTTONDOWN()
    ON_WM_CHAR()
    ON_COMMAND(ID_SCROLL_TO_BOTTOM, OnScrollToBottom)
    ON_COMMAND(ID_SCROLL_TO_TOP, OnScrollToTop)
    ON_COMMAND(ID_SELECT_TO_TOP, OnSelectToTop)
    ON_COMMAND(ID_SELECT_TO_BOTTOM, OnSelectToBottom)
    ON_COMMAND(ID_PGDN, OnPgdn)
    ON_COMMAND(ID_PGUP, OnPgup)
    ON_COMMAND(ID_LEFT, OnLeft)
    ON_COMMAND(ID_HOME, OnHome)
    ON_COMMAND(ID_END, OnEnd)
    ON_COMMAND(ID_RIGHT, OnRight)
    ON_COMMAND(ID_DOWN, OnDown)
    ON_COMMAND(ID_UP, OnUp)
    ON_COMMAND(ID_BACK, OnBack)
    ON_COMMAND(ID_DELETE, OnDelete)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
    ON_COMMAND(ID_FORMAT_FONT, OnFormatFont)
    ON_COMMAND(ID_FORMAT_PARAGRAPH, OnFormatParagraph)
    ON_COMMAND(ID_SELECT_RIGHT, OnSelectRight)
    ON_COMMAND(ID_SELECT_LEFT, OnSelectLeft)
    ON_COMMAND(ID_SELECT_TO_END, OnSelectToEnd)
    ON_COMMAND(ID_SELECT_TO_HOME, OnSelectToHome)
    ON_COMMAND(ID_SELECT_UP, OnSelectUp)
    ON_COMMAND(ID_SELECT_DOWN, OnSelectDown)
    ON_COMMAND(ID_INSERT_DIACRITIC_MENU, OnInsertDiacritic)
    ON_COMMAND(ID_INSERT_DIACRITIC_KEY, OnInsertDiacritic)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
    ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
    ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
    ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
    ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
    ON_COMMAND(ID_FINDANDREPLACE_FIND, OnMenuFindandreplaceFind)
    ON_COMMAND(ID_FINDANDREPLACE_REPLACE, OnMenuFindandreplaceReplace)
    ON_REGISTERED_MESSAGE(WM_FIND, OnFind)
    ON_REGISTERED_MESSAGE(WM_REPLACE, OnReplace)
    ON_WM_DESTROY()
    ON_WM_HSCROLL()
    ON_WM_NCHITTEST()
    ON_WM_SETCURSOR()
    ON_WM_MOUSEWHEEL()
    ON_COMMAND(ID_WORD_LEFT, OnWordLeft)
    ON_COMMAND(ID_WORD_RIGHT, OnWordRight)
    ON_WM_RBUTTONDOWN()
    ON_WM_CONTEXTMENU()
    ON_COMMAND(ID_CONTEXT_CUT, OnContextCut)
    ON_COMMAND(ID_CONTEXT_COPY, OnContextCopy)
    ON_COMMAND(ID_CONTEXT_PASTE, OnContextPaste)
    ON_COMMAND(ID_CONTEXT_PARAGRAPH, OnContextParagraph)
    ON_COMMAND(ID_TO_BOLD, OnToBold)
    ON_COMMAND(ID_TO_ITALIC, OnToItalic)
    ON_COMMAND(ID_TO_UNDERLINE, OnToUnderline)
//    ON_COMMAND(ID_TOOLS_TRANSCRIBE, OnToolsTranscribe)
//    ON_COMMAND(ID_TOOLS_ACCENTUATE, OnToolsAccentuate)
    ON_COMMAND(ID_TOOLS_ASCIIDECODER_VIEW, OnToolsAsciiDecoder)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKaiView construction/destruction

CKaiView::CKaiView()
{
    v_Init_();
}

CKaiView::~CKaiView()
{
//    v_Null_();
}

BOOL CKaiView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs
    HCURSOR h_cursor;
    h_cursor = LoadCursor (NULL, IDC_IBEAM);
  
    cs.lpszClass = AfxRegisterWndClass (CS_HREDRAW | CS_VREDRAW |
                                            CS_BYTEALIGNWINDOW | CS_DBLCLKS,
                                        h_cursor,
                                        NULL,  // no brush
                                        NULL);

    return CView::PreCreateWindow (cs);
}

void CKaiView::v_Init_()
{
    b_Rendered_ = false;

    st_DisplayWindow_.i_firstParagraph = 0;
    st_DisplayWindow_.i_firstLine = 0;
    st_DisplayWindow_.i_linesShown = 0;
    st_DisplayWindow_.b_bottom = false;
    st_DisplayWindow_.co_topLeft.cx = ec_HorizontalScreenOffset_;
    st_DisplayWindow_.co_topLeft.cy = 0;

    eo_RefreshMode_ = ec_InitialUpdate;
    eo_SelectionMode_ = ec_NoSelection;

    pco_CaretPos_ = new CT_TextPos;
    pco_SelectionStart_ = new CT_TextPos;
    pco_SelectionEnd_ = new CT_TextPos;

    i_SavedTextX_ = -1;
    ui_RequestedFont_ = 0;
    b_DiacrInput_ = false;
    b_CaretShown_ = false;

    i_HScrollPos_ = 0;
    i_HScrollStep_= 100;

    b_LButtonDown_ = false;

    pco_MemDC_ = NULL;

    b_TrackRightBorder_ = false;

    b_ShowParagraphEnd_ = true; 
    // ^^-- KLUDGE - need to set in menu/ini file/registry

    pco_FindDlg_ = NULL;
    pco_Search_ = NULL;
    i_CurrentMatch_ = -1;

    i_TrackerPos_ = -1;

}    // v_Init_()

void CKaiView::v_Null_()
{    
    st_DisplayWindow_.i_firstParagraph = -1;
    st_DisplayWindow_.i_firstLine = -1;
    st_DisplayWindow_.i_linesShown = -1;
    st_DisplayWindow_.b_bottom = false;
    st_DisplayWindow_.co_topLeft.cx = ec_HorizontalScreenOffset_;
    st_DisplayWindow_.co_topLeft.cy = -1;

    if (pco_CaretPos_)
    {
        delete pco_CaretPos_;
        pco_CaretPos_ = NULL;
    }
    if (pco_SelectionStart_)
    {
        delete pco_SelectionStart_;
        pco_SelectionStart_ = NULL;
    }
    if (pco_SelectionEnd_)
    {
        delete pco_SelectionEnd_;
        pco_SelectionEnd_ = NULL;
    }

    i_SavedTextX_ = -1;
    ui_RequestedFont_ = 0;

    i_HScrollPos_ = -1;
    i_HScrollStep_= -1;

    //
    // Create an empty bitmap and select into memory DC so we can
    // unselect the bitmap we created on the heap and delete it
    //
    CBitmap co_tmp;
    if (pco_MemDC_)
    {
        BOOL i_bitmap = co_tmp.CreateCompatibleBitmap (pco_MemDC_, 0,0); 
        CBitmap * pco_deleteMe = pco_MemDC_->SelectObject (&co_tmp);
        delete pco_deleteMe;
        delete pco_MemDC_;
        pco_MemDC_ = NULL;
    }

}    // v_Null()

/////////////////////////////////////////////////////////////////////////////
// CKaiView message handlers

void CKaiView::OnInitialUpdate() 
{
    CView::OnInitialUpdate();

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    std::vector<CT_Paragraph *>& vpo_paragraphs = pco_doc->vpo_GetParagraphs();
    if (vpo_paragraphs.empty())
    {
        CT_Paragraph * pco_p = NULL;
        pco_doc->v_CreateDeafultParagraph (pco_p);
        pco_p->v_SetPrevious (NULL);
        pco_p->v_SetNext (NULL);
        pco_p->v_SetLastParagraph (true);
        STL_STRING str_endMark = pco_p->str_GetEndOfParagraphChars();
        pco_p->append (str_endMark);
        pco_p->v_Invalidate();
        pco_doc->v_AddParagraph (pco_p, -1);
    }

    //
    // Create a compatible DC.
    //
    CDC * pco_screenDC = GetDC();
    pco_MemDC_ = new CDC;
    BOOL i_cdc = pco_MemDC_->CreateCompatibleDC (pco_screenDC);

    ((CMainFrame *)AfxGetApp()->GetMainWnd())->v_SetIdleMessage (_T("Rendering..."));
    int i_progress = 0;
    for (unsigned int ui_at = 0; (ui_at < vpo_paragraphs.size()); ++ui_at)
    {
        bool b_r = vpo_paragraphs[ui_at]->b_Render (pco_MemDC_);
        if (!b_r)
        {
            break;
        }
        
        int i_done = (int)(((double)ui_at/(double)vpo_paragraphs.size()) * 100);
        if (i_done > i_progress)
        {
            i_progress = i_done;
            AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, i_progress);
        }
    }
    AfxGetApp()->GetMainWnd()->SendMessage (KAIWM_PROGRESS, 0);

    ReleaseDC (pco_screenDC);

    b_InitialUpdate_ = true;

}    // void CKaiView::OnInitialUpdate() 

/////////////////////////////////////////////////////////////////////////////
// CKaiView drawing

void CKaiView::OnDraw (CDC * pco_screenDC)
{
    switch (eo_RefreshMode_)
    {
        case ec_InitialUpdate:
        case ec_WholeScreen:
        case ec_WholeScreenNoRepaint:
        {
            bool b_ = b_RenderScreen_ (pco_screenDC);
            eo_RefreshMode_ = ec_NoUpdate;
            break;
        }
        case ec_ScrollDown:
        {
            v_LineDown_();
            eo_RefreshMode_ = ec_NoUpdate;
            break;
        }
        case ec_ScrollUp:
        {
            v_LineUp_();
            eo_RefreshMode_ = ec_NoUpdate;
            break;
        }
        case ec_NoUpdate:
        {
//            v_RefreshScreen_();
            break;

        }    //  ec_NoAction

    }  // switch (eo_RefreshMode_)

    if (b_Rendered_)
    {
        v_AdjustScrollbars_();
    }
    v_RefreshScreen_();

}  //  OnDraw

BOOL CKaiView::OnEraseBkgnd(CDC* pDC) 
{
    // TODO: Add your message handler code here and/or call default
    
//    return CView::OnEraseBkgnd(pDC);

    return (LRESULT)1; // Say we handled it
}

void CKaiView::OnSize (UINT ui_type, int i_x, int i_y) 
{
    CView::OnSize (ui_type, i_x, i_y);
      
    // TODO: Add your message handler code here

    if (eo_RefreshMode_ != ec_InitialUpdate)
    {
        eo_RefreshMode_ = ec_WholeScreen;
    }
}

void CKaiView::OnVScroll (UINT ui_SBCode, 
                          UINT ui_pos, 
                          CScrollBar * pco_scrollBar)
{
    if (!b_Rendered_)
    {
        return;
    }

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        HideCaret();
        b_CaretShown_ = false;
    }
  
    switch (ui_SBCode)
    {
        case SB_TOP: 
        {
            v_ScrollToTop_();
            break;
        }

        case SB_BOTTOM:
        {
            v_ScrollToBottom_();
            break;
        }
    
        case SB_ENDSCROLL:        // End scroll
        {
//            eo_RefreshMode_ = ec_WholeScreen;
            break;
        }
    
        case SB_LINEDOWN:
        {            
            eo_RefreshMode_ = ec_ScrollDown;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();
            break;
        }

        case SB_LINEUP:
        {
            eo_RefreshMode_ = ec_ScrollUp;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();
            break;
        }

        case SB_PAGEDOWN: 
        {
            v_PageDown_();
            eo_RefreshMode_ = ec_WholeScreen;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();

            break;
        }

        case SB_PAGEUP:
        {
            v_PageUp_();
            eo_RefreshMode_ = ec_WholeScreen;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();

            break;
        }

        case SB_THUMBPOSITION:    // Scroll to the absolute position 
        {
            SCROLLINFO st_scrollInfo;
            GetScrollInfo (SB_VERT, &st_scrollInfo, SIF_ALL);
            if (st_scrollInfo.nTrackPos + (int)st_scrollInfo.nPage >= 
                                                    st_scrollInfo.nMax)
            {
                v_ScrollToBottom_();
                eo_RefreshMode_ = ec_WholeScreen;
                InvalidateRect (NULL, TRUE);
                st_DisplayWindow_.b_bottom = false;
                UpdateWindow();
                break;
            }
            v_LineFromScrollPos_ (st_DisplayWindow_.i_firstParagraph, 
                                  st_DisplayWindow_.i_firstLine);
            eo_RefreshMode_ = ec_WholeScreen;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();

            break;

        }    // case SB_THUMBPOSITION
                
        case SB_THUMBTRACK:       // Drag scroll box to specified position 
        {
//            UpdateWindow();
        }

    }  //  switch (ui_code)

    v_AdjustScrollbars_();

    if ((eo_SelectionMode_ == ec_NoSelection) && (b_IsCaretPosVisible_()))
    {
        bool b_ = b_RepositionCaret_();
    }
    
//    CView::OnVScroll (ui_SBCode, ui_pos, pco_scrollBar);
}

void CKaiView::OnHScroll (UINT ui_sbCode, 
                          UINT ui_pos, 
                          CScrollBar * pco_scrollBar)
{
    if (!b_Rendered_)
    {
        return;
    }

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        HideCaret();
        b_CaretShown_ = false;
    }

    SCROLLINFO st_scrollInfo;
    GetScrollInfo (SB_HORZ, &st_scrollInfo, SIF_ALL | SIF_DISABLENOSCROLL);

    switch (ui_sbCode)
    {
        case SB_LEFT: 
        {
            break;
        }

        case SB_RIGHT:
        {
            break;
        }
    
        case SB_ENDSCROLL:        // End scroll
        {
//            eo_RefreshMode_ = ec_WholeScreen;
            break;
        }
    
        case SB_LINELEFT:
        case SB_PAGELEFT: 
        {
            i_HScrollPos_ = max (i_HScrollPos_ - i_HScrollStep_, 0);
            eo_RefreshMode_ = ec_WholeScreen;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();
            break;
        }

        case SB_LINERIGHT:
        case SB_PAGERIGHT:
        {
            if (i_HScrollPos_ + static_cast<int>(st_scrollInfo.nPage) < 
                                        static_cast<int>(st_scrollInfo.nMax))
            {
                i_HScrollPos_ +=  i_HScrollStep_;
            }
            eo_RefreshMode_ = ec_WholeScreen;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();
            break;
        }

        case SB_THUMBPOSITION:    // Scroll to the absolute position 
        {
            i_HScrollPos_ = ui_pos;
            eo_RefreshMode_ = ec_WholeScreen;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();
            break;
        }
                
        case SB_THUMBTRACK:       // Drag scroll box to specified position 
        {
//            UpdateWindow();
        }

    }  //  switch (ui_code)

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        if (b_IsCaretPosVisible_())
        {
            bool b_ = b_RepositionCaret_();
        }
        else
        {
            if (b_CaretShown_)
            {
                HideCaret();
                b_CaretShown_ = false;
            }
        }
    }
    
//    CView::OnHScroll(nSBCode, nPos, pScrollBar);

}    //  OnHScroll (...)

//////////////////////////////////////    ///////////////////////////////////////
// CKaiView printing

BOOL CKaiView::OnPreparePrinting (CPrintInfo* pst_info)
{
    b_Paginated_ = false;
    
    // default preparation
    return CKaiView::DoPreparePrinting (pst_info);
}

void CKaiView::OnBeginPrinting(CDC * pco_dc/*pDC*/, CPrintInfo * pst_info/*pInfo*/)
{
    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    // TODO: add extra initialization before printing
    pco_doc->pco_GetDocDescriptor()->v_ResetPrintFonts();
    v_Paginate_ (pco_dc);

    pst_info->SetMaxPage (vec_PrintPages_.size());
    pst_info->m_pPD->m_pd.nToPage = vec_PrintPages_.size();
}

void CKaiView::OnPrint (CDC * pco_dc, CPrintInfo * pst_info)
{
    //
    // Don't call OnDraw
    //

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);

    pco_dc->SelectStockObject (SYSTEM_FONT);
    v_Print_ (pco_dc, pst_info);
    pco_dc->SelectStockObject (SYSTEM_FONT);
}

void CKaiView::OnFilePrintPreview()
{
//AfxMessageBox (_T("Print preview is temporarily disabled due to MSFT/MFC bug."));
//return;

    CChildFrame * pco_cf = (CChildFrame *)GetParent();
    pco_cf->v_SetPrintPreview (true);

    CView::OnFilePrintPreview();
}

void CKaiView::OnEndPrintPreview (CDC * pco_dc, 
                                  CPrintInfo* pInfo, 
                                  POINT point, 
                                  CPreviewView* pView)
{
    // TODO: Add your specialized code here and/or call the base class
    CChildFrame * pco_cf = (CChildFrame *)GetParent();
    pco_cf->v_SetPrintPreview (false);

    CView::OnEndPrintPreview (pco_dc, pInfo, point, pView);
}

void CKaiView::OnEndPrinting (CDC * pco_dc, CPrintInfo * pst_info)
{
    // TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CKaiView diagnostics

#ifdef _DEBUG
void CKaiView::AssertValid() const
{
    CView::AssertValid();
}

void CKaiView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CKaiDoc * CKaiView::GetDocument_() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKaiDoc)));
    return (CKaiDoc*)m_pDocument;
}
#endif //_DEBUG

//
// Virtual key processing
//
void CKaiView::OnLeft() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_Left_();    
}

void CKaiView::OnRight() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_Right_();    
}

void CKaiView::OnHome() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_Home_();    
}

void CKaiView::OnEnd() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_End_();    
}

void CKaiView::OnScrollToTop() 
{
    if (!b_Rendered_)
    {
        return;
    }
    BOOL mfcb_ = SendMessage (WM_VSCROLL, SB_TOP);
//    UpdateWindow();    
}

void CKaiView::OnScrollToBottom() 
{
    if (!b_Rendered_)
    {
        return;
    }
    BOOL mfcb_ = SendMessage (WM_VSCROLL, SB_BOTTOM);
//    UpdateWindow();    
}

void CKaiView::OnUp() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_Up_();    
}

void CKaiView::OnDown() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_Down_();    
}

void CKaiView::OnPgdn() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_Next_();
}

void CKaiView::OnPgup() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_Prior_();
}

void CKaiView::OnBack() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_VkBackSpace_();
}

void CKaiView::OnDelete() 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_VkDelete_();
}

void CKaiView::OnEditDelete()
{
    if (!b_Rendered_)
    {
        return;
    }
    v_VkDelete_();    
}

void CKaiView::OnChar(UINT ui_char, UINT nRepCnt, UINT nFlags) 
{
    if (!b_Rendered_)
    {
        return;
    }
    v_Char_ (ui_char);    
    CView::OnChar(ui_char, nRepCnt, nFlags);
}

void CKaiView::OnSetFocus (CWnd * pco_oldWnd)
{
    CView::OnSetFocus (pco_oldWnd);

    if (!b_Rendered_)
    {
        return;
    }

    UpdateWindow();

    CKaiDoc* pco_doc = GetDocument_();
    ASSERT_VALID (pco_doc);
    if (pco_doc->i_NParagraphs() < 1)
    {
        return;
    }
    if (st_DisplayWindow_.i_linesShown < 1)
    {
        return;
    }
    if ((eo_SelectionMode_ == ec_NoSelection) && b_IsCaretPosVisible_())
    {
        bool b_ = b_RepositionCaret_();
    }
}

void CKaiView::OnKillFocus (CWnd* pNewWnd) 
{
    CView::OnKillFocus(pNewWnd);
    
    // TODO: Add your message handler code here    
    BOOL mfcb_ = DestroyCaret(); // MSDN says this is required

}

void CKaiView::OnLButtonDown (UINT ui_flags, CPoint co_point) 
{
    b_LButtonDown_ = true;
    
    if (!b_Rendered_)
    {
        return;
    }

    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
    }

    if (eo_RefreshMode_ == ec_InitialUpdate)
    {
        CView::OnLButtonDown (ui_flags, co_point);
        return;
    }

    //
    // Clicked above first/below last fully displayed line?
    //
    if (co_point.y < st_DisplayWindow_.co_topLeft.cy)
    {
        v_LineUp_();
    }
    else
    {
        if (co_point.y > i_GetTotalHeight_())
        {
            if (!st_DisplayWindow_.b_bottom)
            {
                v_LineDown_();
            }
        }
    }
    bool b_result = b_ScreenToTextPos_ (co_point, pco_CaretPos_);
    b_result = b_RepositionCaret_();

    CView::OnLButtonDown (ui_flags, co_point);

}    // OnLButtonDown

void CKaiView::OnLButtonUp (UINT ui_flags, CPoint co_point) 
{
    b_LButtonDown_ = false;

    //
    // Resizing text window?
    //
    if (b_TrackRightBorder_)
    {
        b_TrackRightBorder_ = false;
        co_point.x = max (ec_MinLineWidth_, co_point.x);
        v_Track_ (co_point.x);
        int i_pixPerInch = pco_MemDC_->GetDeviceCaps (LOGPIXELSX);
        CT_DocDescriptor * pco_doc = pco_GetDocDescriptor_();
        pco_doc->v_SetMaxParagraphWidth ((i_TrackerPos_*100)/i_pixPerInch);
        i_TrackerPos_ = -1;
        eo_RefreshMode_ = ec_WholeScreen;
        for (int i_ = 0; i_ < pco_doc->i_NParagraphs(); ++i_)
        {
            CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (i_);
            pco_p->v_Invalidate();
            pco_p->b_Render (pco_MemDC_);
        }
        pco_CaretPos_->v_Invalidate (CT_TextPos::ec_ParagraphPos);
        st_DisplayWindow_.v_Reset (*pco_CaretPos_);
        InvalidateRect (NULL, TRUE);
        UpdateWindow();
        pco_CaretPos_->v_Invalidate (CT_TextPos::ec_ParagraphPos);
        bool b_ = b_RepositionCaret_();
    }

    if (!b_Rendered_)
    {
        return;
    }

    //
    // Selection in progress?
    //
    if (GetCapture())
    {
        BOOL mfcb_result = ReleaseCapture();
        if (mfcb_result == 0)
        {
            DWORD dw_error = GetLastError();
            STL_STRING str_errorMsg;
            bool b_ = B_FormatWindowsMessage (dw_error, str_errorMsg);
            str_errorMsg = _T("ReleaseCapture() failed: ") + str_errorMsg;
            ERROR_TRACE (str_errorMsg);
        }
    }

    CView::OnLButtonUp (ui_flags, co_point);

}  // OnLButtonUp

void CKaiView::OnLButtonDblClk (UINT ui_flags, CPoint co_point)
{
    // TODO: Add your message handler code here and/or call default
    if (!b_Rendered_)
    {
        return;
    }

    CRect co_textFrame;
    v_GetTextFrame_ (co_textFrame);

    if (!co_textFrame.PtInRect (co_point))
    {
        CView::OnLButtonDblClk (ui_flags, co_point);
        return;
    }

    CT_TextPos co_pos;
    bool b_ = b_ScreenToTextPos_ (co_point, &co_pos);
    if (!b_)
    {
        CView::OnLButtonDblClk (ui_flags, co_point);
        return;
    }

    CT_Paragraph * pco_p = co_pos.pco_GetParagraph();
    co_textFrame.left += pco_p->i_GetLeftIndent();
    co_textFrame.right -= pco_p->i_GetRightIndent();
    if (!co_textFrame.PtInRect (co_point))
    {
        CView::OnLButtonDblClk (ui_flags, co_point);
        return;
    }

    v_SelectTextField_ (co_pos);

    CView::OnLButtonDblClk (ui_flags, co_point);

    for (int i_msg = 0; i_msg < 10; ++i_msg)
    {
        MSG st_msg;
        if (GetMessage (&st_msg, NULL, 0, 0) == 0)
        {
            break;
        }
        
        if (st_msg.message != WM_LBUTTONUP)
        {
            DispatchMessage (&st_msg);
        }
        else
        {
            break;
        }
    }

}   //  OnLButtonDblClk (...)

void CKaiView::OnMouseMove (UINT ui_flags, CPoint co_point) 
{
    if ((ui_flags == MK_LBUTTON) && b_TrackRightBorder_)
    {
        if (co_point.x >= ec_MinLineWidth_)
        {
            v_Track_ (co_point.x);
        }
    }

    if (!b_Rendered_)
    {
        return;
    }

    if ((ui_flags == MK_LBUTTON) && !GetInputState() && !b_TrackRightBorder_)    
    {
        SetCapture();

        CT_TextPos co_selectTo;
        CRect co_rectClient;
        GetClientRect (&co_rectClient);
        if (co_point.y < co_rectClient.top)
        {
            //
            // Mouse is above the text: scroll up
            //
            bool b_ = b_ScreenToTextPos_ (co_point, &co_selectTo);
            if (!b_)
            {
                return;
            }
            co_selectTo.v_Update (pco_GetParagraph (st_DisplayWindow_.i_firstParagraph),
                                  st_DisplayWindow_.i_firstLine,
                                  CT_TextPos::ec_DoNotChange,
                                  CT_TextPos::ec_ParagraphPos);
            eo_SelectionMode_ = ec_DynamicSelection;
            v_SetMouseSelection_ (&co_selectTo);
            v_ScrollAndSelect_ (ec_DirectionUp);
        }
        else
        {
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
            if (co_point.y > co_rectClient.bottom)
            {
                //
                // Mouse below text: scroll down
                //
                int i_lineOffset = pco_bottomLine->i_GetVisibleTextLength();
                if (pco_bottomLine->b_IsLastInParagraph())
                {
                    --i_lineOffset; // assert >= 0?
                }
                co_selectTo.v_Update (pco_GetParagraph (i_lastParagraph), 
                                      i_lastLine, 
                                      i_lineOffset);
                eo_SelectionMode_ = ec_DynamicSelection;
                v_SetMouseSelection_ (&co_selectTo);
                v_ScrollAndSelect_ (ec_DirectionDown);
            }
            else
            {
                //
                // Mouse inside the client area - static selection
                //
                bool b_ = b_ScreenToTextPos_ (co_point, &co_selectTo);
                if (!b_)
                {
                    return;
                }
                CT_Paragraph * pco_p = pco_GetParagraph (co_selectTo.i_GetParagraph());
                if (co_selectTo.i_GetParagraphOffset() == pco_p->length()-1)
                {
                    ++co_selectTo;
                }
                v_SetMouseSelection_ (&co_selectTo);
            }
        }        
    }

    CView::OnMouseMove (ui_flags, co_point);

}    // OnMouseMove

void CKaiView::OnFormatFont()
{
    if (!b_Rendered_)
    {
        return;
    }

    CKaiDoc* pco_doc = GetDocument_();
    unsigned int ui_font = 0;

    bool b_ = true;
    if (eo_SelectionMode_ != ec_NoSelection)
    {
        ATLASSERT (ec_StaticSelection == eo_SelectionMode_);
        CT_TextPos co_selectionTop = (*pco_SelectionStart_ <= *pco_SelectionEnd_) 
            ? *pco_SelectionStart_ 
            : *pco_SelectionEnd_;
        b_ = co_selectionTop.pco_GetParagraph()->
                b_GetFontKey (co_selectionTop.i_GetParagraphOffset(), ui_font);
    }
    else
    {
        b_ = pco_CaretPos_->pco_GetParagraph()->
                b_GetFontKey (pco_CaretPos_->i_GetParagraphOffset(), ui_font);
    }
    ATLASSERT (b_);
    if (!b_)
    {
        return;
    }

    CT_FontDescriptor * pco_curFD = NULL;
    try
    {
        pco_curFD = pco_doc->pco_GetFontDescriptor (ui_font);
    }
    catch (CT_KaiException& co_ex)
    {
        co_ex.v_Report();
        return;
    }
    ATLASSERT (pco_curFD);

    CClientDC co_cdc (this);
    CFont * pco_font = pco_curFD->pco_GetWindowsFont();
    LOGFONT st_lf;
    pco_font->GetLogFont (&st_lf);
    CFontDialog co_dlg (&st_lf);
    INT_PTR pi_result = co_dlg.DoModal();
    if (pi_result == IDOK)
    {
        if (0 != ui_RequestedFont_)
        {
            //
            // Current font unused since last change
            //
            pco_doc->b_UnRegisterFont (ui_RequestedFont_);
        }

        CT_KaiString kstr_typeface = CT_KaiString (co_dlg.GetFaceName());
        LOGFONT st_newLf;
        co_dlg.GetCurrentFont (&st_newLf);
        ET_Charset eo_charset = static_cast<ET_Charset>(st_newLf.lfCharSet);
        unsigned int ui_pointSize = co_dlg.GetSize()/10;
        bool b_bold = (co_dlg.IsBold()) ? true : false;
        bool b_italic = (co_dlg.IsItalic()) ? true : false;
        bool b_underline = (co_dlg.IsUnderline()) ? true : false;
        bool b_strikeout = (co_dlg.IsStrikeOut()) ? true : false;

        CT_FontDescriptor co_fd (kstr_typeface, 
                                 eo_charset, 
                                 ui_pointSize, 
                                 b_bold, 
                                 b_italic, 
                                 b_underline, 
                                 b_strikeout);
                                                      
        bool b_result = pco_doc->b_RegisterFont (co_fd, ui_RequestedFont_);
        if (b_result)
        {
            CKaiDoc * pco_doc = GetDocument_();
            ASSERT_VALID (pco_doc);
            pco_doc->SetModifiedFlag();
        }

        if (eo_SelectionMode_ == ec_StaticSelection)
        {
            int i_savedSelStart = pco_SelectionStart_->i_GetVisibleParagraphOffset();
            int i_savedSelEnd = pco_SelectionEnd_->i_GetVisibleParagraphOffset();

            // Add event to undo stack
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

            CT_UndoStack * pco_us = GetDocument_()->pco_GetUndoStack();
            pco_us->v_SetEventType (ec_UndoChangeFont);
            pco_us->v_SetSelection (true);
            pco_us->v_SetParagraph (co_top.i_GetParagraph());
            pco_us->v_SetOffset (co_top.i_GetParagraphOffset());
            pco_us->v_SetStartVisibleOffset (co_top.i_GetVisibleParagraphOffset());
            pco_us->v_SetEndParagraph (co_bottom.i_GetParagraph());
            pco_us->v_SetEndVisibleOffset (co_bottom.i_GetVisibleParagraphOffset());
//            pco_us->v_SetCaretAtStart (co_end < co_start);
            
            int i_p = co_top.i_GetParagraph();
            for (; i_p <= co_bottom.i_GetParagraph(); ++i_p)
            {
                pco_us->v_PushParagraph (pco_GetParagraph (i_p));
            }

            pco_us->v_AddEvent();

            v_ChangeTextSegmentFont_ (ui_RequestedFont_, 
                                      *pco_SelectionStart_, 
                                      *pco_SelectionEnd_);

            pco_SelectionStart_->v_SetVisibleParagraphOffset (i_savedSelStart);
            pco_SelectionEnd_->v_SetVisibleParagraphOffset (i_savedSelEnd);
            
//            st_DisplayWindow_.v_Reset (*pco_CaretPos_);

            eo_RefreshMode_ = ec_WholeScreen;
            InvalidateRect (NULL, TRUE);
            UpdateWindow();

            ui_RequestedFont_ = 0;
        }
    }
}    // void CKaiView::OnFormatFont()

void CKaiView::OnFormatParagraph()
{
    v_ChangeParagraphProperties_();

}    // void CKaiView::OnFormatParagraph()

void CKaiView::OnInsertDiacritic()
{
    if (!b_Rendered_)
    {
        return;
    }

    if (eo_SelectionMode_ != ec_NoSelection)
    {
//        MessageBeep (MB_ICONHAND);
        MessageBeep (-1);
        return;
    }

    if (pco_CaretPos_->i_GetParagraphOffset() == 
            pco_CaretPos_->pco_GetParagraph()->length() - 1)
    {
        MessageBeep (-1);
        return;
    }

    b_DiacrInput_ = true;
}

void CKaiView::OnUpdateEditUndo(CCmdUI *pco_menu)
{
    if (!b_Rendered_)
    {
        return;
    }

    CT_UndoStack * pco_us = GetDocument_()->pco_GetUndoStack();
    if (pco_us->b_EventsAvailable())
    {
        pco_menu->Enable (TRUE);
    }
    else
    {
        pco_menu->Enable (FALSE);
    }
}

void CKaiView::OnUpdateEditCopy (CCmdUI * pco_menu)
{
    if (!b_Rendered_)
    {
        return;
    }

    if (eo_SelectionMode_ != ec_NoSelection)
    {
        pco_menu->Enable (TRUE);
    }
    else
    {
        pco_menu->Enable (FALSE);
    }
}

void CKaiView::OnUpdateEditCut (CCmdUI * pco_menu)
{
    if (!b_Rendered_)
    {
        return;
    }

    if (eo_SelectionMode_ != ec_NoSelection)
    {
        pco_menu->Enable (TRUE);
    }
    else
    {
        pco_menu->Enable (FALSE);
    }
}

void CKaiView::OnUpdateEditPaste (CCmdUI * pco_menu)
{
    if (!b_Rendered_)
    {
        return;
    }

    COleDataObject co_do;
    BOOL ui_ = co_do.AttachClipboard();
    if (!ui_)
    {
        ERROR_TRACE (_T("AttachClipboard failed"));
        return;
    }
    ui_ = co_do.IsDataAvailable (((CKaiApp *)AfxGetApp())->ui_GetClipboardFormat());
    if (ui_)
    {
        pco_menu->Enable (TRUE);
    }
    else
    {
#ifdef _UNICODE
        ui_ = co_do.IsDataAvailable (CF_UNICODETEXT);
#endif
        if (!ui_)
        {
            ui_ = co_do.IsDataAvailable (CF_TEXT);
        }
        if (!ui_)
        {
            pco_menu->Enable (FALSE);
        }
        else
        {
            pco_menu->Enable (TRUE);
        }
    }

}    //  OnUpdateEditPaste (...)

void CKaiView::OnUpdateEditDelete(CCmdUI * pco_menu)
{
    if (!b_Rendered_)
    {
        return;
    }

    if (eo_SelectionMode_ != ec_NoSelection)
    {
        pco_menu->Enable (TRUE);
    }
    else
    {
        pco_menu->Enable (FALSE);
    }
}

void CKaiView::OnEditCopy()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_CopySelection_();
}

void CKaiView::OnEditCut()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_CopySelection_();
    v_VkDelete_();    
}

void CKaiView::OnEditPaste()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_Paste_();

}    //  void CKaiView::OnEditPaste()

void CKaiView::OnEditUndo()
{
    if (!b_Rendered_)
    {
        return;
    }

    CT_UndoStack * pco_us = GetDocument_()->pco_GetUndoStack();
    if (!pco_us->b_EventsAvailable())
    {
        return;
    }

    pco_us->v_Block();

    CKaiDoc * pco_doc = GetDocument_();
    bool b_chained = false;
    bool b_selection = false;
    do
    {
        pco_us->v_PopLastEvent();
        b_chained = pco_us->b_IsLinkedToPrevious();
        b_selection = pco_us->b_Selection();
        v_UndoSingleEvent_ (pco_us);
        pco_us->v_RemoveLastEvent();

    }    while (b_chained);

    if (!pco_us->b_EventsAvailable())
    {
        pco_doc->v_ClearModified();
    }

    CT_TextPos co_anchor;
    if (b_selection)
    {
        if (*pco_SelectionStart_ < *pco_SelectionEnd_)
        {
            co_anchor = *pco_SelectionStart_;
        }
        else
        {
            co_anchor = *pco_SelectionEnd_;
        }
    }
    else
    {
        co_anchor = *pco_CaretPos_;
    }

    eo_RefreshMode_ = ec_WholeScreenNoRepaint;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

    if (!b_IsVisible_ (co_anchor))
    {
        v_MakeVisible_ (co_anchor);
    }

    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();

    if (b_selection)
    {
        HideCaret();
        b_CaretShown_ = false;
        eo_SelectionMode_ = ec_StaticSelection;
//        bool b_ = b_InvertSelection_ (*pco_SelectionStart_, *pco_SelectionEnd_);
//        v_RefreshScreen_();
    }
    else
    {
        eo_SelectionMode_ = ec_NoSelection;
        bool b_ = b_RepositionCaret_();
    }

    pco_us->v_UnBlock();
    
}    //  void CKaiView::OnEditUndo()

void CKaiView::OnContextMenu (CWnd * pco_wnd, CPoint co_point)
{
    if (!b_Rendered_)
    {
        return;
    }

    ScreenToClient (&co_point);
    bool b_result = b_ScreenToTextPos_ (co_point, pco_CaretPos_);
    b_result = b_RepositionCaret_();

    CMenu co_context;
    BOOL B_ = co_context.LoadMenu (IDR_CONTEXT);
    CMenu * pco_submenu = co_context.GetSubMenu (0);
    if (ec_NoSelection == eo_SelectionMode_)
    {
        pco_submenu->EnableMenuItem (ID_CONTEXT_COPY, MF_GRAYED);
        pco_submenu->EnableMenuItem (ID_CONTEXT_CUT, MF_GRAYED);
    }
    else
    {
        pco_submenu->EnableMenuItem (ID_CONTEXT_COPY, MF_ENABLED);
        pco_submenu->EnableMenuItem (ID_CONTEXT_CUT, MF_ENABLED);
    }

    COleDataObject co_do;
    BOOL ui_ = co_do.AttachClipboard();
    if (!ui_)
    {
        ERROR_TRACE (_T("AttachClipboard failed"));
        return;
    }
//    CT_AppDescriptor * pco_app = 
//            (static_cast <CKaiApp *>(AfxGetApp()))->pco_KaiApp;
    ui_ = co_do.IsDataAvailable (((CKaiApp *)AfxGetApp())->ui_GetClipboardFormat());
    if (ui_)
    {
        pco_submenu->EnableMenuItem (ID_CONTEXT_PASTE, MF_ENABLED);
    }
    else
    {
#ifdef _UNICODE
        ui_ = co_do.IsDataAvailable (CF_UNICODETEXT);
#endif
        if (!ui_)
        {
            ui_ = co_do.IsDataAvailable (CF_TEXT);
        }
        if (!ui_)
        {
            pco_submenu->EnableMenuItem (ID_CONTEXT_PASTE, MF_GRAYED);
        }
        else
        {
            pco_submenu->EnableMenuItem (ID_CONTEXT_PASTE, MF_ENABLED);
        }
    }

    B_ = pco_submenu->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                                      co_point.x + 40, 
                                      co_point.y + 170, 
                                      pco_wnd, 
                                      0);
}   //  OnContextMenu (...)

void CKaiView::OnContextCut()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_CopySelection_();
    v_VkDelete_();    
}

void CKaiView::OnContextCopy()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_CopySelection_();
}

void CKaiView::OnContextPaste()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_Paste_();
}

void CKaiView::OnContextParagraph()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_ChangeParagraphProperties_();
}

void CKaiView::OnPrepareDC (CDC* pco_dc, CPrintInfo * pst_info)
{
    // TODO: Add your specialized code here and/or call the base class
    CView::OnPrepareDC(pco_dc, pst_info);
}

void CKaiView::OnDestroy()
{
    CView::OnDestroy();

    // TODO: Add your message handler code here
    v_Null_();

    if (pco_FindDlg_)
    {
        delete pco_Search_;
        pco_FindDlg_->SendMessage (WM_DESTROY);
    }
}

LRESULT CKaiView::OnNcHitTest (CPoint co_point)
{
    CRect co_textFrame;
    v_GetTextFrame_ (co_textFrame);

    CRect co_clientRect;
    GetClientRect (&co_clientRect);

    ClientToScreen (co_textFrame);
    ClientToScreen (co_clientRect);

    UINT ui_pos = CView::OnNcHitTest (co_point);

    if ((co_textFrame.right < co_clientRect.right) && 
        (abs (co_textFrame.right - co_point.x) <= 5))
    {
        if (IDC_SIZEWE != MAKEINTRESOURCE (GetCursor()))
        {
            SetCursor ((AfxGetApp()->LoadStandardCursor (IDC_SIZEWE)));
            b_CursorArrow_ = false;
        }

        if (b_LButtonDown_)
        {
            b_TrackRightBorder_ = true;
        }

        return ui_pos;
    }

    if (co_clientRect.PtInRect (co_point) && co_textFrame.PtInRect (co_point))
    {
        if (IDC_IBEAM != MAKEINTRESOURCE (GetCursor()))
        {
            SetCursor ((AfxGetApp()->LoadStandardCursor (IDC_IBEAM)));
            b_CursorArrow_ = false;
        }
    }
    else
    {
        if (IDC_ARROW != MAKEINTRESOURCE (GetCursor()))
        {
            SetCursor ((AfxGetApp()->LoadStandardCursor (IDC_ARROW)));
            b_CursorArrow_ = true;
        }
    }

    return CView::OnNcHitTest (co_point);

    return ui_pos;

}   //  OnNcHitTest (...)

BOOL CKaiView::OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message)
{
    return TRUE;    // we handle it in OnNcHitTest (...), see above
}

BOOL CKaiView::OnMouseWheel (UINT ui_flags, short sh_delta, CPoint co_pt)
{
    if (!b_Rendered_)
    {
        return CView::OnMouseWheel (ui_flags, sh_delta, co_pt);
    }

    if (eo_SelectionMode_ == ec_NoSelection)
    {
        HideCaret();
        b_CaretShown_ = false;
    }

    int i_notches = abs (sh_delta/WHEEL_DELTA);
    if (sh_delta > 0)
    {
        for (int i_ = 0; i_ < i_notches; ++i_)
        {
            v_LineUp_();
        }
    }
    else
    {
        for (int i_ = 0; i_ < i_notches; ++i_)
        {
            v_LineDown_();
        }
    }

    if ((eo_SelectionMode_ == ec_NoSelection) && (b_IsCaretPosVisible_()))
    {
        if (!b_CaretShown_)
        {
            ShowCaret();
            b_CaretShown_ = true;
        }
    }

    eo_RefreshMode_ = ec_NoUpdate;

    return CView::OnMouseWheel (ui_flags, sh_delta, co_pt);

}   //  OnMouseWheel (...)

void CKaiView::OnWordLeft()
{
    if (!b_Rendered_)
    {
        return;
    }

    int i_l = pco_CaretPos_->i_GetLine();
    int i_p = pco_CaretPos_->i_GetParagraph();
    if (0 == pco_CaretPos_->i_GetLineOffset())
    {
        if ((0 == i_p) && (0 == i_l))
        {
            return;
        }
    }
    int i_pOffset = pco_CaretPos_->i_GetParagraphOffset();
    CT_Paragraph * pco_p = pco_CaretPos_->pco_GetParagraph();
    ST_Token st_t = pco_p->st_GetTokenFromOffset (i_pOffset);

    if (i_pOffset > st_t.i_Offset)
    {
        pco_CaretPos_->v_SetParagraphOffset (st_t.i_Offset);
        bool b_ = b_RepositionCaret_();
        return;
    }

    bool b_ = pco_p->b_GetPrevToken (st_t);
    while (!b_)
    {
        pco_p = pco_p->pco_GetPrevious();
        if (!pco_p)
        {
            return;
        }
        st_t = pco_p->st_GetToken (pco_p->i_NTokens()-1);
        b_ = pco_p->b_GetPrevToken (st_t);
    }

    while ((ec_TokenText != st_t.eo_TokenType) &&
           (ec_TokenPunctuation != st_t.eo_TokenType))
    {
        bool b_ = pco_p->b_GetPrevToken (st_t);
        if (!b_)
        {
            pco_p = pco_p->pco_GetPrevious();
            if (!pco_p)
            {
                return;
            }
            st_t = pco_p->st_GetToken (pco_p->i_NTokens()-1);
        }
    }
 
    pco_CaretPos_->v_SetParagraph (pco_p);
    pco_CaretPos_->v_SetParagraphOffset (st_t.i_Offset);

    b_ = b_RepositionCaret_();

}    //  OnWordLeft()

void CKaiView::OnWordRight()
{
    if (!b_Rendered_)
    {
        return;
    }

    if (b_IsEndOfDocument (*pco_CaretPos_))
    {
        return;
    }

    int i_pOffset = pco_CaretPos_->i_GetParagraphOffset();
    CT_Paragraph * pco_p = pco_CaretPos_->pco_GetParagraph();
    ST_Token st_t = pco_p->st_GetTokenFromOffset (i_pOffset);

    bool b_ = pco_p->b_GetNextToken (st_t);
    while (!b_)
    {
        pco_p = pco_p->pco_GetNext();
        if (!pco_p)
        {
            return;
        }
        st_t = pco_p->st_GetToken (0);
        bool b_ = pco_p->b_GetNextToken (st_t);
    }

    while ((ec_TokenText != st_t.eo_TokenType) &&
           (ec_TokenPunctuation != st_t.eo_TokenType))
    {
        bool b_ = pco_p->b_GetNextToken (st_t);
        if (!b_)
        {
            pco_p = pco_p->pco_GetNext();
            if (!pco_p)
            {
                return;
            }
            st_t = pco_p->st_GetToken (0);
        }
    }
 
    pco_CaretPos_->v_SetParagraph (pco_p);
    pco_CaretPos_->v_SetParagraphOffset (st_t.i_Offset);

    b_ = b_RepositionCaret_();

}    //  OnWordRight()

void CKaiView::OnToBold()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_HandleOnFontModify (ec_Bold);

}   //  OnToBold()

void CKaiView::OnToItalic()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_HandleOnFontModify (ec_Italic);

}   //  OnToItalic()

void CKaiView::OnToUnderline()
{
    if (!b_Rendered_)
    {
        return;
    }

    v_HandleOnFontModify (ec_Underline);

}   //  OnToUnderline()

#pragma managed

void CKaiView::OnToolsAsciiDecoder()
{
#pragma push_macro ("new")
#undef new
    AsciiDecoder::AsciiDecoderForm ^ co_decoder = gcnew AsciiDecoder::AsciiDecoderForm (true);
                                                                    // always embedded in 1.4

    co_decoder->StartPosition = System::Windows::Forms::FormStartPosition::Manual;

    CRect co_wndRect;
    GetWindowRect (&co_wndRect);
    co_decoder->SetBounds (co_wndRect.TopLeft().x + co_wndRect.Width()/4, 
                           co_wndRect.TopLeft().y + co_wndRect.Height()/3, 
                           co_decoder->Width, 
                           co_decoder->Height); 

    KaiLibCS::DotNetWindowHandle ^ co_handle = gcnew KaiLibCS::DotNetWindowHandle ((int)m_hWnd);
    co_decoder->Show (co_handle);

#pragma pop_macro ("new")

}

/*
#pragma managed

void CKaiView::OnToolsTranscribe()
{
#pragma push_macro ("new")
#undef new

    KaiLib::Transcriber ^ co_transcriber = gcnew KaiLib::Transcriber();

#pragma pop_macro ("new")

}

#pragma unmanaged
void CKaiView::OnToolsAccentuate()
{
    std::wstring str_language;

    CT_LanguageDlg co_languageDlg; 
    INT_PTR i_ret = co_languageDlg.DoModal();
    if (IDOK == i_ret)
    {
        str_language = co_languageDlg.str_GetLanguage();
    }

}
*/
