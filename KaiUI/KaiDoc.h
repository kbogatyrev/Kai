//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: MFC CDocument class interface.
//
//    $Id: KaiDoc.h,v 1.28 2007-12-30 05:18:50 kostya Exp $
//    
//==============================================================================

#if !defined(AFX_KAIDOC_H__AFD7FFED_6BDD_11D4_8CE3_C05553C10000__INCLUDED_)
#define AFX_KAIDOC_H__AFD7FFED_6BDD_11D4_8CE3_C05553C10000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "KaiDocDescriptor.h"

//
// Forward decl
//
class CT_Paragraph;
class CT_FontDescriptor;
class CT_UndoStack;

class CKaiDoc : public CDocument
{
protected: // create from serialization only
    CKaiDoc();
    DECLARE_DYNCREATE(CKaiDoc)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CKaiDoc)
    public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CKaiDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CT_DocDescriptor * pco_Doc_;
    void v_Null_();
    bool b_Init_();

public:
    void v_CreateDeafultParagraph (CT_Paragraph *& pco_paragraph);

    CT_DocDescriptor * pco_GetDocDescriptor() const
    {
        return pco_Doc_;
    }
    std::vector<CT_Paragraph *>& vpo_GetParagraphs()
    {
        return pco_Doc_->vpo_GetParagraphs();
    }

    void v_AddParagraph (CT_Paragraph * pco_paragraph, int i_insertAfter)
    {
        pco_Doc_->v_AddParagraph (pco_paragraph, i_insertAfter);
    }

    void v_RemoveParagraph (int i_at)
    {
        pco_Doc_->v_RemoveParagraph (i_at);
    }

    CT_Paragraph * pco_GetParagraph (int i_p) const
    {
        return pco_Doc_->pco_GetParagraph (i_p);
    }

    int i_NParagraphs()
    {
        return pco_Doc_->i_NParagraphs();
    }

    CT_FontDescriptor * pco_GetFontDescriptor (unsigned int ui_key)
    {
        return pco_Doc_->pco_GetFontDescriptor (ui_key);
    }

    bool b_RegisterFont (CT_FontDescriptor& rco_fd, unsigned int& ui_fontKey)
    {
        return pco_Doc_->b_RegisterFont (rco_fd, ui_fontKey);
    }

    bool b_UnRegisterFont (unsigned int ui_fontKey)
    {
        return pco_Doc_->b_UnRegisterFont (ui_fontKey);
    }

    bool b_ModifyFontDescriptor (UINT ui_old, 
                                 ET_FontModifier eo_changeTo,
                                 bool b_add,
                                 UINT& rui_new)
    {
        return pco_Doc_->b_ModifyFontDescriptor (ui_old,
                                                 eo_changeTo,
                                                 b_add,
                                                 rui_new);
    }

    void v_DeleteText (CT_TextPos& co_first, 
                       CT_TextPos& co_last, 
                       bool b_caretAtStart = false,         // needed for undoing
                       bool b_selection = true)             //     -- " --
    {
        pco_Doc_->v_DeleteText (co_first, 
                                co_last, 
                                b_caretAtStart,
                                b_selection);
    }

    CT_UndoStack * pco_GetUndoStack()
    {
        return pco_Doc_->pco_UndoStack_;
    }

    void v_SetModified()
    {
        if (pco_Doc_->str_Title_.empty())
        {
            pco_Doc_->str_Title_ = GetTitle();
        }
        SetTitle (CString (pco_Doc_->str_Title_.data()) + _T("*"));
        SetModifiedFlag();
    }

    void v_ClearModified()
    {
        if (pco_Doc_->str_Title_.empty())
        {
            pco_Doc_->str_Title_ = GetTitle();
        }
        SetTitle (CString (pco_Doc_->str_Title_.data()));
        SetModifiedFlag (0);
    }

// Generated message map functions
protected:
    //{{AFX_MSG(CKaiDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    virtual BOOL SaveModified();
public:
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KAIDOC_H__AFD7FFED_6BDD_11D4_8CE3_C05553C10000__INCLUDED_)
