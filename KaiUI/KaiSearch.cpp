//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2006
//    
//    Facility: Kai 1.2
//
//    Module description:    implementation of a search class.
//
//    $Id: KaiSearch.cpp,v 1.14 2008-11-19 07:39:03 kostya Exp $
//
//==============================================================================

//
// Disable STL-related compiler warnings
//
#pragma warning (disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiDocDescriptor.h"
#include "KaiParagraph.h"
#include "KaiSearch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CT_Search::CT_Search (const CT_DocDescriptor * pco_doc,
                      const STL_STRING& str_findMe)
 : pco_Document_ (pco_doc), kstr_SearchString_ (str_findMe)
{
    b_IsBooleanSearch_= false;

    vec_Exclude_.push_back (ec_TokenDiacritics);
    vec_Exclude_.push_back (ec_TokenFormatting);

    b_MatchCase_ = false;
    b_WholeWord_ = false;
    pst_BooleanTreeTop_ = NULL;

    v_Preprocess_();
}

CT_Search::CT_Search (const CT_DocDescriptor * pco_doc,
                      const STL_STRING& str_findMe,
                      vector<ET_SearchType> vec_params)
 : pco_Document_ (pco_doc), kstr_SearchString_ (str_findMe)
{
    b_IsBooleanSearch_= false;

    b_MatchCase_ = false;
    b_WholeWord_ = false;
    pst_BooleanTreeTop_ = NULL;

    v_ResetSearch (vec_params);

}    //  ctor

CT_Search::CT_Search (const CT_DocDescriptor * pco_doc,
                      ST_BoolTreeNode * pst_top,
                      vector<ET_SearchType> vec_params, 
                      int i_contextWindow) 
: pco_Document_ (pco_doc), 
  pst_BooleanTreeTop_ (pst_top), 
  i_ContextWindow_ (i_contextWindow)
{
    b_IsBooleanSearch_= true;

    b_MatchCase_ = false;
    b_WholeWord_ = false;

    v_ResetSearch (vec_params);
}

CT_Search::~CT_Search()
{
}

void CT_Search::v_ResetSearch (const vector<ET_SearchType>& vec_params)
{
    b_MatchCase_ = false;
    b_WholeWord_ = false;

    vec_Exclude_.clear();
    vector<ET_SearchType>::const_iterator it_ = vec_params.begin();
    for (; it_ != vec_params.end(); ++it_)
    {
        ET_SearchType eo_type = *it_;
        switch (eo_type)
        {
            case ec_SearchTypeExcludeDiacritics:
            {
                vec_Exclude_.push_back (ec_TokenDiacritics);
                break;
            }

            case ec_SearchTypeExcludeFormatting:
            {
                vec_Exclude_.push_back (ec_TokenFormatting);
                break;
            }

            case ec_SearchTypeMatchCase:
            {
                b_MatchCase_ = true;
                break;
            }

            case ec_SearchTypeWholeWord:
            {
                b_WholeWord_ = true;
                break;
            }

            case ec_SearchTypeBoolean:
            {
                b_IsBooleanSearch_ = true;
                break;
            }

            case ec_ProximityParagraphs:
            case ec_ProximitySentences:
            case ec_ProximityWords:
            {
                eo_Proximity_ = eo_type;
                break;
            }

            case ec_SearchTypeFront:
            case ec_SearchTypeEnd:
            {
                ERROR_TRACE (_T("Illegal search type"));
                return;
            }

            default:
            {
                ERROR_TRACE (_T("Unknown search type"));
                return;
            }

        }    // switch

    }  //  for...

    v_Preprocess_();

}    //  void CT_Search::v_ResetSearch (...)

bool CT_Search::b_DoSearch()
{
    bool b_ret = true;

    if (!b_IsBooleanSearch_)
    {
        b_TermSearch_ (kstr_SearchString_);
    }
    else
    {
        b_SearchBoolSubtree_ (pst_BooleanTreeTop_);
        vector<vector<ST_Match> >::iterator it_results = 
                                    pst_BooleanTreeTop_->vec_Matches.begin();
        for (; 
             it_results != pst_BooleanTreeTop_->vec_Matches.end(); 
             ++it_results)
        {
            vector<ST_Match>::iterator it_matchSet = (*it_results).begin();
            for (; it_matchSet != (*it_results).end(); ++it_matchSet)
            {
                if (vec_Matches_.empty())
                {
                    vec_Matches_.push_back (*it_matchSet);
                }
                else
                {
                    if (vec_Matches_.back() != *it_matchSet)
                    {
                        vec_Matches_.push_back (*it_matchSet);
                    }
                }
            }
        }
//        sort (vec_Matches_.begin(), vec_Matches_.end());
    }

    return true;

}    //  b_DoSearch()

//
//                      --- Private stuff ---
//

//
// Preprocess document, copy all paragraphs removing 
// non-searchable stuff
//
void CT_Search::v_Preprocess_()
{
    vec_Targets_.clear();
    vec_Matches_.clear();

    vec_Targets_.reserve (pco_Document_->i_NParagraphs());
    for (int i_p = 0; i_p < pco_Document_->i_NParagraphs(); ++i_p)
    {
        ST_Target st_target;
        CT_KaiString kstr_in = *(pco_Document_->pco_GetParagraph (i_p));
        st_target.kstr_Target.CT_KaiString::v_CopyAttributes (kstr_in);

        vector<et_TokenType>::iterator it_found;
        for (int i_t = 0; i_t < kstr_in.i_NTokens(); ++i_t)
        {
            ST_Token st_t = kstr_in.rst_GetToken (i_t);
            it_found = find (vec_Exclude_.begin(), 
                             vec_Exclude_.end(), 
                             st_t.eo_TokenType);
            if (it_found == vec_Exclude_.end())
            {
                st_target.kstr_Target.append (kstr_in.str_GetToken (i_t));
            }
            else
            {
                ST_Conversion st_conv (st_target.kstr_Target.length(), 
                                       st_t.i_Length);
                st_target.vec_Offsets.push_back (st_conv);
            }
        }

        if (!b_MatchCase_)
        {
            st_target.kstr_Target.v_ToUpper();
        }
        vec_Targets_.push_back (st_target);
    }

}    //  v_Preprocess_()

bool CT_Search::b_TermSearch_ (const CT_KaiString& kstr_, 
                               bool b_negated,
                               vector<vector<ST_Match> > * pvec_booleanMatches)
{
    CT_KaiString kstr_searchString (kstr_);

    if (!b_MatchCase_)
    {
        kstr_searchString.v_ToUpper();
    }

    STL_STRING str_copy;
    for (unsigned int ui_p = 0; ui_p < vec_Targets_.size(); ++ui_p)
    {
        vector<ST_Conversion>& vec_offsetMap = vec_Targets_[ui_p].vec_Offsets;
        CT_KaiString& rkstr_target = vec_Targets_[ui_p].kstr_Target;
        STL_STRING::size_type se_at = STL_STRING::npos;
        STL_STRING::size_type se_start = 0;
        do
        {
            se_at = rkstr_target.find (kstr_searchString, se_start);
            if (se_at == STL_STRING::npos)
            {
                continue;
            }
            if (b_WholeWord_)
            {
                ST_Token st_t = rkstr_target.st_GetTokenFromOffset (se_at);
                int i_t = rkstr_target.i_GetTokenNum (st_t);
                if (kstr_searchString != rkstr_target.str_GetToken (i_t))
                {
                    se_start = se_at + kstr_searchString.length();
                    continue;
                }
                if (i_t > 0)
                {
                    ST_Token st_prev = rkstr_target.st_GetToken (i_t - 1);
                    if ((st_prev.eo_TokenType == ec_TokenText) ||
                        (st_prev.eo_TokenType == ec_TokenDiacritics) ||
                        (st_prev.eo_TokenType == ec_TokenFormatting))
                    {
                        se_start = se_at + kstr_searchString.length();
                        continue;
                    }
                }
                if (i_t < rkstr_target.i_NTokens()-1)
                {
                    ST_Token st_next = rkstr_target.st_GetToken (i_t + 1);
                    if ((st_next.eo_TokenType == ec_TokenText) ||
                        (st_next.eo_TokenType == ec_TokenDiacritics) ||
                        (st_next.eo_TokenType == ec_TokenFormatting))
                    {
                        se_start = se_at + kstr_searchString.length();
                        continue;
                    }
                }
            }
            ST_Match st_m;
            st_m.i_Paragraph = static_cast<int>(ui_p);
            st_m.i_Offset = static_cast<int> (se_at);
            vector<ST_Conversion>::iterator it_ = vec_offsetMap.begin();
            int i_add = 0;
            for (; it_ != vec_offsetMap.end(); ++it_)
            {
                if ((*it_).i_Offset <= st_m.i_Offset)
                {
                    i_add += (*it_).i_Add;
                }
                else
                {
                    break;
                }
            }
            st_m.i_Offset += i_add;
            st_m.i_Length = kstr_searchString.length();
            if (b_IsBooleanSearch_)
            {
                vector<ST_Match> vec_match (1, st_m);
                pvec_booleanMatches->push_back (vec_match);
            }
            else
            {
                vec_Matches_.push_back (st_m);
            }
            se_start = se_at + kstr_searchString.length();

        } while (se_at != STL_STRING::npos);

    }    //  for (unsigned int ui_p ...)

    return true;

}    //  b_TermSearch_ (...)

bool CT_Search::b_SearchBoolSubtree_ (ST_BoolTreeNode * pst_node)
{
    bool b_ret = true;

    if (pst_node->pst_Left)
    {
        b_ret = b_SearchBoolSubtree_ (pst_node->pst_Left);
        if (!b_ret)
        {
            return false;
        }
    }
    else
    {
        if (pst_node->st_LeftTerm.kstr_Term.empty())
        {
            ERROR_TRACE (_T("No search string in term node"));
            return false;
        }

        b_ret = b_TermSearch_ (pst_node->st_LeftTerm.kstr_Term, 
                               pst_node->st_LeftTerm.b_Negated,
                               &pst_node->st_LeftTerm.vec_Matches);
        if (!b_ret)
        {
            ERROR_TRACE (_T("Search error, left term"));
            return false;
        }

    }

    if (pst_node->pst_Right)
    {
        b_ret = b_SearchBoolSubtree_ (pst_node->pst_Right);
        if (!b_ret)
        {
            return false;
        }
    }
    else
    {
//        if (pst_node->st_RightTerm.kstr_Term.empty())
//        {
//            ERROR_TRACE (_T("No search string in term node"));
//            return false;
//        }
        if (!pst_node->st_RightTerm.kstr_Term.empty())
        {
        b_ret = b_TermSearch_ (pst_node->st_RightTerm.kstr_Term,
                               pst_node->st_RightTerm.b_Negated,
                               &pst_node->st_RightTerm.vec_Matches);
        if (!b_ret)
        {
            ERROR_TRACE (_T("Search error, right term"));
            return false;
        }
        }
    }

    v_MergeResults_ (pst_node);

    return true;

}    //  b_SearchBoolSubtree_

void CT_Search::v_MergeResults_ (ST_BoolTreeNode * pst_node)
{
    vector<vector<ST_Match> >& vec_left = pst_node->pst_Left 
                                    ? pst_node->pst_Left->vec_Matches
                                    : pst_node->st_LeftTerm.vec_Matches;
    vector<vector<ST_Match> >& vec_right = pst_node->pst_Right
                                    ? pst_node->pst_Right->vec_Matches
                                    : pst_node->st_RightTerm.vec_Matches;

    switch (pst_node->eo_Operator)
    {
        case ec_OperatorNotSet:
        {
            if ((NULL != pst_node->pst_Parent) || (vec_right.size() > 0))
            {
                ATLASSERT(0);
                ERROR_TRACE (_T("Boolean operator missing. "));
                return;
            }

            vector<vector<ST_Match> >::iterator it_left = vec_left.begin();
            while (it_left != vec_left.end())
            {
                pst_node->vec_Matches.push_back (*it_left);
                ++it_left;
            }
            break;
        }
                                    
        case ec_AND:
        {
            if (vec_left.empty() || vec_right.empty())
            {
                return;
            }

            vector<vector<ST_Match> >::iterator it_left = vec_left.begin();
            vector<vector<ST_Match> >::iterator it_right = vec_right.begin();
            while (it_left != vec_left.end() && it_right != vec_right.end())
            {
                int i_distance = i_Distance_ (*it_left, *it_right);
                if (i_distance <= i_ContextWindow_)
                {
                    vector<ST_Match> vec_merged;
                    v_MergeSortedVectors (*it_left, *it_right, vec_merged);
                    pst_node->vec_Matches.push_back (vec_merged);
                }
            
                if ((*it_left)[0] < (*it_right)[0])
                {
                    ++it_left;
                }
                else
                {
                    ++it_right;
                }
            }
            break;

        }    // AND

        case ec_ANDNOT:
        {
            if (vec_left.empty())
            {
                return;
            }

            vector<vector<ST_Match> >::iterator it_left = vec_left.begin();
            vector<vector<ST_Match> >::iterator it_right = vec_right.begin();
            while (it_left != vec_left.end() && it_right != vec_right.end())
            {
                int i_distance = i_Distance_ (*it_left, *it_right);
                if (i_distance <= i_ContextWindow_)
                {
                    ++it_left;
                }
                else
                {
                    if ((*it_left)[0] < (*it_right)[0])
                    {
                        pst_node->vec_Matches.push_back (*it_left);
                        ++it_left;
                    }
                    else
                    {
                        ++it_right;
                    }
                }            
            }

            while (it_left != vec_left.end())
            {
                pst_node->vec_Matches.push_back (*it_left++);
            }

            break;
    
        }    // ANDNOT

        case ec_OR:
        {
            vector<vector<ST_Match> >::iterator it_left = vec_left.begin();
            vector<vector<ST_Match> >::iterator it_right = vec_right.begin();
            while (it_left != vec_left.end() && it_right != vec_right.end())
            {
                while ((*it_left)[0] < (*it_right)[0])
                {
                    pst_node->vec_Matches.push_back (*it_left);
                    ++it_left;
                    if (it_left == vec_left.end())
                    {
                        break;
                    }
                }
                if (it_left == vec_left.end())
                {
                    continue;
                }
                while ((*it_right)[0] < (*it_left)[0])
                {
                    pst_node->vec_Matches.push_back (*it_right);
                    ++it_right;
                    if (it_right == vec_right.end())
                    {
                        break;
                    }
                }
            }
            while (it_left != vec_left.end())
            {
                pst_node->vec_Matches.push_back (*it_left++);
            }
            while (it_right != vec_right.end())
            {
                pst_node->vec_Matches.push_back (*it_right++);
            }
            break;
    
        }    // OR
    }   // switch

}    // v_MergeResults_ (...)

//
// Assume vectors are sorted
//
int CT_Search::i_Distance_ (vector<ST_Match>& vec_1, 
                            vector<ST_Match>& vec_2)
{
    ST_Match& st_first = vec_1[0] < vec_2[0] ? vec_1[0] : vec_2[0];
    int i_last1 = vec_1.size() - 1;
    int i_last2 = vec_2.size() - 1;
    ST_Match& st_last = vec_2[i_last2] < vec_1[i_last1] 
                                                ? vec_1[i_last1] 
                                                : vec_2[i_last2];                            
    int i_distance = 0;
    switch (eo_Proximity_)
    {
        case ec_ProximityWords:
        {
            CT_Paragraph * pco_p = 
                        pco_Document_->pco_GetParagraph (st_first.i_Paragraph);
            if (st_first.i_Paragraph == st_last.i_Paragraph)
            {
                i_distance = pco_p->i_NFields (st_first.i_Offset, 
                                               st_last.i_Offset - 
                                                st_first.i_Offset);
            }
            else
            {
                i_distance = pco_p->i_NFields (st_first.i_Offset, 
                                               pco_p->length() - 
                                                st_first.i_Offset);
                while ((i_distance <= MAX_PROXIMITY_DISTANCE) && 
                       (!pco_p->b_IsLastParagraph()))
                {
                    pco_p = pco_p->pco_GetNext();
                    if (pco_p->i_At() < st_last.i_Paragraph)
                    {
                        i_distance += pco_p->i_NFields();
                    }
                    else
                    {
                        i_distance += pco_p->i_NFields (0, st_last.i_Offset);
                    }
                }
            }
            ++i_distance;

            break;
        }
        case ec_ProximitySentences:
        {
            CT_Paragraph * pco_p = 
                pco_Document_->pco_GetParagraph (st_first.i_Paragraph);
            CT_KaiString kstr_ = *pco_p;
            kstr_ = kstr_.substr (st_first.i_Offset);
            kstr_.v_ClearAttributes();
            kstr_.v_SetBreakChars (_T("."));
            i_distance = kstr_.i_GetNumOfFields();
            while ((st_first.i_Paragraph < st_last.i_Paragraph) &&
                   (i_distance < MAX_PROXIMITY_DISTANCE))
            {
                pco_p = pco_p->pco_GetNext();
                kstr_ = *pco_p;
                kstr_.v_ClearAttributes();
                kstr_.v_SetBreakChars (_T("."));
                if (pco_p->i_At() < st_last.i_Paragraph)
                {
                    i_distance += kstr_.i_NFields();
                }
                else
                {
                    kstr_ = kstr_.substr (0, st_last.i_Offset + 1);
                    i_distance += kstr_.i_NFields();
                    break;
                }
                if (pco_p->b_IsLastParagraph())
                {
                    ERROR_TRACE (_T("Illegal match length. "));
                    return i_distance;
                }
            }

            break;
        }
        case ec_ProximityParagraphs:
        {
            i_distance = abs (st_first.i_Paragraph - st_last.i_Paragraph) + 1;
            break;
        }
        default:
        {
            ERROR_TRACE (_T("Illegal proximity type"));
        }
    }

    return i_distance;

}    // i_Distance (...)

void CT_Search::v_MergeSortedVectors (const vector<ST_Match>& vec_left,
                                      const vector<ST_Match>& vec_right,
                                      vector<ST_Match>& vec_merged)
{
    vector<ST_Match>::const_iterator it_l = vec_left.begin();
    vector<ST_Match>::const_iterator it_r = vec_right.begin();
    while ((it_l != vec_left.end()) && (it_r != vec_right.end()))
    {
        if (*it_l < *it_r)
        {
            vec_merged.push_back (*it_l);
            ++it_l;
        }
        else
        {
            vec_merged.push_back (*it_r);
            ++it_r;
        }
    }
    while (it_l != vec_left.end())
    {
        vec_merged.push_back (*it_l);
        ++it_l;
    }
    while (it_r != vec_right.end())
    {
        vec_merged.push_back (*it_r);
        ++it_r;
    }

}    //  v_MergeSortedVectors (...)

bool operator < (const ST_Match& st_lhs, const ST_Match& st_rhs)
{
    if (st_lhs.i_Paragraph < st_rhs.i_Paragraph)
    {
        return true;
    }
    if (st_lhs.i_Paragraph == st_rhs.i_Paragraph)
    {
        if (st_lhs.i_Offset < st_rhs.i_Offset)
        {
            return true;
        }
    }
    return false;
}

bool operator == (const ST_Match& st_lhs, const ST_Match& st_rhs)
{
    if (st_lhs.i_Paragraph != st_rhs.i_Paragraph)
    {
        return false;
    }
    if (st_lhs.i_Offset != st_rhs.i_Offset)
    {
        return false;
    }
    if (st_lhs.i_Length != st_rhs.i_Length)
    {
        return false;
    }
    return true;
}

bool operator != (const ST_Match& st_lhs, const ST_Match& st_rhs)
{
    return !(st_lhs == st_rhs);
}
