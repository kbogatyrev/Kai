using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace AsciiDecoder
{
    public partial class AsciiDecoderForm : Form
    {
        Hashtable hashEncodings = new Hashtable();
        byte[] byteSource;        
        long lSourceLength = 0;
        const int ciChunkSize = 10;
        int iCodePage = 20127;          // us-ascii
        bool bEmbed = false;
        string sOpenPath;
        string sSavePath;
        string sDecoded;

        public string SourcePath
        {
            set { sOpenPath = value; }
        }

        public string DecodedString
        {
            get { return sDecoded; }
        }

        public AsciiDecoderForm (bool bRunEmbedded)
        {
            bEmbed = bRunEmbedded;

            InitializeComponent();

            if (bEmbed)
            {
                fileMenuItem.Visible = false;
                btnOk.Text = "OK";
            }
            else
            {
                fileMenuItem.Visible = true;
                btnOk.Text = "Save";
            }

            statusLabel.Text = "Source Encoding: not selected";
            toolStripComboBox.Sorted = true;
            ArrayList cyrillicEncodings = new ArrayList();
            EncodingInfo[] encodingInfos = Encoding.GetEncodings();

            foreach (EncodingInfo ei in encodingInfos)
            {
                ToolStripItem tsi = new ToolStripMenuItem();
                tsi.Text = ei.DisplayName + " [" + ei.Name + "]";
                tsi.Tag = ei.CodePage;
                toolStripComboBox.Items.Add(tsi);

                if (ei.DisplayName.Contains ("Cyrillic"))
                {
                    cyrillicEncodings.Add (ei);
                }

                hashEncodings.Add (ei.CodePage, ei);
            }

            toolStripComboBox.Sorted = false;
            foreach (EncodingInfo cei in cyrillicEncodings)
            {
                ToolStripItem tsi = new ToolStripMenuItem();
                tsi.Text = cei.DisplayName + " [" + cei.Name + "]";
                tsi.Tag = cei.CodePage;
                toolStripComboBox.Items.Insert (0, tsi);
            }
        }

        public AsciiDecoderForm (string sPath)
        {
            sOpenPath = sPath;
            if (!File.Exists (sOpenPath))
            {
                MessageBox.Show ("Invalid path.", "Internal error",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
            }
            else
            {
                ReadSourceData (sOpenPath);
                char[] chrTemp = new char[lSourceLength];
                for (long lAt = 0; lAt < lSourceLength; ++lAt)
                {
                    chrTemp[lAt] = (char)byteSource[lAt];
                }
                bConvert();
            }
        }

        public void SetSourceData()
        {
            if (!File.Exists (sOpenPath))
            {
                MessageBox.Show ("Invalid path.", "Internal error",
                                 MessageBoxButtons.OK, 
                                 MessageBoxIcon.Exclamation);
            }
            else
            {
                ReadSourceData (sOpenPath);
                char[] chrTemp = new char[lSourceLength];
                for (long lAt = 0; lAt < lSourceLength; ++lAt)
                {
                    chrTemp[lAt] = (char)byteSource[lAt];
                }
                richTextBox.Text = new string (chrTemp);
            }
        }

        private void ReadSourceData (String sPath)
        {
            FileStream inStream = File.OpenRead (sPath);
            BinaryReader br = new BinaryReader (inStream);
            if (inStream.Length >= int.MaxValue)
            {
                string sMsg = "File ";
                sMsg += sPath;
                sMsg += " is too big";
                MessageBox.Show (sMsg, 
                                 "Open file error",
                                 MessageBoxButtons.OK, 
                                 MessageBoxIcon.Exclamation);
                return;
            }

            byteSource = new byte[inStream.Length];
            br.BaseStream.Position = 0;
            int iSaveAt = 0;
            bool bContinue = true;
            while (bContinue)
            {
                try
                {
                    int iBytesRead = br.Read (byteSource,
                                              iSaveAt,
                                              Math.Min (ciChunkSize,
                                                        byteSource.Length - iSaveAt));
                    if (iBytesRead < ciChunkSize)
                    {
                        bContinue = false;
                    }
                    else
                    {
                        iSaveAt += iBytesRead;
                    }
                }
                catch (EndOfStreamException)
                {
                    lSourceLength = 0;
                    bContinue = false;
                    MessageBox.Show ("Unexpected end of file", 
                                     "Internal error",
                                     MessageBoxButtons.OK, 
                                     MessageBoxIcon.Exclamation);
                }
                catch (IOException eError)
                {
                    bContinue = false;
                    lSourceLength = 0;
                    MessageBox.Show (eError.Message,
                                     "I/O Error",
                                     MessageBoxButtons.OK,
                                     MessageBoxIcon.Exclamation);
                }
            }

            lSourceLength = inStream.Length;

            inStream.Dispose();

        }   //  ReadSourceData (...)

        private bool bConvert()
        {
            if (0 == byteSource.Length)
            {
                MessageBox.Show ("Nothing to convert",
                                 "Error",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);

                return false;
            }

            Encoding encoding;
            try
            {
                encoding = Encoding.GetEncoding(iCodePage);
            }
            catch (Exception ex)
            {
                string sMsg = string.Format ("Encoding error: {0}", ex.Message);
                MessageBox.Show (sMsg,
                                 "ASCII Decoder",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                return false;
            }

            long lSize = Math.Min (int.MaxValue, lSourceLength);
            sDecoded = encoding.GetString (byteSource, 0, (int)lSize);

            return true;

        }   //  Convert()

        void Save()
        {
            if (sSavePath.Length < 1)
            {
                MessageBox.Show ("Empty save-as path.",
                                 "Internal Error",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                return;
            }

            try
            {
                StreamWriter sw = new StreamWriter (sSavePath, false, Encoding.Unicode);
                sw.Write (sDecoded);
                sw.Flush();
                sw.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show (ex.Message,
                                 "I/O error",
                                 MessageBoxButtons.OK, 
                                 MessageBoxIcon.Exclamation);
            }
        }       //  Save()

        void SaveAs()
        {
            SaveFileDialog saveDlg = new SaveFileDialog();
            saveDlg.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            saveDlg.FilterIndex = 2;
            saveDlg.RestoreDirectory = true;
            saveDlg.OverwritePrompt = true;
            if (saveDlg.ShowDialog() == DialogResult.OK)
            {
                if (saveDlg.FileName != null)
                {
                    sSavePath = saveDlg.FileName;
                    try
                    {
                        StreamWriter sw = new StreamWriter (sSavePath, false, Encoding.Unicode);
                        sw.Write (sDecoded);
                        sw.Flush();
                        sw.Close();
                    }

                    catch (Exception ex)
                    {
                        string sMsg = string.Format ("IO error: {0}", ex.Message);
                        MessageBox.Show (sMsg, 
                                         "ASCII Decoder",
                                         MessageBoxButtons.OK, 
                                         MessageBoxIcon.Exclamation);
                    }
                }
            }
        }       // SaveAs()

        //
        //  Events
        //
        private void toolStripComboBox_SelectedIndexChanged (object sender, EventArgs e)
        {
            ToolStripItem tsi = (ToolStripItem)toolStripComboBox.SelectedItem;
            EncodingInfo ei = (EncodingInfo)hashEncodings[tsi.Tag];
            statusLabel.Text = "Source Encoding: " + ei.DisplayName + " [" + ei.Name + "]";
            iCodePage = ei.CodePage;

            if (iCodePage < 0)
            {
                MessageBox.Show ("Internal error: invalid code page number.",
                                 "Internal error",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                return;
            }

            try
            {
                Encoding encoding = Encoding.GetEncoding (iCodePage);
                long lSize = Math.Min (int.MaxValue, lSourceLength);
                richTextBox.Text = encoding.GetString (byteSource, 0, (int)lSize);

            }
            catch (Exception ex)
            {
                string sMsg = string.Format ("Internal error: {0}", ex.Message);
                MessageBox.Show (sMsg,
                                 "ASCII Decoder",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                return;
            }

            btnOk.Enabled = true;
            saveAsMenuItem.Enabled = true;

        }  //  toolStripComboBox_SelectedIndexChanged (...)

        private void openMenuItem_Click (object sender, EventArgs e)
        {
            OpenFileDialog openDlg = new OpenFileDialog();

            openDlg.InitialDirectory = "c:\\";
            openDlg.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
            openDlg.FilterIndex = 2;
            //            openDlg.RestoreDirectory = true ;

            if (openDlg.ShowDialog() == DialogResult.OK)
            {
                String sPath;
                if ((sPath = openDlg.FileName) != null)
                {
                    if (!File.Exists (sPath))
                    {
                        MessageBox.Show ("No such file",
                                         "Error",
                                         MessageBoxButtons.OK, 
                                         MessageBoxIcon.Exclamation);
                    }
                    else
                    {
                        ReadSourceData (sPath);
                        char[] chrTemp = new char[lSourceLength];
                        for (long lAt = 0; lAt < lSourceLength; ++lAt)
                        {
                            chrTemp[lAt] = (char)byteSource[lAt];
                        }
                        richTextBox.Text = new string (chrTemp);
                    }
                }
            }
        }
        
        private void btnClose_Click (object sender, EventArgs e)
        {
            Close();
        }

        private void btnOK_Click (object sender, EventArgs e)
        {
            bool b_ret = bConvert();
            if (!b_ret)
            {
                return;
            }

/*
            if (0 == byteSource.Length)
            {
                MessageBox.Show ("Nothing to convert",
                                 "Error",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);

                return;
            }

            Encoding encoding;
            try
            {
                encoding = Encoding.GetEncoding (iCodePage);
            }
            catch (Exception ex)
            {
                string sMsg = string.Format ("Encoding error: {0}", ex.Message);
                MessageBox.Show (sMsg,
                                 "ASCII Decoder",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                return;
            }

            long lSize = Math.Min (int.MaxValue, lSourceLength);
            sDecoded = encoding.GetString (byteSource, 0, (int)lSize);
 */ 
            if (!bEmbed)
            {
                if ((null != sSavePath) && (sSavePath.Length > 0))
                {
                    Save();
                }
                else
                {
                    SaveAs();
                }
            }
            else
            {
                Close();
            }
        }       //  btnOK_Click (...)

        private void saveMenuItem_Click (object sender, EventArgs e)
        {
            if (0 == byteSource.Length)
            {
                MessageBox.Show ("Nothing to save",
                                 "Error",
                                 MessageBoxButtons.OK, 
                                 MessageBoxIcon.Exclamation);
                return;
            }

            Encoding encoding;
            try
            {
                encoding = Encoding.GetEncoding (iCodePage);
            }
            catch (Exception ex)
            {
                string sMsg = string.Format ("Encoding error: {0}", ex.Message);
                MessageBox.Show (sMsg,
                                 "ASCII Decoder",
                                 MessageBoxButtons.OK,
                                 MessageBoxIcon.Exclamation);
                return;
            }

            long lSize = Math.Min (int.MaxValue, lSourceLength);
            sDecoded = encoding.GetString (byteSource, 0, (int)lSize);
            if (!bEmbed)
            {
                SaveAs();
            }
        }

    }   // class

}
