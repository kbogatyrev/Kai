//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: MFC CDocument class implementation
//
//    $Id: KaiDoc.cpp,v 1.28 2008-02-20 20:13:22 kostya Exp $
//
//==============================================================================

#pragma warning (disable: 4786) // Isn't MFC a shared DLL?! 
#pragma warning (disable: 4275)    // MSVC issue

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiUndoStack.h"
#include "shlwapi.h"
#include "KaiDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CKaiDoc, CDocument)

BEGIN_MESSAGE_MAP(CKaiDoc, CDocument)
    //{{AFX_MSG_MAP(CKaiDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_FILE_SAVE, OnFileSave)
    ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
END_MESSAGE_MAP()

//
// Ctor
//
CKaiDoc::CKaiDoc()
{
    EnableAutomation();
    AfxOleLockApp();

    pco_Doc_ = new CT_DocDescriptor;

//    v_Null();
}

//
// Dtor
//
CKaiDoc::~CKaiDoc()
{
    AfxOleUnlockApp();
    v_Null_();
}

void CKaiDoc::v_Null_()
{
    delete pco_Doc_;
}

BOOL CKaiDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}

void CKaiDoc::Serialize (CArchive& ar)
{
    CFile * pco_file = ar.GetFile();

    ULONGLONG qwLength = pco_file->GetLength();
    if (qwLength > INT_MAX)
    {
        AfxMessageBox (_T("File too long."), MB_ICONEXCLAMATION);
        return;
    }

    bool b_result = true;
    if (ar.IsStoring())
    {
        b_result = pco_Doc_->b_Store (*pco_file);
        if (b_result)
        {
//            SetModifiedFlag(0);
            v_ClearModified();
            pco_Doc_->b_Imported_ = false;
        }
    }
    else
    {
        pco_Doc_->b_Unrenderable_ = false;

        unsigned int ui_version = 0;
        b_result = pco_Doc_->b_CheckDocFormat (*pco_file, ui_version);
        if (b_result)
        {
            b_result = pco_Doc_->b_Load (*pco_file);
        }
        else
        {
            b_result = pco_Doc_->b_ImportTextFile (*pco_file);
            if (b_result)
            {
//                SetModifiedFlag();
                v_SetModified();
            }
            else
            {
                pco_Doc_->b_Unrenderable_ = true;
            }
        }
        if (!b_result)
        {
            AfxMessageBox (_T("Unable to read this file."), MB_ICONEXCLAMATION);
        }
    }
}

#ifdef _DEBUG
void CKaiDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CKaiDoc::Dump (CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


BOOL CKaiDoc::OnOpenDocument (LPCTSTR cchr0_pathname) 
{
    if (!CDocument::OnOpenDocument (cchr0_pathname))
        return FALSE;

    if (pco_Doc_->b_Unrenderable_)
    {
        return FALSE;
    }

    // TODO: Add your specialized creation code here    
    return TRUE;
}

void CKaiDoc::v_CreateDeafultParagraph (CT_Paragraph *& pco_paragraph)
{
    int i_defaultLineWidth = 0;
    int i_defaultLeftIndent = 0;
    int i_defaultRightIndent = 0;
    int i_defaultSpacing = 0;
    ET_Alignment eo_defaultAlignment = ec_AlignmentFront;

    bool b_ = pco_Doc_->b_GetDefaultParStyle (i_defaultLineWidth,
                                              i_defaultLeftIndent,
                                              i_defaultRightIndent,
                                              eo_defaultAlignment);
    if (!b_)
    {
        STL_STRING str_ (_T("Warning: Unable to find default paragraph style"));
        ERROR_TRACE (str_);
        i_defaultLineWidth = 800;
        i_defaultSpacing = 10;
        eo_defaultAlignment = ec_Left;
    }

    pco_Doc_->v_SetMaxParagraphWidth (i_defaultLineWidth);

    STL_STRING str_typeface;
    int i_size = 0;
    ET_Charset eo_charset = ec_CharsetFront;
    b_ = pco_Doc_->b_GetDefaultFont (str_typeface, i_size, eo_charset);
    if (!b_)
    {
        ERROR_TRACE (_T("Warning: b_GetDefaultFont returned \"false\""));
        str_typeface = _T("Courier New");
        i_size = 10;
        eo_charset = ec_RussianCharset;
    }

    CT_FontDescriptor co_fd (CT_KaiString (str_typeface), eo_charset, i_size);
    pco_Doc_->v_CreateDefaultParagraph (i_defaultLeftIndent,
                                        i_defaultRightIndent,
                                        eo_defaultAlignment,
                                        STL_STRING(_T("")),
                                        co_fd,
                                        pco_paragraph);

}    //  v_CreateDeafultParagraph (...)

BOOL CKaiDoc::SaveModified()
{
    // TODO: Add your specialized code here and/or call the base class
    if (pco_Doc_->b_IsImported())
    {
        // Adapted from MFC code: doccore.cpp
	    CString cstr_prompt;
	    AfxFormatString1 (cstr_prompt, AFX_IDP_ASK_TO_SAVE, GetPathName());
	    switch (AfxMessageBox ( cstr_prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	    {
	        case IDCANCEL:
            {
		        return FALSE;       // don't continue
            }
	        case IDYES:
            {
                OnFileSaveAs();
		        break;
            }
	        case IDNO:
            {
		        // If not saving changes, revert the document
		        break;
            }

	        default:
            {
		        ATLASSERT(FALSE);
		        break;
            }
	    }

//        OnFileSaveAs();
        return TRUE;
    }

    if (!pco_GetUndoStack()->b_EventsAvailable())
    {
        return TRUE;
    }

    SetTitle (pco_Doc_->str_Title_.data());
    BOOL ui_ = CDocument::SaveModified();
    if (ui_)
    {
        SetModifiedFlag (FALSE);
    }
    else
    {
        SetTitle (CString (pco_Doc_->str_Title_.data()) + _T("*"));
    }

    return ui_;
}

void CKaiDoc::OnFileSave()
{
    // TODO: Add your command handler code here
    if (pco_Doc_->b_IsImported())
    {
        OnFileSaveAs();
        return;
    }

    CString cstr_path = GetPathName();
    if (!cstr_path.IsEmpty())
    {
        CString cstr_ext = PathFindExtension (cstr_path);
        SetPathName (CString (cstr_path.Left (cstr_path.GetLength() - 
                                              cstr_ext.GetLength())) + 
                              _T(".kai"));
    }
    else
    {
        SetTitle (pco_Doc_->str_Title_.data());
    }

    CDocument::OnFileSave();

    if (IsModified())
    {
        SetTitle (CString (pco_Doc_->str_Title_.data()) + _T("*"));
    }

}

void CKaiDoc::OnFileSaveAs()
{
    // TODO: Add your command handler code here
    CString cstr_path = GetPathName();
    if (!cstr_path.IsEmpty())
    {
        CString cstr_ext = PathFindExtension (cstr_path);
        cstr_path = CString (cstr_path.Left (cstr_path.GetLength() - 
                             cstr_ext.GetLength()) + 
                             _T(".kai"));
    }
    else
    {
        SetTitle (pco_Doc_->str_Title_.data());
    }

//    CDocument::OnFileSaveAs();

    bool b_continue = false;
    INT_PTR i_ret = 0;
    do
    {
        CFileDialog co_dlg (FALSE, // i.e. use Save As dlg template
                            _T("*.kai"), 
                            cstr_path, 
                            OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
                            _T("Kai Documents|*.kai|Unicode Texts|*.txt|All Files|*.*||"));

        i_ret = co_dlg.DoModal();

        if (IDOK == i_ret)
        {
            if (1 == co_dlg.m_ofn.nFilterIndex)     // *.kai (it's zero-based)
            {
                try
                {
                    CFile co_file (co_dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate);
                    bool b_result = pco_Doc_->b_Store (co_file);
                    if (b_result)
                    {
                        SetTitle (co_dlg.GetFileName());
                        v_ClearModified();
                        pco_Doc_->b_Imported_ = false;
                    }
                }
                catch (CFileException * pco_ex)
                {
                    STL_STRING str_ (_T("Unable to save document. "));
                    TCHAR psz_errData[1000] = { 0 };
                    pco_ex->GetErrorMessage (psz_errData, 1000);
                    str_ += STL_STRING (psz_errData);
                    str_ += _T(".");
                    ERROR_TRACE (str_);
                    AfxMessageBox (str_.c_str(), MB_ICONEXCLAMATION);
                }
            }
            else
            {
                CString cstr_text (_T("You chose to save the document as a Unicode text. "));
                cstr_text += _T("\nFormatting, linguistic data, and some diacritics may be lost. ");
                cstr_text += _T("\nTo continue, click OK. To keep all data, click Cancel and then ");
                cstr_text += _T("\nselect the \"Kai Documents\" option from the menu");
                int i_ret = AfxMessageBox (cstr_text, MB_ICONEXCLAMATION | MB_OKCANCEL);
                if (IDOK == i_ret)
                {
                    CFile co_file (co_dlg.GetPathName(), CFile::modeWrite | CFile::modeCreate);
                    bool b_result = pco_Doc_->b_StoreAsPlainText (co_file);
                    if (b_result)
                    {
                        SetTitle (co_dlg.GetFileName());
                        v_ClearModified();
                        pco_Doc_->b_Imported_ = false;
                    }
                }
                else
                {
                    if (IDCANCEL == i_ret)
                    {
                        b_continue = true;
                    }
                    else
                    {
                        ERROR_TRACE (_T("Unexpected return from AfxMessageBox"));
                        AfxMessageBox (_T("Internal error. Unable to save document."), 
                                       MB_ICONEXCLAMATION);
                    }
                }
            }

        }   //  if (IDOK == i_ret)
        else
        {
            if (IDCANCEL == i_ret)
            {
                if (IsModified())
                {
                    SetTitle (CString (pco_Doc_->str_Title_.data()) + _T("*"));
                }
            }
            else
            {
                STL_STRING str_ (_T("Unexpected return from CFileDialog::DoModal(). Return value = "));
                str_ += STR_IToStr (i_ret);
                str_ += _T(".");
                ERROR_TRACE (str_);
                AfxMessageBox (_T("Unexpected error. Document may not be saved correctly"), 
                               MB_ICONEXCLAMATION);
            }
        }

    }   while (b_continue);
    
}   //  OnFileSaveAs()
