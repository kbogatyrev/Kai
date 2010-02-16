//==============================================================================
//
//  Copyright (c) Konstantin Bogatyrev, 2006
//  
//  Facility: Kai 1.2
//
//  Module description: interface of a search class.
//
//  $Id: KaiSearch.h,v 1.11 2007-06-26 15:48:21 kostya Exp $
//
//==============================================================================

#ifndef KAI_SEARCH_H
#define KAI_SEARCH_H

#include "KaiStdAfx.h"

using namespace std;

//
// Search info
//
enum ET_SearchType
{
    ec_SearchTypeFront                = 3000,
    ec_SearchTypeBoolean,
    ec_SearchTypeSingleWord,
    ec_SearchTypeExcludeDiacritics,
    ec_SearchTypeExcludeFormatting,
    ec_SearchTypeMatchCase,
    ec_SearchTypeWholeWord,
    ec_ProximityWords,
    ec_ProximitySentences,
    ec_ProximityParagraphs,
    ec_SearchTypeEnd
};

enum ET_BoolOp
{
    ec_OperatorNotSet,
    ec_AND,
    ec_ANDNOT,
    ec_OR,
};

struct ST_Match
{
    int i_Paragraph;
    int i_Offset;
    int i_Length;
};

bool operator < (const ST_Match& st_lhs, const ST_Match& st_rhs);
bool operator == (const ST_Match& st_lhs, const ST_Match& st_rhs);
bool operator != (const ST_Match& st_lhs, const ST_Match& st_rhs);

const static int MAX_PROXIMITY_DISTANCE = 100;

struct ST_BoolTerm
{
    CT_KaiString kstr_Term;
    bool b_Negated;
    vector<vector<ST_Match> > vec_Matches;    // each element will contain
                                              // exactly one match; this will
                                              // help optimize merging
    ST_BoolTerm()
    {
        b_Negated = false;
    }
};

//
// Boolean search tree node
//
struct ST_BoolTreeNode
{
    ST_BoolTreeNode * pst_Parent;
    vector <vector<ST_Match> > vec_Matches;
    ET_BoolOp eo_Operator;
    ST_BoolTreeNode * pst_Left;
    ST_BoolTreeNode * pst_Right;
    ST_BoolTerm st_LeftTerm;
    ST_BoolTerm st_RightTerm;

    ST_BoolTreeNode()
    {
        eo_Operator = ec_OperatorNotSet;
        pst_Parent = NULL;
        pst_Left = NULL;
        pst_Right = NULL;
    }

    ~ST_BoolTreeNode()
    {
    }
};

//
// Conversions between target offsets and doc offsets
//
struct ST_Conversion
{
    int i_Offset;
    int i_Add;

    ST_Conversion (int i_offset, int i_add) 
        : i_Offset (i_offset), i_Add (i_add) {};
};

//
// Copy of a doc paragraph without tokens excluded from search
//
struct ST_Target
{
    CT_KaiString kstr_Target;
    vector<ST_Conversion> vec_Offsets;
};

class CT_DocDescriptor;

class KAI_API CT_Search
{
public:
    CT_Search (const CT_DocDescriptor * pco_doc,
               const STL_STRING& str_findMe);
    CT_Search (const CT_DocDescriptor * pco_doc,
               const STL_STRING& str_findMe,
               vector<ET_SearchType> vec_exclude);
    CT_Search (const CT_DocDescriptor * pco_doc,
               ST_BoolTreeNode * pst_treeTop,
               vector<ET_SearchType> vec_context,
               int i_contextWindow);
    ~CT_Search();

    void v_ResetSearch (const vector<ET_SearchType>& vec_exclude);
    bool b_DoSearch();

    void v_SetSearchString (const STL_STRING& str_find)
    {
        kstr_SearchString_ = str_find;
    }

    void v_SetBooleanTree (ST_BoolTreeNode * pst_top)
    {
        pst_BooleanTreeTop_ = pst_top;
    }

    int i_NMatches()
    {
        return static_cast<int> (vec_Matches_.size());
    }

    bool b_GetMatch (int i_match, ST_Match& st_match)
    {
        if ((i_match < 0) || (i_match >= static_cast<int> (vec_Matches_.size())))
        {
            return false;
        }
        st_match = vec_Matches_[i_match];
        return true;
    }

private:
    CT_Search();

protected:
    const CT_DocDescriptor * pco_Document_;
    bool b_IsBooleanSearch_;
    int i_StartOffset_;
    int i_CurrentOffset_;
    bool b_MatchCase_;
    bool b_WholeWord_;
    ET_SearchType eo_Proximity_;
    int i_ContextWindow_;
    CT_KaiString kstr_SearchString_;
    ST_BoolTreeNode * pst_BooleanTreeTop_;
    vector<ST_Match> vec_Matches_;
    vector<et_TokenType> vec_Exclude_;
    vector<ST_Target> vec_Targets_;

protected:
    void v_Init_();
    void v_Preprocess_();
    bool b_TermSearch_ (const CT_KaiString& str_string, 
                        bool b_negated = false,
                        vector<vector<ST_Match> >* pvec_BooleanMatches = NULL); 
                                        // ^-- NULL is non-Boolean search
    bool b_SearchBoolSubtree_ (ST_BoolTreeNode * pst_leftBottomNode);
    void v_MergeTermMatches_ (ST_BoolTreeNode * pst_node);
    void v_MergeResults_ (ST_BoolTreeNode * pst_node);
    int i_Distance_ (vector<ST_Match>& vec_1, 
                     vector<ST_Match>& vec_2);
    void v_MergeSortedVectors (const vector<ST_Match>& vec_left,
                               const vector<ST_Match>& vec_right,
                               vector<ST_Match>& vec_merged);
};

#endif
