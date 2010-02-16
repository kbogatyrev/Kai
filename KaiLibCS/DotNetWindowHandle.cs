using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace KaiLibCS
{
    public class DotNetWindowHandle : System.Windows.Forms.IWin32Window
    {
        public DotNetWindowHandle (int iHandle)
        {
            pHandle_ = new IntPtr (iHandle);
        }

        public IntPtr Handle
        {
            get { return pHandle_; }
        }

        private IntPtr pHandle_;
    }
}
