// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__AFD7FFE9_6BDD_11D4_8CE3_C05553C10000__INCLUDED_)
#define AFX_MAINFRM_H__AFD7FFE9_6BDD_11D4_8CE3_C05553C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProgStatusBar.h"

class CMainFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

// Attributes
public:

protected:
    bool b_Overstrike_;

// Operations
public:
    bool b_GetOverstrikeStatus();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CMainFrame)
    public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

public:
    BOOL LoadWindowPlacement(LPWINDOWPLACEMENT pwp);
    void SaveWindowPlacement(LPWINDOWPLACEMENT pwp);

protected:  // control bar embedded members
//    CStatusBar  m_wndStatusBar;
    CProgStatusBar m_wndStatusBar; // progress control status bar
    CToolBar    m_wndToolBar;

// Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnToggleInsert();
    afx_msg void OnClose();
    //}}AFX_MSG
    afx_msg void OnUpdateOverstrike (CCmdUI *pCmdUI);       //KB
    afx_msg void OnToolsAsciiDecoder();
    afx_msg LRESULT OnProgress(WPARAM wp, LPARAM lp);
    DECLARE_MESSAGE_MAP()

public:

    void v_SetIdleMessage (const STL_STRING str_text)
    {
        m_wndStatusBar.SetWindowText (str_text.data());
    }
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__AFD7FFE9_6BDD_11D4_8CE3_C05553C10000__INCLUDED_)
