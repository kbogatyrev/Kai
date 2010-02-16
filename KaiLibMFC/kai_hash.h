#pragma once

#include "StdAfx.h"

namespace KaiLibMFC
{
    AFX_EXT_CLASS unsigned long int hash (register unsigned char  *k, 
                                          register unsigned long int length, 
                                          register unsigned long int initval);
}
