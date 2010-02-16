//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    implementation of the word class.
//
//    $Id: KaiWord.cpp,v 1.27 2007-05-28 19:03:10 kostya Exp $
//    
//==============================================================================

//
// Disable STL-related compiler warnings
//
#pragma warning (disable: 4786)

#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiWord.h"
#include "KaiParagraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CT_Word::CT_Word (CT_Paragraph * const pco_parent) : 
 pco_Parent_ (pco_parent)
{}

CT_Word::CT_Word (CT_Paragraph * const pco_parent,
                  const int i_firstToken,
                  const int i_numOfTokens) : 
 pco_Parent_ (pco_parent),
 i_FirstToken_ (i_firstToken),
 i_NumOfTokens_ (i_numOfTokens)
{}

CT_Word::CT_Word (const CT_Word& co_word) :
    pco_Parent_ (co_word.pco_Parent_),
    i_FirstToken_ (co_word.i_FirstToken_),
    i_NumOfTokens_ (co_word.i_NumOfTokens_)
{}

CT_Word::~CT_Word()
{
    v_Null();
}

void CT_Word::v_Null()
{
    eo_WordType_ = ec_ItemTypeFront;
    i_FirstToken_ = -1;
    i_NumOfTokens_ = -1;
}

//
// Assignment
//
CT_Word& CT_Word::operator = (const CT_Word& co_word)
{
    pco_Parent_ = co_word.pco_Parent_;
    i_FirstToken_ = co_word.i_FirstToken_;
    i_NumOfTokens_ = co_word.i_NumOfTokens_;

    return *this;
}

//
// Serialization
//
void CT_Word::v_Load (CFile& co_inFile)
{
    v_Null();

    unsigned int ui_bytesRead = 0;

    //
    // Load the type id
    //
    V_LoadItem (co_inFile, eo_WordType_);

    //
    // Load the numbers
    //
    V_LoadItem (co_inFile, i_NumOfTokens_);
    V_LoadItem (co_inFile, i_FirstToken_);

    return;

}    //  v_Load (...)

void CT_Word::v_Store (CFile& co_outFile)
{
    try
    {
        co_outFile.Write (&eo_WordType_, sizeof(ET_ItemType));
        co_outFile.Write (&i_FirstToken_, sizeof(int));
        co_outFile.Write (&i_NumOfTokens_, sizeof(int));
    }
    catch (CFileException * pco_ex)
    {
        V_ReportMFCFileException (*pco_ex, READ_ERROR);
        throw CT_KaiException (CT_KaiException::ec_StoreError,
                               _T("KaiString.cpp"),
                               _T("CT_Word"),
                               _T("v_Store"),
                               _T("Write error."));
    }

    return;

}    //  b_Store (..)

//
// Find all possible hyphenation split positions
// return as pais of integers (token, offset)
//
void CT_Word::v_Split (vector <int>& vi_pos)
{
    //
    // Stub - return whole word
    //
    int i_token = i_NumOfTokens_ - 1;
    int i_offset = i_FirstToken_ + i_token;

    vi_pos.push_back (i_token);
    vi_pos.push_back (i_offset);
}

void CT_Word::v_Dump()
{
    DUMP_TRACE (_T("-------- CT_Word ----------------------------"));

    STL_STRING str_type;
    switch (eo_WordType_)
    {
        case ec_Word:
        {
            str_type = _T("ec_Word");
            break;
        }
        case ec_WordBreak:
        {
            str_type = _T("ec_WordBreak");
            break;
        }
        case ec_Punctuation:
        {
            str_type = _T("ec_Punctuation");
            break;
        }
        default:
        {
            str_type = _T("Unknown; numeric value = ");
            str_type += 
             STR_UIToStr (static_cast <unsigned int>(eo_WordType_));
        }
    }    
    DUMP_TRACE (CT_KaiString (_T("\teo_WordType_ = ")) + str_type);

    DUMP_TRACE (CT_KaiString (_T("\ti_FirstToken_ = "))
                 + STR_IToStr (i_FirstToken_));

    DUMP_TRACE (CT_KaiString (_T("\ti_NumOfTokens_ = "))
                 + STR_IToStr (i_NumOfTokens_));

    DUMP_TRACE (_T("-------- CT_Word: end of dump ---------------"));

}  // v_Dump()
