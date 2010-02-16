//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: MFC child frame implementation
//
//    $Id: KaiChildFrm.cpp,v 1.10 2007-03-24 16:31:36 kostya Exp $
//
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiChildFrm.h"
#include "KaiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
    //{{AFX_MSG_MAP(CChildFrame)
    ON_WM_GETMINMAXINFO()
    ON_WM_SHOWWINDOW()
    ON_WM_SIZE()
    //}}AFX_MSG_MAP
    ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
    // TODO: add member initialization code here
    b_PrintPreview_ = false;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow (CREATESTRUCT& st_cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    if (!CMDIChildWnd::PreCreateWindow (st_cs))
        return FALSE;

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
    CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
    CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::OnGetMinMaxInfo (MINMAXINFO FAR* pso_minMaxInfo)
{
    CKaiView * pco_activeView = (CKaiView *)GetActiveView();
    if (pco_activeView)
    {
        CPoint co_minSize (300, 200);
        pso_minMaxInfo->ptMinTrackSize = co_minSize;
    }    
}

void CChildFrame::OnShowWindow (BOOL b_show, UINT ui_status) 
{
    CMDIChildWnd::OnShowWindow (b_show, ui_status);

    //    
    //  The following will create maximized children (instead of 
    //  default size); feel free to change if you don't like this
    
/*    WINDOWPLACEMENT st_wndpl;
    BOOL b_retn;
  
    b_retn = GetWindowPlacement (&st_wndpl);
    ASSERT (b_retn);

    st_wndpl.showCmd = SW_SHOWMAXIMIZED;

    b_retn = SetWindowPlacement (&st_wndpl);
    ASSERT (b_retn);
*/    
}

void CChildFrame::OnSize(UINT nType, int cx, int cy) 
{
    CMDIChildWnd::OnSize(nType, cx, cy);
    
    // TODO: Add your message handler code here    
}

void CChildFrame::OnClose()
{
    if (b_PrintPreview_)
    {
        CView * pco_pv = DYNAMIC_DOWNCAST (CView, GetActiveView());
        if (!pco_pv)
        {
            ATLASSERT(0);
            ERROR_TRACE (_T("Active view not print preview"));
        }

        pco_pv->PostMessage (WM_COMMAND, AFX_ID_PREVIEW_CLOSE); 

        return;
    }

    CMDIChildWnd::OnClose();
}
