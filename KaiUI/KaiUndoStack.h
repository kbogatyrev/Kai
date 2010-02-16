//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2005
//    
//    Facility: Kai 1.2
//
//    Module description: Interface of the stack of undoable events
//
//    $Id: KaiUndoStack.h,v 1.16 2007-12-30 05:20:25 kostya Exp $
//
//==============================================================================

#ifndef KAI_UNDOSTACK_H
#define KAI_UNDOSTACK_H

using namespace std;

#include "KaiString.h"
#include "KaiFontDescriptor.h"
#include "KaiParagraph.h"

struct ST_UndoableEvent
{
    ET_UndoEventType eo_Event;
    int i_Paragraph;
    int i_Length;
    CT_KaiString str_First;
    int i_EndParagraph;
    bool b_Merged;
    vector<CT_Paragraph *> vec_WholeParagraphs;
    CT_KaiString str_Last;
    bool b_Selection;
    int i_StartVisibleOffset;
    int i_EndVisibleOffset;
    bool b_CaretAtStart;
    bool b_Chained;

    ST_UndoableEvent()
    {
        eo_Event = ec_UndoEventFront;
        i_Paragraph = 0;
        i_Length = 0;
        b_Selection = false;
        i_EndParagraph = 0;
        b_Merged = false;
        i_StartVisibleOffset = 0;
        i_EndVisibleOffset = 0;
        b_CaretAtStart = false;
        b_Chained = false;
    }

    ST_UndoableEvent (ET_UndoEventType eo_event, 
                      int i_paragraph,
                      int i_offset, 
                      int i_length,
                      bool b_selection) :
        eo_Event (eo_event),
        i_Paragraph (i_paragraph),
        i_StartVisibleOffset (i_offset),
        i_Length (i_length),
        b_Selection (b_selection)
//        str_First (str_text)
    {
            i_EndVisibleOffset = 0;
            b_CaretAtStart = false;
            b_Chained = false;
    };

    ST_UndoableEvent (const ST_UndoableEvent& st_rhs)
    {
        eo_Event = st_rhs.eo_Event;
        i_Paragraph = st_rhs.i_Paragraph;
        i_StartVisibleOffset = st_rhs.i_StartVisibleOffset;
        i_Length = st_rhs.i_Length;
        i_EndParagraph = st_rhs.i_EndParagraph;
        b_Merged = st_rhs.b_Merged;
        str_First = st_rhs.str_First;
        for (unsigned int ui_ = 0; 
             ui_ < st_rhs.vec_WholeParagraphs.size(); 
             ++ui_)
        {
            CT_Paragraph * pco_p = 
                new CT_Paragraph (*st_rhs.vec_WholeParagraphs[ui_]);
            vec_WholeParagraphs.push_back (pco_p);
        }
        str_Last = st_rhs.str_Last;
        b_Selection = st_rhs.b_Selection;
        i_EndVisibleOffset = st_rhs.i_EndVisibleOffset;
        b_CaretAtStart = st_rhs.b_CaretAtStart;
        b_Chained = st_rhs.b_Chained;
    }

    const ST_UndoableEvent& operator = (const ST_UndoableEvent st_rhs)
    {
        eo_Event = st_rhs.eo_Event;
        i_Paragraph = st_rhs.i_Paragraph;
        i_StartVisibleOffset = st_rhs.i_StartVisibleOffset;
        i_Length = st_rhs.i_Length;
        i_EndParagraph = st_rhs.i_EndParagraph;
        b_Merged = st_rhs.b_Merged;
        str_First = st_rhs.str_First;
        for (unsigned int ui_ = 0;
             ui_ < st_rhs.vec_WholeParagraphs.size();
             ++ui_)
        {
            CT_Paragraph * pco_p = 
                new CT_Paragraph (*st_rhs.vec_WholeParagraphs[ui_]);
            vec_WholeParagraphs.push_back (pco_p);
        }
        str_Last = st_rhs.str_Last;
        b_Selection = st_rhs.b_Selection;
        i_EndVisibleOffset = st_rhs.i_EndVisibleOffset;
        b_CaretAtStart = st_rhs.b_CaretAtStart;
        b_Chained = st_rhs.b_Chained;

        return *this;
    }

    virtual ~ST_UndoableEvent()
    {
        for (unsigned int ui_ = 0; ui_ < vec_WholeParagraphs.size(); ++ui_)
        {
            delete vec_WholeParagraphs[ui_];
        }
    }

    void Clear()
    {
        eo_Event = ec_UndoEventFront;
        i_Paragraph = 0;
        i_Length = 0;
        i_EndParagraph = 0;
        b_Merged = false;
        b_Selection = false;
        i_StartVisibleOffset = 0;
        i_EndVisibleOffset = 0;
        str_First.clear();
        for (unsigned int ui_ = 0; ui_ < vec_WholeParagraphs.size(); ++ui_)
        {
            delete vec_WholeParagraphs[ui_];
        }
        vec_WholeParagraphs.clear();
        str_Last.clear();
        b_CaretAtStart = false;
        b_Chained = false;
    }
};

class KAI_API CT_UndoStack
{

protected:
    ST_UndoableEvent st_CurrentEvent_;
    vector<ST_UndoableEvent> vec_Events_;
    bool b_Blocked_;

public:
    CT_UndoStack();
    ~CT_UndoStack();

    void v_AddSimpleEvent (ET_UndoEventType eo_type, 
                           int i_paragraph,
                           int i_offset, 
                           int i_length,
                           bool b_selection);
    void v_PopLastEvent();
    void v_AddEvent();
    void v_RemoveLastEvent();
    bool b_EventsAvailable();

    //
    // Setters and Getters
    //
    void v_ClearCurrentEvent()
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.Clear();
    }

    void v_SetEventType (ET_UndoEventType eo_event)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.eo_Event = eo_event;
    }

    ET_UndoEventType eo_GetEventType() const
    {
        return st_CurrentEvent_.eo_Event;
    }

    void v_SetParagraph (int i_p)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.i_Paragraph = i_p;
    }

    int i_GetParagraph() const
    {
        return st_CurrentEvent_.i_Paragraph;
    }

    void v_SetOffset (int i_offset)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.i_StartVisibleOffset = i_offset;
    }

    int i_GetOffset() const
    {
        return st_CurrentEvent_.i_StartVisibleOffset;
    }

    void v_SetLength (int i_l)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.i_Length = i_l;
    }

    int i_GetLength() const
    {
        return st_CurrentEvent_.i_Length;
    }

    void v_SetEndParagraph (int i_ep)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.i_EndParagraph = i_ep;
    }

    int i_GetEndParagraph() const
    {
        return st_CurrentEvent_.i_EndParagraph;
    }

    void v_SetMerged()
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.b_Merged = true;
    }

    bool b_IsMerged() const
    {
        return st_CurrentEvent_.b_Merged;
    }

    void v_SetSelection (bool b_s)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.b_Selection = b_s;
    }

    bool b_Selection() const
    {
        return st_CurrentEvent_.b_Selection;
    }

    void v_SetInitialFragment (STL_STRING str_)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.str_First = str_;
    }

    void v_SetText (STL_STRING str_)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.str_First = str_;
    }

    STL_STRING str_GetInitialFragment() const
    {
        return st_CurrentEvent_.str_First;
    }

    STL_STRING str_GetText() const
    {
        return st_CurrentEvent_.str_First;
    }

    void v_PushParagraph (CT_Paragraph * pco_p)
    {
        if (b_Blocked_)
        {
            return;
        }
        CT_Paragraph * pco_copy = new CT_Paragraph (*pco_p);
//        delete pco_p;
        pco_copy->v_SetNext (NULL);
        pco_copy->v_SetPrevious (NULL);
        st_CurrentEvent_.vec_WholeParagraphs.push_back (pco_copy);
    }

    CT_Paragraph * pco_PopParagraph()
    {
        if (st_CurrentEvent_.vec_WholeParagraphs.empty())
        {
            return NULL;
        }

        vector<CT_Paragraph *>::iterator it_p =
            st_CurrentEvent_.vec_WholeParagraphs.begin();
    
        if (it_p == st_CurrentEvent_.vec_WholeParagraphs.end())
        {
            return NULL;
        }
        else
        {
            CT_Paragraph * pco_p = *it_p;
            st_CurrentEvent_.vec_WholeParagraphs.erase (it_p);
            return pco_p;
        }
    }

    void v_SetFinalFragment (STL_STRING str_)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.str_Last = str_;
    }

    STL_STRING str_GetFinalFragment() const
    {
        return st_CurrentEvent_.str_Last;
    }

    void v_SetStartVisibleOffset (int i_offset)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.i_StartVisibleOffset = i_offset;
    }

    int i_GetStartVisibleOffset() const 
    {
        return st_CurrentEvent_.i_StartVisibleOffset;
    }

    void v_SetVisibleOffset (int i_offset)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.i_StartVisibleOffset = i_offset;
    }

    int i_GetVisibleOffset() const 
    {
        return st_CurrentEvent_.i_StartVisibleOffset;
    }

    void v_SetEndVisibleOffset (int i_offset)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.i_EndVisibleOffset = i_offset;
    }

    int i_GetEndVisibleOffset() const 
    {
        return st_CurrentEvent_.i_EndVisibleOffset;
    }

    void v_SetCaretAtStart (bool b_caretAtStart)
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.b_CaretAtStart = b_caretAtStart;
    }

    bool b_IsCaretAtStart() const
    {
        return st_CurrentEvent_.b_CaretAtStart;
    }

    void v_Block()
    {
        b_Blocked_ = true;
    }

    void v_UnBlock()
    {
        b_Blocked_ = false;
    }

    bool b_IsBlocked() const
    {
        return b_Blocked_;
    }

    void v_LinkToPrevious()
    {
        if (b_Blocked_)
        {
            return;
        }
        st_CurrentEvent_.b_Chained = true;
    }

    bool b_IsLinkedToPrevious() const 
    {
        return st_CurrentEvent_.b_Chained;
    }

};

#endif
