using System;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class DGVBaseNumberUpDownEditingControl : NumericUpDown, IDataGridViewEditingControl
	{
		// Needed to forward keyboard messages to the child TextBox control.
		[System.Runtime.InteropServices.DllImport("USER32.DLL", CharSet = System.Runtime.InteropServices.CharSet.Auto)]
		private static extern IntPtr SendMessage(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam);

		// The grid that owns this editing control
		private DataGridView dataGridView;
		// Stores whether the editing control's value has changed or not
		private bool valueChanged;
		// Stores the row index in which the editing control resides
		private int rowIndex;

		public DGVBaseNumberUpDownEditingControl()
        {
            // The editing control must not be part of the tabbing loop
            this.TabStop = false;
        }

		public virtual DataGridView EditingControlDataGridView
		{
			get
			{
				return this.dataGridView;
			}
			set
			{
				this.dataGridView = value;
			}
		}

		public virtual object EditingControlFormattedValue
		{
			get
			{
				return GetEditingControlFormattedValue(DataGridViewDataErrorContexts.Formatting);
			}
			set
			{
				this.Text = (string)value;
			}
		}

		public virtual int EditingControlRowIndex
		{
			get
			{
				return this.rowIndex;
			}
			set
			{
				this.rowIndex = value;
			}
		}

		public virtual bool EditingControlValueChanged
		{
			get
			{
				return this.valueChanged;
			}
			set
			{
				this.valueChanged = value;
			}
		}

		public virtual Cursor EditingPanelCursor
		{
			get
			{
				return Cursors.Default;
			}
		}

		public virtual bool RepositionEditingControlOnValueChange
		{
			get
			{
				return false;
			}
		}

		public virtual void ApplyCellStyleToEditingControl(DataGridViewCellStyle dataGridViewCellStyle)
		{
			this.Font = dataGridViewCellStyle.Font;
			if (dataGridViewCellStyle.BackColor.A < 255)
			{
				// The NumericUpDown control does not support transparent back colors
				Color opaqueBackColor = Color.FromArgb(255, dataGridViewCellStyle.BackColor);
				this.BackColor = opaqueBackColor;
				this.dataGridView.EditingPanel.BackColor = opaqueBackColor;
			}
			else
			{
				this.BackColor = dataGridViewCellStyle.BackColor;
			}
			this.ForeColor = dataGridViewCellStyle.ForeColor;
			this.TextAlign = DGVBaseNumberUpDownCell.TranslateAlignment(dataGridViewCellStyle.Alignment);
		}

		public virtual bool EditingControlWantsInputKey(Keys keyData, bool dataGridViewWantsInputKey)
		{
			switch (keyData & Keys.KeyCode)
			{
				case Keys.Right:
					{
						TextBox textBox = this.Controls[1] as TextBox;
						if (textBox != null)
						{
							// If the end of the selection is at the end of the string,
							// let the DataGridView treat the key message
							if ((this.RightToLeft == RightToLeft.No && !(textBox.SelectionLength == 0 && textBox.SelectionStart == textBox.Text.Length)) ||
								(this.RightToLeft == RightToLeft.Yes && !(textBox.SelectionLength == 0 && textBox.SelectionStart == 0)))
							{
								return true;
							}
						}
						break;
					}

				case Keys.Left:
					{
						TextBox textBox = this.Controls[1] as TextBox;
						if (textBox != null)
						{
							// If the end of the selection is at the begining of the string
							// or if the entire text is selected and we did not start editing,
							// send this character to the dataGridView, else process the key message
							if ((this.RightToLeft == RightToLeft.No && !(textBox.SelectionLength == 0 && textBox.SelectionStart == 0)) ||
								(this.RightToLeft == RightToLeft.Yes && !(textBox.SelectionLength == 0 && textBox.SelectionStart == textBox.Text.Length)))
							{
								return true;
							}
						}
						break;
					}

				case Keys.Down:
					// If the current value hasn't reached its minimum yet, handle the key. Otherwise let
					// the grid handle it.
					if (this.Value > this.Minimum)
					{
						return true;
					}
					break;

				case Keys.Up:
					// If the current value hasn't reached its maximum yet, handle the key. Otherwise let
					// the grid handle it.
					if (this.Value < this.Maximum)
					{
						return true;
					}
					break;

				case Keys.Home:
				case Keys.End:
					{
						// Let the grid handle the key if the entire text is selected.
						TextBox textBox = this.Controls[1] as TextBox;
						if (textBox != null)
						{
							if (textBox.SelectionLength != textBox.Text.Length)
							{
								return true;
							}
						}
						break;
					}

				case Keys.Delete:
					{
						// Let the grid handle the key if the carret is at the end of the text.
						TextBox textBox = this.Controls[1] as TextBox;
						if (textBox != null)
						{
							if (textBox.SelectionLength > 0 ||
								textBox.SelectionStart < textBox.Text.Length)
							{
								return true;
							}
						}
						break;
					}
			}
			return !dataGridViewWantsInputKey;
		}

		public virtual object GetEditingControlFormattedValue(DataGridViewDataErrorContexts context)
		{
			bool userEdit = this.UserEdit;
			try
			{
				// Prevent the Value from being set to Maximum or Minimum when the cell is being painted.
				this.UserEdit = (context & DataGridViewDataErrorContexts.Display) == 0;
				return this.Value.ToString((this.ThousandsSeparator ? "N" : "F") + this.DecimalPlaces.ToString());
			}
			finally
			{
				this.UserEdit = userEdit;
			}
		}

		public virtual void PrepareEditingControlForEdit(bool selectAll)
		{
			TextBox textBox = this.Controls[1] as TextBox;
			if (textBox != null)
			{
				if (selectAll)
				{
					textBox.SelectAll();
				}
				else
				{
					// Do not select all the text, but
					// position the caret at the end of the text
					textBox.SelectionStart = textBox.Text.Length;
				}
			}
		}

		private void NotifyDataGridViewOfValueChange()
		{
			if (!this.valueChanged)
			{
				this.valueChanged = true;
				this.dataGridView.NotifyCurrentCellDirty(true);
			}
		}

		protected override void OnKeyPress(KeyPressEventArgs e)
		{
			base.OnKeyPress(e);

			// The value changes when a digit, the decimal separator, the group separator or
			// the negative sign is pressed.
			bool notifyValueChange = false;
			if (char.IsDigit(e.KeyChar))
			{
				notifyValueChange = true;
			}
			else
			{
				System.Globalization.NumberFormatInfo numberFormatInfo = System.Globalization.CultureInfo.CurrentCulture.NumberFormat;
				string decimalSeparatorStr = numberFormatInfo.NumberDecimalSeparator;
				string groupSeparatorStr = numberFormatInfo.NumberGroupSeparator;
				string negativeSignStr = numberFormatInfo.NegativeSign;
				if (!string.IsNullOrEmpty(decimalSeparatorStr) && decimalSeparatorStr.Length == 1)
				{
					notifyValueChange = decimalSeparatorStr[0] == e.KeyChar;
				}
				if (!notifyValueChange && !string.IsNullOrEmpty(groupSeparatorStr) && groupSeparatorStr.Length == 1)
				{
					notifyValueChange = groupSeparatorStr[0] == e.KeyChar;
				}
				if (!notifyValueChange && !string.IsNullOrEmpty(negativeSignStr) && negativeSignStr.Length == 1)
				{
					notifyValueChange = negativeSignStr[0] == e.KeyChar;
				}
			}

			if (notifyValueChange)
			{
				// Let the DataGridView know about the value change
				NotifyDataGridViewOfValueChange();
			}
		}

		protected override void OnValueChanged(EventArgs e)
		{
			base.OnValueChanged(e);
			if (this.Focused)
			{
				// Let the DataGridView know about the value change
				NotifyDataGridViewOfValueChange();
			}
		}

		protected override bool ProcessKeyEventArgs(ref Message m)
		{
			TextBox textBox = this.Controls[1] as TextBox;
			if (textBox != null)
			{
				SendMessage(textBox.Handle, m.Msg, m.WParam, m.LParam);
				return true;
			}
			else
			{
				return base.ProcessKeyEventArgs(ref m);
			}
		}
	}
}
