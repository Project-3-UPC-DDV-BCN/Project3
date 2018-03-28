using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace TheEngine
{
    class TheDialogManager
    {
        //List<Dialog> dialogs;
        //Dialog curr_dialog = null;
        //int curr_dialog_line = 0;
        //TheTimer curr_dialog_timer;

        //private Dialog GetDialog(string name)
        //{
        //    for (int i = 0; i < dialogs.Count; ++i)
        //    {
        //        if (dialogs[i].GetName() == name)
        //        {
        //            return dialogs[i];
        //        }
        //    }

        //    return null;
        //}

        //public void CreateDialog(string name)
        //{
        //    Dialog d = new Dialog(name);
        //    dialogs.Add(d);
        //}

        //public void AddLineToDialog(string dialog_name, string line_text, float time)
        //{
        //    Dialog d = GetDialog(dialog_name);

        //    if (d != null)
        //    {
        //        d.AddLine(line_text, time);
        //    }
        //}
            
        //public void FireDialog(string name)
        //{
        //    Dialog d = GetDialog(name);

        //    if (d != null)
        //    {
        //        curr_dialog = d;
        //        curr_dialog_line = 0;
        //        curr_dialog_timer.Start();
        //    }      
        //}

        //string GetCurrentText()
        //{
        //    string current_text = "";

        //    if (curr_dialog != null)
        //    {
        //        if (curr_dialog_line < curr_dialog.GetDialogLinesCount())
        //        {                   
        //            DialogLine line = curr_dialog.GetDialogLine(curr_dialog_line);

        //            if (line != null)
        //            {
        //                current_text = line.text;

        //                if (curr_dialog_timer.ReadTime() > line.time)
        //                {
        //                    curr_dialog_timer.Start();
        //                    curr_dialog_line++;

        //                    if (curr_dialog_line >= curr_dialog.GetDialogLinesCount())
        //                        curr_dialog = null;
        //                }
        //            }
                       
        //        }
        //    }

        //    return current_text;
        //}

        //public class Dialog
        //{
        //    public Dialog(string name, bool automatic = true)
        //    {
        //        dialog_name = name;
        //        dialog_automatic = automatic;
        //    }

        //    public string GetName() { return dialog_name; }
        //    public int GetDialogLinesCount() { return dialog_lines.Count; }
        //    public DialogLine GetDialogLine(int index)
        //    {
        //        if (index < dialog_lines.Count)
        //        {
        //            return dialog_lines[index];
        //        }
        //        return null;
        //    }

        //    public void AddLine(string text, float time)
        //    {
        //        DialogLine line = new DialogLine();
        //        line.text = text;
        //        line.time = time;
        //    }

        //    string dialog_name;
        //    bool dialog_automatic;

        //    List<DialogLine> dialog_lines;
        //}

        //public class DialogLine
        //{
        //    public string text;
        //    public float time;
        //}
    }
}

