//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2002
//    
//    Facility: Kai 1.2
//
//    Module description:    implementation of the Text Position class.
//
//    $Id: KaiTextPos.cpp,v 1.48 2007-12-10 16:33:43 kostya Exp $
//
//==============================================================================

//
// Disable STL-related compiler warnings
//
#pragma warning (disable: 4786)

#include "KaiStdAfx.h"
#include "kai.h"
#include "KaiView.h"
#include "KaiParagraph.h"
#include "KaiLineDescriptor.h"
#include "KaiDocDescriptor.h"
#include "KaiTextPos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CT_TextPos::CT_TextPos()
{
    v_Null_();
}

CT_TextPos::CT_TextPos (CT_Paragraph * pco_parent,
                        int i_line,
                        int i_lineOffset) :
pco_Parent_ (pco_parent), i_Line_ (i_line), i_LineOffset_ (i_lineOffset)
{
    b_PastEndOfLine_ = false;
    b_Invalid_ = false;
    eo_InputType_ = ec_InputTypeFront;

    bool b_ = b_ComputeAbsoluteOffset_();
}

CT_TextPos::CT_TextPos (CT_Paragraph * pco_parent,
                        int i_paragraphOffset) :
pco_Parent_ (pco_parent), i_ParagraphOffset_ (i_paragraphOffset)
{
    b_PastEndOfLine_ = false;
    b_Invalid_ = false;
    eo_InputType_ = ec_InputTypeFront;

    bool b_ = b_ComputeLinePos_();
}

CT_TextPos::CT_TextPos (const CT_TextPos& co_textPos) :
 pco_Parent_ (co_textPos.pco_Parent_),
 i_ParagraphOffset_ (co_textPos.i_ParagraphOffset_),
 b_PastEndOfLine_ (co_textPos.b_PastEndOfLine_),
 b_Invalid_ (co_textPos.b_Invalid_),
 eo_InputType_ (co_textPos.eo_InputType_),
 i_Line_ (co_textPos.i_Line_),
 i_LineOffset_ (co_textPos.i_LineOffset_)
{
}

CT_TextPos::~CT_TextPos()
{
    v_Null_();
}

void CT_TextPos::v_Null_()
{
    pco_Parent_ = NULL;
    i_ParagraphOffset_ = -1;
    b_PastEndOfLine_ = false;
    b_Invalid_ = true;
    eo_InputType_ = ec_InputTypeFront;
    i_Line_ = -1;
    i_LineOffset_ = -1;
}

//
// From line offset to abs offset
//
bool CT_TextPos::b_ComputeAbsoluteOffset_()
{
    i_ParagraphOffset_ = 0;
    if ((i_Line_ >= pco_Parent_->i_NLines()) || (i_Line_ < 0))
    {
        ERROR_TRACE (_T("Illegal line number."));
        return false;
    }

    if (i_LineOffset_ < 0)
    {
        ERROR_TRACE (_T("Negative line offset."));
        return false;
    }

    CT_Line * pco_line = pco_Parent_->pco_GetLine (i_Line_);

//    if (i_LineOffset_ == 0)
//    {
//        i_ParagraphOffset_ = pco_line->i_GetStart();
//        return true;
//    }

//    if (b_PastEndOfLine_)
    if (b_PastEndOfLine_ && (!pco_line->b_IsLastInParagraph()))
    {
//        if (pco_line->b_IsLastInParagraph())
//        {
//            V_ErrorTrace (str_location, _T("No next line."));
//            return false;
//        }
        CT_Line * pco_nextLine = pco_Parent_->pco_GetLine (i_Line_ + 1);
        i_ParagraphOffset_ = pco_nextLine->i_GetStart();
    }
    else
    {
        bool b_ = pco_line->b_ParagraphOffsetFromLineOffset (i_LineOffset_, 
                                                             i_ParagraphOffset_);
        if (!b_)
        {
            return false;
        }
    }

    b_Invalid_ = false;

    return true;

}    // bool CT_TextPos::b_ComputeAbsoluteOffset_()

//
// From paragraph offset to line offset    
//
bool CT_TextPos::b_ComputeLinePos_()
{
    //
    // Get visible text length for whole tokens to the left
    //
    CT_Line * pco_line = NULL;
    for (i_Line_ = 0; i_Line_ < pco_Parent_->i_NLines(); ++i_Line_)
    {
        pco_line = pco_Parent_->pco_GetLine (i_Line_);
        int i_firstPosOfNextLine = pco_line->i_GetStart() + pco_line->i_GetTextLength();
        if (!b_PastEndOfLine_ && (i_ParagraphOffset_ < i_firstPosOfNextLine))
        {
            break;
        }
        if (b_PastEndOfLine_ && (i_ParagraphOffset_ <= i_firstPosOfNextLine))
        {
            break;
        }
    }
    if ((i_Line_ == pco_Parent_->i_NLines()) || (i_Line_ < 0))
    {
        ERROR_TRACE (_T("Illegal paragraph offset"));
        i_Line_ = 0;
        i_LineOffset_ = 0;
        return false;
    }

    bool b_ = pco_line->b_LineOffsetFromParagraphOffset (i_ParagraphOffset_, 
                                                         i_LineOffset_);
    if (!b_)
    {
        return false;
    }
    b_Invalid_ = false;
    return true;

}    //  bool CT_TextPos::b_ComputeLinePos_ ()

bool CT_TextPos::b_Validate_()
{
    if (!b_Invalid_)
    {
        return true;
    }

    if (eo_InputType_ == ec_LinePos)
    {
        bool b_ = b_ComputeAbsoluteOffset_();
        if (!b_)
        {
            return false;
        }
    }
    else if (eo_InputType_ == ec_ParagraphPos)
    {
        bool b_ = b_ComputeLinePos_();
        if (!b_)
        {
            return false;
        }
    }
    else
    {
        ATLASSERT(0);
        ERROR_TRACE (_T("Illegal input type."));
        return false;
    }
    b_Invalid_ = false;
    return true;

}    //  bool CT_TextPos::b_Validate_()

CT_Paragraph * CT_TextPos::pco_GetParagraph()
{
    return pco_Parent_;
}

int CT_TextPos::i_GetParagraph()
{
    return pco_Parent_->i_At();
}

void CT_TextPos::v_Invalidate (ET_InputType eo_inputType)
{
    b_Invalid_ = true;
    eo_InputType_ = eo_inputType;
}

void CT_TextPos::v_Update (CT_Paragraph * pco_parent, 
                           int i_line, 
                           int i_linePos,
                           ET_InputType eo_input)
{
    pco_Parent_ = pco_parent;

    eo_InputType_ = eo_input;

    if (i_line != ec_DoNotChange)
    {
        i_Line_ = i_line;
    }
    if (i_linePos != ec_DoNotChange)
    {
        i_LineOffset_ = i_linePos;
    }

    b_Invalid_ = true;
    if (eo_InputType_ == ec_LinePos)
    {
        bool b_ = b_ComputeAbsoluteOffset_();
        if (!b_)
        {
            return;
        }
    }
    else if (eo_InputType_ == ec_ParagraphPos)
    {
        bool b_ = b_ComputeLinePos_();
        if (!b_)
        {
            return;
        }
    }
    else
    {
        ERROR_TRACE (_T("Illegal input type."));
        return;
    }

    b_Invalid_ = false;

}    // void CT_TextPos::v_Update (...)

bool CT_TextPos::b_UpdateLinePos()
{
    bool b_ = b_ComputeLinePos_();
    return b_;
}

void CT_TextPos::v_MoveToStartOfLine()
{
    b_PastEndOfLine_ = false;

//    if (b_PastEndOfLine_)
//    {
//        --(*this);
//        b_PastEndOfLine_ = false;
//    }
    i_LineOffset_ = 0;
    bool b_ = b_ComputeAbsoluteOffset_();
}

void CT_TextPos::v_MoveToEndOfLine()
{
    if (b_PastEndOfLine_)
    {
//        --(*this);
        b_PastEndOfLine_ = false;
    }
    else
    {
        CT_Line * pco_line = pco_Parent_->pco_GetLine (i_Line_);
        i_LineOffset_ = pco_line->i_GetVisibleTextLength() - 1;
        bool b_ = b_ComputeAbsoluteOffset_();
    }
}

void CT_TextPos::v_MovePastEndOfLine()
{
    b_PastEndOfLine_ = true;
    CT_Line * pco_line = pco_Parent_->pco_GetLine (i_Line_);
    i_LineOffset_ = pco_line->i_GetVisibleTextLength();

    if (pco_line->b_IsLastInParagraph())
    {
        ERROR_TRACE (_T("Warning: last line in paragraph."));
        b_PastEndOfLine_ = false;
        --i_LineOffset_;
    }

    bool b_ = b_ComputeAbsoluteOffset_();
}

void CT_TextPos::v_SetParagraph (CT_Paragraph * pco_p)
{
    pco_Parent_ = pco_p;
    b_Invalid_ = true;
}

CT_Paragraph * CT_TextPos::pco_GetParagraph() const 
{ 
    return pco_Parent_; 
}

void CT_TextPos::v_SetParagraphOffset (int i_offset)
{ 
    i_ParagraphOffset_ = i_offset;
    eo_InputType_ = ec_ParagraphPos;
    b_Invalid_ = true;
    bool b_ = const_cast<CT_TextPos *>(this)->b_Validate_();
}

int CT_TextPos::i_GetParagraphOffset() const
{ 
    if (eo_InputType_ == ec_LinePos)
    {
        bool b_ = const_cast<CT_TextPos *>(this)->b_Validate_();
        if (!b_)
        {
            return 0;
        }
    }
    return i_ParagraphOffset_; 
}

void CT_TextPos::v_SetVisibleParagraphOffset (int i_offset)
{
    bool b_ = pco_Parent_->b_LinearOffsetToAbsoluteOffset (i_offset, i_ParagraphOffset_);

    b_Invalid_ = true;
    eo_InputType_ = ec_ParagraphPos;

}

int CT_TextPos::i_GetVisibleParagraphOffset() const
{
    int i_offset = 0;
    bool b_ = pco_Parent_->b_AbsoluteOffsetToLinearOffset (i_ParagraphOffset_, i_offset);

    return i_offset;

}   //  i_GetVisibleParagraphOffset()

void CT_TextPos::v_ClearPastEndOfLine()
{ 
    b_PastEndOfLine_ = false;
}

bool CT_TextPos::b_PastEndOfLine() const
{ 
    return b_PastEndOfLine_;
}

void CT_TextPos::v_SetLine (int i_line)
{
    i_Line_ = i_line;
    b_Invalid_ = true;
    eo_InputType_ = ec_LinePos;
}

int CT_TextPos::i_GetLine() 
{ 
    if (eo_InputType_ == ec_ParagraphPos)
    {
        bool b_ = b_Validate_();
        if (!b_)
        {
            return 0;
        }
    }
    return i_Line_; 
}

void CT_TextPos::v_SetLineOffset (int i_offset) 
{
    i_LineOffset_ = i_offset;
    b_Invalid_ = true;
    eo_InputType_ = ec_LinePos;
}

int CT_TextPos::i_GetLineOffset() 
{ 
    if (eo_InputType_ == ec_ParagraphPos)
    {
        bool b_ = b_Validate_();
        if (!b_)
        {
            return 0;
        }
    }
    return i_LineOffset_; 
}

bool CT_TextPos::b_FirstPos()
{
    if (pco_Parent_->pco_GetPrevious() == NULL)
    {
        if (eo_InputType_ == ec_ParagraphPos)
        {
            bool b_ = b_Validate_();
            if (!b_)
            {
                return false;
            }
        }

        if ((i_Line_ == 0) && (i_LineOffset_ == 0))
        {
            return true;
        }
    }
    return false;
}

bool CT_TextPos::b_LastPos()
{
    CT_Line * pco_line = pco_Parent_->pco_GetLine (i_Line_);
    if (pco_Parent_->pco_GetNext() == NULL)
    {
        if (eo_InputType_ == ec_ParagraphPos)
        {
            bool b_ = b_Validate_();
            if (!b_)
            {
                return false;
            }
        }
        
        if ((i_Line_ == pco_Parent_->i_NLines() - 1) &&
            (i_LineOffset_ == pco_line->i_GetVisibleTextLength() - 1))
        {
            return true;
        }
    }

    return false;
}

CT_TextPos& CT_TextPos::operator = (CT_TextPos& co_textPos)
{
    pco_Parent_ = co_textPos.pco_Parent_;
    i_ParagraphOffset_ = co_textPos.i_ParagraphOffset_;
    b_PastEndOfLine_ = co_textPos.b_PastEndOfLine_;
    b_Invalid_ = co_textPos.b_Invalid_;
    eo_InputType_ = co_textPos.eo_InputType_;
    i_Line_ = co_textPos.i_Line_;
    i_LineOffset_ = co_textPos.i_LineOffset_;

    return *this;
}

bool CT_TextPos::operator == (CT_TextPos& co_textPos)
{
    b_Validate_();
    co_textPos.b_Validate_();

    if ((pco_Parent_ == co_textPos.pco_Parent_) &&
        (i_Line_ == co_textPos.i_Line_) &&
        (i_LineOffset_ == co_textPos.i_LineOffset_) &&
        (b_PastEndOfLine_ == co_textPos.b_PastEndOfLine_))
    {
        return true;
    }
    return false;
}

bool CT_TextPos::operator > (CT_TextPos& co_textPos)
{
    b_Validate_();
    co_textPos.b_Validate_();

    if (pco_Parent_->i_At() > co_textPos.pco_Parent_->i_At())
    {
        return true;
    }
    if (pco_Parent_->i_At() == co_textPos.pco_Parent_->i_At())
    {
        if (i_Line_ > co_textPos.i_Line_)
        {
            return true;
        }
        if (i_Line_ == co_textPos.i_Line_)
        {
            if (i_LineOffset_ > co_textPos.i_LineOffset_)
            {
                return true;
            }
        }
    }
    return false;
}

bool CT_TextPos::operator >= (CT_TextPos& co_textPos)
{
    b_Validate_();
    co_textPos.b_Validate_();

    if (this->operator == (co_textPos))
    {
        return true;
    }
    if (this->operator > (co_textPos))
    {
        return true;
    }
    return false;
}

bool CT_TextPos::operator < (CT_TextPos& co_textPos)
{
    b_Validate_();
    co_textPos.b_Validate_();

    if (pco_Parent_->i_At() < co_textPos.pco_Parent_->i_At())
    {
        return true;
    }
    if (pco_Parent_->i_At() == co_textPos.pco_Parent_->i_At())
    {
        if (i_Line_ < co_textPos.i_Line_)
        {
            return true;
        }
        if (i_Line_ == co_textPos.i_Line_)
        {
            if (i_LineOffset_ < co_textPos.i_LineOffset_)
            {
                return true;
            }
        }
    }

    return false;
}

bool CT_TextPos::operator <= (CT_TextPos& co_textPos)
{
    b_Validate_();
    co_textPos.b_Validate_();

    if (this->operator == (co_textPos))
    {
        return true;
    }
    if (this->operator < (co_textPos))
    {
        return true;
    }
    return false;
}

//
// Prefix form only!
//
CT_TextPos& CT_TextPos::operator ++()
{
    b_Validate_();

    if (b_LastPos())
    {
        return *this;
    }

    CT_Line * pco_line = pco_Parent_->pco_GetLine (i_Line_);
    int i_lineLength = pco_line->i_GetVisibleTextLength();
    if (i_LineOffset_ >= i_lineLength - 1)
    {
        if ((i_LineOffset_ > i_lineLength) && !b_PastEndOfLine_)
        {
            throw CT_KaiException (CT_KaiException::ec_LineIndexOutOfBounds,
                                   _T("KaiTextPos.cpp"),
                                   _T("CT_TextPos"),
                                   _T("operator ++()"),
                                   _T("Illegal line position"));
        }
        if (i_Line_ == pco_Parent_->i_NLines() - 1)
        {
            CT_Paragraph * pco_next = pco_Parent_->pco_GetNext();
            if (pco_next == NULL)
            {
                throw CT_KaiException (CT_KaiException::ec_ParagraphNotFound,
                                       _T("KaiTextPos.cpp"),
                                       _T("CT_TextPos"),
                                       _T("operator ++()"),
                                       _T("No next paragraph"));
            }
            pco_Parent_ = pco_next;
            i_Line_ = 0;
            i_LineOffset_ = 0;
        }
        else
        {
            ++i_Line_;
            i_LineOffset_ = 0;
            if (b_PastEndOfLine_)
            {
                CT_Line * pco_nextLine = pco_Parent_->pco_GetLine (i_Line_);
                if (pco_nextLine->i_GetVisibleTextLength() > 1)
                {
                    i_LineOffset_ = 1;
                }
            }
        }
    }
    else
    {
        ++i_LineOffset_;
    }

    b_PastEndOfLine_ = false;
    b_ComputeAbsoluteOffset_();

    return *this;

}    //  CT_TextPos& CT_TextPos::operator ++()

//
// Prefix form only!
//
CT_TextPos& CT_TextPos::operator --()
{
    b_Validate_();

    if (b_FirstPos())
    {
        return *this;
    }

    b_PastEndOfLine_ = false;
    if (i_LineOffset_ == 0)
    {
        if (i_Line_ > 0)
        {
            --i_Line_;
            CT_Line * pco_line = pco_Parent_->pco_GetLine (i_Line_);
            i_LineOffset_ = pco_line->i_GetVisibleTextLength() - 1;
        }
        else
        {
            CT_Paragraph * pco_prev = pco_Parent_->pco_GetPrevious();
            if (pco_prev == NULL)
            {
                throw CT_KaiException (CT_KaiException::ec_ParagraphNotFound,
                                       _T("KaiTextPos.cpp"),
                                       _T("CT_TextPos"),
                                       _T("operator --()"),
                                       _T("No previous paragraph."));
            }
            pco_Parent_ = pco_prev;
            i_Line_ = pco_Parent_->i_NLines() - 1;
            CT_Line * pco_line = pco_Parent_->pco_GetLine (i_Line_);
            i_LineOffset_ = pco_line->i_GetVisibleTextLength() - 1;
        }
    }
    else
    {
        --i_LineOffset_;
    }

    b_ComputeAbsoluteOffset_();

    return *this;

}    //  CT_TextPos& CT_TextPos::operator --()

void CT_TextPos::v_Dump()
{
    DUMP_TRACE (_T("-------- CT_TextPos -------------------------"));
    STL_STRING str_parent = _T("\tpco_Parent_ = ");
    str_parent += STR_UIToStr (reinterpret_cast <int>(pco_Parent_));
    DUMP_TRACE (str_parent);

    DUMP_TRACE (CT_KaiString (_T("\ti_Paragraph_ = ")) + 
                STR_IToStr (pco_Parent_->i_At()));

    DUMP_TRACE (CT_KaiString (_T("\ti_ParagraphOffset_ = ")) +
                STR_IToStr (i_ParagraphOffset_));

    STL_STRING str_value = b_PastEndOfLine_ ? _T("true") : _T("false");
    DUMP_TRACE (CT_KaiString (_T("\tb_PastEndOfLine_ = ")) + str_value);
                 
    str_value = b_Invalid_ ? _T("true") : _T("false");
    DUMP_TRACE (CT_KaiString (_T("\tb_Invalid_ = ")) + str_value);

    switch (eo_InputType_)
    {
        case ec_InputTypeFront:
        {
            str_value = _T("ec_InputTypeFront");
            break;
        }
        case ec_LinePos:
        {
            str_value = _T("ec_LinePos");
            break;
        }
        case ec_ParagraphPos:
        {
            str_value = _T("ec_ParagraphPos");
            break;
        }
        default:
        {
            str_value = _T("UKNOWN");
        }
    }
    DUMP_TRACE (CT_KaiString (_T("\teo_InputType = ")) + str_value);

    DUMP_TRACE (CT_KaiString (_T("\ti_Line_ = ")) + STR_IToStr (i_Line_));

    DUMP_TRACE (CT_KaiString (_T("\ti_LineOffset_ = ")) + STR_IToStr (i_LineOffset_));

    DUMP_TRACE (_T("-------- CT_TextPos: end of dump ------------"));

}  // v_Dump()
