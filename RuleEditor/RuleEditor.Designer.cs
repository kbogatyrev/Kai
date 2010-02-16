namespace RuleEditor
{
    partial class RuleEditor
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>

        private void InitializeComponent ()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RuleEditor));
            this.buttonClose = new System.Windows.Forms.Button();
            this.buttonDelete = new System.Windows.Forms.Button();
            this.buttonEdit = new System.Windows.Forms.Button();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.recentFilesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.buttonAdd = new System.Windows.Forms.Button();
            this.kaiListView = new KaiListView.KaiListView();
            this.columnHeaderInput = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderStress = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderLC = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderRC = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderException = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderOutput = new System.Windows.Forms.ColumnHeader();
            this.columnHeaderModifiers = new System.Windows.Forms.ColumnHeader();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // buttonClose
            // 
            this.buttonClose.Location = new System.Drawing.Point(557, 384);
            this.buttonClose.Margin = new System.Windows.Forms.Padding(2);
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.Size = new System.Drawing.Size(65, 22);
            this.buttonClose.TabIndex = 1;
            this.buttonClose.Text = "Close";
            this.buttonClose.UseVisualStyleBackColor = true;
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // buttonDelete
            // 
            this.buttonDelete.Enabled = false;
            this.buttonDelete.Location = new System.Drawing.Point(491, 384);
            this.buttonDelete.Margin = new System.Windows.Forms.Padding(2);
            this.buttonDelete.Name = "buttonDelete";
            this.buttonDelete.Size = new System.Drawing.Size(65, 22);
            this.buttonDelete.TabIndex = 2;
            this.buttonDelete.Text = "Delete rule";
            this.buttonDelete.UseVisualStyleBackColor = true;
            this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
            // 
            // buttonEdit
            // 
            this.buttonEdit.Enabled = false;
            this.buttonEdit.Location = new System.Drawing.Point(425, 384);
            this.buttonEdit.Margin = new System.Windows.Forms.Padding(2);
            this.buttonEdit.Name = "buttonEdit";
            this.buttonEdit.Size = new System.Drawing.Size(65, 22);
            this.buttonEdit.TabIndex = 3;
            this.buttonEdit.Text = "Edit rule";
            this.buttonEdit.UseVisualStyleBackColor = true;
            this.buttonEdit.Click += new System.EventHandler(this.buttonEdit_Click);
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Padding = new System.Windows.Forms.Padding(3, 2, 0, 2);
            this.menuStrip1.Size = new System.Drawing.Size(1002, 24);
            this.menuStrip1.TabIndex = 4;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.toolStripSeparator1,
            this.recentFilesToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "&File";
            this.fileToolStripMenuItem.DropDownOpening += new System.EventHandler(this.fileToolStripMenuItem_DropDownOpening);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.openToolStripMenuItem.Text = "&Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Enabled = false;
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.saveToolStripMenuItem.Text = "&Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Enabled = false;
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.saveAsToolStripMenuItem.Text = "Save &As...";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(133, 6);
            // 
            // recentFilesToolStripMenuItem
            // 
            this.recentFilesToolStripMenuItem.Enabled = false;
            this.recentFilesToolStripMenuItem.Name = "recentFilesToolStripMenuItem";
            this.recentFilesToolStripMenuItem.Size = new System.Drawing.Size(136, 22);
            this.recentFilesToolStripMenuItem.Text = "&Recent Files";
            // 
            // buttonAdd
            // 
            this.buttonAdd.Location = new System.Drawing.Point(360, 384);
            this.buttonAdd.Margin = new System.Windows.Forms.Padding(2);
            this.buttonAdd.Name = "buttonAdd";
            this.buttonAdd.Size = new System.Drawing.Size(65, 22);
            this.buttonAdd.TabIndex = 5;
            this.buttonAdd.Text = "Add rule";
            this.buttonAdd.UseVisualStyleBackColor = true;
            this.buttonAdd.Click += new System.EventHandler(this.buttonAdd_Click);
            // 
            // kaiListView
            // 
            this.kaiListView.Alignment = System.Windows.Forms.ListViewAlignment.Left;
            this.kaiListView.AllowDrop = true;
            this.kaiListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderInput,
            this.columnHeaderStress,
            this.columnHeaderLC,
            this.columnHeaderRC,
            this.columnHeaderException,
            this.columnHeaderOutput,
            this.columnHeaderModifiers});
            this.kaiListView.Dock = System.Windows.Forms.DockStyle.Top;
            this.kaiListView.EditButtonVisible = false;
            this.kaiListView.Font = new System.Drawing.Font("Times New Roman", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.kaiListView.FullRowSelect = true;
            this.kaiListView.GridLines = true;
            this.kaiListView.Location = new System.Drawing.Point(0, 24);
            this.kaiListView.Margin = new System.Windows.Forms.Padding(2);
            this.kaiListView.MultiSelect = false;
            this.kaiListView.Name = "kaiListView";
            this.kaiListView.OwnerDraw = true;
            this.kaiListView.Size = new System.Drawing.Size(1002, 325);
            this.kaiListView.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.kaiListView.TabIndex = 0;
            this.kaiListView.UseCompatibleStateImageBehavior = false;
            this.kaiListView.View = System.Windows.Forms.View.Details;
            this.kaiListView.SelectedIndexChanged += new System.EventHandler(this.kaiListView_SelectedIndexChanged);
            this.kaiListView.DoubleClick += new System.EventHandler(this.kaiListView_DoubleClick);
            // 
            // columnHeaderInput
            // 
            this.columnHeaderInput.Text = "Input";
            // 
            // columnHeaderStress
            // 
            this.columnHeaderStress.Text = "Stress";
            this.columnHeaderStress.Width = 125;
            // 
            // columnHeaderLC
            // 
            this.columnHeaderLC.Text = "Left Context";
            this.columnHeaderLC.Width = 200;
            // 
            // columnHeaderRC
            // 
            this.columnHeaderRC.Text = "Right Context";
            this.columnHeaderRC.Width = 200;
            // 
            // columnHeaderException
            // 
            this.columnHeaderException.Text = "Exceptions";
            this.columnHeaderException.Width = 200;
            // 
            // columnHeaderOutput
            // 
            this.columnHeaderOutput.Text = "Output";
            this.columnHeaderOutput.Width = 80;
            // 
            // columnHeaderModifiers
            // 
            this.columnHeaderModifiers.Text = "Modifiers";
            this.columnHeaderModifiers.Width = 125;
            // 
            // RuleEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1002, 405);
            this.Controls.Add(this.buttonAdd);
            this.Controls.Add(this.buttonEdit);
            this.Controls.Add(this.buttonDelete);
            this.Controls.Add(this.buttonClose);
            this.Controls.Add(this.kaiListView);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MaximizeBox = false;
            this.MinimumSize = new System.Drawing.Size(341, 272);
            this.Name = "RuleEditor";
            this.Text = "Transcription Rules Editor";
            this.SizeChanged += new System.EventHandler(this.RuleEditor_SizeChanged);
            this.Shown += new System.EventHandler(this.RuleEditor_Shown);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.RuleEditor_FormClosing);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private KaiListView.KaiListView kaiListView;
        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.Button buttonDelete;
        private System.Windows.Forms.Button buttonEdit;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.Button buttonAdd;
        private System.Windows.Forms.ColumnHeader columnHeaderInput;
        private System.Windows.Forms.ColumnHeader columnHeaderStress;
        private System.Windows.Forms.ColumnHeader columnHeaderLC;
        private System.Windows.Forms.ColumnHeader columnHeaderRC;
        private System.Windows.Forms.ColumnHeader columnHeaderException;
        private System.Windows.Forms.ColumnHeader columnHeaderOutput;
        private System.Windows.Forms.ColumnHeader columnHeaderModifiers;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem recentFilesToolStripMenuItem;


    }

}

