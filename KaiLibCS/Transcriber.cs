using System;
using System.Xml;

namespace KaiLibCS
{
    public struct Context
    {


    }

    public struct Rule
    {
        string sLetter;
        string sOutput;

    
    }

	/// <summary>
	/// Summary description for Class1.
	/// </summary>
    public class Transcriber
    {
        public Transcriber()
        {
            //
            // TODO: Add constructor logic here
            //
            ReadXMLFile();
        }

        string sCurrentElement;
        public void ReadXMLFile()
        {
            XmlTextReader xmlReader = null;
            try
            {
                string strPath = "C:\\temp\\kai\\Rules\\Russian_1.xml";
                xmlReader = new XmlTextReader (strPath);
                while (xmlReader.Read())
                {
                    if (XmlNodeType.Element == xmlReader.NodeType)
                    {
                        sCurrentElement = xmlReader.Name;
                        int nAttr = xmlReader.AttributeCount;
                        for (int iAttr = 0; iAttr < nAttr; ++iAttr)
                        {
                            xmlReader.MoveToAttribute (iAttr);
                            string sAttr = xmlReader.Name;
//                                int ggg = 0;
                        }
                    }   // if
                }   //  while...
            }   // try
            catch
            {
                int ggg = 0;
            }
            finally
            {
                xmlReader.Close();
            }
        }   //  public void ReadXMLFile (string strPath)
    }
}
