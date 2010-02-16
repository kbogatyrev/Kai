namespace KaiListView
{
    partial class KaiListView
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose (bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent ()
        {
            this.SuspendLayout();
            // 
            // KaiListView
            // 
            this.AllowDrop = true;
            this.Dock = System.Windows.Forms.DockStyle.Top;
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold);
            this.FullRowSelect = true;
            this.GridLines = true;
            this.MultiSelect = false;
            this.OwnerDraw = true;
            this.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.View = System.Windows.Forms.View.Details;
            this.DrawColumnHeader += new System.Windows.Forms.DrawListViewColumnHeaderEventHandler(this.KaiListView_DrawColumnHeader);
            this.DrawItem += new System.Windows.Forms.DrawListViewItemEventHandler(this.KaiListView_DrawItem);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.KaiListView_MouseUp);
            this.DragDrop += new System.Windows.Forms.DragEventHandler(this.KaiListView_DragDrop);
            this.ColumnClick += new System.Windows.Forms.ColumnClickEventHandler(this.KaiListView_ColumnClick);
            this.DragEnter += new System.Windows.Forms.DragEventHandler(this.KaiListView_DragEnter);
            this.ItemDrag += new System.Windows.Forms.ItemDragEventHandler(this.KaiListView_ItemDrag);
            this.DrawSubItem += new System.Windows.Forms.DrawListViewSubItemEventHandler(this.KaiListView_DrawSubItem);
            this.ResumeLayout(false);

        }

        #endregion

    }
}
