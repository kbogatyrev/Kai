//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2006
//    
//    Facility: Kai 1.2
//
//    Module description: Interface of the search dialog
//
//    $Id: KaiFindDialog.h,v 1.17 2007-03-25 00:45:23 kostya Exp $
//==============================================================================
#pragma once

#include "KaiSearch.h"

class CT_FindDialog : public CDialog
{
    DECLARE_DYNAMIC(CT_FindDialog)

public:
//    CT_FindDialog (CWnd * pParent = NULL);   // standard constructor
    CT_FindDialog (CWnd * pParent = NULL,
                   ET_DialogType eo_type = ec_DialogTypeFind);
    virtual ~CT_FindDialog();

// Dialog Data
    enum { IDD = IDD_SEARCH_DLG };

    enum ET_InputType
    {
        ec_Term,
        ec_Operator,
        ec_Parenth
    };

    enum ET_Parenth
    {
        ec_LeftParenth,
        ec_RightParenth
    };

    enum ET_NodeState
    {
        ec_NodeStateUnknown,
        ec_BothVacant,
        ec_RightVacant,
        ec_NoneVacant
    };

protected:

    struct ST_Input
    {
        ET_InputType eo_Type;
        CT_KaiString kstr_Term;
        ET_Parenth eo_Parenth;
        ET_BoolOp eo_Operator;
    };

    struct ST_Fragment
    {
        ST_BoolTreeNode * pst_Node;
        ST_Fragment (ST_BoolTreeNode * pst_node) : pst_Node (pst_node){};
    };

    vector<ST_Input> vec_Expression_;
    vector<ST_Fragment> vec_Fragments_;

    ST_BoolTreeNode * pst_CurrentNode_;

    int i_ParenthDepth_;

    RECT st_FullDlgRect_;

    CComboBox * pco_ReplaceCombo_;
    CStatic * pco_ReplaceLabel_;
    CWnd * pco_Parent_;

    bool b_IsTerminating_;
    bool b_ContextChanged_;
    bool b_NotButtonClicked_;
    bool b_ShowingReplaceDlg_;
    ET_DialogType eo_FindOrReplace_;

    DECLARE_MESSAGE_MAP()

protected:
    void v_ClearBoolSearchSubTree_ (ST_BoolTreeNode * pst_node);
    void v_OperatorClick_ (ET_BoolOp eo_op);
    void v_AddTerm_ (CT_KaiString kstr_term);
    void v_AddNegation_();
    void v_AddLeftParenth_();
    void v_AddRightParenth_();
    void v_AddOperator_ (ET_BoolOp eo_op);
    void v_BuildSearchTree_();
    ET_NodeState eo_GetNodeState (const ST_BoolTreeNode * pst_node);
    void v_ShowReplaceDialog();

public:

    ET_DialogType eo_GetDialogType()
    {
        return eo_FindOrReplace_;
    }

    bool b_IsTerminating()
    {
        return b_IsTerminating_;
    }

    bool b_ContextChanged()
    {
        return b_ContextChanged_;
    }

    //void v_ContextRead()
    //{
    //    b_ContextChanged_ = false;
    //}

    void v_GetSearchContext (vector<ET_SearchType>& vec_context);

    ET_Direction eo_GetDirection()
    {
        return (IsDlgButtonChecked (IDC_RADIO_UP) == 0) 
            ? ec_DirectionDown 
            : ec_DirectionUp;
    }

    ET_SearchType eo_GetSearchType()
    {
        if (IsDlgButtonChecked (IDC_CHECK_BOOLEAN_SEARCH))
        {
            return ec_SearchTypeBoolean;
        }
        else
        {
            return ec_SearchTypeSingleWord;
        }
    }

    ST_BoolTreeNode * pst_GetBoolTreeTop()
    {
        ST_BoolTreeNode * pst_top = pst_CurrentNode_;
        while (pst_top && pst_top->pst_Parent)
        {
            pst_top = pst_top->pst_Parent;
        }
        return pst_top;
    }

    bool b_MatchWholeWord()
    {
        return (IsDlgButtonChecked (IDC_CHECK_MATCH_WHOLE_WORD) != 0);
    }

    bool b_MatchCase()
    {
        return (IsDlgButtonChecked (IDC_CHECK_MATCH_CASE) != 0);
    }

    bool b_IgnoreDiacritics()
    {
        return (IsDlgButtonChecked (IDC_CHECK_IGNORE_DIACRITICS) != 0);
    }

    bool b_BooleanSearch()
    {
        return (IsDlgButtonChecked (IDC_CHECK_BOOLEAN_SEARCH) != 0);
    }
    
    bool b_RegexSearch()
    {
        return (IsDlgButtonChecked (IDC_CHECK_REGEX) != 0);
    }

    STL_STRING str_GetSearchString()
    {
        CEdit * pco_edit = (CEdit *) GetDlgItem (IDC_COMBO_FIND_WHAT);
        CString str_;
        pco_edit->GetWindowText (str_);
        return STL_STRING (str_.GetBuffer());
    }

    STL_STRING str_GetReplaceString()
    {
        CEdit * pco_edit = (CEdit *) GetDlgItem (IDC_COMBO_REPLACE_WITH);
        CString str_;
        pco_edit->GetWindowText (str_);
        return STL_STRING (str_.GetBuffer());
    }

    ET_SearchType eo_GetProximityType()
    {
        if (IsDlgButtonChecked (IDC_RADIO_PROXIMITY_WORDS))
        {
            return ec_ProximityWords;
        }
        if (IsDlgButtonChecked (IDC_RADIO_PROXIMITY_SENTENCES))
        {
            return ec_ProximitySentences;
        }
        if (IsDlgButtonChecked (IDC_RADIO_PROXIMITY_PARAGRAPHS))
        {
            return ec_ProximityParagraphs;
        }
    }

    int i_GetContextWindow()
    {
        CComboBox * pco_proximityCombo = 
                    (CComboBox *) GetDlgItem (IDC_PROXIMITY_COMBO);
        return pco_proximityCombo->GetCurSel();
    }

    afx_msg void OnBnClickedFindNext();
    afx_msg void OnBnClickedCancel();
//    virtual void PostNcDestroy();
    afx_msg BOOL OnInitDialog();
    afx_msg void OnCbnEditchangeComboFindWhat();
    afx_msg void OnCbnEditchangeComboReplaceWith();
    afx_msg void OnBnClickedCheckMatchWholeWord();
    afx_msg void OnBnClickedCheckMatchCase();
    afx_msg void OnBnClickedCheckIgnoreDiacritics();
    afx_msg void OnBnClickedCheckBooleanSearch();
    afx_msg void OnBnClickedCheckRegex();
    afx_msg void OnEnChangeEditEnterTerm();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedAND();
    afx_msg void OnBnClickedNOT();
    afx_msg void OnBnClickedOR();
    afx_msg void OnBnClickedLP();
    afx_msg void OnBnClickedRP();
    afx_msg void OnCbnCloseupComboFindWhat();
    afx_msg void OnBnClickedButtonClear();
    afx_msg void OnBnClickedButtonBoolsearch();
    afx_msg void OnBnClickedReplace();
};
