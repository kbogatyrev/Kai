//==============================================================================
//
//  Copyright (c) Konstantin Bogatyrev, 2001
//  
//  Facility: Kai 1.2
//
//  Module description: interface for the paragraph class.
//
//  $Id: KaiParagraph.h,v 1.73 2007-12-23 19:48:17 kostya Exp $
//
//==============================================================================

#pragma once

using namespace std;

#include "KaiStdAfx.h"
#include "KaiLineDescriptor.h"

//
// Forward decls
//
class CT_DocDescriptor;
class CT_FontDescriptor;
class CT_TextPos;

struct ST_Spacing
{
    ET_LineSpacingType eo_Type;
    int i_Size;

    ST_Spacing() 
    { 
        eo_Type = ec_LineSpacingFront; 
        i_Size = -1; 
    }
    
    ST_Spacing (const ST_Spacing& st_sp) 
        : eo_Type (st_sp.eo_Type), i_Size (st_sp.i_Size) 
    {};

    bool operator == (const ST_Spacing& st_sp) const 
    { 
        return ((st_sp.eo_Type == eo_Type) && (st_sp.i_Size == i_Size));
    }

    bool operator != (const ST_Spacing& st_sp) const 
    { 
        return ((st_sp.eo_Type != eo_Type) || (st_sp.i_Size != i_Size));
    }

};

class  CT_Paragraph : public CT_KaiString
{

friend class CT_Line;

protected:
    CT_DocDescriptor * pco_ParentDoc_;
    CT_Paragraph * pco_Previous_;
    CT_Paragraph * pco_Next_;    

    bool b_Current_;
    bool b_IsLastParagraph_;
    ET_Alignment eo_Alignment_;
    int i_MaxLineWidth_;    // volatile; discarded after formatting

    ST_Spacing st_Spacing_;
    int i_LeftIndent_;      // 100ths of an inch
    int i_RightIndent_;     //      - " -

    int i_Height_;

    template class KAI_API std::vector<int>;
    vector<int> vi_TabStops_;       // tab stop positions in lu's

    //
    // Same as above, but volatile (to simplify some MFC calls)
    //
    int arri_TabStops_[100];

    template class KAI_API std::vector<CT_Line *>;
    std::vector<CT_Line *> vpo_Lines_;

    //
    // Volatile properties: used to simplify & optimize paragraph rendering
    //
    CDC * pco_Dc_;
    int i_CurrentToken_;
    int i_CurrentScreenLength_;
    int i_TokensInCompound_;
    int i_CompoundScreenLength_;
    int i_CurrentLine_;
    CT_Line * pco_CurrentLine_;
    bool b_EndOfParagraph_;

public:
    CT_Paragraph();
    CT_Paragraph (CT_DocDescriptor * pco_doc);
    CT_Paragraph (const CT_Paragraph& co_paragraph);
    ~CT_Paragraph();
    void v_Null();

    CT_Paragraph& operator = (const CT_Paragraph& co_paragraph);

    bool b_GetFontKey (int i_offset, unsigned int& ui_fontKey);
    CFont * pco_GetWindowsFont (CDC * pco_dc, int i_paragraphOffset);
    CT_Line * pco_GetLine (int i_line) const;
    int i_GetLine (CT_Line * pco_line);
    int i_At();    // paragraph's index in the document
    CT_FontDescriptor * pco_GetFontDescriptor (unsigned long ui_key);
    int i_Height();

    bool b_Load (CFile& co_file);
    bool b_Store (CFile& co_file);
    bool b_StoreAsPlainText (CFile& co_file);

    void v_CopyAttributes (const CT_Paragraph&);
    bool b_CreateFDString (unsigned int ui_fd, STL_STRING& str_fd);
    bool b_CreateDiacrString (const _TCHAR chr_, STL_STRING& str_d);

    bool b_LinearOffsetToAbsoluteOffset (int i_lOffset, int& ri_aOffset);
    bool b_AbsoluteOffsetToLinearOffset (int i_aOffset, int& ri_lOffset);

    void v_MergeWithNext();
    bool b_Delete (CT_TextPos& co_first, 
                   CT_TextPos& co_onePastEnd,
                   STL_STRING& str_deletedText);
    void v_FdCleanup();    // remove dup/irrelevant FD's
    bool b_MeasureToken (ST_Token& st_token, 
                         unsigned int ui_font);
    bool b_MeasureString (CDC * pco_dc,
                          int i_offset, 
                          int i_length, 
                          CSize& co_mfcSize);

    bool b_Render (CDC * pco_dc);

    // optimized rendering; use after single char insert/deletion
    void v_PartialRender (CDC * pco_dc, 
                          int i_tokensAdded,        // can be negative
                          int i_lastTokenBefore, 
                          int i_tokensAfter);

    bool b_InsertOrReplaceChar (CDC * pco_dc, 
                                int i_offset, 
                                _TUCHAR ui_char, 
                                bool& b_insert);

    bool b_DeleteChar (CDC * pco_dc, CT_TextPos& co_char);

    bool b_ChangeFont (CT_TextPos& co_start,
                       CT_TextPos& co_onePastEnd,
                       unsigned int ui_newFont,
                       ET_FontModifier eo_newStyle = ec_FontModifierFront,
                       bool b_add = false);

    // Find a segment, i.e. word, punctuation mark, etc. that contains co_in
    bool b_GetTokenPos (const CT_TextPos& co_in, CT_TextPos& co_start, CT_TextPos& co_end);

    void v_Invalidate()
    {
        b_Current_ = false; 
    }

    void v_InsertLine()
    {
        if (i_CurrentLine_ >= (int)vpo_Lines_.size())
        {
            ATLASSERT (i_CurrentLine_ == (int)vpo_Lines_.size());
            vpo_Lines_.push_back (pco_CurrentLine_);
        }
        ++i_CurrentLine_;
    }

    //
    // Mutators & accessors
    //
    void v_SetDocument (CT_DocDescriptor * pco_parentDoc)
    { 
        pco_ParentDoc_ = pco_parentDoc; 
    }
    CT_DocDescriptor * pco_GetDocument()
    {
        return pco_ParentDoc_; 
    }

    void v_SetPrevious (CT_Paragraph * pco_previous)
    { 
        pco_Previous_ = pco_previous;
    }
    CT_Paragraph * pco_GetPrevious()
    { 
        return pco_Previous_;
    }

    void v_SetNext (CT_Paragraph * pco_next)
    { 
        pco_Next_ = pco_next;
    }
    CT_Paragraph * pco_GetNext()
    {
        return pco_Next_;
    }

    void v_SetAlignment (ET_Alignment eo_alignment)
    {
        eo_Alignment_ = eo_alignment; 
    }
    ET_Alignment eo_GetAlignment()
    {
        return eo_Alignment_; 
    }

//    int i_GetPageWidth();

//    void v_SetPageWidth (int i_width)
//    {
//        i_PageWidth_ = i_width;
//    }
//    int i_GetPageWidth()
//    {
//        return i_PageWidth_;
//    }

    int i_GetMaxLineWidth();    // device units

    int i_GetPageWidth();       // 100ths of inch

    void v_SetSpacing (ET_LineSpacingType eo_type,  int i_size = 0)
    { 
        st_Spacing_.eo_Type = eo_type;
        st_Spacing_.i_Size = i_size;
    }

    const ST_Spacing& rst_GetSpacing()
    {
        return st_Spacing_;
    }

    ET_LineSpacingType eo_GetSpacingType()
    { 
        return st_Spacing_.eo_Type;
    }

    int i_GetSpacingSize()
    { 
        return st_Spacing_.i_Size;
    }

    void v_SetLeftIndent (int i_lIndent)
    {
        i_LeftIndent_ = i_lIndent;
    }
    int i_GetLeftIndent()
    {
        return i_LeftIndent_;
    }

    void v_SetRigthIndent (int i_rIndent)
    {
        i_RightIndent_ = i_rIndent;
    }
    int i_GetRightIndent()
    {
        return i_RightIndent_;
    }

    void v_SetLastParagraph (bool b_isLastParagraph)
    { 
        b_IsLastParagraph_ = b_isLastParagraph;
    }

    bool b_IsLastParagraph() const
    { 
        //return b_IsLastParagraph_;
        return (pco_Next_ == NULL);
    }

    int i_NTabs() const
    { 
        return static_cast <int> (vi_TabStops_.size()); 
    }

    const int * pi_GetTabStops() const
    { 
        return arri_TabStops_; 
    }

//    int i_NWords() const
//    { 
//        return static_cast <int> (vpo_Words_.size()); 
//    }

    int i_NLines() const
    { 
        return static_cast <int> (vpo_Lines_.size()); 
    }

    void v_Dump();

    //
    // Protected methods
    //
protected:
    void v_DeepCopy_ (const CT_Paragraph& co_paragraph);
    bool b_Measure_();
    bool b_AddToken_();
    bool b_SplitNoHyphen_();
    bool b_SplitToken_ (const ST_Token& st_token, 
                        int i_start, 
                        int i_maxScreenLength,
                        int& i_length, 
                        int& i_screenLength);

    void v_ClearLines_();
//    void v_ClearWords_();
};

