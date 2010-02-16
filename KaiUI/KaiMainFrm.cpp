//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    MFC CMainFrame implementation.
//
//    $Id: KaiMainFrm.cpp,v 1.16 2008/05/25 00:51:04 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
    ON_WM_MENUSELECT()
    //{{AFX_MSG_MAP(CMainFrame)
    ON_WM_CREATE()
    ON_COMMAND(ID_TOGGLE_INSERT, OnToggleInsert)
    ON_WM_CLOSE()
    ON_WM_SETCURSOR()
    //}}AFX_MSG_MAP
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_OVR, OnUpdateOverstrike)  
    ON_COMMAND(ID_TOOLS_ASCIIDECODER, OnToolsAsciiDecoder)
    ON_MESSAGE(KAIWM_PROGRESS,OnProgress)
END_MESSAGE_MAP()

static UINT arrui_indicators[] =
{
    ID_SEPARATOR,
    ID_INDICATOR_PROGRESS,
    ID_INDICATOR_CAPS,
    ID_INDICATOR_NUM,      
    ID_INDICATOR_SCRL,
    ID_INDICATOR_OVR,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // TODO: add member initialization code here
    b_Overstrike_ = false;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
        | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
        !m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
    {
        TRACE0("Failed to create toolbar\n");
        return -1;      // fail to create
    }

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(arrui_indicators,
          sizeof(arrui_indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    // TODO: Delete these three lines if you don't want the toolbar to
    //  be dockable
    m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_wndToolBar);

    LoadBarState(_T("ControlBars\\State"));    // added: load ctrl bar states

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if( !CMDIFrameWnd::PreCreateWindow(cs) )
        return FALSE;
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnClose() 
{
    WINDOWPLACEMENT wp;
    if (GetWindowPlacement (&wp))
    {
        if (IsZoomed ())
        {
            wp.showCmd = SW_SHOWMAXIMIZED;
        }
        SaveWindowPlacement (&wp);
    }

    SaveBarState (_T("ControlBars\\State"));

    CMDIFrameWnd::OnClose();
}

BOOL CMainFrame::LoadWindowPlacement(LPWINDOWPLACEMENT pwp)
{
    CString strBuffer = AfxGetApp ()->
     GetProfileString (_T("Settings"), _T("WindowPos"));

    if (strBuffer.IsEmpty ())
        return FALSE;

    int cRead = _stscanf_s (strBuffer, _T("%i:%i:%i:%i:%i:%i:%i:%i:%i:%i"),
            &pwp->flags, &pwp->showCmd,
            &pwp->ptMinPosition.x, &pwp->ptMinPosition.y,
            &pwp->ptMaxPosition.x, &pwp->ptMaxPosition.y,
            &pwp->rcNormalPosition.left, &pwp->rcNormalPosition.top,
            &pwp->rcNormalPosition.right, &pwp->rcNormalPosition.bottom);
    if (cRead != 10)
        return FALSE;

    return TRUE;

}

void CMainFrame::SaveWindowPlacement(LPWINDOWPLACEMENT pwp)
{
    // SaveWindowPlacement
    CString strBuffer;
    strBuffer.Format (_T("%i:%i:%i:%i:%i:%i:%i:%i:%i:%i"),
        pwp->flags, pwp->showCmd,
        pwp->ptMinPosition.x, pwp->ptMinPosition.y,
        pwp->ptMaxPosition.x, pwp->ptMaxPosition.y,
        pwp->rcNormalPosition.left, pwp->rcNormalPosition.top,
        pwp->rcNormalPosition.right, pwp->rcNormalPosition.bottom);

    AfxGetApp ()->WriteProfileString (_T("Settings"), 
                                      _T("WindowPos"), 
                                      strBuffer);
    // end of insert
}

void CMainFrame::OnUpdateOverstrike (CCmdUI* pCmdUI)
{
    if (b_Overstrike_)
    {
        pCmdUI->Enable (TRUE);
    }
    else
    {
        pCmdUI->Enable (FALSE);
    }
}

void CMainFrame::OnToggleInsert() 
{
    // TODO: Add your command handler code here
    b_Overstrike_ = !b_Overstrike_;  
}

bool CMainFrame::b_GetOverstrikeStatus()
{
   return b_Overstrike_;
}

#pragma managed
void CMainFrame::OnToolsAsciiDecoder()
{
#pragma push_macro ("new")
#undef new
    AsciiDecoder::AsciiDecoderForm ^ co_decoder = gcnew AsciiDecoder::AsciiDecoderForm (true);
                                            // in 1.4 it is always embedded

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
#pragma unmanaged

LRESULT CMainFrame::OnProgress(WPARAM wp, LPARAM lp)
{
    m_wndStatusBar.OnProgress(wp); // pass to prog/status bar
    return 0;
}
