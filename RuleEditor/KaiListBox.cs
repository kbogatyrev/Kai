using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Forms;
using System.Text;

namespace KaiListBox
{
    public partial class KaiListBox : ListBox
    {
        const int WM_SETFOCUS = 0x0007;

        bool bDisableEdit;
        public bool DisableEdit
        {
            set
            {
                this.bDisableEdit = value;
                if (this.bDisableEdit)
                {
                    this.SelectedItem = null;
                }
            }

            get
            {
                return this.bDisableEdit;
            }
        }

        public KaiListBox()
        {
            InitializeComponent();
        }

        protected override void WndProc (ref Message m)
        {
            if (m.Msg == WM_SETFOCUS)
            {
                if (bDisableEdit)
                {
                    return;
                }
            }
            base.WndProc(ref m);
        }

        private void KaiListBox_SelectedIndexChanged (object sender, EventArgs e)
        {
            if (bDisableEdit)
            {
                SelectedItem = null;
            }
        }
    }
}
