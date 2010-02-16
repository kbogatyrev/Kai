using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace KaiListView
{
    public partial class KaiListView : ListView
    {
        [DllImport("user32.dll", EntryPoint="SystemParametersInfo", SetLastError = true)]
        public static extern bool SystemParametersInfoGet (uint uiAction, 
                                                           uint uiParam, 
                                                           ref uint ruiVparam, 
                                                           uint uiInit);

        [DllImport("user32.dll", EntryPoint = "SystemParametersInfo", SetLastError = true)]
        public static extern bool SystemParametersInfoSet (uint uiAction, 
                                                           uint uiParam, 
                                                           ref uint ruiVparam, 
                                                           uint uiInit);

        [StructLayout(LayoutKind.Sequential)]
        struct RECT
        {
            public int left;
            public int top;
            public int right;
            public int bottom;
        }

        [DllImport("user32.dll")]
        private static extern int SendMessage (IntPtr hWnd, 
                                               int wMsg, 
                                               int wParam, 
                                               ref RECT lParam);

        const int WM_VSCROLL = 0x0115;
        const int LVM_FIRST = 0x1000;
        const int LVM_GETSUBITEMRECT = (LVM_FIRST + 56);
        const int LVIR_BOUNDS = 0;
        const int LVIR_ICON = 1;
        const uint SPI_GETLISTBOXSMOOTHSCROLLING = 0x1006;

        bool bMSListViewBugPresent;

        Button editBtn; 
        public bool EditButtonVisible
        {
            get
            {
                return editBtn.Visible;
            }
            set
            {
                editBtn.Visible = value;
            }
        }

        public KaiListView ()
        {
            InitializeComponent();

            editBtn = new Button();
            Controls.Add (editBtn);
            editBtn.Visible = false;
            bMSListViewBugPresent = false;
            CheckForMSScrollBug();
        }

        protected override void WndProc (ref Message m)
        {
            if (m.Msg == 0x0115)
            {
                editBtn.Visible = false;
                if (bMSListViewBugPresent)
                {
                    Invalidate();
                    Update();
                }
            }
            base.WndProc(ref m);
        }

        private void KaiListView_MouseUp (object sender, MouseEventArgs e)
        {
//            ShowEditButton(e);
            ListViewItem clickedItem = GetItemAt (5, e.Y);
            if (clickedItem != null)
            {
                clickedItem.Selected = true;
                clickedItem.Focused = true;
            }
        }

        protected void CheckForMSScrollBug()
        {
            uint uiBugPresent = 0;
            OperatingSystem os = Environment.OSVersion;
            if (5 != os.Version.Major)
            {
                bMSListViewBugPresent = false;
                return;
            }

            bool bRet = SystemParametersInfoGet (SPI_GETLISTBOXSMOOTHSCROLLING, 
                                                 0, 
                                                 ref uiBugPresent, 
                                                 0);
            if ((!bRet) || (0 != uiBugPresent))
            {
                bMSListViewBugPresent = true;
            }
            else
            {
                bMSListViewBugPresent = false;
            }
        }

        private void KaiListView_DrawColumnHeader (object sender, DrawListViewColumnHeaderEventArgs e)
        {
            StringFormat sf = new StringFormat();
            sf.LineAlignment = StringAlignment.Center;

            if (0 == e.ColumnIndex)
            {
                sf.Alignment = StringAlignment.Center;
            }
            else
            {
                switch (e.Header.TextAlign)
                {
                    case HorizontalAlignment.Left:
                    {
                        sf.Alignment = StringAlignment.Near;
                        break;
                    }
                    case HorizontalAlignment.Center:
                    {
                        sf.Alignment = StringAlignment.Center;
                        break;
                    }
                    case HorizontalAlignment.Right:
                    {
                        sf.Alignment = StringAlignment.Far;
                        break;
                    }
                }
            }

            e.DrawBackground();
            Font f = new Font("Arial", 9, FontStyle.Bold);
            e.Graphics.DrawString(e.Header.Text, f, SystemBrushes.WindowText, e.Bounds, sf);
        }

        private void KaiListView_DrawItem (object sender, DrawListViewItemEventArgs e)
        {
            if ((e.State & ListViewItemStates.Selected) != 0)
            {
                e.Graphics.FillRectangle(SystemBrushes.Highlight, e.Bounds);
                e.DrawFocusRectangle();
            }
        }

        private void KaiListView_DrawSubItem (object sender, DrawListViewSubItemEventArgs e)
        {
            StringFormat sf = new StringFormat (StringFormatFlags.NoWrap);
            sf.LineAlignment = StringAlignment.Far;
            sf.Trimming = StringTrimming.EllipsisCharacter;

            if (0 == e.ColumnIndex)
            {
                sf.Alignment = StringAlignment.Center;
            }
            else
            {
                switch (e.Header.TextAlign)
                {
                    case HorizontalAlignment.Center:
                    {
                        sf.Alignment = StringAlignment.Center;
                        break;
                    }
                    case HorizontalAlignment.Right:
                    {
                        sf.Alignment = StringAlignment.Far;
                        break;
                    }
                }
            }

            if ((e.ItemState & ListViewItemStates.Selected) != 0)
            {
                e.Graphics.DrawString (e.SubItem.Text, 
                                       e.Item.Font, 
                                       SystemBrushes.HighlightText, 
                                       e.Bounds, 
                                       sf);
            }
            else
            {
                e.Graphics.DrawString (e.SubItem.Text, 
                                       e.Item.Font, 
                                       SystemBrushes.WindowText, 
                                       e.Bounds, 
                                       sf);
            }
        }

        private void ShowEditButton (MouseEventArgs e)
        {
            editBtn.Visible = false;
            RECT stRect;
            ListViewItem lvItem;
            int intLVSubItemIndex = 0;
            ListViewItem.ListViewSubItem lvSubItem = null;
            lvItem = FocusedItem;
            int intSendMessage;

            stRect = new RECT();
            for (int iItem = 0;
                 (0 == intLVSubItemIndex) && (iItem < Items.Count);
                 ++iItem)
            {
                intLVSubItemIndex = 0;
                lvItem = Items[iItem];
                for (int iSubItem = 1; iSubItem < lvItem.SubItems.Count; ++iSubItem)
                {
                    lvSubItem = lvItem.SubItems[iSubItem];
                    stRect.top = iSubItem;
                    stRect.left = LVIR_BOUNDS;
                    intSendMessage = SendMessage (Handle,
                                                  LVM_GETSUBITEMRECT,
                                                  lvItem.Index,
                                                  ref stRect);
                    if (e.Y < stRect.top || e.Y > stRect.bottom)
                    {
                        intLVSubItemIndex = 0;
                        continue;
                    }
                    if (e.X < stRect.left)
                    {
                        lvSubItem = lvItem.SubItems[0];
                        intLVSubItemIndex = 0;
                        break;
                    }
                    else if (e.X >= stRect.left & e.X <= stRect.right)
                    {
                        intLVSubItemIndex = iSubItem;
                        break;
                    }
                    else
                    {
                        intLVSubItemIndex = 0;
                        lvSubItem = null;
                    }
                }
            }

            if (intLVSubItemIndex < 1)
            {
                return;
            }

            editBtn.Width = Math.Min(stRect.right - stRect.left - 10, 30);
            editBtn.Height = stRect.bottom - stRect.top;
            editBtn.Font = new Font("Microsoft Sans Serif", 5F, FontStyle.Regular, GraphicsUnit.Point);
            editBtn.Text = "•••";
            editBtn.Location = new Point(stRect.right - editBtn.Width, stRect.top);
            editBtn.Visible = true;
        }

        private void KaiListView_ItemDrag(object sender, ItemDragEventArgs e)
        {
            DoDragDrop (SelectedItems, DragDropEffects.Move);
        }

        private void KaiListView_DragEnter(object sender, DragEventArgs e)
        {
            int iFormats = e.Data.GetFormats().Length - 1;
            int iAt;
            for (iAt = 0; iAt <= iFormats; ++iAt)
            {
                if (e.Data.GetFormats()[iAt].
                    Equals("System.Windows.Forms.ListView+SelectedListViewItemCollection"))
                {
                    e.Effect = DragDropEffects.Move;
                }
            }
        }

        private void KaiListView_DragDrop(object sender, DragEventArgs e)
        {
            if (SelectedItems.Count == 0)
            {
                return;
            }

            Point pt = PointToClient (new Point(e.X, e.Y));
            ListViewItem lviDragTarget = GetItemAt (pt.X, pt.Y);
            if (null == lviDragTarget)
            {
                return;
            }

            ListViewItem[] arrlviSelected = new ListViewItem[SelectedItems.Count];
            for (int iItem = 0; iItem <= SelectedItems.Count - 1; iItem++)
            {
                arrlviSelected[iItem] = SelectedItems[iItem];
            }

            int iInsertAt = lviDragTarget.Index;
            for (int iItem = 0; iItem < arrlviSelected.GetLength(0); iItem++)
            {
                ListViewItem lviDragItem = arrlviSelected[iItem];
                if (iInsertAt == lviDragItem.Index)
                {
                    return;
                }
                if (lviDragTarget.Index > lviDragItem.Index)
                {
                    iInsertAt++;
                }
                else
                {
                    iInsertAt = lviDragTarget.Index + iItem;
                }

                Items.Insert(iInsertAt, (ListViewItem)lviDragItem.Clone());
                Items.Remove(lviDragItem);
            }

        }

        private void KaiListView_ColumnClick(object sender, ColumnClickEventArgs e)
        {            
            if (SortOrder.None == this.Sorting)
            {
                this.Sorting = SortOrder.Ascending;
            }
            else if (SortOrder.Ascending == this.Sorting)
            {
                this.Sorting = SortOrder.Descending;
            }
            else
            {
                this.Sorting = SortOrder.Ascending;
            }
            this.ListViewItemSorter = new ListViewItemComparer(e.Column);
        }

        //
        // Helper class for listview column sorting
        //
        class ListViewItemComparer : System.Collections.IComparer
        {
            private int m_iCol;
            public ListViewItemComparer()
            {
                m_iCol = 0;
            }
            public ListViewItemComparer(int iCol)
            {
                m_iCol = iCol;
            }
            public int Compare(object x, object y)
            {
                int iResult = String.Compare(((ListViewItem)x).SubItems[m_iCol].Text,
                                             ((ListViewItem)y).SubItems[m_iCol].Text);
                if (SortOrder.Descending == ((ListViewItem)x).ListView.Sorting)
                {
                    iResult *= -1;
                }
                return iResult;
            }
        }
    }

}
