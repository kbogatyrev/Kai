namespace RuleEditor
{
    partial class RuleDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RuleDialog));
            this.labelInput = new System.Windows.Forms.Label();
            this.labelStress = new System.Windows.Forms.Label();
            this.labelLContext = new System.Windows.Forms.Label();
            this.labelRContext = new System.Windows.Forms.Label();
            this.buttonOK = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.buttonAddStress = new System.Windows.Forms.Button();
            this.buttonDeleteStress = new System.Windows.Forms.Button();
            this.buttonAddLeftContext = new System.Windows.Forms.Button();
            this.buttonDeleteLeftContext = new System.Windows.Forms.Button();
            this.buttonAddRightContext = new System.Windows.Forms.Button();
            this.buttonDeleteRightContext = new System.Windows.Forms.Button();
            this.comboBox = new System.Windows.Forms.ComboBox();
            this.textBox = new System.Windows.Forms.TextBox();
            this.comboBoxOut = new System.Windows.Forms.ComboBox();
            this.textBoxInput = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBoxInput = new System.Windows.Forms.GroupBox();
            this.buttonDeleteExceptions = new System.Windows.Forms.Button();
            this.buttonAddException = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonAddOutputModifier = new System.Windows.Forms.Button();
            this.buttonDeleteOutputModifiers = new System.Windows.Forms.Button();
            this.groupBoxOutput = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.kaiListBoxRightContext = new KaiListBox.KaiListBox();
            this.kaiListBoxLeftContext = new KaiListBox.KaiListBox();
            this.kaiListBoxStress = new KaiListBox.KaiListBox();
            this.kaiListBoxExceptions = new KaiListBox.KaiListBox();
            this.kaiListBoxOutputModifiers = new KaiListBox.KaiListBox();
            this.groupBoxInput.SuspendLayout();
            this.groupBoxOutput.SuspendLayout();
            this.SuspendLayout();
            // 
            // labelInput
            // 
            this.labelInput.AutoSize = true;
            this.labelInput.Location = new System.Drawing.Point(20, 32);
            this.labelInput.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelInput.Name = "labelInput";
            this.labelInput.Size = new System.Drawing.Size(69, 13);
            this.labelInput.TabIndex = 7;
            this.labelInput.Text = "Input symbol:";
            this.labelInput.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // labelStress
            // 
            this.labelStress.AutoSize = true;
            this.labelStress.Location = new System.Drawing.Point(19, 59);
            this.labelStress.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelStress.Name = "labelStress";
            this.labelStress.Size = new System.Drawing.Size(39, 13);
            this.labelStress.TabIndex = 8;
            this.labelStress.Text = "Stress:";
            this.labelStress.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // labelLContext
            // 
            this.labelLContext.AutoSize = true;
            this.labelLContext.Location = new System.Drawing.Point(20, 128);
            this.labelLContext.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelLContext.Name = "labelLContext";
            this.labelLContext.Size = new System.Drawing.Size(67, 13);
            this.labelLContext.TabIndex = 9;
            this.labelLContext.Text = "Left Context:";
            this.labelLContext.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // labelRContext
            // 
            this.labelRContext.AutoSize = true;
            this.labelRContext.Location = new System.Drawing.Point(20, 197);
            this.labelRContext.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.labelRContext.Name = "labelRContext";
            this.labelRContext.Size = new System.Drawing.Size(74, 13);
            this.labelRContext.TabIndex = 10;
            this.labelRContext.Text = "Right Context:";
            this.labelRContext.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // buttonOK
            // 
            this.buttonOK.Enabled = false;
            this.buttonOK.Location = new System.Drawing.Point(296, 496);
            this.buttonOK.Margin = new System.Windows.Forms.Padding(2);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(58, 21);
            this.buttonOK.TabIndex = 12;
            this.buttonOK.Text = "OK";
            this.buttonOK.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            this.buttonOK.UseVisualStyleBackColor = true;
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.Location = new System.Drawing.Point(356, 496);
            this.buttonCancel.Margin = new System.Windows.Forms.Padding(2);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(58, 21);
            this.buttonCancel.TabIndex = 13;
            this.buttonCancel.Text = "Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // buttonAddStress
            // 
            this.buttonAddStress.Location = new System.Drawing.Point(350, 77);
            this.buttonAddStress.Margin = new System.Windows.Forms.Padding(2);
            this.buttonAddStress.Name = "buttonAddStress";
            this.buttonAddStress.Size = new System.Drawing.Size(58, 20);
            this.buttonAddStress.TabIndex = 14;
            this.buttonAddStress.Text = "Add";
            this.buttonAddStress.UseVisualStyleBackColor = true;
            this.buttonAddStress.Click += new System.EventHandler(this.buttonAddStress_Click);
            // 
            // buttonDeleteStress
            // 
            this.buttonDeleteStress.Enabled = false;
            this.buttonDeleteStress.Location = new System.Drawing.Point(350, 100);
            this.buttonDeleteStress.Margin = new System.Windows.Forms.Padding(2);
            this.buttonDeleteStress.Name = "buttonDeleteStress";
            this.buttonDeleteStress.Size = new System.Drawing.Size(58, 20);
            this.buttonDeleteStress.TabIndex = 15;
            this.buttonDeleteStress.Text = "Delete";
            this.buttonDeleteStress.UseVisualStyleBackColor = true;
            this.buttonDeleteStress.Click += new System.EventHandler(this.buttonDeleteStress_Click);
            // 
            // buttonAddLeftContext
            // 
            this.buttonAddLeftContext.Location = new System.Drawing.Point(354, 145);
            this.buttonAddLeftContext.Margin = new System.Windows.Forms.Padding(2);
            this.buttonAddLeftContext.Name = "buttonAddLeftContext";
            this.buttonAddLeftContext.Size = new System.Drawing.Size(58, 20);
            this.buttonAddLeftContext.TabIndex = 16;
            this.buttonAddLeftContext.Text = "Add";
            this.buttonAddLeftContext.UseVisualStyleBackColor = true;
            this.buttonAddLeftContext.Click += new System.EventHandler(this.buttonAddLeftContext_Click);
            // 
            // buttonDeleteLeftContext
            // 
            this.buttonDeleteLeftContext.Enabled = false;
            this.buttonDeleteLeftContext.Location = new System.Drawing.Point(354, 167);
            this.buttonDeleteLeftContext.Margin = new System.Windows.Forms.Padding(2);
            this.buttonDeleteLeftContext.Name = "buttonDeleteLeftContext";
            this.buttonDeleteLeftContext.Size = new System.Drawing.Size(58, 20);
            this.buttonDeleteLeftContext.TabIndex = 17;
            this.buttonDeleteLeftContext.Text = "Delete";
            this.buttonDeleteLeftContext.UseVisualStyleBackColor = true;
            this.buttonDeleteLeftContext.Click += new System.EventHandler(this.buttonDeleteLeftContext_Click);
            // 
            // buttonAddRightContext
            // 
            this.buttonAddRightContext.Location = new System.Drawing.Point(354, 214);
            this.buttonAddRightContext.Margin = new System.Windows.Forms.Padding(2);
            this.buttonAddRightContext.Name = "buttonAddRightContext";
            this.buttonAddRightContext.Size = new System.Drawing.Size(58, 20);
            this.buttonAddRightContext.TabIndex = 18;
            this.buttonAddRightContext.Text = "Add";
            this.buttonAddRightContext.UseVisualStyleBackColor = true;
            this.buttonAddRightContext.Click += new System.EventHandler(this.buttonAddRightContext_Click);
            // 
            // buttonDeleteRightContext
            // 
            this.buttonDeleteRightContext.Enabled = false;
            this.buttonDeleteRightContext.Location = new System.Drawing.Point(354, 236);
            this.buttonDeleteRightContext.Margin = new System.Windows.Forms.Padding(2);
            this.buttonDeleteRightContext.Name = "buttonDeleteRightContext";
            this.buttonDeleteRightContext.Size = new System.Drawing.Size(58, 20);
            this.buttonDeleteRightContext.TabIndex = 19;
            this.buttonDeleteRightContext.Text = "Delete";
            this.buttonDeleteRightContext.UseVisualStyleBackColor = true;
            this.buttonDeleteRightContext.Click += new System.EventHandler(this.buttonDeleteRightContext_Click);
            // 
            // comboBox
            // 
            this.comboBox.FormattingEnabled = true;
            this.comboBox.Location = new System.Drawing.Point(19, 496);
            this.comboBox.Margin = new System.Windows.Forms.Padding(2);
            this.comboBox.Name = "comboBox";
            this.comboBox.Size = new System.Drawing.Size(38, 21);
            this.comboBox.TabIndex = 20;
            this.comboBox.SelectedIndexChanged += new System.EventHandler(this.comboBox_SelectedIndexChanged);
            this.comboBox.Leave += new System.EventHandler(this.comboBox_Leave);
            this.comboBox.KeyUp += new System.Windows.Forms.KeyEventHandler(this.comboBox_KeyUp);
            // 
            // textBox
            // 
            this.textBox.Location = new System.Drawing.Point(60, 498);
            this.textBox.Margin = new System.Windows.Forms.Padding(2);
            this.textBox.Name = "textBox";
            this.textBox.Size = new System.Drawing.Size(31, 20);
            this.textBox.TabIndex = 24;
            // 
            // comboBoxOut
            // 
            this.comboBoxOut.FormattingEnabled = true;
            this.comboBoxOut.Location = new System.Drawing.Point(104, 370);
            this.comboBoxOut.Margin = new System.Windows.Forms.Padding(2);
            this.comboBoxOut.Name = "comboBoxOut";
            this.comboBoxOut.Size = new System.Drawing.Size(244, 21);
            this.comboBoxOut.TabIndex = 27;
            this.comboBoxOut.SelectedIndexChanged += new System.EventHandler(this.comboBoxOut_SelectedIndexChanged);
            this.comboBoxOut.TextChanged += new System.EventHandler(this.comboBoxOut_TextChanged);
            // 
            // textBoxInput
            // 
            this.textBoxInput.Location = new System.Drawing.Point(92, 32);
            this.textBoxInput.Margin = new System.Windows.Forms.Padding(2);
            this.textBoxInput.Name = "textBoxInput";
            this.textBoxInput.Size = new System.Drawing.Size(256, 20);
            this.textBoxInput.TabIndex = 28;
            this.textBoxInput.TextChanged += new System.EventHandler(this.textBoxInput_TextChanged);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 371);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(77, 13);
            this.label1.TabIndex = 30;
            this.label1.Text = "Output symbol:";
            // 
            // groupBoxInput
            // 
            this.groupBoxInput.Controls.Add(this.buttonDeleteExceptions);
            this.groupBoxInput.Controls.Add(this.buttonAddException);
            this.groupBoxInput.Controls.Add(this.label2);
            this.groupBoxInput.Location = new System.Drawing.Point(11, 9);
            this.groupBoxInput.Margin = new System.Windows.Forms.Padding(2);
            this.groupBoxInput.Name = "groupBoxInput";
            this.groupBoxInput.Padding = new System.Windows.Forms.Padding(2);
            this.groupBoxInput.Size = new System.Drawing.Size(414, 330);
            this.groupBoxInput.TabIndex = 33;
            this.groupBoxInput.TabStop = false;
            this.groupBoxInput.Text = "Input";
            // 
            // buttonDeleteExceptions
            // 
            this.buttonDeleteExceptions.Enabled = false;
            this.buttonDeleteExceptions.Location = new System.Drawing.Point(343, 297);
            this.buttonDeleteExceptions.Margin = new System.Windows.Forms.Padding(2);
            this.buttonDeleteExceptions.Name = "buttonDeleteExceptions";
            this.buttonDeleteExceptions.Size = new System.Drawing.Size(58, 20);
            this.buttonDeleteExceptions.TabIndex = 3;
            this.buttonDeleteExceptions.Text = "Delete";
            this.buttonDeleteExceptions.UseVisualStyleBackColor = true;
            this.buttonDeleteExceptions.Click += new System.EventHandler(this.buttonDeleteExceptions_Click);
            // 
            // buttonAddException
            // 
            this.buttonAddException.Location = new System.Drawing.Point(343, 274);
            this.buttonAddException.Margin = new System.Windows.Forms.Padding(2);
            this.buttonAddException.Name = "buttonAddException";
            this.buttonAddException.Size = new System.Drawing.Size(58, 20);
            this.buttonAddException.TabIndex = 2;
            this.buttonAddException.Text = "Add";
            this.buttonAddException.UseVisualStyleBackColor = true;
            this.buttonAddException.Click += new System.EventHandler(this.buttonAddException_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(9, 258);
            this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 0;
            this.label2.Text = "Exceptons:";
            // 
            // buttonAddOutputModifier
            // 
            this.buttonAddOutputModifier.Location = new System.Drawing.Point(356, 421);
            this.buttonAddOutputModifier.Margin = new System.Windows.Forms.Padding(2);
            this.buttonAddOutputModifier.Name = "buttonAddOutputModifier";
            this.buttonAddOutputModifier.Size = new System.Drawing.Size(58, 20);
            this.buttonAddOutputModifier.TabIndex = 35;
            this.buttonAddOutputModifier.Text = "Add";
            this.buttonAddOutputModifier.UseVisualStyleBackColor = true;
            this.buttonAddOutputModifier.Click += new System.EventHandler(this.buttonAddOutputModifier_Click);
            // 
            // buttonDeleteOutputModifiers
            // 
            this.buttonDeleteOutputModifiers.Enabled = false;
            this.buttonDeleteOutputModifiers.Location = new System.Drawing.Point(356, 444);
            this.buttonDeleteOutputModifiers.Margin = new System.Windows.Forms.Padding(2);
            this.buttonDeleteOutputModifiers.Name = "buttonDeleteOutputModifiers";
            this.buttonDeleteOutputModifiers.Size = new System.Drawing.Size(58, 20);
            this.buttonDeleteOutputModifiers.TabIndex = 36;
            this.buttonDeleteOutputModifiers.Text = "Delete";
            this.buttonDeleteOutputModifiers.UseVisualStyleBackColor = true;
            this.buttonDeleteOutputModifiers.Click += new System.EventHandler(this.buttonDeleteOutputModifiers_Click);
            // 
            // groupBoxOutput
            // 
            this.groupBoxOutput.Controls.Add(this.label3);
            this.groupBoxOutput.Location = new System.Drawing.Point(11, 344);
            this.groupBoxOutput.Margin = new System.Windows.Forms.Padding(2);
            this.groupBoxOutput.Name = "groupBoxOutput";
            this.groupBoxOutput.Padding = new System.Windows.Forms.Padding(2);
            this.groupBoxOutput.Size = new System.Drawing.Size(414, 140);
            this.groupBoxOutput.TabIndex = 37;
            this.groupBoxOutput.TabStop = false;
            this.groupBoxOutput.Text = "Output";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 54);
            this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(52, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Modifiers:";
            // 
            // kaiListBoxRightContext
            // 
            this.kaiListBoxRightContext.DisableEdit = false;
            this.kaiListBoxRightContext.FormattingEnabled = true;
            this.kaiListBoxRightContext.Location = new System.Drawing.Point(22, 214);
            this.kaiListBoxRightContext.Margin = new System.Windows.Forms.Padding(2);
            this.kaiListBoxRightContext.Name = "kaiListBoxRightContext";
            this.kaiListBoxRightContext.Size = new System.Drawing.Size(327, 43);
            this.kaiListBoxRightContext.TabIndex = 26;
            this.kaiListBoxRightContext.SelectedIndexChanged += new System.EventHandler(this.kaiListBoxRightContext_SelectedIndexChanged);
            this.kaiListBoxRightContext.Leave += new System.EventHandler(this.kaiListBoxRightContext_Leave);
            // 
            // kaiListBoxLeftContext
            // 
            this.kaiListBoxLeftContext.DisableEdit = false;
            this.kaiListBoxLeftContext.FormattingEnabled = true;
            this.kaiListBoxLeftContext.Location = new System.Drawing.Point(22, 145);
            this.kaiListBoxLeftContext.Margin = new System.Windows.Forms.Padding(2);
            this.kaiListBoxLeftContext.Name = "kaiListBoxLeftContext";
            this.kaiListBoxLeftContext.Size = new System.Drawing.Size(327, 43);
            this.kaiListBoxLeftContext.TabIndex = 25;
            this.kaiListBoxLeftContext.SelectedIndexChanged += new System.EventHandler(this.kaiListBoxLeftContext_SelectedIndexChanged);
            this.kaiListBoxLeftContext.Leave += new System.EventHandler(this.kaiListBoxLeftContext_Leave);
            // 
            // kaiListBoxStress
            // 
            this.kaiListBoxStress.DisableEdit = false;
            this.kaiListBoxStress.FormattingEnabled = true;
            this.kaiListBoxStress.Location = new System.Drawing.Point(21, 76);
            this.kaiListBoxStress.Margin = new System.Windows.Forms.Padding(2);
            this.kaiListBoxStress.Name = "kaiListBoxStress";
            this.kaiListBoxStress.Size = new System.Drawing.Size(327, 43);
            this.kaiListBoxStress.TabIndex = 21;
            this.kaiListBoxStress.SelectedIndexChanged += new System.EventHandler(this.kaiListBoxStress_SelectedIndexChanged);
            this.kaiListBoxStress.Leave += new System.EventHandler(this.kaiListBoxStress_Leave);
            // 
            // kaiListBoxExceptions
            // 
            this.kaiListBoxExceptions.DisableEdit = false;
            this.kaiListBoxExceptions.FormattingEnabled = true;
            this.kaiListBoxExceptions.Location = new System.Drawing.Point(24, 283);
            this.kaiListBoxExceptions.Margin = new System.Windows.Forms.Padding(2);
            this.kaiListBoxExceptions.Name = "kaiListBoxExceptions";
            this.kaiListBoxExceptions.Size = new System.Drawing.Size(327, 43);
            this.kaiListBoxExceptions.TabIndex = 1;
            this.kaiListBoxExceptions.SelectedIndexChanged += new System.EventHandler(this.kaiListBoxExceptions_SelectedIndexChanged);
            this.kaiListBoxExceptions.Leave += new System.EventHandler(this.kaiListBoxExceptions_Leave);
            // 
            // kaiListBoxOutputModifiers
            // 
            this.kaiListBoxOutputModifiers.DisableEdit = false;
            this.kaiListBoxOutputModifiers.FormattingEnabled = true;
            this.kaiListBoxOutputModifiers.Location = new System.Drawing.Point(24, 414);
            this.kaiListBoxOutputModifiers.Margin = new System.Windows.Forms.Padding(2);
            this.kaiListBoxOutputModifiers.Name = "kaiListBoxOutputModifiers";
            this.kaiListBoxOutputModifiers.Size = new System.Drawing.Size(327, 56);
            this.kaiListBoxOutputModifiers.TabIndex = 38;
            this.kaiListBoxOutputModifiers.SelectedIndexChanged += new System.EventHandler(this.kaiListBoxOutputModifiers_SelectedIndexChanged);
            this.kaiListBoxOutputModifiers.Leave += new System.EventHandler(this.kaiListBoxOutputModifiers_Leave);
            // 
            // RuleDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(436, 526);
            this.Controls.Add(this.buttonDeleteOutputModifiers);
            this.Controls.Add(this.buttonAddOutputModifier);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBoxInput);
            this.Controls.Add(this.comboBoxOut);
            this.Controls.Add(this.kaiListBoxRightContext);
            this.Controls.Add(this.kaiListBoxLeftContext);
            this.Controls.Add(this.textBox);
            this.Controls.Add(this.kaiListBoxStress);
            this.Controls.Add(this.comboBox);
            this.Controls.Add(this.buttonDeleteRightContext);
            this.Controls.Add(this.buttonAddRightContext);
            this.Controls.Add(this.buttonDeleteLeftContext);
            this.Controls.Add(this.buttonAddLeftContext);
            this.Controls.Add(this.buttonDeleteStress);
            this.Controls.Add(this.buttonAddStress);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonOK);
            this.Controls.Add(this.labelRContext);
            this.Controls.Add(this.labelLContext);
            this.Controls.Add(this.labelStress);
            this.Controls.Add(this.labelInput);
            this.Controls.Add(this.kaiListBoxExceptions);
            this.Controls.Add(this.kaiListBoxOutputModifiers);
            this.Controls.Add(this.groupBoxInput);
            this.Controls.Add(this.groupBoxOutput);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "RuleDialog";
            this.Text = "Edit Transcription Rule";
            this.groupBoxInput.ResumeLayout(false);
            this.groupBoxInput.PerformLayout();
            this.groupBoxOutput.ResumeLayout(false);
            this.groupBoxOutput.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelInput;
        private System.Windows.Forms.Label labelStress;
        private System.Windows.Forms.Label labelLContext;
        private System.Windows.Forms.Label labelRContext;
        private System.Windows.Forms.Button buttonOK;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Button buttonAddStress;
        private System.Windows.Forms.Button buttonDeleteStress;
        private System.Windows.Forms.Button buttonAddLeftContext;
        private System.Windows.Forms.Button buttonDeleteLeftContext;
        private System.Windows.Forms.Button buttonAddRightContext;
        private System.Windows.Forms.Button buttonDeleteRightContext;
        private System.Windows.Forms.ComboBox comboBox;
        private KaiListBox.KaiListBox kaiListBoxStress;
        private System.Windows.Forms.TextBox textBox;
        private KaiListBox.KaiListBox kaiListBoxLeftContext;
        private KaiListBox.KaiListBox kaiListBoxRightContext;
        private System.Windows.Forms.ComboBox comboBoxOut;
        private System.Windows.Forms.TextBox textBoxInput;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBoxInput;
        private System.Windows.Forms.Button buttonAddOutputModifier;
        private System.Windows.Forms.Button buttonDeleteOutputModifiers;
        private System.Windows.Forms.GroupBox groupBoxOutput;
        private KaiListBox.KaiListBox kaiListBoxOutputModifiers;
        private KaiListBox.KaiListBox kaiListBoxExceptions;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button buttonDeleteExceptions;
        private System.Windows.Forms.Button buttonAddException;
        private System.Windows.Forms.Label label3;
    }
}