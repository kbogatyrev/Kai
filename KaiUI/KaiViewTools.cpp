//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2007
//    
//    Facility: Kai 1.2
//
//    Module description:    Interface for c# tools library.
//
//    $Id: KaiViewTools.cpp,v 1.2 2008-05-26 23:33:13 kostya Exp $
//    
//==============================================================================

//
// Disable compiler warning 4786 -- MSVC issue
//
#pragma warning(disable: 4786)

#using <mscorlib.dll>
//#using <KaiLibCS.dll>
#include "KaiStdAfx.h"
#include "KaiView.h"

using namespace System;
using namespace KaiLibCS;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef new

void CKaiView::v_Transcribe_()
{
//    Transcriber * pco_ = new Transcriber();
//    pco_->ReadXMLFile ("C:\test.xml");
}  // v_Transcribe()
