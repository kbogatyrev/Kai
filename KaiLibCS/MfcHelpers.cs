using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace KaiLibCS
{
    public class MfcHelpers
    {
        [DllImport("KaiLibMFC.dll", EntryPoint = "LogError", CharSet = CharSet.Auto)]
        private static extern int LogErrorPinvoke (string sFile, 
                                                   int iLine, 
                                                   string sMethod, 
                                                   string sMsg);
        
        public static void LogError (string sMsg)
        {
            StackTrace st = new StackTrace(new StackFrame(true));
            StackFrame sf = st.GetFrame(0);
            string sFile = sf.GetFileName();
            string sMethod = sf.GetMethod().Name;
            int iRet = LogErrorPinvoke (sFile, sf.GetFileLineNumber(), sMethod, sMsg);
        }

        [DllImport("KaiLibMFC.dll", EntryPoint = "GetRegistryPath", CharSet = CharSet.Auto)]
        private static extern string GetRegistryPathPinvoke();

        public static string GetRegistryPath()
        {
            return GetRegistryPathPinvoke();
        }

    }
}
