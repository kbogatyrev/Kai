using System.Collections.Generic;
using System.Collections;

namespace RuleEditor
{
    public enum EDataType
    {
        InputTypeUndefined = 4001,
        Stress = 4002,
        LeftContext = 4003,
        RightContext = 4004,
        Exceptions = 4005,
        OutputModifiers = 4006
    }

    public class Settings
    {
        public string sLanguage;
        public Dictionary<string, string> dictStressTypes;
        public Dictionary<string, string> dictLeftContexts;
        public Dictionary<string, string> dictRightContexts;
        public Dictionary<string, string> dictExceptions;
        public Dictionary<string, string> dictOutputSymbols;
        public Dictionary<string, string> dictOutputModifiers;
        public Settings()
        {
            dictStressTypes = new Dictionary<string,string>();
            dictLeftContexts = new Dictionary<string, string>();
            dictRightContexts = new Dictionary<string, string>();
            dictExceptions = new Dictionary<string, string>();
            dictOutputSymbols = new Dictionary<string, string>();
            dictOutputModifiers = new Dictionary<string, string>();
        }
    }

    public struct StRule
    {
//        public int iNumber;
        public string sInput;
        public Dictionary<string, string> dictStressPositions;
        public Dictionary<string, string> dictLeftContexts;
        public Dictionary<string, string> dictRightContexts;
        public Dictionary<string, string> dictExceptions;
        public Dictionary<string, string> dictOutputSymbols;
        public Dictionary<string, string> dictOutputModifiers;
    }


}
