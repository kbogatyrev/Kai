//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2005
//    
//    Facility: Kai 1.2
//
//    Module description:.
//
//    $Id: KaiUndoStack.cpp,v 1.13 2007-11-25 19:38:10 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#include "KaiStdAfx.h"
#include "Kai.h"
#include "KaiFontDescriptor.h"
#include "KaiParagraph.h"
#include "KaiUndoStack.h"

#pragma warning(disable: 4786)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

CT_UndoStack::CT_UndoStack()
{
    st_CurrentEvent_.Clear();
    b_Blocked_ = false;
}

CT_UndoStack::~CT_UndoStack()
{
}

void CT_UndoStack::v_AddSimpleEvent (ET_UndoEventType eo_type, 
                                     int i_paragraph,
                                     int i_offset, 
                                     int i_length,
                                     bool b_selection)
{
    if (b_Blocked_)
    {
        return;
    }
    CT_FontDescriptor * pco_font = NULL;
    ST_UndoableEvent st_e (eo_type, 
                           i_paragraph, 
                           i_offset, 
                           i_length,
                           b_selection);
    vec_Events_.push_back (st_e);
}

void CT_UndoStack::v_PopLastEvent()
{
    vector<ST_UndoableEvent>::reverse_iterator rit_ = vec_Events_.rbegin();
    st_CurrentEvent_ = *rit_;
}

void CT_UndoStack::v_AddEvent()
{
    if (!b_Blocked_)
    {
        vec_Events_.push_back (st_CurrentEvent_);
        st_CurrentEvent_.Clear();
    }
}

void CT_UndoStack::v_RemoveLastEvent()
{
    if (vec_Events_.empty())
    {
        ERROR_TRACE (_T("Undo stack empty"));
        return;
    }

    vec_Events_.pop_back();
    st_CurrentEvent_.Clear();
}

bool CT_UndoStack::b_EventsAvailable()
{
    return vec_Events_.empty() ? false : true;
}
