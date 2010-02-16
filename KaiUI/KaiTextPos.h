//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2002
//    
//    Facility: Kai 1.2
//
//    Module description:    interface for the Text Position class.
//
//    $Id: KaiTextPos.h,v 1.32 2007-11-24 09:06:09 kostya Exp $
//    
//==============================================================================

#pragma once

using namespace std;

//
// Forward decl 
//
class CKaiView;
class CT_Paragraph;

class  KAI_API CT_TextPos
{
public:
    enum ET_LinePos
    {
        ec_LinePosFront       = -100000,
        ec_DoNotChange        = -100004,
        ec_LinePosBack        = -100005
    };

    enum ET_InputType
    {
        ec_InputTypeFront = 2000,
        ec_LinePos        = 2001,
        ec_ParagraphPos   = 2002,
        ec_InputTypeBack  = 2003
    };

protected:
    CT_Paragraph * pco_Parent_;

    int i_Line_;
    int i_LineOffset_;
    bool b_PastEndOfLine_;

    bool b_Invalid_;            
    ET_InputType eo_InputType_;

    int i_ParagraphOffset_;    // number of printable non-diacr. characters 
                            // preceding this one

public:
    CT_TextPos();
    CT_TextPos (CT_Paragraph * pco_parent,
                int i_Line_,
                int i_LineOffset);
    CT_TextPos (CT_Paragraph * pco_parent,
                int i_paragraphOffset);
    CT_TextPos (const CT_TextPos& co_textPos);

    ~CT_TextPos();

private:
    void v_Null_();
    bool b_ComputeAbsoluteOffset_(); // from line offset to abs offset
    bool b_ComputeLinePos_();         // from abs offset to line offset
    bool b_Validate_();

public:
    CT_Paragraph * pco_GetParagraph();
    int i_GetParagraph();

    void v_Invalidate (ET_InputType = ec_LinePos);

    void v_Update (CT_Paragraph * pco_paragraph, 
                   int i_line = ec_DoNotChange, 
                   int i_linePos = ec_DoNotChange,
                   ET_InputType eo_inputType = ec_LinePos);

    bool b_UpdateLinePos();

    void v_MoveToStartOfLine();
    void v_MoveToEndOfLine();
    void v_MovePastEndOfLine();

    //
    // Const versions provided for those accessors that do not
    // feature "lazy" update
    //
    void v_SetParagraph (CT_Paragraph * pco_p);
    CT_Paragraph * pco_GetParagraph() const;
    void v_SetParagraphOffset (int i_offset);
    int i_GetParagraphOffset() const;
    void v_SetVisibleParagraphOffset (int i_offset);
    int i_GetVisibleParagraphOffset() const;
    void v_SetLine (int);
    int i_GetLine();
    void v_SetLineOffset (int);
    int i_GetLineOffset();
    void v_ClearPastEndOfLine();

    bool b_FirstPos();
    bool b_LastPos();
    bool b_PastEndOfLine() const;

    CT_TextPos& operator = (CT_TextPos& co_textPos);

    bool operator == (CT_TextPos& co_textPos);
    bool operator > (CT_TextPos& co_textPos);
    bool operator >= (CT_TextPos& co_textPos);
    bool operator < (CT_TextPos& co_textPos);
    bool operator <= (CT_TextPos& co_textPos);
    CT_TextPos& operator++();    // prefix only!!
    CT_TextPos& operator--();    // prefix only!!

    void v_Dump();
};
