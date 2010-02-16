using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Collections;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using Microsoft.Win32;

namespace RuleEditor
{
    public partial class RuleEditor :Form
    {
        string sRulesFilePath;
        public List<Settings> liSettings = new List<Settings>();
        public List<string> liMru = new List<string>();
        const int ciMaxMruCount = 10;
        bool bDirty;

        public RuleEditor()
        {
            InitializeComponent();
            bDirty = false;
            ReadSettings();
            AdjustSizes();
            ReadMru();
            UpdateMruDropdown();
        }

        //
        // Helpers
        //
        private void RetrieveRule (RuleDialog dlgRule, ref StRule stRule)
        {
            stRule.sInput = dlgRule.InputString;

            stRule.dictStressPositions = new Dictionary<string, string> (dlgRule.StressPositions);
            stRule.dictLeftContexts = new Dictionary<string, string> (dlgRule.LeftContexts);
            stRule.dictRightContexts = new Dictionary<string, string> (dlgRule.RightContexts);
            stRule.dictExceptions = new Dictionary<string, string> (dlgRule.Exceptions);
            stRule.dictOutputSymbols = new Dictionary<string, string>();
            if (dlgRule.OutputString.Length > 0)
            {
                stRule.dictOutputSymbols["Literal"] = dlgRule.OutputString;
            }
            if ("Literal" != dlgRule.OutputSymbol.Key)
            {
                stRule.dictOutputSymbols.Add(dlgRule.OutputSymbol.Key, dlgRule.OutputSymbol.Value);
            }
            stRule.dictOutputModifiers = new Dictionary<string, string> (dlgRule.OutputModifiers);
        }

        private void FormatRow (StRule stRule, ListViewItem lvi)
        {
            lvi.Tag = stRule;
            lvi.Text = stRule.sInput;

            try
            {
                ListViewItem.ListViewSubItem lvsiStress = new ListViewItem.ListViewSubItem();
                foreach (string sValue in stRule.dictStressPositions.Values)
                {
                    if (lvsiStress.Text.Length > 0)
                    {
                        lvsiStress.Text += ", ";
                    }
                    lvsiStress.Text += sValue;
                }
                lvi.SubItems.Add(lvsiStress);

                ListViewItem.ListViewSubItem lvsiLeft = new ListViewItem.ListViewSubItem();
                foreach (string sValue in stRule.dictLeftContexts.Values)
                {
                    if (lvsiLeft.Text.Length > 0)
                    {
                        lvsiLeft.Text += ", ";
                    }
                    lvsiLeft.Text += sValue;
                }
                lvi.SubItems.Add(lvsiLeft);

                ListViewItem.ListViewSubItem lvsiRight = new ListViewItem.ListViewSubItem();
                foreach (string sValue in stRule.dictRightContexts.Values)
                {
                    if (lvsiRight.Text.Length > 0)
                    {
                        lvsiRight.Text += ", ";
                    }
                    lvsiRight.Text += sValue;
                }
                lvi.SubItems.Add(lvsiRight);

                ListViewItem.ListViewSubItem lvsiExceptions = new ListViewItem.ListViewSubItem();
                foreach (string sValue in stRule.dictExceptions.Values)
                {
                    if (lvsiExceptions.Text.Length > 0)
                    {
                        lvsiExceptions.Text += ", ";
                    }
                    lvsiExceptions.Text += sValue;
                }
                lvi.SubItems.Add (lvsiExceptions);

                ListViewItem.ListViewSubItem lvsiOutput = new ListViewItem.ListViewSubItem();
                foreach (KeyValuePair<string, string> kvp in stRule.dictOutputSymbols)
                {
                    lvsiOutput.Text = kvp.Value;
                }
                lvi.SubItems.Add(lvsiOutput);

                ListViewItem.ListViewSubItem lvsiOutputModifiers = new ListViewItem.ListViewSubItem();
                foreach (string sValue in stRule.dictOutputModifiers.Values)
                {
                    if (lvsiOutputModifiers.Text.Length > 0)
                    {
                        lvsiOutputModifiers.Text += ", ";
                    }
                    lvsiOutputModifiers.Text += sValue;
                }
                lvi.SubItems.Add(lvsiOutputModifiers);
            }
            catch (Exception ex)
            {
                string sText = string.Format("Error formatting rule: {0}", ex.Message);
                MessageBox.Show (sText,
                                 "RuleEditor",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
            }

        }   //  FormatRow (...)

        private void ResizeControls()
        {
            int iSpace = 3;
            int iHorzOffset = 3;
            int iVertOffset = 4;
            kaiListView.Size = new Size (this.Size.Width, 
                                         this.ClientRectangle.Height - 
                                           kaiListView.Bounds.Y - 
                                           buttonAdd.Size.Height - 
                                           2*iVertOffset);

            buttonAdd.SetBounds (kaiListView.Size.Width - 4 * (buttonAdd.Size.Width + iSpace) - iHorzOffset,
                                 this.ClientRectangle.Height - buttonAdd.Size.Height - iVertOffset,
                                 buttonAdd.Size.Width,
                                 buttonAdd.Size.Height);

            buttonEdit.SetBounds (kaiListView.Size.Width - 3 * (buttonEdit.Size.Width + iSpace) - iHorzOffset,
                                  this.ClientRectangle.Height - buttonEdit.Size.Height - iVertOffset,
                                  buttonEdit.Size.Width,
                                  buttonEdit.Size.Height);

            buttonDelete.SetBounds (kaiListView.Size.Width - 2 * (buttonDelete.Size.Width + iSpace) - iHorzOffset,
                                    this.ClientRectangle.Height - buttonDelete.Size.Height - iVertOffset,
                                    buttonDelete.Size.Width,
                                    buttonDelete.Size.Height);

            buttonClose.SetBounds (kaiListView.Size.Width - buttonClose.Size.Width - iSpace - iHorzOffset,
                                   this.ClientRectangle.Height - buttonClose.Size.Height - iVertOffset,
                                   buttonClose.Size.Width,
                                   buttonClose.Size.Height);
        }

        private void ReadSettings()
        {
            string sSettingsPath;
            try
            {
                sSettingsPath = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
                sSettingsPath = Path.Combine(sSettingsPath, Application.CompanyName);
                sSettingsPath = Path.Combine(sSettingsPath, Application.ProductName);
                sSettingsPath = Path.Combine(sSettingsPath, "RuleSettings.xml");
            }
            catch (Exception ex)
            {
                string sText = string.Format ("Error accessing settings file: {0}", ex.Message);
                MessageBox.Show (sText,
                                 "RuleEditor",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                return;
            }

            Console.WriteLine();
            try
            {
                XmlReaderSettings xmlSettings = new XmlReaderSettings();
                xmlSettings.ConformanceLevel = ConformanceLevel.Document;
                xmlSettings.IgnoreComments = true;
                xmlSettings.IgnoreWhitespace = true;

                XmlReader xmlReader = XmlReader.Create(sSettingsPath, xmlSettings);

                xmlReader.Read();

                while (!xmlReader.IsStartElement("Language"))
                {
                    xmlReader.Read();
                }

                Settings Settings = new Settings();
                while (xmlReader.IsStartElement("Language"))
                {
                    xmlReader.Read();

                    Settings.sLanguage = xmlReader.ReadString();

                    while (!xmlReader.EOF && !xmlReader.IsStartElement())
                    {
                        xmlReader.Read();
                    }

                    while (xmlReader.IsStartElement())
                    {
                        switch (xmlReader.LocalName)
                        {
                            case "Stress":
                            {
                                ReadAttributes (xmlReader, Settings.dictStressTypes);
                                break;
                            }
                            case "LeftContext":
                            {
                                ReadAttributes( xmlReader, Settings.dictLeftContexts);
                                break;
                            }
                            case "RightContext":
                            {
                                ReadAttributes (xmlReader, Settings.dictRightContexts);
                                break;
                            }
                            case "Exception":
                            {
                                ReadAttributes (xmlReader, Settings.dictExceptions);
                                break;
                            }
                            case "OutputSymbol":
                            {
                                ReadAttributes (xmlReader, Settings.dictOutputSymbols);
                                break;
                            }
                            case "Modifier":
                            {
                                ReadAttributes(xmlReader, Settings.dictOutputModifiers);
                                break;
                            }
                            default:
                            {
                                string sMsg = string.Format ("Unknown keyword: {0} in {1}",
                                                             xmlReader.LocalName,
                                                             sSettingsPath);
                                MessageBox.Show (sMsg,
                                                 "Rule Editor",
                                                 MessageBoxButtons.OK,
                                                 MessageBoxIcon.Exclamation);
                                KaiLibCS.MfcHelpers.LogError (sMsg);
                                continue;
                            }

                        }   // switch

                    }   // while (xmlReader.IsStartElement())

                    liSettings.Add(Settings);

                }   //  while (xmlReader.IsStartElement("Language"))

                xmlReader.Close();

            }
            catch (Exception ex)
            {
                string sText = string.Format ("Error reading settings: {0}", ex.Message);
                MessageBox.Show (sText,
                                 "RuleEditor",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
            }

        }   //  ReadSettings()

        private void ReadRules()
        {
            kaiListView.Items.Clear();

            try
            {
                XmlReaderSettings xmlSettings = new XmlReaderSettings();
                xmlSettings.ConformanceLevel = ConformanceLevel.Document;
                xmlSettings.IgnoreComments = true;
                xmlSettings.IgnoreWhitespace = true;

                XmlReader xmlReader = XmlReader.Create (sRulesFilePath, xmlSettings);
    
                xmlReader.Read();
                xmlReader.ReadStartElement("Language");
                string sLanguage = xmlReader.ReadString();
                if ("Russian" != sLanguage)
                {
                    string sMsg = string.Format ("Source file: {0} is invalid", sRulesFilePath);
                    MessageBox.Show (sMsg, 
                                     "Rule Editor",
                                     MessageBoxButtons.OK, 
                                     MessageBoxIcon.Exclamation);
                    KaiLibCS.MfcHelpers.LogError(sMsg);
                    return;
                }

                while (xmlReader.IsStartElement("Rule"))
                {
                    StRule stRule = new StRule();
                    xmlReader.Read();

                    while (!xmlReader.EOF && !xmlReader.IsStartElement())
                    {
                        xmlReader.Read();
                    }

                    stRule.dictStressPositions = new Dictionary<string, string>();
                    stRule.dictLeftContexts = new Dictionary<string, string>();
                    stRule.dictRightContexts = new Dictionary<string, string>();
                    stRule.dictExceptions = new Dictionary<string, string>();
                    stRule.dictOutputSymbols = new Dictionary<string, string>();
                    stRule.dictOutputModifiers = new Dictionary<string, string>();

                    while (xmlReader.IsStartElement())
                    {
                        switch (xmlReader.LocalName)
                        {
                            case "Input":
                            {
                                Dictionary<string, string> dictInput = new Dictionary<string, string>();
                                ReadAttributes (xmlReader, dictInput);
                                stRule.sInput = dictInput["Value"];
                                break;
                            }
                            case "Stress":
                            {
                                ReadAttributes (xmlReader, stRule.dictStressPositions);
                                break;
                            }
                            case "LeftContext":
                            {
                                ReadAttributes (xmlReader, stRule.dictLeftContexts);
                                break;
                            }
                            case "RightContext":
                            {
                                ReadAttributes (xmlReader, stRule.dictRightContexts);
                                break;
                            }
                            case "Exception":
                            {
                                ReadAttributes (xmlReader, stRule.dictExceptions);
                                break;
                            }
                            case "Output":
                            {
                                ReadAttributes (xmlReader, stRule.dictOutputSymbols);
                                break;
                            }
                            case "Modifier":
                            {
                                ReadAttributes (xmlReader, stRule.dictOutputModifiers);
                                break;
                            }
                            default:
                            {
                                string sMsg = string.Format ("Unknown keyword: {0} in {1}", 
                                                             xmlReader.LocalName,
                                                             sRulesFilePath);
                                MessageBox.Show (sMsg,
                                                 "Rule Editor",
                                                 MessageBoxButtons.OK,
                                                 MessageBoxIcon.Exclamation);
                                KaiLibCS.MfcHelpers.LogError (sMsg);
                                continue;
                            }

                        }   // switch...
                    }
                               
                    ListViewItem lvi = new ListViewItem();
                    FormatRow (stRule, lvi);
                    kaiListView.Items.Add (lvi);

                    xmlReader.Read();
                }

                xmlReader.Close();

                if (kaiListView.SelectedItems.Count > 0)
                {
                    buttonEdit.Enabled = true;
                    buttonDelete.Enabled = true;
                }
                else
                {
                    buttonEdit.Enabled = false;
                    buttonDelete.Enabled = false;
                }
            }
            catch (Exception ex)
            {
                string sMsg = string.Format ("IO error: {0}", ex.Message);
                MessageBox.Show (sMsg, 
                                 "Rule Editor",
                                 MessageBoxButtons.OK, 
                                 MessageBoxIcon.Exclamation);
            }

            int iCount = liMru.Count;
            int iRemoved = 0;
            for (int iItem = 0; iItem < iCount; ++iItem)
            {
                if (sRulesFilePath == liMru[iItem - iRemoved])
                {
                    liMru.RemoveAt(iItem - iRemoved);
                    iRemoved++;
                }
            }

            liMru.Insert(0, sRulesFilePath);
            if (liMru.Count > ciMaxMruCount)
            {
                liMru.RemoveAt(liMru.Count - 1);
            }

            //
            // Re-create MRU menu
            //
            while (recentFilesToolStripMenuItem.DropDownItems.Count > 0)
            {
                recentFilesToolStripMenuItem.DropDownItems.RemoveAt(0);
            }
            foreach (string sPath in liMru)
            {
                ToolStripMenuItem mi = new ToolStripMenuItem (sPath);
                mi.DropDownItemClicked += new ToolStripItemClickedEventHandler(toolStripDropDownItem_DropDownItemClicked);
                recentFilesToolStripMenuItem.DropDownItems.Add(mi);
            }
            recentFilesToolStripMenuItem.Enabled = true;

        }   //  readRules (string sPath)

        private void ReadAttributes (XmlReader xmlReader,
                                     Dictionary<string, string> dictData)
        {
            try
            {
                xmlReader.ReadStartElement();
                while (!xmlReader.EOF && xmlReader.IsStartElement())
                {
                    if (xmlReader.AttributeCount > 0)
                    {
                        string sKey = xmlReader.LocalName;
                        for (int iAttr = 0; iAttr < xmlReader.AttributeCount; ++iAttr)
                        {
                            xmlReader.MoveToAttribute(iAttr);
                            dictData[sKey] = xmlReader.Value;
                        }
                    }
                    xmlReader.Read();
                }
                xmlReader.ReadEndElement();
            }
            catch (Exception ex)
            {
                string sMsg = string.Format("Error reading settings: {0}", ex.Message);
                MessageBox.Show (sMsg,
                                 "RuleEditor",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                KaiLibCS.MfcHelpers.LogError (sMsg);
            }
        }

        private void WriteRules (string sPath)
        {
            XmlWriterSettings xmlSettings = new XmlWriterSettings();
            xmlSettings.Indent = true;
            xmlSettings.IndentChars = "    ";
            xmlSettings.Encoding = Encoding.Unicode;
            xmlSettings.NewLineHandling = NewLineHandling.Replace;
            
            XmlWriter xmlWriter = XmlWriter.Create (sPath, xmlSettings);
            try
            {
                xmlWriter.WriteStartDocument();
                xmlWriter.WriteComment ("This file was automatically generated by Kai transcription module.");
                xmlWriter.WriteStartElement ("Language");
                xmlWriter.WriteString ("Russian");
 
                foreach (ListViewItem lvi in kaiListView.Items)
                {
                    StRule stRule = (StRule)lvi.Tag;
                    xmlWriter.WriteStartElement ("Rule");
//                    xmlWriter.WriteAttributeString ("Num", stRule.iNumber.ToString());
                    xmlWriter.WriteStartElement("Input");
                    xmlWriter.WriteStartElement("Value");
                    xmlWriter.WriteAttributeString ("Value", stRule.sInput);
                    xmlWriter.WriteEndElement();    // Value
                    xmlWriter.WriteEndElement();    // Input

                    xmlWriter.WriteStartElement("Stress");
                    foreach (KeyValuePair<string, string> kvp in stRule.dictStressPositions)
                    {
                        xmlWriter.WriteStartElement (kvp.Key);
                        xmlWriter.WriteAttributeString ("Caption", kvp.Value);
                        xmlWriter.WriteEndElement();
                    }
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement ("LeftContext");
                    foreach (KeyValuePair<string, string> kvp in stRule.dictLeftContexts)
                    {
                        xmlWriter.WriteStartElement (kvp.Key);
                        xmlWriter.WriteAttributeString ("Caption", kvp.Value);
                        xmlWriter.WriteEndElement();
                    }
                    xmlWriter.WriteEndElement();

                    xmlWriter.WriteStartElement ("RightContext");
                    foreach (KeyValuePair<string, string> kvp in stRule.dictRightContexts)
                    {
                        xmlWriter.WriteStartElement (kvp.Key);
                        xmlWriter.WriteAttributeString ("Caption", kvp.Value);
                        xmlWriter.WriteEndElement();
                    }
                    xmlWriter.WriteEndElement();

                    if (stRule.dictExceptions.Count > 0)
                    {
                        xmlWriter.WriteStartElement ("Exception");
                        foreach (KeyValuePair<string, string> kvp in stRule.dictExceptions)
                        {
                            xmlWriter.WriteStartElement (kvp.Key);
                            xmlWriter.WriteAttributeString ("Caption", kvp.Value);
                            xmlWriter.WriteEndElement();
                        }
                        xmlWriter.WriteEndElement();
                    }

                    xmlWriter.WriteStartElement ("Output");
                    foreach (KeyValuePair<string, string> kvp in stRule.dictOutputSymbols)
                    {
                        xmlWriter.WriteStartElement(kvp.Key);
                        xmlWriter.WriteAttributeString("Caption", kvp.Value);
                        xmlWriter.WriteEndElement();
                    }
                    xmlWriter.WriteEndElement();

                    if (stRule.dictOutputModifiers.Count > 0)
                    {
                        xmlWriter.WriteStartElement("Modifier");
                        foreach (KeyValuePair<string, string> kvp in stRule.dictOutputModifiers)
                        {
                            xmlWriter.WriteStartElement(kvp.Key);
                            xmlWriter.WriteAttributeString("Caption", kvp.Value);
                            xmlWriter.WriteEndElement();
                        }
                        xmlWriter.WriteEndElement();
                    }
                    
                    xmlWriter.WriteEndElement();    // rule

                }   // foreach (ListViewItem lvi in kaiListView.Items)

                xmlWriter.WriteEndDocument();
                xmlWriter.Flush();
                xmlWriter.Close();
            }
            catch (Exception ex)
            {
                string sMsg = string.Format ("Error writing rule: {0}", ex.Message);
                MessageBox.Show (sMsg,
                                 "Rule Editor",
                                 MessageBoxButtons.OK, 
                                 MessageBoxIcon.Exclamation);
                KaiLibCS.MfcHelpers.LogError (sMsg);
                xmlWriter.Close();
            }

        }   //  writeRules (string sPath)

        private void SaveAs()
        {
            SaveFileDialog saveDlg = new SaveFileDialog();
            saveDlg.Filter = "XML files (*.xml)|*.xml|All files (*.*)|*.*";
            saveDlg.FilterIndex = 1;
            saveDlg.RestoreDirectory = true;
            saveDlg.OverwritePrompt = true;
            if (saveDlg.ShowDialog() == DialogResult.OK)
            {
                if (saveDlg.FileName != null)
                {
                    sRulesFilePath = saveDlg.FileName;
                    WriteRules (sRulesFilePath);
                    bDirty = false;
                }
            }

        }   // SaveAs()

        private DialogResult PromptToSave()
        {
            DialogResult dr = MessageBox.Show ("Save current rules?",
                                               "Rule editor",
                                               MessageBoxButtons.YesNoCancel,
                                               MessageBoxIcon.Question);
            if (DialogResult.Cancel == dr)
            {
                return dr;
            }

            if (DialogResult.Yes == dr)
            {
                SaveFileDialog saveDlg = new SaveFileDialog();
                saveDlg.Filter = "XML files (*.xml)|*.xml|All files (*.*)|*.*";
                saveDlg.FilterIndex = 1;
                saveDlg.RestoreDirectory = true;
                saveDlg.OverwritePrompt = true;
                if (saveDlg.ShowDialog() == DialogResult.OK)
                {
                    if (saveDlg.FileName != null)
                    {
                        string sSavePath = saveDlg.FileName;
                        WriteRules (sSavePath);
                        bDirty = false;
                    }
                }
            }

            return dr;

        }   // PromptToSave()

        private void EditRule()
        {
            ListViewItem lviOld = kaiListView.SelectedItems[0];
            StRule stOldRule = (StRule)lviOld.Tag;
            RuleDialog dlgRule = new RuleDialog (liSettings[0], stOldRule);
            DialogResult dr = dlgRule.ShowDialog();
            if (DialogResult.OK == dr)
            {
                StRule stR = new StRule();
                RetrieveRule (dlgRule, ref stR);
                ListViewItem lviNew = new ListViewItem();
                FormatRow (stR, lviNew);
                int iAt = kaiListView.Items.IndexOf(lviOld);
                kaiListView.Items.Remove (lviOld);
                kaiListView.Items.Insert (iAt, lviNew);
                bDirty = true;
            }
        }

        private void AdjustSizes()
        {
            RegistryKey regHkcu = Registry.CurrentUser;
            string sSubKey = "SOFTWARE\\" + Application.CompanyName + "\\" + Application.ProductName;
            try
            {
                RegistryKey regSubKey = regHkcu.OpenSubKey(sSubKey);
                if (null != regSubKey)
                {
                    int iWidth = (int)regSubKey.GetValue ("Width");
                    int iHeight = (int)regSubKey.GetValue ("Height");
                    this.ClientSize = new Size(iWidth, iHeight);
                    string sColumnWidths = (string)regSubKey.GetValue ("Column Widths");
                    if (sColumnWidths.Length > 0)
                    {
                        string[] arrWidths = sColumnWidths.Split(new char[] { ':' });
                        if (arrWidths.Length == kaiListView.Columns.Count)
                        {
                            for (int iColumn = 0; iColumn < kaiListView.Columns.Count; ++iColumn)
                            {
                                kaiListView.Columns[iColumn].Width =
                                    Convert.ToInt32(arrWidths[iColumn]);
                            }
                        }
                        else
                        {
                            KaiLibCS.MfcHelpers.LogError
                                ("Illegal number of tokens in \"Column Widths\" field");
                        }
                    }
                    else
                    {
                        KaiLibCS.MfcHelpers.LogError ("Missing data in \"Column Widths\" field");
                    }
                }
                else
                {
                    KaiLibCS.MfcHelpers.LogError ("Registry subkey string is null");
                }
            }
            catch (Exception ex)
            {
                KaiLibCS.MfcHelpers.LogError(ex.Message);
            }

        }  //   AdjustSizes()

        private void ReadMru()
        {
            RegistryKey regHkcu = Registry.CurrentUser;
            string sSubKey = "SOFTWARE\\" +
                             Application.CompanyName +
                             "\\" +
                             Application.ProductName +
                             "\\" +
                             "Recent File List";
            try
            {
                RegistryKey regSubKey = regHkcu.OpenSubKey(sSubKey);
                if (null != regSubKey)
                {
                    string[] arrNames = regSubKey.GetValueNames();
                    foreach (string sName in arrNames)
                    {
                        liMru.Add ((string)regSubKey.GetValue (sName));
                    }
                }
                else
                {
                    KaiLibCS.MfcHelpers.LogError("Registry subkey string is null");
                }
            }
            catch (Exception ex)
            {
                KaiLibCS.MfcHelpers.LogError(ex.Message);
            }

        }   //  ReadMru()


        private void UpdateMruDropdown()
        {
            if (liMru.Count > 0)
            {
                foreach (string sPath in liMru)
                {
//                    recentFilesToolStripMenuItem.DropDownItems.Add(sPath);
                    ToolStripMenuItem mi = new ToolStripMenuItem(sPath);
                    mi.Click += new EventHandler(toolStripDropDownItem_DropDownItemClicked);
                    recentFilesToolStripMenuItem.DropDownItems.Add(mi);
                }
                recentFilesToolStripMenuItem.Enabled = true;
            }

        }   //  UpdateMruDropdown()


        //
        // Event handlers
        //
        private void RuleEditor_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (bDirty)
            {
                DialogResult dr = PromptToSave();
                if (DialogResult.Cancel == dr)
                {
                    e.Cancel = true;
                    return;
                }
            }

            RegistryKey regHkcu = Registry.CurrentUser;
            string sSubKey = "SOFTWARE\\" + Application.CompanyName + "\\" + Application.ProductName;
            try
            {
                RegistryKey regSubKey = regHkcu.CreateSubKey(sSubKey);
                regSubKey.SetValue("Width", this.ClientSize.Width);
                regSubKey.SetValue("Height", this.ClientSize.Height);
                string sColumnWidths = "";
                for (int iColumn = 0; iColumn < kaiListView.Columns.Count; ++iColumn)
                {
                    sColumnWidths += kaiListView.Columns[iColumn].Width.ToString();
                    if (iColumn < kaiListView.Columns.Count - 1)
                    {
                        sColumnWidths += ":";
                    }
                }
                regSubKey.SetValue("Column Widths", sColumnWidths);

            }
            catch (Exception ex)
            {
                KaiLibCS.MfcHelpers.LogError(ex.Message);
            }

            sSubKey = "SOFTWARE\\" + 
                      Application.CompanyName + 
                      "\\" + 
                      Application.ProductName + 
                      "\\Recent File List";
            try
            {
                RegistryKey regSubKey = regHkcu.CreateSubKey (sSubKey);
                string[] arrNames = regSubKey.GetValueNames();
                foreach (string sName in arrNames)
                {
                    regSubKey.DeleteValue (sName);
                }
                for (int iMruItem = 1; iMruItem <= liMru.Count; ++iMruItem)
                {
                    string sName = "File" + iMruItem.ToString();
                    regSubKey.SetValue (sName, liMru[iMruItem-1]);
                }

            }
            catch (Exception ex)
            {
                KaiLibCS.MfcHelpers.LogError(ex.Message);
            }

        }

        private void buttonAdd_Click(object sender, EventArgs e)
        {
            RuleDialog dlgRule = new RuleDialog(liSettings[0]);
            DialogResult dr = dlgRule.ShowDialog();
            if (DialogResult.OK == dr)
            {
                StRule stR = new StRule();
                RetrieveRule (dlgRule, ref stR);
                ListViewItem lvi = new ListViewItem();
                FormatRow (stR, lvi);
                kaiListView.Items.Add (lvi);
                bDirty = true;
            }
        }

        private void buttonEdit_Click(object sender, EventArgs e)
        {
            EditRule();
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void buttonDelete_Click(object sender, EventArgs e)
        {
            DialogResult dr = MessageBox.Show(this,
                                               "Are you sure you want to delete the selected rule?",
                                               "Rule Editor",
                                               MessageBoxButtons.YesNo,
                                               MessageBoxIcon.Question);
            if (DialogResult.Yes == dr)
            {
                foreach (ListViewItem lvi in kaiListView.SelectedItems)
                {
                    kaiListView.Items.Remove(lvi);
                }
                bDirty = true;
            }
        }

        private void kaiListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (kaiListView.SelectedItems.Count > 0)
            {
                buttonEdit.Enabled = true;
                buttonDelete.Enabled = true;
            }
            else
            {
                buttonEdit.Enabled = false;
                buttonDelete.Enabled = false;
            }
        }

        private void RuleEditor_SizeChanged(object sender, EventArgs e)
        {
            ResizeControls();
        }

        private void RuleEditor_Shown(object sender, EventArgs e)
        {
            if (0 == liSettings.Count)
            {
                string sMsg = "Unable to initialize Kai Rule Editor; exiting";
                MessageBox.Show (sMsg,
                                 "Kai",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                KaiLibCS.MfcHelpers.LogError (sMsg);
                Close();
            }

            ResizeControls();
        }

        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (sRulesFilePath != null)
            {
                WriteRules (sRulesFilePath);
                bDirty = false;
            }
            else
            {
                SaveAs();
            }
        }

        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveAs();
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (bDirty)
            {
                DialogResult dr = PromptToSave();
                if (DialogResult.Cancel == dr)
                {
                    return;
                }
            }

            OpenFileDialog openDlg = new OpenFileDialog();
            openDlg.Filter = "XML files (*.xml)|*.xml|All files (*.*)|*.*";
            openDlg.FilterIndex = 1;
            if (openDlg.ShowDialog() == DialogResult.OK)
            {
                if (openDlg.FileName != null)
                {
                    sRulesFilePath = openDlg.FileName;
                    ReadRules();
                    saveAsToolStripMenuItem.Enabled = true;
                }
            }
        }

        private void kaiListView_DoubleClick(object sender, EventArgs e)
        {
            EditRule();
        }

        private void fileToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
        {
            if (bDirty)
            {
                saveToolStripMenuItem.Enabled = true;
            }
            else
            {
                saveToolStripMenuItem.Enabled = false;
            }
        }

        private void toolStripDropDownItem_DropDownItemClicked(object sender, EventArgs e)
        {
            sRulesFilePath = ((ToolStripMenuItem)sender).Text;
            ReadRules();
        }

    }   //  class RuleEditor

}
