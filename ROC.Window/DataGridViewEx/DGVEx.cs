using System;
using System.Windows.Forms;

using BindingListEx;
using System.ComponentModel;
using System.Drawing;
using System.IO;

namespace DataGridViewEx
{
	public enum NotifyTypes
	{
		None,
		Border,
		BackGround,
		ForeGround,
	}

	[System.ComponentModel.DesignerCategory("Code")]
	public class DGVEx : DataGridView
	{
		private DGVPrinter _printer;
		public DGVPrinter Printer
		{
			get
			{
				if (_printer == null)
				{
					_printer = new DGVPrinter();
					_printer.PageNumbers = true;
					_printer.ShowTotalPageNumber = true;

					_printer.PageNumberOnSeparateLine = true;
					_printer.PageNumberAlignment = StringAlignment.Center;

					_printer.PageSettings.Landscape = true;
				}
				return _printer;
			}
			set
			{
				_printer = value;
			}
		}

		public DGVEx()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer | ControlStyles.OptimizedDoubleBuffer, true);
		}

		#region - Copy & Paste -

		private string _clipboardString;
		private string[] _rows;
		private string[] _cells;

		public void CopyToClipboard()
		{
			if (Rows.Count > 0)
			{
				ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableWithAutoHeaderText;

				MultiSelect = true;
				SelectAll();

				if (GetCellCount(DataGridViewElementStates.Selected) > 0)
				{
					try
					{
						Clipboard.SetDataObject(GetClipboardContent());

						_clipboardString = Clipboard.GetText();
					}
					catch (Exception ex)
					{
						_clipboardString = ex.Message;
					}

					Clipboard.SetDataObject(_clipboardString);
				}

				MultiSelect = false;
				ScrollToLastRow();
			}
			else
			{
				MessageBox.Show("Nothing To Copy");
			}
		}

		public void PastFromClipboard()
		{
			if (SelectedRows.Contains(Rows[NewRowIndex]))
			{
				Rows[NewRowIndex].Selected = false;
				CancelEdit();
			}

			_clipboardString = Clipboard.GetText();

			_rows = _clipboardString.Split(new string[] { "\r\n" }, StringSplitOptions.None);
			foreach (string row in _rows)
			{
				_cells = row.Split(new string[] { "\t" }, StringSplitOptions.None);
				Rows.Add(_cells);
			}
		}

		#endregion

		#region - Save -

		private SaveFileDialog _saveDialog;
		public SaveFileDialog SaveDialog
		{
			get
			{
				if (_saveDialog == null)
				{
					_saveDialog = new SaveFileDialog();
					_saveDialog.Title = "Save Orders";
					_saveDialog.Filter = "Comma Seperated (*.csv)|*.csv";
					_saveDialog.DefaultExt = "csv";
					_saveDialog.InitialDirectory = @"..\";
					_saveDialog.FileOk += new CancelEventHandler(SaveDialog_FileOk);
				}
				return _saveDialog;
			}
			set
			{
				_saveDialog = value;
			}
		}

		private string _csvString;

		public void Save()
		{
			SaveDialog.ShowDialog();
		}

		private void SaveDialog_FileOk(object sender, CancelEventArgs e)
		{
			if (!e.Cancel)
			{
				if (SaveDialog.FileName != "")
				{
					SaveToCSV(SaveDialog.FileName);
				}
			}
		}

		private void SaveToCSV(string path)
		{
			if (Rows.Count > 0)
			{
				ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableWithAutoHeaderText;

				MultiSelect = true;
				SelectAll();

				if (GetCellCount(DataGridViewElementStates.Selected) > 0)
				{
					try
					{
						Clipboard.SetDataObject(GetClipboardContent());

						_csvString = Clipboard.GetText().Replace("\t", ",");
					}
					catch (Exception ex)
					{
						_csvString = ex.Message;
					}

					Clipboard.SetDataObject("");
				}

				MultiSelect = false;

				try
				{
					if (File.Exists(path))
					{
						File.Delete(path);
					}
					File.WriteAllText(path, _csvString);
				}
				catch (Exception ex)
				{
					MessageBox.Show(ex.Message, "Save Failed");
				}
			}
			else
			{
				MessageBox.Show("Nothing To Save");
			}
		}

		#endregion


		#region - Auto Scroll -

		public void ScrollToLastRow()
		{
			if (Rows.Count > 0)
			{
				for (int iRow = Rows.Count - 1; iRow >= 0; iRow--)
				{
					if ((Rows[iRow].State & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible)
					{
						FirstDisplayedScrollingRowIndex = iRow;
						Rows[iRow].Selected = true;
						break;
					}
				}
			}
		}

		#endregion
	}
}
