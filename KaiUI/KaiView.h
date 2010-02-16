//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: MFC CView class interface.
//
//    $Id: KaiView.h,v 1.91 2008-11-18 18:43:19 kostya Exp $
//==============================================================================

#pragma once

class CT_Paragraph;
class CT_FontDescriptor;
class CT_Line;
class CT_FindDialogBase;
class CT_FindDialog;
class CT_ReplaceDialog;
class CT_Search;
class CT_UndoStack;
class CKaiDoc;

#include "KaiTextPos.h"
#include "KaiSearch.h"

using namespace KaiLibMFC;

struct ST_Highlight
{
    CT_TextPos * pco_Start;
    CT_TextPos * pco_End;
};

class CT_Highlights
{
protected:
    std::vector<ST_Highlight> vec_Highlights_;
    std::vector<ST_Highlight>::iterator it_Hl_;

public:
    CT_Highlights() {};

    ~CT_Highlights()
    {
        v_Clear();
    }

    void v_Clear()
    {
        it_Hl_ = vec_Highlights_.begin();
        for (; it_Hl_ != vec_Highlights_.end(); ++it_Hl_)
        {
            delete (*it_Hl_).pco_Start;
            delete (*it_Hl_).pco_End;
        }
        vec_Highlights_.clear();
    }

    void v_Add (const CT_TextPos& co_start, const CT_TextPos& co_end)
    {
        ST_Highlight st_hl;

        st_hl.pco_Start = new CT_TextPos (co_start);
        st_hl.pco_End = new CT_TextPos (co_end);
        vec_Highlights_.push_back (st_hl);
    }

    bool b_Empty()
    {
        return vec_Highlights_.empty();
    }

    bool b_Enum()    // starts/resets enumeration
    {
        if (vec_Highlights_.empty())
        {
            return false;
        }

        it_Hl_ = vec_Highlights_.begin();
        return true;
    }

    bool b_GetNext (CT_TextPos *& pco_start, CT_TextPos *& pco_end)
    {
        if (vec_Highlights_.end() == it_Hl_)
        {
            return false;
        }

        pco_start = (*it_Hl_).pco_Start;
        pco_end = (*it_Hl_).pco_End;

        ++it_Hl_;

        return true;
    }
};

class CKaiView : public CView
{
protected: // create from serialization only
    CKaiView();
    DECLARE_DYNCREATE(CKaiView)

    friend class CT_TextPos;
    friend class CT_Search;

    enum ET_RefreshMode
    {
        ec_NoUpdate                 = 100,
        ec_ScrollUp                 = 101,
        ec_ScrollDown               = 102,
        ec_WholeScreen              = 103,
        ec_InitialUpdate            = 104,
        ec_WholeScreenNoRepaint     = 105,

        ec_ActionUnknown            = 199

    } eo_RefreshMode_;

    enum ET_SelectionMode
    {
        ec_NoSelection      =    200,
        ec_StaticSelection  =    201,
        ec_DynamicSelection =    202

    } eo_SelectionMode_;

    ET_Direction eo_Direction;

    struct ST_DisplayWindow
    {
        int i_firstParagraph;
        int i_firstLine;
        int i_linesShown;    // up to & including last fully visible line
        bool b_bottom;

        CSize co_topLeft;

        void v_Reset (CT_TextPos& co_top)
        {
            i_firstParagraph = co_top.i_GetParagraph();
            i_firstLine = co_top.i_GetLine();
            b_bottom = false;
            co_topLeft = CSize (0, 0);
            i_linesShown = 0;
        }

    } st_DisplayWindow_;

    bool b_Rendered_;

    enum ET_HorizontalScreenOffset
    {
        ec_HorizontalScreenOffset_ = 0, // obsolete
        ec_MaxLineWidth_ = 10000,
        ec_MinLineWidth_ = 400,
        ec_PastEndOfLine = 10000        // caret in the rightmost pos
    };

    int i_SavedTextX_;

    int i_LogPixelsX_;         // screen resolution
    int i_LogPixelsY_;

    unsigned int ui_RequestedFont_;
    bool b_DiacrInput_;
    bool b_CursorArrow_;

    CDC * pco_MemDC_;         // memory device context -- needed to reduce flicker

    CT_TextPos * pco_CaretPos_, 
               * pco_SelectionStart_, 
               * pco_SelectionEnd_;

    CT_Highlights co_Highlights_;

    int i_VScrollPos_;
    int i_HScrollPos_;
    int i_HScrollStep_;

    bool b_LButtonDown_;
    bool b_CaretShown_;
    bool b_CaretAtLineEnd_;
    bool b_SelectToLineEnd_;
    bool b_InitialUpdate_;
//    bool b_DiacriticMode_;        // treat keyboard input as diacritics
    bool b_TrackRightBorder_;

    //
    // Text window resizing
    //
    int i_TrackerPos_;

    //
    // View options
    //
    bool b_ShowParagraphEnd_;

    //
    // Printing & print preview
    //
    int i_PageWidth_;
    bool b_Paginated_;
    struct ST_PRINT_PAGE
    {
        int i_FirstParagraph;
        int i_FirstLine;
        int i_Lines;
        ST_PRINT_PAGE (int i_fp, int i_fl, int i_l) : 
            i_FirstParagraph (i_fp), i_FirstLine (i_fl), i_Lines (i_l)
            {};
    };
    vector <ST_PRINT_PAGE> vec_PrintPages_;
    

    //
    // Search
    //
    struct ST_PAST_SEARCH
    {
        ET_SearchType eo_Type;
        STL_STRING str_Search;

    };
    struct Functor_LessThan
    {
        bool operator() (const ST_PAST_SEARCH& rst_lhs, const ST_PAST_SEARCH& rst_rhs) const
        {
            if (rst_lhs.str_Search < rst_rhs.str_Search)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    };

    typedef std::set<ST_PAST_SEARCH, Functor_LessThan> SET_PAST_SEARCHES;
    SET_PAST_SEARCHES set_PastSearches_;

    CT_FindDialog * pco_FindDlg_;
    CT_ReplaceDialog * pco_ReplaceDlg_;
    CT_Search * pco_Search_;
    int i_CurrentMatch_;
    bool b_Cycle_;

protected:
    void v_Init_();
    void v_Null_();
    CKaiDoc* GetDocument_();
    bool b_IsOverstrikeOn_();

    //
    //  KaiViewDraw.cpp:
    //
    bool b_RenderScreen_ (CDC *);
    void v_Print_ (CDC *, CPrintInfo *);
    void v_ScrollToTop_();
    void v_ScrollToBottom_();
    void v_LineUp_();
    void v_LineDown_();
    void v_PageUp_();
    void v_PageDown_();
    void v_RefreshScreen_();
    void v_AdjustScrollbars_();
    void v_Track_ (int i_xPos);

    //
    //  KaiViewKeyDn.cpp:
    //
    void v_Char_ (UINT ui_char);
    void v_CharUpdate();
    void v_Home_();
    void v_End_();
    void v_Prior_();
    void v_Next_();
    void v_Left_();
    void v_Right_();
    void v_Up_();
    void v_Down_();
    void v_VkDelete_();
    void v_VkBackSpace_();
    void v_SplitParagraph_ (CT_Paragraph * pco_paragraph, int i_at);

    //
    //  KaiViewSelect
    //
    bool b_SetSelection_ (CT_TextPos * pco_from, CT_TextPos * pco_to);
    void v_SetMouseSelection_ (CT_TextPos *);
    void v_CancelSelection_();
    bool b_InvertSelection_ (CT_TextPos& co_start, 
                              CT_TextPos& co_end);
    void v_ScrollAndSelect_ (ET_Direction);
    void v_SelectTextField_ (const CT_TextPos& co_pos);
    void v_CopySelection_();
    void v_DeleteSelection_ (bool b_repaint = true);
    void v_ReplaceSelection_ (const STL_STRING& str_replaceWith);

    //
    //  KaiViewHelpers.cpp
    //
    int i_LinesInDocument_();
    CT_Line * pco_GetLine_ (int i_paragraph, int i_line);
    bool b_GetLineFromYPos_ (int i_yPos, CT_Line *& pco_line);
    bool b_GetNextLine_ (int i_curParagraph, 
                         int i_curLine, 
                         int& i_nextParagraph, 
                         int& i_nextLine);
    bool b_GetPrevLine_ (int i_curParagraph,
                         int i_curLine,
                         int& i_prevParagraph, 
                         int& i_prevLine);
    bool b_ScreenToTextPos_ (CPoint co_point, CT_TextPos * pco_pos);
    bool b_TextPosToClientPos_ (CT_TextPos * pco_textPos, CSize& co_screenPos);
    int i_GetYPos_ (CT_TextPos&);
    int i_GetTotalHeight_();    // calculate compound height of all displayed lines
    int i_GetDocumentHeight_();
    void v_MakeVisible_ (CT_TextPos&);
    bool b_RepositionCaret_();
    int i_CalculateVScrollPos_ (CT_TextPos&);
    bool b_IsCaretPosVisible_();
    bool b_IsVisible_ (CT_TextPos&);
    bool b_CountVisibleLines_();
    bool b_IsLastLine_ (int i_paragraph, int i_line);
    bool b_IsEndOfDocument (CT_TextPos&);
    bool b_LineExists_ (int i_paragraph, int i_line);
    void v_UndoSingleEvent_ (CT_UndoStack * pco_us);
    void v_DeleteChar_ (bool b_backSpace);
    int i_GetDocHeightAboveClient_();
    void v_LineFromScrollPos_ (int& i_paragraph, int& i_line);
    void v_GetTextFrame_ (CRect& co_rect);
    CT_DocDescriptor * CKaiView::pco_GetDocDescriptor_();
    void v_Paste_();
    void v_ChangeParagraphProperties_();
    void v_Paginate_ (CDC * pco_dc);
    int i_GetDisplayWidthInPixels_();
    void v_ChangeTextSegmentFont_ (unsigned int ui_fontKey,
                                   CT_TextPos& co_start,
                                   CT_TextPos& co_onePastEnd);

    void v_HandleOnFontModify (ET_FontModifier eo_newStyle);
                            // called from OnBold(), OnItalics(), OnUnderline()

    void v_ModifyTextSegmentFont_ (ET_FontModifier eo_newStyle,
                                   CT_TextPos& co_start,
                                   CT_TextPos& co_onePastEnd);

    //
    // KaiViewFindReplace.cpp
    //
    void v_ShowFindReplaceDialog_ (ET_DialogType);
    void v_BooleanSearch_();
    void v_SingleWordSearch_();
    void v_FindFirstMatch_();
    void CKaiView::v_ShowMatch_();

    //
    // KaiViewTools.cpp
    //
    void v_Transcribe_();

//
// Public accessors
//
public:
    CT_Paragraph * const pco_GetParagraph (int i_paragraph);

    bool b_ShowParagraphEnd()
    {
        return b_ShowParagraphEnd_;
    }

    bool b_SelectionOn()
    {
        return !(eo_SelectionMode_ == ec_NoSelection); 
    }
    
    CT_TextPos& co_GetSelectionStart() const
    {
        return *pco_SelectionStart_;
    }
    
    CT_TextPos& co_GetSelectionEnd() const
    {
        return *pco_SelectionEnd_;
    }
    
    int CKaiView::i_GetLogPixelsX() const
    { 
        return i_LogPixelsX_; 
    };
    
    int CKaiView::i_GetLogPixelsY() const
    {
        return i_LogPixelsY_; 
    };

    bool b_EnumHighlights()
    {
        return co_Highlights_.b_Enum();
    }

    bool b_GetNextHighlight (CT_TextPos *& pco_start, CT_TextPos *& pco_end)
    {
        return co_Highlights_.b_GetNext (pco_start, pco_end);
    }

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CKaiView)
    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CKaiView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    //{{AFX_MSG(CKaiView)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnScrollToBottom();
    afx_msg void OnScrollToTop();
    afx_msg void OnSelectToTop();
    afx_msg void OnSelectToBottom();
    afx_msg void OnPgdn();
    afx_msg void OnPgup();
    afx_msg void OnLeft();
    afx_msg void OnHome();
    afx_msg void OnEnd();
    afx_msg void OnRight();
    afx_msg void OnDown();
    afx_msg void OnUp();
    afx_msg void OnBack();
    afx_msg void OnDelete();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnFormatFont();
    afx_msg void OnFormatParagraph();
    afx_msg void OnSelectRight();
    afx_msg void OnSelectLeft();
    afx_msg void OnSelectToEnd();
    afx_msg void OnSelectToHome();
    afx_msg void OnSelectUp();
    afx_msg void OnSelectDown();
//    afx_msg void OnMove(int x, int y);
//    afx_msg void OnMoving(UINT fwSide, LPRECT pRect);
//    afx_msg void OnMove(int x, int y);
    afx_msg void OnInsertDiacritic();
    afx_msg void OnUpdateEditCopy(CCmdUI *pCmdUI);
    afx_msg void OnUpdateEditCut(CCmdUI *pCmdUI);
    afx_msg void OnEditCopy();
    afx_msg void OnEditCut();
    afx_msg void OnUpdateEditPaste(CCmdUI *pCmdUI);
    afx_msg void OnEditPaste();
    afx_msg void OnEditUndo();
protected:
    virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
public:
    virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
    afx_msg void OnEditDelete();
    afx_msg void OnFilePrintPreview();
    afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
    afx_msg void OnUpdateEditDelete(CCmdUI *pCmdUI);
    afx_msg void OnEditSelectAll();
    afx_msg void OnMenuFindandreplaceFind();
    afx_msg void OnMenuFindandreplaceReplace();
    afx_msg LONG OnFind(WPARAM wParam, LPARAM lParam);
    afx_msg LONG OnReplace(WPARAM wParam, LPARAM lParam);

//    afx_msg void OnClose();
    afx_msg void OnDestroy();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
//    afx_msg UINT OnNcHitTest(CPoint point);
    afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnWordLeft();
    afx_msg void OnWordRight();
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    afx_msg void OnContextCut();
    afx_msg void OnContextCopy();
    afx_msg void OnContextPaste();
    afx_msg void OnContextParagraph();
    afx_msg void OnToBold();
    afx_msg void OnToItalic();
    afx_msg void OnToUnderline();
//    afx_msg void OnClose();
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
//    afx_msg void OnToolsTranscribe();
//    afx_msg void OnToolsAccentuate();
    afx_msg void OnToolsAsciiDecoder();
};

#ifndef _DEBUG  // debug version in KaiView.cpp
inline CKaiDoc* CKaiView::GetDocument_()
   { return (CKaiDoc*)m_pDocument; }

#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
