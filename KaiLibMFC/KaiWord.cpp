//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    implementation of the word class.
//
//    $Id: KaiWord.cpp,v 1.28 2008-01-13 20:22:59 kostya Exp $
//    
//==============================================================================

//
// Disable STL-related compiler warnings
//
#pragma warning (disable: 4786)

#include "StdAfx.h"
#include "KaiWord.h"
#include "KaiString.h"
#include "KaiGeneric.h"
#include "KaiException.h"

using namespace KaiLibMFC;
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CT_Word::CT_Word (CT_KaiString * const pco_parent) : pco_Parent_ (pco_parent)
{}

CT_Word::CT_Word (const CT_Word& co_word) :
    pco_Parent_ (co_word.pco_Parent_),
    vec_Tokens_ (co_word.vec_Tokens_),
    str_Language_ (co_word.str_Language_),
    eo_StressType_ (co_word.eo_StressType_),
    i_StressPos_ (co_word.i_StressPos_),
    str_Transcription_ (co_word.str_Transcription_)
{}

//
// Assignment
//
CT_Word& CT_Word::operator = (const CT_Word& co_word)
{
    pco_Parent_  = co_word.pco_Parent_;
    vec_Tokens_ = co_word.vec_Tokens_;
    str_Language_ = co_word.str_Language_;
    eo_StressType_ = co_word.eo_StressType_;
    i_StressPos_ = co_word.i_StressPos_;
    str_Transcription_ = co_word.str_Transcription_;

    return *this;
}

wstring CT_Word::str_GetText()
{
    wstring str_text;
    vector<int>::iterator it_ = vec_Tokens_.begin();
    for (; it_ != vec_Tokens_.end(); ++it_)
    {
        str_text += pco_Parent_->str_GetToken (*it_);
    }
    return str_text;
}

//
// Find all possible hyphenation split positions
// return as pairs of integers (token, offset)
//
//void CT_Word::v_Split (std::vector <int>& vi_pos)
//{
// Not implemented
//}

/*
void CT_Word::v_Dump()
{
    DUMP_TRACE (_T("-------- CT_Word ----------------------------"));

// Not implemented

    DUMP_TRACE (_T("-------- CT_Word: end of dump ---------------"));

}  // v_Dump()
*/
