using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;

namespace VO_soft
{
    class SaverLoaderSettings
    {
        internal static ArrayList get_objects_to_save(Form1 form1, FormSettings formSettings)
        {
            ArrayList to_save = new ArrayList();

            foreach (Control obj in form1.Controls)
            {
                if ((obj.GetType() == typeof(NumericUpDown)) | (obj.GetType() == typeof(TextBox)) | (obj.GetType() == typeof(CheckBox)))
                    to_save.Add(obj);
            }
            foreach (Control obj in formSettings.Controls)
            {
                if ((obj.GetType() == typeof(NumericUpDown)) | (obj.GetType() == typeof(TextBox)) | (obj.GetType() == typeof(CheckBox)))
                    to_save.Add(obj);
            }

            foreach (Control obj in formSettings.panel1.Controls)
            {
                if (obj.GetType() == typeof(ComboBox))
                    to_save.Add(obj);
            }

            foreach (Control obj in formSettings.panel2.Controls)
            {
                if (obj.GetType() == typeof(NumericUpDown))
                    to_save.Add(obj);
            }

            foreach (Control obj in formSettings.Controls)
            {
                if (obj.GetType() == typeof(ComboBox))
                    to_save.Add(obj);
            }

            return to_save;
        }


        internal static void Save(Form1 form1, string filename)
        {
            ArrayList to_save = SaverLoaderSettings.get_objects_to_save(form1, form1.formSettings);
            Dictionary<string, object> _data = new Dictionary<string, object>();


            foreach (object obj in to_save)
            {
                if (obj.GetType() == typeof(NumericUpDown))
                {
                    NumericUpDown obj_converted = (NumericUpDown)Convert.ChangeType(obj, typeof(NumericUpDown));
                    _data.Add(obj_converted.Name, obj_converted.Value);
                }
                if (obj.GetType() == typeof(TextBox))
                {
                    TextBox obj_converted = (TextBox)Convert.ChangeType(obj, typeof(TextBox));
                    _data.Add(obj_converted.Name, obj_converted.Text);
                }
                if (obj.GetType() == typeof(ComboBox))
                {
                    ComboBox obj_converted = (ComboBox)Convert.ChangeType(obj, typeof(ComboBox));
                    _data.Add(obj_converted.Name, obj_converted.Text);
                }
                if (obj.GetType() == typeof(CheckBox))
                {
                    CheckBox obj_converted = (CheckBox)Convert.ChangeType(obj, typeof(CheckBox));
                    _data.Add(obj_converted.Name, obj_converted.Checked);
                }
            }
            _data.Add("version", form1.formAbout.label_version.Text);

            string json = JsonConvert.SerializeObject(_data);
            File.WriteAllText(filename, json);
        }

        internal static void Load(Form1 form1, string filename)
        {
            if (File.Exists(filename))
            {
                string json_loaded = File.ReadAllText(filename);
                Dictionary<string, object> _data = JsonConvert.DeserializeObject<Dictionary<string, object>>(json_loaded);
                ArrayList to_save = SaverLoaderSettings.get_objects_to_save(form1, form1.formSettings);

                for (int i = 0; i < to_save.Count; i++)
                {
                    object obj = to_save[i];
                    if (obj.GetType() == typeof(NumericUpDown))
                    {
                        NumericUpDown obj_converted = (NumericUpDown)Convert.ChangeType(obj, typeof(NumericUpDown));
                        var name = obj_converted.Name;
                        if (name == "numericUpDown_LED")
                            obj_converted.ValueChanged -= form1.numericUpDown_LED_ValueChanged;
                        obj_converted.Value = (decimal)Convert.ChangeType(_data.ElementAt(i).Value, typeof(decimal));
                        if (name == "numericUpDown_LED")
                            obj_converted.ValueChanged += form1.numericUpDown_LED_ValueChanged;
                    }
                    if (obj.GetType() == typeof(TextBox))
                    {
                        TextBox obj_converted = (TextBox)Convert.ChangeType(obj, typeof(TextBox));
                        obj_converted.Text = (string)Convert.ChangeType(_data.ElementAt(i).Value, typeof(string));
                    }
                    if (obj.GetType() == typeof(ComboBox))
                    {
                        ComboBox obj_converted = (ComboBox)Convert.ChangeType(obj, typeof(ComboBox));
                        obj_converted.SelectedIndexChanged -= form1.formSettings.comboBox_SelectedIndexChanged;
                        obj_converted.Text = (string)Convert.ChangeType(_data.ElementAt(i).Value, typeof(string));
                        obj_converted.SelectedIndexChanged += form1.formSettings.comboBox_SelectedIndexChanged;
                    }
                    if (obj.GetType() == typeof(CheckBox))
                    {
                        CheckBox obj_converted = (CheckBox)Convert.ChangeType(obj, typeof(CheckBox));
                        var name = obj_converted.Name;
                        if (name == "checkBox_LED")
                            obj_converted.CheckedChanged -= form1.checkBox_LED_CheckedChanged;
                        obj_converted.Checked = (bool)Convert.ChangeType(_data.ElementAt(i).Value, typeof(bool));
                        if (name == "checkBox_LED")
                            obj_converted.CheckedChanged += form1.checkBox_LED_CheckedChanged;
                    }

                }

            }
            else{
                MessageBox.Show("settings.json not found!", "Load error");

            }

        }

    }


}
