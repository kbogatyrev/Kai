//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2007
//    
//    Facility: Kai 1.2
//
//    Module description:    interface for the word class.
//
//    $Id: KaiWord.h,v 1.3 2008-01-13 20:16:23 kostya Exp $
//
//==============================================================================

#pragma once

#include "StdAfx.h" 

namespace KaiLibMFC
{

    enum ET_StressType
    {
        ec_StressTypeFront = 10201,
        ec_StressTypeNormal,
        ec_StressTypeProclitic,
        ec_StressTypeEnclitic,
        ec_StressTypeUnstressed,
        ec_StressTypeUnknown,
        ec_StressTypeBack
    };

//
// Forward decl for the paragraph object
//
class CT_KaiString;

class AFX_EXT_CLASS CT_Word
{
friend class CT_KaiString;

protected:

    //CT_Paragraph * pco_Parent_;
    CT_KaiString * pco_Parent_;

//    ET_ItemType eo_WordType_;

    std::vector<int> vec_Tokens_;

    std::wstring str_Language_;     // ISO 639.2 alpha-3 code?

    ET_StressType eo_StressType_;
    int i_StressPos_; // chars from start of 1st token

    std::wstring str_Transcription_;


public:
    CT_Word (CT_KaiString * const pco_parent);
    CT_Word (const CT_Word& co_word);
    ~CT_Word() {};

    CT_Word& operator = (const CT_Word& co_word);

//    void v_Load (CFile& co_file);
//    void v_Store (CFile& co_file);

    const CT_KaiString * pco_GetParent();
    std::wstring str_GetText();


//    void v_Split (std::vector <int>&);

//    void v_Dump();
};

inline const CT_KaiString * CT_Word::pco_GetParent() { return pco_Parent_; }

}