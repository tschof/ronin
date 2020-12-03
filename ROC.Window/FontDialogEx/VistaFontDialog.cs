using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Diagnostics;

namespace FontDialogEx
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class VistaFontDialog : Form, INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -

		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, e);
			}
		}

		#endregion

		private ArrayList _fonts;
		private Font _currentFont;
		private Font _newFont;

		public VistaFontDialog()
		{
			InitializeComponent();
		}

		private void SelectedFontChanged(object sender, EventArgs e)
		{
			try
			{
				if (_newFont == null)
				{
					_newFont = Font;
				}

				string fontSelectedFamilyName = "";
				double fontSelectedSize = 0;
				FontStyle fontSelectedStyle = FontStyle.Regular;

				#region - Font Name -
				if (_newFont.Name != listBoxFontNames.SelectedItem.ToString())
				{
					fontSelectedFamilyName = listBoxFontNames.SelectedItem.ToString();
				}
				else
				{
					fontSelectedFamilyName = _newFont.Name;
				}
				lblSelectedFontName.Text = listBoxFontNames.SelectedItem.ToString();
				#endregion

				#region - Font Size -
				if (listBoxFontSizes.SelectedItem != null)
				{
					fontSelectedSize = Double.Parse(listBoxFontSizes.SelectedItem.ToString());
				}
				else
				{
					fontSelectedSize = _newFont.SizeInPoints;
				}
				lblSelectedFontSize.Text = fontSelectedSize.ToString();
				#endregion

				#region - Font Style -
				if (listBoxFontStyles.SelectedItem != null)
				{
					switch (listBoxFontStyles.SelectedItem.ToString())
					{
						case "Italic":
							fontSelectedStyle = FontStyle.Italic;
							break;
						case "Bold":
							fontSelectedStyle = FontStyle.Bold;
							break;
						case "Bold Italic":
							fontSelectedStyle = FontStyle.Bold | FontStyle.Italic;
							break;
						case "Regular":
						default:
							fontSelectedStyle = FontStyle.Regular;
							break;
					}
				}
				else
				{
					fontSelectedStyle = _newFont.Style;
				}
				lblSelectFontStyle.Text = fontSelectedStyle.ToString();
				#endregion

				#region - Font Underline -
				if (checkBoxUnderline.Checked)
				{
					fontSelectedStyle = fontSelectedStyle | FontStyle.Underline;
				}
				#endregion

				_newFont = new Font(fontSelectedFamilyName, (float)fontSelectedSize, fontSelectedStyle);

				lblFontPreview.Font = _newFont;
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message);
				//Debug.Assert(!Debugger.IsAttached, ex.Message, ex.StackTrace);
			}
		}

		private void SelectCurrentFont()
		{
			if (_currentFont != null)
			{
				if (_newFont == null)
				{
					_newFont = _currentFont;
				}

				LoadFontList();
				if (!_fonts.Contains(_currentFont.Name))
				{
					LoadFontList(true);
				}

				#region - Font Name -
				if (!_fonts.Contains(_currentFont.Name))
				{
					listBoxFontNames.SelectedItem = _fonts[0];
				}
				else
				{
					listBoxFontNames.SelectedItem = _currentFont.Name;
				}
				lblSelectedFontName.Text = listBoxFontNames.SelectedItem.ToString();
				#endregion

				#region - Font Size -
				if (!listBoxFontSizes.Items.Contains(_currentFont.Size.ToString()))
				{
					// default to the font size 8.25;
					listBoxFontSizes.SelectedItem = "8.25";
				}
				else
				{
					listBoxFontSizes.SelectedItem = _currentFont.SizeInPoints.ToString();
				}
				lblSelectedFontSize.Text = listBoxFontSizes.SelectedItem.ToString();
				#endregion

				#region - Font Style -
				if (!listBoxFontStyles.Items.Contains(_currentFont.Style.ToString()))
				{
					// default to Regular
					listBoxFontStyles.SelectedItem = "Regular";
				}
				else
				{
					listBoxFontStyles.SelectedItem = _currentFont.Style.ToString();
				}
				lblSelectFontStyle.Text = listBoxFontStyles.SelectedItem.ToString();
				#endregion

				#region - Font Underline -
				checkBoxUnderline.Checked = _currentFont.Underline;
				#endregion
			}
		}

		private void LoadFontList()
		{
			LoadFontList(false);
		}
		private void LoadFontList(bool reload)
		{
			if (_fonts == null || reload)
			{
				_fonts = new ArrayList();
				foreach (FontFamily f in FontFamily.Families)
				{
					_fonts.Add(f.Name);
				}
			}
			listBoxFontNames.DataSource = _fonts;
		}

		public Font NewFont
		{
			get
			{
				return _newFont;
			}
		}

		public Font CurrentFont
		{
			get
			{
				return _currentFont;
			}
			set
			{
				_currentFont = value;
				SelectCurrentFont();
			}
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void cmdApply_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Font"));
			Close();
		}
	}
}
