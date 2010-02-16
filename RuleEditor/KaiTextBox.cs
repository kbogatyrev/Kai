using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Forms;
using System.Text;

namespace KaiTextBox
{
    public partial class KaiTextBox : TextBox
    {
        const int WM_GETDLGCODE = 0x0087;

        public KaiTextBox ()
        {
            InitializeComponent();
        }

        public KaiTextBox (IContainer container)
        {
            container.Add(this);

            InitializeComponent();
        }

        protected override void WndProc (ref Message m)
        {
            if (m.Msg == WM_GETDLGCODE)
            {
            }
            base.WndProc(ref m);
        }

    }
}
