using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace RuleEditor
{
    public partial class RuleDialog :Form
    {
        Settings Settings;

        //
        // Helper class to store/display data in controls
        //
        private class DataItem
        {
            KeyValuePair<string, string> m_kvp;

            public DataItem (KeyValuePair<string, string> kvp)
            {
                m_kvp = kvp;
            }

            public string Key
            {
                get
                {
                    return m_kvp.Key;
                }
            }

            public string Value
            {
                get
                {
                    return m_kvp.Value;
                }
            }

            public KeyValuePair<string, string> Data
            {
                get
                {
                    return m_kvp;
                }
            }

            public override string ToString()
            {
                return m_kvp.Value;
            }
        }

        //
        // Properties
        //
        public string InputString
        {
            get
            {
                return textBoxInput.Text;
            }
        }

        public Dictionary<string, string> StressPositions
        {
            get
            {
                Dictionary<string, string> dictSp = new Dictionary<string, string>();
                foreach (DataItem di in kaiListBoxStress.Items)
                {
                    dictSp.Add (di.Key, di.Value);
                }
                return dictSp;
            }
        }

        public Dictionary<string, string> LeftContexts
        {
            get
            {
                Dictionary<string, string> dictSp = new Dictionary<string, string>();
                foreach (DataItem di in kaiListBoxLeftContext.Items)
                {
                    dictSp.Add(di.Key, di.Value);
                }
                return dictSp;
            }
        }

        public Dictionary<string, string> RightContexts
        {
            get
            {
                Dictionary<string, string> dictSp = new Dictionary<string, string>();
                foreach (DataItem di in kaiListBoxRightContext.Items)
                {
                    dictSp.Add(di.Key, di.Value);
                }
                return dictSp;
            }
        }

        public Dictionary<string, string> Exceptions
        {
            get
            {
                Dictionary<string, string> dictSp = new Dictionary<string, string>();
                foreach (DataItem di in kaiListBoxExceptions.Items)
                {
                    dictSp.Add(di.Key, di.Value);
                }
                return dictSp;
            }
        }

        public Dictionary<string, string> OutputModifiers
        {
            get
            {
                Dictionary<string, string> dictSp = new Dictionary<string, string>();
                foreach (DataItem di in kaiListBoxOutputModifiers.Items)
                {
                    dictSp.Add(di.Key, di.Value);
                }
                return dictSp;
            }
        }

        public string OutputString
        {
            get
            {
                if (comboBoxOut.SelectedIndex >= 0)
                {
                    return "";
                }
                else
                {
                    return comboBoxOut.Text;
                }
            }
        }

        public KeyValuePair<string, string> OutputSymbol
        {
            get
            {
                if (comboBoxOut.SelectedIndex < 0)
                {
                    return new KeyValuePair<string, string> ("Literal", "Literal");
                }
                else
                {
                    DataItem di = (DataItem)comboBoxOut.Items[comboBoxOut.SelectedIndex];
                    return new KeyValuePair<string, string>(di.Key, di.Value);
                        
                }
            }
        }

        //
        // Internal data
        //
        EDataType eDataType = EDataType.InputTypeUndefined;
        int iCurrentItem;

        //
        // Construction
        //
        public RuleDialog (Settings st)
        {
            Settings = st;

            InitializeComponent();
            
            InitializeOutputCombo();

            kaiListBoxStress.DisableEdit = false;

            iCurrentItem = -1;

            comboBox.Visible = false;
            textBox.Visible = false;
        }

        public RuleDialog (Settings st, StRule stCurrentRule)
        {
            Settings = st;

            InitializeComponent();

            InitializeOutputCombo();

            kaiListBoxStress.DisableEdit = false;

            iCurrentItem = -1;

            comboBox.Visible = false;

            textBox.Visible = false;

            textBoxInput.Text = stCurrentRule.sInput;

            foreach (KeyValuePair<string, string> kvp in stCurrentRule.dictStressPositions)
            {
                kaiListBoxStress.Items.Add (new DataItem (kvp));
            }

            foreach (KeyValuePair<string, string> kvp in stCurrentRule.dictLeftContexts)
            {
                kaiListBoxLeftContext.Items.Add (new DataItem(kvp));
            }

            foreach (KeyValuePair<string, string> kvp in stCurrentRule.dictRightContexts)
            {
                kaiListBoxRightContext.Items.Add (new DataItem(kvp));
            }

            foreach (KeyValuePair<string, string> kvp in stCurrentRule.dictExceptions)
            {
                kaiListBoxExceptions.Items.Add (new DataItem(kvp));
            }

            foreach (KeyValuePair<string, string> kvp in stCurrentRule.dictOutputModifiers)
            {
                kaiListBoxOutputModifiers.Items.Add (new DataItem(kvp));
            }

            foreach (KeyValuePair<string, string> kvp in stCurrentRule.dictOutputSymbols)
            {
                comboBoxOut.Text = kvp.Value;
            }

        }   //  RuleDialog (StRule stCurrentRule)

        //
        // Events
        //

        // Cancel button
        private void buttonCancel_Click (object sender, EventArgs e)
        {
            Close();
        }

        // OK button
        private void buttonOK_Click (object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
            Close();
        }

        // Add buttons
        private void buttonAddStress_Click (object sender, EventArgs e)
        {
            eDataType = EDataType.Stress;
            ActivateListBox();
        }

        private void buttonAddLeftContext_Click (object sender, EventArgs e)
        {
            eDataType = EDataType.LeftContext;
            ActivateListBox();
        }

        private void buttonAddRightContext_Click (object sender, EventArgs e)
        {
            eDataType = EDataType.RightContext;
            ActivateListBox();
        }

        private void buttonAddOutputModifier_Click (object sender, EventArgs e)
        {
            eDataType = EDataType.OutputModifiers;
            ActivateListBox();
        }

        private void buttonAddException_Click (object sender, EventArgs e)
        {
            eDataType = EDataType.Exceptions;
            ActivateListBox();
        }

        // Delete buttons
        private void buttonDeleteStress_Click (object sender, EventArgs e)
        {
            kaiListBoxStress.Items.RemoveAt(kaiListBoxStress.SelectedIndex);
            enableDisableButtons();
        }

        private void buttonDeleteLeftContext_Click (object sender, EventArgs e)
        {
            kaiListBoxLeftContext.Items.RemoveAt(kaiListBoxLeftContext.SelectedIndex);
            enableDisableButtons();
        }

        private void buttonDeleteRightContext_Click (object sender, EventArgs e)
        {
            kaiListBoxRightContext.Items.RemoveAt(kaiListBoxRightContext.SelectedIndex);
            enableDisableButtons();
        }

        private void buttonDeleteExceptions_Click (object sender, EventArgs e)
        {
            kaiListBoxExceptions.Items.RemoveAt(kaiListBoxExceptions.SelectedIndex);
        }

        private void buttonDeleteOutputModifiers_Click (object sender, EventArgs e)
        {
            kaiListBoxOutputModifiers.Items.RemoveAt(kaiListBoxOutputModifiers.SelectedIndex);
        }

        // Combos
        private void comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBox.SelectedIndex < 0)
            {
                return;
            }

            KaiListBox.KaiListBox klb = null;
            Button btnAdd = null;

            switch (eDataType)
            {
                case EDataType.InputTypeUndefined:
                {
                    MessageBox.Show ("Internal Error: InputTypeUndefined");
                    return;
                        //                    break;
                }
                case EDataType.Stress:
                {
                    klb = kaiListBoxStress;
                    btnAdd = buttonAddStress;
                    break;
                }
                case EDataType.LeftContext:
                {
                    klb = kaiListBoxLeftContext;
                    btnAdd = buttonAddLeftContext;
                    break;
                }
                case EDataType.RightContext:
                {
                    klb = kaiListBoxRightContext;
                    btnAdd = buttonAddRightContext;
                    break;
                }
                case EDataType.Exceptions:
                {
                    klb = kaiListBoxExceptions;
                    btnAdd = buttonAddException;
                    break;
                }
                case EDataType.OutputModifiers:
                {
                    klb = kaiListBoxOutputModifiers;
                    btnAdd = buttonAddOutputModifier;
                    break;
                }
                default:
                {
                    MessageBox.Show("Internal Error: Illegal input type");
                    return;
                }
            }

            klb.DisableEdit = false;

            // i couldn't figure out how to use ObjectCollection.Contains()
            bool bDuplicate = false;
            foreach (DataItem kvpLeft in klb.Items)
            {
                DataItem kvpRight =
                    (DataItem)comboBox.Items[comboBox.SelectedIndex];
                if ((kvpLeft.Key == kvpRight.Key) && (kvpLeft.Value == kvpRight.Value))
                {
                    bDuplicate = true;
                }
            }
            if (!bDuplicate)
            {
                klb.Items.Insert(iCurrentItem,
                                   comboBox.Items[comboBox.SelectedIndex]);
            }
            btnAdd.Enabled = true;
            comboBox.Visible = false;
            comboBox.SelectedIndex = -1;

            eDataType = EDataType.InputTypeUndefined;

            enableDisableButtons();

        }   //  comboBox_SelectedIndexChanged

        private void comboBox_KeyUp (object sender, KeyEventArgs e)
        {
            ListBox lb = null;
            Button btnAdd = null;
            switch (eDataType)
            {
                case EDataType.InputTypeUndefined:
                {
                    MessageBox.Show("Internal Error: InputTypeUndefined");
                    return;
                        //                    break;
                }
                case EDataType.Stress:
                {
                    lb = kaiListBoxStress;
                    btnAdd = buttonAddStress;
                    kaiListBoxStress.DisableEdit = false;
                    break;
                }
                case EDataType.LeftContext:
                {
                    lb = kaiListBoxLeftContext;
                    btnAdd = buttonAddLeftContext;
                    kaiListBoxLeftContext.DisableEdit = false;
                    break;
                }
                case EDataType.RightContext:
                {
                    lb = kaiListBoxRightContext;
                    btnAdd = buttonAddRightContext;
                    kaiListBoxRightContext.DisableEdit = false;
                    break;
                }
                case EDataType.Exceptions:
                {
                    lb = kaiListBoxExceptions;
                    btnAdd = buttonAddException;
                    kaiListBoxExceptions.DisableEdit = false;
                    break;
                }
                case EDataType.OutputModifiers:
                {
                    lb = kaiListBoxOutputModifiers;
                    btnAdd = buttonAddOutputModifier;
                    kaiListBoxOutputModifiers.DisableEdit = false;
                    break;
                }
                default:
                {
                    MessageBox.Show("Internal Error: Illegal input type");
                    return;
                }
            }

            if (Keys.Escape == e.KeyCode)
            {
//                lb.Items.RemoveAt(iCurrentItem);
                comboBox.Visible = false;
                comboBox.SelectedValue = null;
                btnAdd.Enabled = true;
            }
            if (Keys.Enter == e.KeyCode)
            {
                if (comboBox.Text.Length > 0)
                {
                    lb.Items.Insert (iCurrentItem,comboBox.Text);
                    btnAdd.Enabled = true;
                    comboBox.Visible = false;
                    comboBox.SelectedIndex = -1;
                }
                else
                {
                    if (comboBox.SelectedIndex >= 0)
                    {
                        lb.Items.RemoveAt(iCurrentItem);
                        lb.Items.Insert(iCurrentItem,
                                         comboBox.Items[comboBox.SelectedIndex]);
                        btnAdd.Enabled = true;
                    }
                }
            }
        }   //  comboBox_KeyUp

        private void comboBoxOut_SelectedIndexChanged (object sender, EventArgs e)
        {
            enableDisableButtons();
        }

        private void comboBoxOut_TextChanged (object sender, EventArgs e)
        {
            enableDisableButtons();
        }

        // Listbox selection
        private void kaiListBoxStress_SelectedIndexChanged (object sender, EventArgs e)
        {
            if (kaiListBoxStress.SelectedIndex < 0)
            {
                buttonDeleteStress.Enabled = false;
            }
            else
            {
                buttonDeleteStress.Enabled = true;
            }
        }

        private void kaiListBoxLeftContext_SelectedIndexChanged (object sender, EventArgs e)
        {
            if (kaiListBoxLeftContext.SelectedIndex < 0)
            {
                buttonDeleteLeftContext.Enabled = false;
            }
            else
            {
                buttonDeleteLeftContext.Enabled = true;
            }
        }

        private void kaiListBoxRightContext_SelectedIndexChanged (object sender, EventArgs e)
        {
            if (kaiListBoxRightContext.SelectedIndex < 0)
            {
                buttonDeleteRightContext.Enabled = false;
            }
            else
            {
                buttonDeleteRightContext.Enabled = true;
            }
        }

        private void kaiListBoxExceptions_SelectedIndexChanged (object sender, EventArgs e)
        {
            if (kaiListBoxExceptions.SelectedIndex < 0)
            {
                buttonDeleteExceptions.Enabled = false;
            }
            else
            {
                buttonDeleteExceptions.Enabled = true;
            }
        }

        private void kaiListBoxOutputModifiers_SelectedIndexChanged (object sender, EventArgs e)
        {
            if (kaiListBoxOutputModifiers.SelectedIndex < 0)
            {
                buttonDeleteOutputModifiers.Enabled = false;
            }
            else
            {
                buttonDeleteOutputModifiers.Enabled = true;
            }
        }

        private void kaiListBoxStress_Leave (object sender, EventArgs e)
        {
            if (buttonDeleteStress != this.ActiveControl)
            {
                kaiListBoxStress.ClearSelected();
            }
        }

        private void kaiListBoxLeftContext_Leave (object sender, EventArgs e)
        {
            if (buttonDeleteLeftContext != this.ActiveControl)
            {
                kaiListBoxLeftContext.ClearSelected();
            }
        }

        private void kaiListBoxRightContext_Leave (object sender, EventArgs e)
        {
            if (buttonDeleteRightContext != this.ActiveControl)
            {
                kaiListBoxRightContext.ClearSelected();
            }
        }

        private void kaiListBoxExceptions_Leave (object sender, EventArgs e)
        {
            if (buttonDeleteExceptions != this.ActiveControl)
            {
                kaiListBoxExceptions.ClearSelected();
            }
        }

        private void kaiListBoxOutputModifiers_Leave (object sender, EventArgs e)
        {
            if (buttonDeleteOutputModifiers != this.ActiveControl)
            {
                kaiListBoxOutputModifiers.ClearSelected();
            }
        }

        private void textBoxInput_TextChanged (object sender, EventArgs e)
        {
            enableDisableButtons();
        }

        //
        // Listbox activation
        //
        private void ActivateListBox()
        {
            KaiListBox.KaiListBox klb = null;

            switch (eDataType)
            {
                case EDataType.Stress:
                {
                    klb = kaiListBoxStress;
                    buttonAddStress.Enabled = false;
                    buttonAddLeftContext.Enabled = true;
                    buttonAddRightContext.Enabled = true;
                    buttonAddException.Enabled = true;
                    buttonAddOutputModifier.Enabled = true;
                    InitializeStressCombo();

                    break;
                }
                case EDataType.LeftContext:
                {
                    klb = kaiListBoxLeftContext;
                    buttonAddStress.Enabled = true;
                    buttonAddLeftContext.Enabled = false;
                    buttonAddRightContext.Enabled = true;
                    buttonAddException.Enabled = true;
                    buttonAddOutputModifier.Enabled = true;
                    InitializeLeftContextCombo();

                    break;
                }
                case EDataType.RightContext:
                {
                    klb = kaiListBoxRightContext;
                    buttonAddStress.Enabled = true;
                    buttonAddLeftContext.Enabled = true;
                    buttonAddRightContext.Enabled = false;
                    buttonAddException.Enabled = true;
                    buttonAddOutputModifier.Enabled = true;
                    InitializeRightContextCombo();

                    break;
                }
                case EDataType.Exceptions:
                {
                    klb = kaiListBoxExceptions;
                    buttonAddStress.Enabled = true;
                    buttonAddLeftContext.Enabled = true;
                    buttonAddRightContext.Enabled = true;
                    buttonAddException.Enabled = false;
                    buttonAddOutputModifier.Enabled = true;
                    InitializeExceptionsCombo();

                    break;
                }
                case EDataType.OutputModifiers:
                {
                    klb = kaiListBoxOutputModifiers;
                    buttonAddStress.Enabled = true;
                    buttonAddLeftContext.Enabled = true;
                    buttonAddRightContext.Enabled = true;
                    buttonAddOutputModifier.Enabled = false;
                    InitializeOutputModifiersCombo();

                    break;
                }
                case EDataType.InputTypeUndefined:
                {
                    MessageBox.Show("Internal error: Unknown input type");
                    return;
                }
                default:
                {
                    MessageBox.Show("Internal error: Illegal input type");
                    return;
                }
            }

            klb.BeginUpdate();

            klb.Items.Add("");
            iCurrentItem = klb.Items.Count - 1;
            Rectangle rcRow = klb.GetItemRectangle(iCurrentItem);
            if ((rcRow.Y + rcRow.Height > klb.Height) &&
                (klb.TopIndex < iCurrentItem))
            {
                ++klb.TopIndex;
                rcRow = klb.GetItemRectangle(iCurrentItem);
            }

            klb.EndUpdate();

            klb.Items.RemoveAt(iCurrentItem);

            Point ptLocation = klb.Location;
            ptLocation.Y += rcRow.Y;

            comboBox.Size = new Size(rcRow.Width, rcRow.Height);
            comboBox.Location = ptLocation;
            comboBox.BringToFront();
            comboBox.DroppedDown = true;

            comboBox.EndUpdate();

            comboBox.Visible = true;
            comboBox.Focus();

            klb.DisableEdit = true;

        }   //  ActivateListBox()

        //
        // Combo initialization
        //
        private void InitializeStressCombo()
        {
            comboBox.Items.Clear();
            foreach (KeyValuePair<string, string> kvp in Settings.dictStressTypes)
            {
                comboBox.Items.Add (new DataItem (kvp));
            }
            comboBox.AutoCompleteMode = AutoCompleteMode.SuggestAppend;
            comboBox.Text = "";
        }

        private void InitializeLeftContextCombo()
        {
            comboBox.Items.Clear();
            foreach (KeyValuePair<string, string> kvp in Settings.dictLeftContexts)
            {
                comboBox.Items.Add (new DataItem(kvp));
            }
//            comboBox.AutoCompleteMode = AutoCompleteMode.SuggestAppend;
            comboBox.Text = "";
        }

        private void InitializeRightContextCombo()
        {
            comboBox.Items.Clear();
            foreach (KeyValuePair<string, string> kvp in Settings.dictRightContexts)
            {
                comboBox.Items.Add (new DataItem(kvp));
            }
//            comboBox.AutoCompleteMode = AutoCompleteMode.SuggestAppend;
            comboBox.Text = "";
        }

        private void InitializeExceptionsCombo()
        {
            comboBox.Items.Clear();
            foreach (KeyValuePair<string, string> kvp in Settings.dictExceptions)
            {
                comboBox.Items.Add (new DataItem(kvp));
            }
            comboBox.AutoCompleteMode = AutoCompleteMode.SuggestAppend;
            comboBox.Text = "";
        }

        private void InitializeOutputCombo()
        {
            comboBoxOut.Items.Clear();
            foreach (KeyValuePair<string, string> kvp in Settings.dictOutputSymbols)
            {
                comboBoxOut.Items.Add (new DataItem(kvp));
            }
            comboBoxOut.AutoCompleteMode = AutoCompleteMode.SuggestAppend;
            comboBox.Text = "";
        }

        private void InitializeOutputModifiersCombo()
        {
            comboBox.Items.Clear();
            foreach (KeyValuePair<string, string> kvp in Settings.dictOutputModifiers)
            {
                comboBox.Items.Add (new DataItem(kvp));
            }
            comboBox.AutoCompleteMode = AutoCompleteMode.SuggestAppend;
            comboBox.Text = "";
        }

        private void enableDisableButtons ()
        {
            //if ((kaiListBoxStress.Items.Count > 0) &&
            //    (kaiListBoxLeftContext.Items.Count > 0) &&
            //    (kaiListBoxRightContext.Items.Count > 0) &&
            //    (kaiListBoxExceptions.Items.Count > 0) &&
            //    (textBoxInput.Text.Length > 0) &&
            //    (comboBoxOut.Text.Length > 0))

            if ((textBoxInput.Text.Length > 0) &&
                (comboBoxOut.Text.Length > 0))
            {
                buttonOK.Enabled = true;
            }
            else
            {
                buttonOK.Enabled = false;
            }
            Update();
        }

        private void comboBox_Leave(object sender, EventArgs e)
        {
            Button btnAdd = null;

            switch (eDataType)
            {
                case EDataType.InputTypeUndefined:
                {
                    MessageBox.Show("Internal Error: InputTypeUndefined");
                    return;
                        //                    break;
                }
                case EDataType.Stress:
                {
                    btnAdd = buttonAddStress;
                    break;
                }
                case EDataType.LeftContext:
                {
                    btnAdd = buttonAddLeftContext;
                    break;
                }
                case EDataType.RightContext:
                {
                    btnAdd = buttonAddRightContext;
                    break;
                }
                case EDataType.Exceptions:
                {
                    btnAdd = buttonAddException;
                    break;
                }
                case EDataType.OutputModifiers:
                {
                    btnAdd = buttonAddOutputModifier;
                    break;
                }
                default:
                {
                    MessageBox.Show("Internal Error: Illegal input type");
                    return;
                }
            }

            btnAdd.Enabled = true;
            comboBox.Visible = false;

            eDataType = EDataType.InputTypeUndefined;

            enableDisableButtons();
        }

    }   //  public partial class RuleDialog :Form

}
