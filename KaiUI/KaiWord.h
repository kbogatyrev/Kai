//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description:    interface for the word class.
//
//    $Id: KaiWord.h,v 1.2 2007-05-28 19:03:34 kostya Exp $
//
//==============================================================================

#pragma once

using namespace KaiLibMFC;

//
// Forward decl for the paragraph object
//
class CT_Paragraph;

class CT_Word
{
protected:

    CT_Paragraph * pco_Parent_;

    //
    // Word type: real word, break char, or punctuation mark
    //
    ET_ItemType eo_WordType_;

    //
    // Num of 1st token in the parent paragraph string
    //
    int i_FirstToken_;

    //
    // Num of tokens in this word
    //
    int i_NumOfTokens_;

    //
    // Word as a lexeme instance
    //
    /*
       --- TBD
    */

public:
    CT_Word (CT_Paragraph * const pco_parent);
    CT_Word (CT_Paragraph * const pco_parent, 
             const int i_firstToken, 
             const int i_numOfTokens);
    CT_Word (const CT_Word& co_word);
    ~CT_Word();

    void v_Null();

    CT_Word& operator = (const CT_Word& co_word);

    void v_Load (CFile& co_file);
    void v_Store (CFile& co_file);

    const CT_Paragraph * pco_GetParent();

    void v_Split (vector <int>&);

    void v_Dump();
};

inline const CT_Paragraph * CT_Word::pco_GetParent() { return pco_Parent_; }
