//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2006
//    
//    Facility: Kai 1.2
//
//    Module description: implementation of a modeless search dialog.
//
//    $Id: KaiFindDialog.cpp,v 1.21 2008-11-19 07:38:40 kostya Exp $
//    
//==============================================================================

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiFindDialog.h"

static UINT WM_FIND = ::RegisterWindowMessage (_T("Kai_FindDialogMsg"));
static UINT WM_REPLACE = ::RegisterWindowMessage (_T("Kai_ReplaceDialogMsg"));

IMPLEMENT_DYNAMIC(CT_FindDialog, CDialog)

CT_FindDialog::CT_FindDialog (CWnd * pco_parent /*=NULL*/, 
                              ET_DialogType eo_type)
: CDialog (CT_FindDialog::IDD, pco_parent), 
  eo_FindOrReplace_ (eo_type)
{
    pco_Parent_ = pco_parent;
    pco_ReplaceCombo_ = NULL;
    pco_ReplaceLabel_ = NULL;
    b_IsTerminating_ = false;
    b_ContextChanged_ = false;
    pst_CurrentNode_ = NULL;
    i_ParenthDepth_ = 0;
    b_NotButtonClicked_ = false;
    b_ShowingReplaceDlg_ = false;
}

CT_FindDialog::~CT_FindDialog()
{
    if (pco_ReplaceCombo_)
    {
        delete pco_ReplaceCombo_;
    }

    if (pco_ReplaceLabel_)
    {
        delete pco_ReplaceLabel_;
    }

    if (!pst_CurrentNode_)
    {
        return;
    }
    while (pst_CurrentNode_->pst_Parent)
    {
        pst_CurrentNode_ = pst_CurrentNode_->pst_Parent;
    }
    v_ClearBoolSearchSubTree_ (pst_CurrentNode_);
}

//
//  Message handlers
//
BEGIN_MESSAGE_MAP(CT_FindDialog, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedFindNext)
    ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_WM_NCDESTROY()
    ON_CBN_EDITCHANGE(IDC_COMBO_FIND_WHAT, OnCbnEditchangeComboFindWhat)
    ON_CBN_EDITCHANGE(IDC_COMBO_REPLACE_WITH, OnCbnEditchangeComboReplaceWith)
    ON_BN_CLICKED(IDC_CHECK_MATCH_WHOLE_WORD, OnBnClickedCheckMatchWholeWord)
    ON_BN_CLICKED(IDC_CHECK_MATCH_CASE, OnBnClickedCheckMatchCase)
    ON_BN_CLICKED(IDC_CHECK_IGNORE_DIACRITICS, OnBnClickedCheckIgnoreDiacritics)
    ON_BN_CLICKED(IDC_CHECK_BOOLEAN_SEARCH, OnBnClickedCheckBooleanSearch)
    ON_BN_CLICKED(IDC_CHECK_REGEX, OnBnClickedCheckRegex)
    ON_EN_CHANGE(IDC_EDIT_ENTER_TERM, OnEnChangeEditEnterTerm)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_AND, OnBnClickedAND)
    ON_BN_CLICKED(IDC_BUTTON_OR, OnBnClickedOR)
    ON_BN_CLICKED(IDC_BUTTON_NOT, OnBnClickedNOT)
    ON_BN_CLICKED(IDC_BUTTON_LP, OnBnClickedLP)
    ON_BN_CLICKED(IDC_BUTTON_RP, OnBnClickedRP)
    ON_CBN_CLOSEUP(IDC_COMBO_FIND_WHAT, OnCbnCloseupComboFindWhat)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
    ON_BN_CLICKED(IDC_BUTTON_BOOLSEARCH, OnBnClickedButtonBoolsearch)
    ON_BN_CLICKED(ID_REPLACE, OnBnClickedReplace)
END_MESSAGE_MAP()


BOOL CT_FindDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();
   
    // TODO: Add extra initialization here
    CheckDlgButton (IDC_RADIO_DOWN, 1);

    CheckDlgButton (IDC_CHECK_IGNORE_DIACRITICS, 1);
    CButton * pco_b1 = (CButton *) GetDlgItem (IDC_CHECK_IGNORE_DIACRITICS);
    pco_b1->EnableWindow (FALSE);

    CheckDlgButton (IDC_CHECK_REGEX, 0);
    CButton * pco_b2 = (CButton *) GetDlgItem (IDC_CHECK_REGEX);
    pco_b2->EnableWindow (FALSE);

    GetWindowRect (&st_FullDlgRect_);
    RECT st_separator;
    (GetDlgItem (IDC_STATIC_SEPARATOR))->GetWindowRect (&st_separator);

    SetWindowPos (&wndTop, 
                  0, 
                  0, 
                  st_FullDlgRect_.right - st_FullDlgRect_.left, 
                  st_separator.bottom - st_FullDlgRect_.top,
                  SWP_NOMOVE);

    if (ec_DialogTypeReplace == eo_FindOrReplace_)
    {
        v_ShowReplaceDialog();
    }
    
    return TRUE;   // return TRUE unless you set the focus to a control
                   // EXCEPTION: OCX Property Pages should return FALSE
}

void CT_FindDialog::OnBnClickedFindNext()
{
    if (!pco_Parent_)
    {
        ERROR_TRACE (_T("No parent ptr"));
        return;
    }

//
// KLUDGE
//
    CWaitCursor co_wait;
//    pco_Parent_->PostMessage (WM_FIND);
    pco_Parent_->SendMessage (WM_FIND);

    UINT ui_boolean = IsDlgButtonChecked (IDC_CHECK_BOOLEAN_SEARCH);
    if (ui_boolean)
    {
        b_IsTerminating_ = true;
        DestroyWindow();
    }
//
// END KLUDGE
//
}

void CT_FindDialog::OnBnClickedCancel()
{
    b_IsTerminating_ = true;
    DestroyWindow();
}

//void CT_FindDialog::PostNcDestroy() 
//{    
//    CDialog::PostNcDestroy();
//    return;
//    HWND h_ = pco_Parent_->GetSafeHwnd();
//    if (b_IsTerminating_)
//    {
//        pco_Parent_->PostMessage (WM_FIND);
//    }
//    else
//    {
//        DestroyWindow();
//        CDialog::OnDestroy();
//        delete this;
//    }
//}

void CT_FindDialog::OnCbnEditchangeComboFindWhat()
{
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    CString str_findText;
    pco_edit->GetWindowText (str_findText);
    CButton * pco_editBtn = (CButton *)GetDlgItem (IDOK);
    pco_editBtn->EnableWindow (!str_findText.IsEmpty());
    pco_edit->SetSel (0, -1);
    if (b_ShowingReplaceDlg_)
    {
        CEdit * pco_replaceEdit = (CEdit *)GetDlgItem (IDC_COMBO_REPLACE_WITH);
        CString str_replaceText;
        pco_replaceEdit->GetWindowText (str_replaceText);
        CButton * pco_btnReplace = (CButton *)GetDlgItem (ID_REPLACE);
        pco_btnReplace->EnableWindow (!str_replaceText.IsEmpty() && 
                                      !str_findText.IsEmpty());
    }
    b_ContextChanged_ = true;
}

void CT_FindDialog::OnCbnEditchangeComboReplaceWith()
{
    CEdit * pco_replaceEdit = (CEdit *)GetDlgItem (IDC_COMBO_REPLACE_WITH);
    CString str_replaceText;
    pco_replaceEdit->GetWindowText (str_replaceText);
    CEdit * pco_findEdit = (CEdit *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    CString str_findText;
    pco_findEdit->GetWindowText (str_findText);
    CButton * pco_btn = (CButton *)GetDlgItem (ID_REPLACE);
    pco_btn->EnableWindow (!str_replaceText.IsEmpty() && !str_findText.IsEmpty());
    pco_replaceEdit->SetSel (0, -1);
    b_ContextChanged_ = true;
}

void CT_FindDialog::OnCbnCloseupComboFindWhat()
{
    CComboBox * pco_cb = (CComboBox *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    CButton * pco_btn = (CButton *)GetDlgItem (IDOK);
    CString str_text;
    int i_idx = pco_cb->GetCurSel();
    if (i_idx >= 0)
    {
        pco_cb->GetLBText (i_idx, str_text);
    }
    pco_btn->EnableWindow (!str_text.IsEmpty());
    pco_cb->SetEditSel (0, -1);

    b_ContextChanged_ = true;
}

void CT_FindDialog::OnBnClickedCheckMatchWholeWord()
{
    b_ContextChanged_ = true;
}

void CT_FindDialog::OnBnClickedCheckMatchCase()
{
    b_ContextChanged_ = true;
}

void CT_FindDialog::OnBnClickedCheckIgnoreDiacritics()
{
    b_ContextChanged_ = true;
}

void CT_FindDialog::OnBnClickedCheckBooleanSearch()
{
    b_ContextChanged_ = true;
    UINT ui_boolean = IsDlgButtonChecked (IDC_CHECK_BOOLEAN_SEARCH);
    if (ui_boolean)
    {
        SetWindowPos (&wndTop, 
                      0, 
                      0, 
                      st_FullDlgRect_.right - st_FullDlgRect_.left, 
                      st_FullDlgRect_.bottom - st_FullDlgRect_.top,
                      SWP_NOMOVE);

        CWnd * pco_wnd = NULL;
        pco_wnd = (CWnd *)GetDlgItem (IDC_STATIC_BOOL);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDC_STATIC_ENTER_TERM);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDC_EDIT_ENTER_TERM);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_AND);
        pco_wnd->EnableWindow (FALSE);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_OR);
        pco_wnd->EnableWindow (FALSE);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_NOT);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd->EnableWindow (FALSE);
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_LP);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd->EnableWindow (FALSE);
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_RP);
        pco_wnd->EnableWindow (FALSE);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_CLEAR);
        pco_wnd->EnableWindow (FALSE);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_START_OVER);
        pco_wnd->EnableWindow (FALSE);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_BOOLSEARCH);
        pco_wnd->EnableWindow (FALSE);
        pco_wnd->ShowWindow (SW_SHOW);
        pco_wnd = (CWnd *)GetDlgItem (IDOK);
        pco_wnd->SetWindowTextW (_T("Search"));

        CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
        pco_edit->EnableWindow (TRUE);
        pco_edit->SetReadOnly (FALSE);
        pco_edit->SetFocus();

        CheckDlgButton (IDC_RADIO_PROXIMITY_PARAGRAPHS, 1);

        CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_PROXIMITY_COMBO);
        pco_wnd->EnableWindow (FALSE);
        int i_ret = pco_combo->InitStorage (MAX_PROXIMITY_DISTANCE, 4);
        ASSERT (i_ret != CB_ERRSPACE);
        CString str_num;
        for (int i_ = 1; i_ <= MAX_PROXIMITY_DISTANCE; ++i_)
        {
            str_num.Format (_T("%d"), i_);
            pco_combo->AddString (str_num);
        }
        pco_combo->SetCurSel (0);

        i_ParenthDepth_ = 0;
    }
    else
    {
        RECT st_separator;
        (GetDlgItem (IDC_STATIC_SEPARATOR))->GetWindowRect (&st_separator);
        SetWindowPos (&wndTop, 
                      0, 
                      0, 
                      st_FullDlgRect_.right - st_FullDlgRect_.left, 
                      st_separator.bottom - st_FullDlgRect_.top,
                      SWP_NOMOVE);
    }

    CEdit * pco_combo = (CEdit *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    pco_combo->EnableWindow (!ui_boolean);
}

void CT_FindDialog::OnBnClickedCheckRegex()
{
    b_ContextChanged_ = true;
}

void CT_FindDialog::OnEnChangeEditEnterTerm()
{
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    CString str_text;
    pco_edit->GetWindowText (str_text);
    BOOL ui_enable = !(str_text.IsEmpty());

    CWnd * pco_wnd = NULL;
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_AND);
    pco_wnd->EnableWindow (!(str_text.IsEmpty()));
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_OR);
    pco_wnd->EnableWindow (!(str_text.IsEmpty()));
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_CLEAR);
    pco_wnd->EnableWindow (!(str_text.IsEmpty()));
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_BOOLSEARCH);
    if (!str_text.IsEmpty() && !i_ParenthDepth_)
    {
        pco_wnd->EnableWindow (TRUE);
    }
    else
    {
        pco_wnd->EnableWindow (FALSE);
    }
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_LP);
    pco_wnd->EnableWindow (str_text.IsEmpty());

    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_NOT);

    if (vec_Expression_.empty())
    {
        pco_wnd->EnableWindow (FALSE);
    }
    else
    {
        ST_Input& st_exp = vec_Expression_.back();
        if (st_exp.eo_Operator == ec_AND)
        {
            pco_wnd->EnableWindow (str_text.IsEmpty());
        }
        else
        {
            pco_wnd->EnableWindow (FALSE);
        }
    }

    if (i_ParenthDepth_ > 0)
    {
        pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_RP);
        pco_wnd->EnableWindow (!str_text.IsEmpty());
    }

    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_START_OVER);
    if (!str_text.IsEmpty())
    {
        pco_wnd->EnableWindow (TRUE);
    }
    else
    {
        pco_wnd->EnableWindow (FALSE);
    }

}    // OnEnChangeEditEnterTerm()

void CT_FindDialog::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here
    HWND h_ = pco_Parent_->GetSafeHwnd();
    if (b_IsTerminating_)
    {
        pco_Parent_->PostMessage (WM_FIND);
    }
    else
    {
        // This only happens when user closes view or app while 
        // the search dialog is still open
        delete this;
    }
}

void CT_FindDialog::OnBnClickedAND()
{
    v_OperatorClick_ (ec_AND);
    CButton * pco_btn = (CButton *)GetDlgItem (IDC_BUTTON_NOT);
    pco_btn->EnableWindow (TRUE);
}

void CT_FindDialog::OnBnClickedOR()
{
    v_OperatorClick_ (ec_OR);
}

void CT_FindDialog::v_OperatorClick_ (ET_BoolOp eo_op)
{
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    CString cstr_term;
    pco_edit->GetWindowText (cstr_term);

    if (!cstr_term.IsEmpty())
    {
        ST_Input st_term;
        st_term.eo_Type = ec_Term;
        st_term.kstr_Term = cstr_term;
        vec_Expression_.push_back (st_term);
    }

    ST_Input st_operator;
    st_operator.eo_Type = ec_Operator;
    st_operator.eo_Operator = eo_op;
    vec_Expression_.push_back (st_operator);

    pco_edit->SetWindowText (_T(""));
    pco_edit->SetFocus();

    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    CString cstr_query;
    pco_combo->GetWindowText (cstr_query);
    CString str_op = eo_op == ec_AND ? _T(" AND ") : _T(" OR ");
    cstr_query += cstr_term + str_op;
    pco_combo->SetWindowText (cstr_query);

    CWnd * pco_btn = (CWnd *)GetDlgItem (IDC_BUTTON_LP);
    pco_btn->EnableWindow (TRUE);
    pco_btn = (CWnd *)GetDlgItem (IDC_BUTTON_RP);
    pco_btn->EnableWindow (FALSE);
    pco_edit->SetReadOnly (FALSE);

}

void CT_FindDialog::OnBnClickedLP()
{
    ++i_ParenthDepth_;

    ST_Input st_parenth;
    st_parenth.eo_Type = ec_Parenth;
    st_parenth.eo_Parenth = ec_LeftParenth;
    vec_Expression_.push_back (st_parenth);

    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    CString cstr_query;
    pco_combo->GetWindowText (cstr_query);
    cstr_query += CString (_T("("));
    pco_combo->SetWindowText (cstr_query);
    CButton * pco_btn = (CButton *)GetDlgItem (IDC_BUTTON_NOT);
    pco_btn->EnableWindow (FALSE);

    pco_btn = (CButton *)GetDlgItem (IDC_BUTTON_BOOLSEARCH);
    pco_btn->EnableWindow (FALSE);

    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    pco_edit->SetFocus();

}

void CT_FindDialog::OnBnClickedRP()
{
    if (i_ParenthDepth_ < 1)
    {
        ERROR_TRACE (_T("Unbalanced right parenth."));
        return;
    }

    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    CString cstr_term;
    pco_edit->GetWindowText (cstr_term);

    if (!cstr_term.IsEmpty())
    {
        ST_Input st_term;
        st_term.eo_Type = ec_Term;
        st_term.kstr_Term = cstr_term;
        vec_Expression_.push_back (st_term);
    }

    ST_Input st_parenth;
    st_parenth.eo_Type = ec_Parenth;
    st_parenth.eo_Parenth = ec_RightParenth;
    vec_Expression_.push_back (st_parenth);
    --i_ParenthDepth_;

    pco_edit->SetWindowText (_T(""));
    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    CString cstr_query;
    pco_combo->GetWindowText (cstr_query);
    cstr_query += cstr_term + CString (_T(")"));
    pco_combo->SetWindowText (cstr_query);

    CButton * pco_bnRight = (CButton *)GetDlgItem (IDC_BUTTON_RP);
    if (i_ParenthDepth_ == 0)
    {
        pco_bnRight->EnableWindow (FALSE);
    }
    else
    {
        pco_bnRight->EnableWindow (TRUE);
    }

    CWnd * pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_LP);
    pco_wnd->EnableWindow (FALSE);

    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_AND);
    pco_wnd->EnableWindow (TRUE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_OR);
    pco_wnd->EnableWindow (TRUE);

    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_BOOLSEARCH);
    if (!i_ParenthDepth_)
    {
        pco_wnd->EnableWindow (TRUE);
    }
    else
    {
        pco_wnd->EnableWindow (FALSE);
    }

    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_NOT);
    pco_wnd->EnableWindow (FALSE);
    pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    pco_edit->SetReadOnly (TRUE);

}    //  OnBnClickedRP()

void CT_FindDialog::OnBnClickedNOT()
{
    ST_Input& st_exp = vec_Expression_.back();
    if (st_exp.eo_Operator != ec_AND)
    {
        ERROR_TRACE (_T("Can only negate AND."));
        return;
    }

    vec_Expression_.pop_back();

    st_exp.eo_Type = ec_Operator;
    st_exp.eo_Operator = ec_ANDNOT;
    vec_Expression_.push_back (st_exp);

    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    CString cstr_query;
    pco_combo->GetWindowText (cstr_query);
    cstr_query += CString (_T("NOT "));
    pco_combo->SetWindowText (cstr_query);

    CWnd * pco_btn = (CWnd *)GetDlgItem (IDC_BUTTON_LP);
    pco_btn->EnableWindow (TRUE);

    pco_btn = (CWnd *)GetDlgItem (IDC_BUTTON_NOT);
    pco_btn->EnableWindow (FALSE);

    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    pco_edit->SetFocus();

}    // void CT_FindDialog::OnBnClickedNOT()

void CT_FindDialog::OnBnClickedButtonBoolsearch()
{
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    CString cstr_term;
    pco_edit->GetWindowText (cstr_term);
    if (!cstr_term.IsEmpty())
    {
        ST_Input st_term;
        st_term.eo_Type = ec_Term;
        st_term.kstr_Term = cstr_term;
        vec_Expression_.push_back (st_term);
    }

    for (int i_ = i_ParenthDepth_; i_ > 0; --i_)
    {
        ST_Input st_parenth;
        st_parenth.eo_Type = ec_Parenth;
        st_parenth.eo_Parenth = ec_LeftParenth;
        vec_Expression_.push_back (st_parenth);
    }

    CComboBox * pco_combo = (CComboBox *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    CString cstr_query;
    pco_combo->GetWindowText (cstr_query);

    if (!cstr_term.IsEmpty())
    {
        cstr_query += cstr_term;
    }

    pco_edit->SetWindowText (_T(""));

    for (int i_ = i_ParenthDepth_; i_ > 0; --i_)
    {
        cstr_query += _T (")");
    }
    pco_combo->SetWindowText (cstr_query);

    CWnd * pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_BOOLSEARCH);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_STATIC_ENTER_TERM);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_AND);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_OR);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_NOT);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_LP);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_RP);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_CLEAR);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDC_BUTTON_BOOLSEARCH);
    pco_wnd->EnableWindow (FALSE);
    pco_wnd = (CWnd *)GetDlgItem (IDOK);
    pco_wnd->EnableWindow (TRUE);

    v_BuildSearchTree_();

}    //  OnBnClickedBoolsearch()

void CT_FindDialog::OnBnClickedButtonClear()
{
    CEdit * pco_edit = (CEdit *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    pco_edit->SetWindowText (_T(""));
    pco_edit = (CEdit *)GetDlgItem (IDC_EDIT_ENTER_TERM);
    pco_edit->SetWindowText (_T(""));

    if (!pst_CurrentNode_)
    {
        return;
    }
    while (pst_CurrentNode_->pst_Parent)
    {
        pst_CurrentNode_ = pst_CurrentNode_->pst_Parent;
    }
    v_ClearBoolSearchSubTree_ (pst_CurrentNode_);
}

void CT_FindDialog::OnBnClickedReplace()
{
    if (!b_ShowingReplaceDlg_)
    {
        v_ShowReplaceDialog();
        b_ShowingReplaceDlg_ = true;
        b_ContextChanged_ = true;
        eo_FindOrReplace_ = ec_DialogTypeReplace;
    }
    else
    {
        pco_Parent_->PostMessage (WM_REPLACE);
    }

}    //  OnBnClickedReplace()

//
// Boolean search tree
//
void CT_FindDialog::v_BuildSearchTree_()
{
    if (vec_Expression_.empty())
    {
        return;
    }

    for (unsigned int ui_ = 0; ui_ < vec_Expression_.size(); ++ui_)
    {
        ST_Input st_input = vec_Expression_[ui_];
        switch (st_input.eo_Type)
        {
            case ec_Term:
            {
                v_AddTerm_ (st_input.kstr_Term);
                break;
            }
            case ec_Operator:
            {
                v_AddOperator_ (st_input.eo_Operator);
                break;
            }
            case ec_Parenth:
            {
                if (st_input.eo_Parenth == ec_LeftParenth)
                {
                    v_AddLeftParenth_();
                }
                else
                {
                    v_AddRightParenth_();
                }

                break;
            }
            default:
            {
                ERROR_TRACE (_T("Unknown input item."));
            }
        }
    }

}    // v_BuildSearchTree_()

void CT_FindDialog::v_AddTerm_ (CT_KaiString kstr_term)
{
    if (!pst_CurrentNode_)
    {
        pst_CurrentNode_ = new ST_BoolTreeNode();
    }

    ET_NodeState eo_state = eo_GetNodeState (pst_CurrentNode_);
    switch (eo_state)
    {
        case ec_BothVacant:
        {
            pst_CurrentNode_->st_LeftTerm.kstr_Term = kstr_term;
            break;
        }
        case ec_RightVacant:
        {
            pst_CurrentNode_->st_RightTerm.kstr_Term = kstr_term;
            break;
        }
        case ec_NoneVacant:
        {
            while (eo_state != ec_RightVacant)
            {
                pst_CurrentNode_ = pst_CurrentNode_->pst_Parent;
                if (!pst_CurrentNode_)
                {
                    ERROR_TRACE (_T("Empty node."));
                    return;
                }
            }
            pst_CurrentNode_->st_RightTerm.kstr_Term = kstr_term;
            break;
        }
        case ec_NodeStateUnknown:
        {
            return;
        }
        default:
        {
            ERROR_TRACE (_T("Unexpected return from eo_GetNodeSate()."));
            return;
        }
    }    // switch

}    //    v_AddTerm_ (...)

void CT_FindDialog::v_AddLeftParenth_()
{
    vec_Fragments_.push_back (ST_Fragment (pst_CurrentNode_));
    ST_BoolTreeNode * pst_newNode = new ST_BoolTreeNode;
    pst_CurrentNode_ = pst_newNode;

}    //  v_AddLeftParenth_()

void CT_FindDialog::v_AddRightParenth_()
{
    if (!pst_CurrentNode_)
    {
        ERROR_TRACE (_T("No current node."));
        return;
    }

    ST_Fragment st_f = vec_Fragments_.back();
    pst_CurrentNode_->pst_Parent = st_f.pst_Node;
    if (st_f.pst_Node)
    {
        ET_NodeState eo_state = eo_GetNodeState (st_f.pst_Node);
        if (eo_state == ec_BothVacant)
        {
            st_f.pst_Node->pst_Left = pst_CurrentNode_;
        }
        else
        {
            if (eo_state == ec_RightVacant)
            {
                st_f.pst_Node->pst_Right = pst_CurrentNode_;
            }
            else
            {
                ERROR_TRACE (_T("Unexpected return from eo_GetNodeSate()."));
            }
        }
    }

    if (st_f.pst_Node)
    {
        pst_CurrentNode_ = st_f.pst_Node;
    }

    vec_Fragments_.pop_back();

}    //  v_AddRightParenth_()

void CT_FindDialog::v_AddOperator_ (ET_BoolOp eo_op)
{
    if (!pst_CurrentNode_)
    {
        ERROR_TRACE (_T("Empty node."));
        return;
    }

    ST_BoolTreeNode * pst_top = pst_CurrentNode_;
    while (pst_top->eo_Operator != ec_OperatorNotSet)
    {
        pst_top = pst_top->pst_Parent;
        if (!pst_top)
        {
            break;
        }
    }

    if (!pst_top)
    {
        pst_top = new ST_BoolTreeNode;
        pst_top->pst_Left = pst_CurrentNode_;
        pst_CurrentNode_->pst_Parent = pst_top;
    }

    pst_CurrentNode_ = pst_top;
    pst_CurrentNode_->eo_Operator = eo_op;

}    // v_AddOperator_ (...)

CT_FindDialog::ET_NodeState CT_FindDialog::eo_GetNodeState 
                                            (const ST_BoolTreeNode * pst_node)
{
    if (!pst_node)
    {
        ERROR_TRACE (_T("Empty node."));
        return ec_NodeStateUnknown;
    }

    if ((pst_node->st_LeftTerm.kstr_Term.empty()) && !pst_node->pst_Left)
    {
        if ((pst_node->st_RightTerm.kstr_Term.empty()) && !pst_node->pst_Right)
        {
            return ec_BothVacant;
        }
        else
        {
            ERROR_TRACE (_T("Missing left branch."));
            return ec_NodeStateUnknown;
        }
    }
    if ((pst_node->st_RightTerm.kstr_Term.empty()) && !pst_node->pst_Right)
    {
        return ec_RightVacant;
    }
    else
    {
        return ec_NoneVacant;
    }

}    //  eo_GetNodeState(...)


//
// Accessor
//
void CT_FindDialog::v_GetSearchContext (vector<ET_SearchType>& vec_context)
{
    vec_context.push_back (ec_SearchTypeExcludeFormatting);
    if (IsDlgButtonChecked (IDC_CHECK_MATCH_WHOLE_WORD))
    {
        vec_context.push_back (ec_SearchTypeWholeWord);
    }
    if (IsDlgButtonChecked (IDC_CHECK_MATCH_CASE))
    {
        vec_context.push_back (ec_SearchTypeMatchCase);
    }
    if (IsDlgButtonChecked (IDC_CHECK_IGNORE_DIACRITICS))
    {
        vec_context.push_back (ec_SearchTypeExcludeDiacritics);
    }
    if (IsDlgButtonChecked (IDC_CHECK_BOOLEAN_SEARCH))
    {
        vec_context.push_back (ec_SearchTypeBoolean);

        if (IsDlgButtonChecked (IDC_RADIO_PROXIMITY_WORDS))
        {
            vec_context.push_back (ec_ProximityWords);
        }
        if (IsDlgButtonChecked (IDC_RADIO_PROXIMITY_SENTENCES))
        {
            vec_context.push_back (ec_ProximitySentences);
        }
        if (IsDlgButtonChecked (IDC_RADIO_PROXIMITY_PARAGRAPHS))
        {
            vec_context.push_back (ec_ProximityParagraphs);
        }
    }

    b_ContextChanged_ = false;

}    //  v_GetSearchContext

//
// Helpers
//
void CT_FindDialog::v_ClearBoolSearchSubTree_ (ST_BoolTreeNode * pst_top)
{
    if (!pst_top)
    {
        return;
    }

    if (pst_top->pst_Left)
    {
        v_ClearBoolSearchSubTree_ (pst_top->pst_Left);
    }

    if (pst_top->pst_Right)
    {
        v_ClearBoolSearchSubTree_ (pst_top->pst_Right);
    }

    delete pst_top;
    pst_top = NULL;

}

void CT_FindDialog::v_ShowReplaceDialog()
{
    //
    // Clone Find what combo box & label
    //
    pco_ReplaceCombo_ = new CComboBox();
    CRect co_replaceComboRect;
    CWnd * pco_wnd = (CWnd *)GetDlgItem (IDC_COMBO_FIND_WHAT);
    pco_wnd->GetWindowRect (&co_replaceComboRect);
    CPoint co_yShift (0, co_replaceComboRect.Height() + 2);
    co_replaceComboRect += co_yShift;
    ScreenToClient (&co_replaceComboRect);
    WINDOWINFO st_wi;
    pco_wnd->GetWindowInfo (&st_wi);
    BOOL b_ = pco_ReplaceCombo_->Create (st_wi.dwStyle, 
                                         co_replaceComboRect, 
                                         this, 
                                         IDC_COMBO_REPLACE_WITH);
    CFont * pco_font = pco_wnd->GetFont();
    pco_ReplaceCombo_->SetFont (pco_font);

    pco_ReplaceLabel_ = new CStatic();
    CRect co_replaceLabelRect;
    pco_wnd = (CWnd *)GetDlgItem (IDC_STATIC_FIND_WHAT);
    pco_wnd->GetWindowRect (&co_replaceLabelRect);
    co_yShift.y = co_replaceLabelRect.Height() + 2;
    co_replaceLabelRect += co_yShift;
    ScreenToClient (&co_replaceLabelRect);
    pco_wnd->GetWindowInfo (&st_wi);
    b_ = pco_ReplaceLabel_->Create (_T("Re&place with:"),
                                    st_wi.dwStyle, 
                                    co_replaceLabelRect, 
                                    this, 
                                    IDC_STATIC_REPLACE_WITH); 
    pco_font = pco_wnd->GetFont();
    pco_ReplaceLabel_->SetFont (pco_font);

    //
    // Make space
    //
    CRect co_wndRect;
    GetWindowRect (&co_wndRect);
    co_wndRect.bottom += co_yShift.y;
    MoveWindow (&co_wndRect);

    unsigned int arr_controls[] = {    IDC_CHECK_MATCH_WHOLE_WORD,
                                    IDC_CHECK_MATCH_CASE,
                                    IDC_CHECK_IGNORE_DIACRITICS,
                                    IDC_CHECK_BOOLEAN_SEARCH,
                                    IDC_CHECK_REGEX,
                                    IDC_STATIC_DIRECTION,
                                    IDC_RADIO_UP,
                                    IDC_RADIO_DOWN,
                                    IDC_STATIC_SEPARATOR,
                                    IDC_STATIC_BOOL,
                                    IDC_STATIC_ENTER_TERM,
                                    IDC_EDIT_ENTER_TERM,
                                    IDC_BUTTON_AND,
                                    IDC_BUTTON_OR,
                                    IDC_BUTTON_NOT,
                                    IDC_BUTTON_LP,
                                    IDC_BUTTON_RP,
                                    IDC_BUTTON_CLEAR,
                                    IDC_BUTTON_START_OVER,
                                    IDC_BUTTON_BOOLSEARCH,
                                    IDC_STATIC_WITHIN,
                                    IDC_PROXIMITY_COMBO,
                                    IDC_RADIO_PROXIMITY_WORDS,
                                    IDC_RADIO_PROXIMITY_SENTENCES,
                                    IDC_RADIO_PROXIMITY_PARAGRAPHS };
    
    for (int i_c = 0; i_c < sizeof (arr_controls)/sizeof (unsigned int); ++i_c)
    {
        CWnd * pco_ctl = (CWnd *)GetDlgItem (arr_controls[i_c]);
        CRect co_ctlRect;
        pco_ctl->GetWindowRect (&co_ctlRect);
        co_ctlRect += co_yShift;
        ScreenToClient (&co_ctlRect);
        pco_ctl->MoveWindow (&co_ctlRect);
    }

    //
    // Disable Boolean and regex check boxes
    //
    CWnd * pco_ctl = (CWnd *)GetDlgItem (IDC_CHECK_BOOLEAN_SEARCH);
    pco_ctl->EnableWindow (FALSE);

    pco_ctl = (CWnd *)GetDlgItem (IDC_CHECK_REGEX);
    pco_ctl->EnableWindow (FALSE);

    //
    // Disable Replace button
    //
    pco_ctl = (CWnd *)GetDlgItem (ID_REPLACE);
    pco_ctl->EnableWindow (FALSE);

    InvalidateRect (NULL);
    UpdateWindow();

    b_ShowingReplaceDlg_ = true;

}    //  v_ShowReplaceDialog()
