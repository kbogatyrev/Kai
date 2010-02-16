//==============================================================================
//
//    Copyright (c) Konstantin Bogatyrev, 2001
//    
//    Facility: Kai 1.2
//
//    Module description: common data types.
//
//    $Id: KaiTypes.h,v 1.31 2008-01-13 20:44:05 kostya Exp $
//
//==============================================================================

#pragma once

//
// Application-specific types 
//
namespace KaiLibMFC
{
enum et_SplitType
{
    ec_SplitTypeFront    = 10101,
    ec_NoSplit,
    ec_SplitNoHyphen,               // join with next token
    ec_Hyphenated,                  // left side of hyphenated word
    ec_SplitTypeBack    = ec_Hyphenated + 1
};

enum ET_Alignment
{
    ec_AlignmentFront =        1000,
    ec_Left,
    ec_Center,
    ec_Right,
    ec_RightJustified,
    ec_AlignmentBack
};

enum ET_ScreenItemType
{
    ec_ScreenItemFront =    1100,
    ec_Text,
    ec_Diacritic,
    ec_ScreenItemBack
};

//
// List of Windows charsets; only ANSI and Russian
// are supported at this time
//
enum ET_Charset
{
    ec_CharsetFront       =     1300,
    ec_ANSICharset        =     ANSI_CHARSET,
    ec_BalticCharset      =     BALTIC_CHARSET,
    ec_ChineseBig5Charset =     CHINESEBIG5_CHARSET,
    ec_DefaultCharset     =     DEFAULT_CHARSET,
    ec_EastEuropeCharset  =     EASTEUROPE_CHARSET,
    ec_GB2312Charset      =     GB2312_CHARSET,
    ec_GreekCharset       =     GREEK_CHARSET,
    ec_HangulCharset      =     HANGUL_CHARSET,
    ec_MacCharset         =     MAC_CHARSET,
    ec_OEMCharset         =     OEM_CHARSET,
    ec_RussianCharset     =     RUSSIAN_CHARSET,
    ec_ShiftjisCharset    =     SHIFTJIS_CHARSET,
    ec_SymbolCharset      =     SYMBOL_CHARSET,
    ec_TurkishCharset     =     TURKISH_CHARSET,
    ec_JohabCharset       =     JOHAB_CHARSET,
    ec_HebrewCharset      =     HEBREW_CHARSET,
    ec_ArabicCharset      =     ARABIC_CHARSET,
    ec_ThaiCharset        =     THAI_CHARSET, 
    ec_CharsetBack        =     ec_ThaiCharset+1
};

//
// Paragraph item type
//
enum ET_ItemType
{
    ec_ItemTypeFront    = 1400,
    ec_Word,
    ec_WordBreak,
    ec_Punctuation,
    ec_ItemTypeBack
};

enum ET_Direction
{
    ec_DirectionFront    = 1500,
    ec_DirectionUp,
    ec_DirectionDown,
    ec_DirectionBack
};

enum ET_DialogType
{
    ec_DialogTypeFront = 1600,
    ec_DialogTypeFind,
    ec_DialogTypeReplace,
    ec_DialogTypeBack
};

enum ET_LineSpacingType
{
    ec_LineSpacingFront = 1700,
    ec_LineSpacingAuto,
    ec_LineSpacingLines,
    ec_LineSpacingPoints,
    ec_LineSpacingBack
};

//
// Undoable changes
//
enum ET_UndoEventType
{
    ec_UndoEventFront    = 1800,
    ec_UndoInsertChar,
    ec_UndoDeleteChar,
    ec_UndoReplaceChar,
    ec_UndoInsertText,
    ec_UndoDeleteText,
    ec_UndoReplaceText,
    ec_UndoInsertDiacritic,
    ec_UndoDeleteDiacritic,
    ec_UndoMergeParagraphs,
    ec_UndoSplitParagraph,
    ec_UndoChangeFont,
    ec_UndoParagraphFormatting,
    ec_UndoEventBack
};

enum ET_FontModifier
{
    ec_FontModifierFront    = 1900,
    ec_Bold,
    ec_Italic,
    ec_Underline,
    ec_FontModifierBack
};

enum { UNDEFINED = 0x10000 };
enum { MAX_PARAGRAPH_LENGTH = 10000 };
enum { MAX_BUF_LENGTH = 1000000 };

enum ET_Color
{
    ec_Yellow = RGB (255, 255, 0),
    ec_DarkYellow = RGB (128, 128, 0)
};

}   //  namespace KaiLibMFC
