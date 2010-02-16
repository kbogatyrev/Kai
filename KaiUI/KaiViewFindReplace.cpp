//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2006
//    
//    Facility: Kai 1.2
//
//    Module description:    find/replace functions - part of the MFC CView class.
//
//    $Id: KaiViewFindReplace.cpp,v 1.15 2008-11-19 07:37:42 kostya Exp $
//
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiResource.h"
#include "KaiDoc.h"
#include "KaiView.h"
#include "KaiParagraph.h"
#include "KaiFindDialog.h"
#include "KaiSearch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


void CKaiView::OnMenuFindandreplaceFind()
{
    v_ShowFindReplaceDialog_ (ec_DialogTypeFind);

}    //  OnMenuFindandreplaceFind()

void CKaiView::OnMenuFindandreplaceReplace()
{
    // TODO: Add your command handler code here
    v_ShowFindReplaceDialog_ (ec_DialogTypeReplace);
        
}    //  OnMenuFindandreplaceReplace()

LONG CKaiView::OnFind (WPARAM wParam, LPARAM lParam)
{
    if (pco_FindDlg_->b_IsTerminating())
    {
        if (pco_Search_)
        {
            delete pco_Search_;
            pco_Search_ = NULL;
        }

        delete pco_FindDlg_;
        pco_FindDlg_ = NULL;

        return 0;
    }

    ET_SearchType eo_type = pco_FindDlg_->eo_GetSearchType();
    if (ec_SearchTypeBoolean == eo_type)
    {
        v_BooleanSearch_();
        return 0;
    }

    //
    // Single word search
    //
    v_SingleWordSearch_();

    v_ShowMatch_();

    return 0;

}    //  OnFind (...)

LONG CKaiView::OnReplace (WPARAM wParam, LPARAM lParam)
{
    CT_DocDescriptor * pco_doc = GetDocument_()->pco_GetDocDescriptor();

    if (!pco_doc->b_SearchInvalid() && (i_CurrentMatch_ >= 0))
    {
        v_ReplaceSelection_ (pco_FindDlg_->str_GetReplaceString());
    }

    v_SingleWordSearch_();

    v_ShowMatch_();

    return 0;

}    //  OnReplace (...)

//
// Helpers
//
void CKaiView::v_ShowFindReplaceDialog_ (ET_DialogType eo_type)
{
    pco_FindDlg_ = new CT_FindDialog (this, eo_type);
    pco_FindDlg_->Create (IDD_SEARCH_DLG);

    CComboBox * pco_cbox = 
        (CComboBox *)pco_FindDlg_->GetDlgItem (IDC_COMBO_FIND_WHAT);
    SET_PAST_SEARCHES::iterator it_ps = set_PastSearches_.begin();
    for (; it_ps != set_PastSearches_.end(); ++it_ps)
    {
        STL_STRING str_ = (*it_ps).str_Search;
        pco_cbox->AddString (str_.data());
    }

    STL_STRING str_sel;
    if (eo_SelectionMode_ == ec_StaticSelection)
    {
        if (pco_SelectionStart_->i_GetParagraph() == 
                            pco_SelectionEnd_->i_GetParagraph())
        {
            CT_Paragraph * pco_p = pco_SelectionStart_->pco_GetParagraph();
            CT_KaiString kstr_sel = 
                pco_p->substr (pco_SelectionStart_->i_GetParagraphOffset(),
                               pco_SelectionEnd_->i_GetParagraphOffset() - 
                                pco_SelectionStart_->i_GetParagraphOffset());
            for (int i_t = 0; i_t < kstr_sel.i_NTokens(); ++i_t)
            {
                ST_Token st_t = kstr_sel.st_GetToken (i_t);
                switch (st_t.eo_TokenType)
                {
                    case ec_TokenText:
                    case ec_TokenPunctuation:
                    case ec_TokenSpace:
                    case ec_TokenTab:
                    {
                        str_sel += kstr_sel.substr (st_t.i_Offset, 
                                                    st_t.i_Length);
                        break;
                    }
                    case ec_TokenDiacritics:
                    case ec_TokenFormatting:
                    case ec_TokenHiddenTag:
                    {
                        break;
                    }
                    case ec_TokenTypeBack:
                    case ec_TokenTypeFront:
                    default:
                    {
                        ERROR_TRACE (_T("Illegal token type"));
                        return;
                    }
                }

            }    //  for (int i_t = 0; ...)

        }    //  if (pco_SelectionStart_->i_GetParagraph() == ...

        pco_cbox->SetWindowText (str_sel.data());
        pco_cbox->SetEditSel (0, str_sel.length());
    }

    CButton * pco_btn = (CButton *)pco_FindDlg_->GetDlgItem (IDOK);
    pco_btn->EnableWindow (!str_sel.empty());
    pco_FindDlg_->ShowWindow (SW_SHOW);

}    //  v_ShowFindReplaceDialog_()

void CKaiView::v_BooleanSearch_()
{
    CT_DocDescriptor * pco_doc = GetDocument_()->pco_GetDocDescriptor();
    vector<ET_SearchType> vec_context;
    pco_FindDlg_->v_GetSearchContext (vec_context);
    
    if (!pco_Search_)
    {
        pco_Search_ = new CT_Search (pco_doc, 
                                     pco_FindDlg_->pst_GetBoolTreeTop(), 
                                     vec_context,
                                     pco_FindDlg_->i_GetContextWindow()+1);
    }
    else
    {
        //
        // This means context has changed
        //
        if (!pco_FindDlg_->b_ContextChanged())
        {
            ERROR_TRACE (_T("Attempt to resume Boolean search."));
            return;
        }
        pco_Search_->v_ResetSearch (vec_context);
        pco_Search_->v_SetBooleanTree (pco_FindDlg_->pst_GetBoolTreeTop());
    }

    pco_doc->v_ValidateSearch();
    i_CurrentMatch_ = -1;

    bool b_ret = pco_Search_->b_DoSearch();
    if (!b_ret)
    {
        return;
    }

    co_Highlights_.v_Clear();
    for (int i_m = 0; i_m < pco_Search_->i_NMatches(); ++i_m)
    {
        ST_Match st_m;
        bool b_ = pco_Search_->b_GetMatch (i_m, st_m);
        if (!b_)
        {
            break;
        }

        CT_TextPos co_start, co_end;
        CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (st_m.i_Paragraph);
        co_start.v_SetParagraph (pco_p);
        co_start.v_SetParagraphOffset (st_m.i_Offset);
        co_end.v_SetParagraph (pco_p);
        co_end.v_SetParagraphOffset (st_m.i_Offset + st_m.i_Length);
        co_Highlights_.v_Add (co_start, co_end);
    }

//        if (!co_Highlights_.b_Empty())
//        {
    eo_RefreshMode_ = ec_WholeScreen;
    InvalidateRect (NULL, TRUE);
    UpdateWindow();
//        }

}    //  v_BooleanSearch_()

void CKaiView::v_SingleWordSearch_()
{
    //
    // Single word search or replace
    //
    vector<ET_SearchType> vec_context;
    STL_STRING str_findWhat;
    ET_SearchType eo_searchType = ec_SearchTypeSingleWord;
    ET_Direction eo_direction = ec_DirectionFront;
    str_findWhat = pco_FindDlg_->str_GetSearchString();
    eo_direction = pco_FindDlg_->eo_GetDirection();

    CT_DocDescriptor * pco_doc = GetDocument_()->pco_GetDocDescriptor();

    if (!pco_Search_)
    {
        b_Cycle_ = false;
        pco_FindDlg_->v_GetSearchContext (vec_context);
        pco_Search_ = new CT_Search (pco_doc, str_findWhat, vec_context);
        pco_doc->v_ValidateSearch();
        i_CurrentMatch_ = -1;

        CWaitCursor co_wait;
        bool b_ret = pco_Search_->b_DoSearch();

        ST_PAST_SEARCH st_ps;
        st_ps.eo_Type = eo_searchType;
        st_ps.str_Search = str_findWhat;
        set_PastSearches_.insert (set_PastSearches_.begin(), st_ps);

        if (pco_Search_->i_NMatches() < 1)
        {
            i_CurrentMatch_ = -1;
        }

        v_FindFirstMatch_();

        return;

    }    //  if (!pco_Search_)
        
    if (pco_doc->b_SearchInvalid() || pco_FindDlg_->b_ContextChanged())
    {
        b_Cycle_ = false;

        str_findWhat = pco_FindDlg_->str_GetSearchString();
        if (str_findWhat.empty())
        {
            ERROR_TRACE (_T("Empty search string"));
            return;
        }

        pco_FindDlg_->v_GetSearchContext (vec_context);
        pco_Search_->v_ResetSearch (vec_context);
        pco_Search_->v_SetSearchString (str_findWhat);
        
        pco_doc->v_ValidateSearch();
        i_CurrentMatch_ = -1;

        CWaitCursor co_wait;
        bool b_ret = pco_Search_->b_DoSearch();

        //if (pco_Search_->i_NMatches() == 0)
        //{
        //    i_CurrentMatch_ = -1;
        //}

        v_FindFirstMatch_();

        return;
    }

    if (pco_Search_->i_NMatches() < 1)
    {
        ::MessageBeep (MB_ICONEXCLAMATION);
        return;
    }

    if (ec_DirectionDown == eo_direction)
    {
        if (i_CurrentMatch_ >= pco_Search_->i_NMatches() - 1)
        {
            ::MessageBeep (MB_ICONEXCLAMATION);
            i_CurrentMatch_ = 0;
            b_Cycle_ = true;
        }
        else
        {
            if (!b_Cycle_)
            {
                ++i_CurrentMatch_;
            }
            else
            {
                b_Cycle_ = false;
            }
        }
    }
    else
    {
        if (i_CurrentMatch_ == 0)
        {
            ::MessageBeep (MB_ICONEXCLAMATION);
            i_CurrentMatch_ = pco_Search_->i_NMatches() - 1;
            b_Cycle_ = true;
        }
        else
        {
            if (!b_Cycle_)
            {
                --i_CurrentMatch_;
            }
            else
            {
                b_Cycle_ = false;
            }
        }
    }

}    //        //  v_SingleWordSearch_()

void CKaiView::v_FindFirstMatch_()
{
    if (pco_Search_->i_NMatches() < 1)
    {
        i_CurrentMatch_ = -1;
        return;
    }

    CT_DocDescriptor * pco_doc = GetDocument_()->pco_GetDocDescriptor();

    int i_m = 0;
    for (; i_m < pco_Search_->i_NMatches() - 1; ++i_m)
    {
        ST_Match st_m;
        bool b_ = pco_Search_->b_GetMatch (i_m, st_m);
        if (!b_)
        {
            return;
        }

        CT_TextPos co_end;
        co_end.v_SetParagraph (pco_doc->pco_GetParagraph (st_m.i_Paragraph));
        co_end.v_SetParagraphOffset (st_m.i_Offset + st_m.i_Length);
        CT_TextPos * pco_ref = (ec_NoSelection == eo_SelectionMode_) 
                ? pco_CaretPos_ 
                : pco_SelectionStart_;
        if (co_end >= *pco_ref)
        {
            break;
        }
    }

    i_CurrentMatch_ = i_m;

}    //  v_FindFirstMatch_()

void CKaiView::v_ShowMatch_()
{
    if (b_Cycle_)
    {
        return;
    }
    if (i_CurrentMatch_ < 0)
    {
        ::MessageBeep (MB_ICONQUESTION);
//        AfxMessageBox (_T("Nothing found"));
        return;
    }

    ST_Match st_m;
    bool b_ = pco_Search_->b_GetMatch (i_CurrentMatch_, st_m);
    if (!b_)
    {
        return;
    }

    CT_DocDescriptor * pco_doc = GetDocument_()->pco_GetDocDescriptor();
    CT_Paragraph * pco_p = pco_doc->pco_GetParagraph (st_m.i_Paragraph);
    CT_TextPos co_start, co_end;
    co_start.v_SetParagraph (pco_p);
    co_start.v_SetParagraphOffset (st_m.i_Offset);
    co_end.v_SetParagraph (pco_p);
    co_end.v_SetParagraphOffset (st_m.i_Offset + st_m.i_Length);

    if (eo_SelectionMode_ != ec_NoSelection)
    {
        v_CancelSelection_();
    }

    b_ = b_SetSelection_ (&co_start, &co_end);
    if (!b_IsVisible_ (co_end))
    {
        v_MakeVisible_ (co_end);
    }
    
}    //  v_ShowMatch_()
