//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    KAI MFC application.
//
//    $Id: Kai.cpp,v 1.26 2009-01-18 02:29:05 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiMainFrm.h"
#include "KaiChildFrm.h"
#include "KaiDoc.h"
#include "KaiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKaiApp

BEGIN_MESSAGE_MAP(CKaiApp, CWinApp)
    //{{AFX_MSG_MAP(CKaiApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
//    ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKaiApp construction

CKaiApp::CKaiApp()
{
    // TODO: add construction code here,

    ui_ClipboardFormat_ = 0;

    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CKaiApp object

CKaiApp theApp;

static const CLSID clsid =
{ 0xdbf687e5, 0x868e, 0x11d1, { 0xb9, 0x9d, 0x0, 0xa0, 0xc9, 0x76, 0x3a, 0xa } };

/////////////////////////////////////////////////////////////////////////////
// CKaiApp initialization

int CALLBACK i_FontEnumCallback (ENUMLOGFONTEX * pst_logFont,
                                 NEWTEXTMETRICEX * pst_physicalData,
                                 DWORD dw_type,
                                 LPARAM l_lParam);

BOOL CKaiApp::InitInstance()
{
    KaiLibMFC::B_GetVersionInfo (_T("Kai.exe"), st_Version_);

    //
    // Register clipboard format
    //
    ui_ClipboardFormat_ = RegisterClipboardFormat (_T("CF_KAI"));

    //
    // Write log message
    //
    std::wstring str_startMsg;
    V_CreateTimeStamp (str_startMsg);
    str_startMsg += _T("\t\t********************* INITIALIZING *********************\r\n");
    B_TraceOutput (str_startMsg);

    STL_STRING str_hkcuKey = _T("Ling Ling San Software");
//    STL_STRING str_major1, str_minor1, str_major2, str_minor2;
//    v_GetVersion (str_major1, str_minor1, str_major2, str_minor2);

//    str_hkcuKey += str_major1[0];
//    str_hkcuKey += _T(".");
//    str_hkcuKey += str_minor2[0];

    //
    // Goes to HKEY_CURRENT_USER
    //
    SetRegistryKey (str_hkcuKey.data());

    LoadStdProfileSettings (10);  // for now
                    // Load standard INI file options (including MRU)

    // Initialize OLE libraries
    if (!AfxOleInit())
    {
        AfxMessageBox (_T("Unable to initialize OLE libraries"));
        return FALSE;
    }

    AfxEnableControlContainer();

    // Register the application's document templates.  Document templates
    // serve as the connection between documents, frame windows and views.

    CMultiDocTemplate* pDocTemplate;
    pDocTemplate = new CMultiDocTemplate(
        IDR_KAITYPE,
        RUNTIME_CLASS(CKaiDoc),
        RUNTIME_CLASS(CChildFrame), // custom MDI child frame
        RUNTIME_CLASS(CKaiView));
    AddDocTemplate(pDocTemplate);

    // Connect the COleTemplateServer to the document template.
    //  The COleTemplateServer creates new documents on behalf
    //  of requesting OLE containers by using information
    //  specified in the document template.
    m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

    // Register all OLE server factories as running.  This enables the
    //  OLE libraries to create objects from other applications.
    COleTemplateServer::RegisterAll();
        // Note: MDI applications register all server objects without regard
        //  to the /Embedding or /Automation on the command line.


    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    m_pMainWnd = pMainFrame;

    // Enable drag/drop open
    m_pMainWnd->DragAcceptFiles();

    // Enable DDE Execute open
    EnableShellOpen();
    RegisterShellFileTypes(TRUE);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo; 
//    cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing; 
                        // ^- don't display an empty child window
    ParseCommandLine(cmdInfo); 

    // Check to see if launched as OLE server
    if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
    {
        // Application was run with /Embedding or /Automation.  Don't show the
        //  main window in this case.
        return TRUE;
    }

    if (cmdInfo.m_strFileName.IsEmpty())
    {
        cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing; 
    }

    // When a server application is launched stand-alone, it is a good idea
    //  to update the system registry in case it has been damaged.
    m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
    COleObjectFactory::UpdateRegistryAll();

    // Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;

    //
    // Stolen from somewhere...
    //
    // The main window has been initialized, so show and update it.
    {
        WINDOWPLACEMENT wp;
        if (pMainFrame->LoadWindowPlacement (&wp))
        {
            pMainFrame->SetWindowPlacement (&wp);
            pMainFrame->ShowWindow(wp.showCmd);
        }
        else
        {
            pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
        }
        pMainFrame->UpdateWindow();
    }

    //
    // Check diacritics font
    //
    STL_STRING str_diacrFont;
    bool b_ = B_GetConfigValue (STL_STRING (_T("Default Font.FontForDiacritics")), str_diacrFont);
    if ((!b_) || (0 == str_diacrFont.length()))
    {
        ERROR_TRACE (_T("Unable to retrieve typeface name for diacritics; assuming Actus"));
        str_diacrFont = _T("Actus");
    }

    LOGFONTW st_lf;
    ZeroMemory (&st_lf, sizeof (LOGFONTW));
    st_lf.lfCharSet = DEFAULT_CHARSET;
    st_lf.lfPitchAndFamily = 0;
    _tcscpy_s (st_lf.lfFaceName, LF_FACESIZE, str_diacrFont.c_str());

    bool b_diacrInstalled = false;
    CDC * pco_dc = pMainFrame->GetDC();
    EnumFontFamiliesEx (pco_dc->m_hDC, 
                        &st_lf, 
                        (FONTENUMPROC)i_FontEnumCallback, 
                        (LPARAM)&b_diacrInstalled, 
                        0);
    pMainFrame->ReleaseDC (pco_dc);

    if (!b_diacrInstalled)
    {
        STL_STRING str_msg (_T("Diacritics font "));
        str_msg += str_diacrFont;
        str_msg += _T(" was not found on this system.\n");
        str_msg += _T("Would you like to select another font?");
        int i_res = AfxMessageBox (str_msg.data(), MB_YESNO);
        if (IDYES == i_res)
        {
            CFontDialog co_fontDlg;
//            LOGFONT st_lf;
//            CFontDialog co_dlg (&st_lf);
            CFontDialog co_dlg;
            int i_res = co_dlg.DoModal();
            if (i_res == IDOK)
            {
                STL_STRING str_newFont (co_dlg.GetFaceName());
                bool b_ = B_SetConfigValue (_T("Default Font.FontForDiacritics"), str_newFont);
                b_diacrInstalled = b_;
            }
        }
//        else
//        {
//            AfxMessageBox (_T("Warning: no diacritics font selected"));
//        }
        if (!b_diacrInstalled)
        {
            AfxMessageBox (_T("Warning: no diacritics font selected"));
        }
    }

    //
    // Original MFC code:
    //
    // The main window has been initialized, so show and update it.
//    pMainFrame->ShowWindow(m_nCmdShow);
//    pMainFrame->UpdateWindow();

   //PRINTDLG pd;
   //pd.lStructSize=(DWORD)sizeof(PRINTDLG);
   //BOOL bRet=GetPrinterDeviceDefaults(&pd);
   //if(bRet)
   //{
   //   // protect memory handle with ::GlobalLock and ::GlobalUnlock
   //   DEVMODE FAR *pDevMode=(DEVMODE FAR *)::GlobalLock(m_hDevMode);
   //   // set orientation to landscape
   //   pDevMode->dmOrientation=DMORIENT_PORTRAIT;
   //   ::GlobalUnlock(m_hDevMode);
   //}

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();
    virtual BOOL OnInitDialog();

// Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
        // No message handlers
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    KaiLibMFC::ST_VersionInfo st_version;
    ((CKaiApp *)AfxGetApp())->v_GetVersionInfo (st_version);

    CStatic * pco_version = (CStatic *)GetDlgItem (IDC_STATIC_VERSION);
    std::wstring str_text (st_version.str_FileDescription);
    pco_version->SetWindowText (str_text.data());

    CStatic * pco_build = (CStatic *)GetDlgItem (IDC_STATIC_BUILD);
    str_text = _T("Build ");
    STL_STRING str_major1, str_minor1, str_major2, str_minor2;
    ((CKaiApp *)AfxGetApp())-> v_GetVersion (str_major1, str_minor1, str_major2, str_minor2);
    str_text += str_major1 + _T(".") + str_minor1 + _T(".") + str_major2 + _T(".") + str_minor2;
    str_text +=  + _T(" ") + st_version.str_Comments;
    pco_build->SetWindowText (str_text.data());

    CStatic * pco_copyrightNotice = (CStatic *)GetDlgItem (IDC_STATIC_COPYRIGHT);
    str_text = st_version.str_LegalCopyright;
    pco_copyrightNotice->SetWindowText (str_text.data());

    return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
        // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CKaiApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CKaiApp message handlers

int CKaiApp::ExitInstance() 
{
    std::wstring str_endMsg;
    V_CreateTimeStamp (str_endMsg);
    str_endMsg += _T("\t\t********************* TERMINATING *********************\r\n\r\n");
    B_TraceOutput (str_endMsg);

    return CWinApp::ExitInstance();
}

/*
void CKaiApp::OnFilePrintSetup()
{
    CPrintDialogEx co_pd (TRUE);
    if (GetPrinterDeviceDefaults (&co_pd.m_pdex))
    {
        LPDEVMODE pst_dev = co_pd.GetDevMode();
        GlobalUnlock (pst_dev);
        pst_dev->dmOrientation = DMORIENT_PORTRAIT;
        pst_dev->dmPaperSize = DMPAPER_LETTER;
    }
    DoPrintDialog (&co_pd); 
//    co_pd.DoModal();

}
*/

/////////////////////////////// Kai-specific stuff /////////////////////////////

void CKaiApp::v_GetVersion (unsigned int& ui_major1, 
                            unsigned int& ui_minor1,
                            unsigned int& ui_major2, 
                            unsigned int& ui_minor2)
{
    ui_major1 = st_Version_.st_Build.ui_major1;
    ui_minor1 = st_Version_.st_Build.ui_minor1;
    ui_major2 = st_Version_.st_Build.ui_major2;
    ui_minor2 = st_Version_.st_Build.ui_minor2;
}

void CKaiApp::v_GetVersion (std::wstring& str_major1, 
                            std::wstring& str_minor1,
                            std::wstring& str_major2, 
                            std::wstring& str_minor2)
{
    str_major1 = STR_UIToStr (st_Version_.st_Build.ui_major1);
    str_minor1 = STR_UIToStr (st_Version_.st_Build.ui_minor1);
    str_major2 = STR_UIToStr (st_Version_.st_Build.ui_major2);
    str_minor2 = STR_UIToStr (st_Version_.st_Build.ui_minor2);
 }

int CALLBACK i_FontEnumCallback (ENUMLOGFONTEX * pst_logFont,
                                 NEWTEXTMETRICEX * pst_physicalData,
                                 DWORD dw_type,
                                 LPARAM l_lParam)

{
    bool * pb_fontFound = (bool *)l_lParam;
    *pb_fontFound = false;

    if (dw_type & TRUETYPE_FONTTYPE)
    {
        STL_STRING str_name;
        bool b_ = B_GetConfigValue (STL_STRING (_T("Default Font.FontForDiacritics")), str_name);
        if (!b_)
        {
            str_name = _T("Actus");
        }

        STL_STRING str_nameFound (pst_logFont->elfLogFont.lfFaceName);
        if (str_name == str_nameFound)
        {
            *pb_fontFound = true;
        }
    }

    if (*pb_fontFound)
    {
        return 0;   // done
    }
    else
    {
        return 1;   // continue enumeration
    }

}   //  i_FontEnumCallback()

BOOL CKaiApp::OnIdle(LONG lCount)
{
    // TODO: Add your specialized code here and/or call the base class
    STL_STRING str_version (_T("Version "));
    str_version += STR_UIToStr (st_Version_.st_Build.ui_major1) + _T(".") + 
        STR_UIToStr (st_Version_.st_Build.ui_minor1) + _T(".") + 
        STR_UIToStr (st_Version_.st_Build.ui_major2) + _T(".") +
        STR_UIToStr (st_Version_.st_Build.ui_minor2) + _T(" ") +
        st_Version_.str_Comments;   // e.g. "alpha"

    ((CMainFrame *)m_pMainWnd)->v_SetIdleMessage (str_version.data());

    return CWinApp::OnIdle(lCount);
}
