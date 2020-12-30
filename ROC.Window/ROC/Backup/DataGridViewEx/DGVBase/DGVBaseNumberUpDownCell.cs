using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVBaseNumberUpDownCell : DGVBaseCellNoPaint
	{
		// Used in TranslateAlignment function
		private static readonly DataGridViewContentAlignment anyRight = DataGridViewContentAlignment.TopRight |
																		DataGridViewContentAlignment.MiddleRight |
																		DataGridViewContentAlignment.BottomRight;
		private static readonly DataGridViewContentAlignment anyCenter = DataGridViewContentAlignment.TopCenter |
																		 DataGridViewContentAlignment.MiddleCenter |
																		 DataGridViewContentAlignment.BottomCenter;

		private DGVBaseNumberUpDownColumn owningColumn
		{
			get
			{
				return (DGVBaseNumberUpDownColumn)OwningColumn;
			}
		}

		private string SymbolDetail
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex <= DataGridView.Rows.Count - 1 && DataGridView.Columns.Contains("SymbolDetail") && OwningRow.Cells["SymbolDetail"] != null)
					{
						return (string)OwningRow.Cells["SymbolDetail"].Value;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
				return "";
			}
		}

		private char[] symbolSpliter = new char[] { ' ' };
		private string UnderlyingSymbol
		{
			get
			{
				try
				{
					if (SymbolDetail != "" && SymbolDetail.Contains(" "))
					{
						string[] t = SymbolDetail.Split(symbolSpliter);
						if (t.Length > 0)
						{
							return t[0];
						}
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return "";
			}
		}

		private string BaseSymbol
		{
			get
			{
				try
				{
					if (UnderlyingSymbol != "" && UnderlyingSymbol.Length >= 4)
					{
						int num = 0;
						if (Int32.TryParse(UnderlyingSymbol.Substring(UnderlyingSymbol.Length - 1, 1), out num))
						{
							switch (UnderlyingSymbol.Substring(UnderlyingSymbol.Length - 2, 1))
							{
								case GLOBAL.MonthCodes.January:
								case GLOBAL.MonthCodes.February:
								case GLOBAL.MonthCodes.March:
								case GLOBAL.MonthCodes.April:
								case GLOBAL.MonthCodes.May:
								case GLOBAL.MonthCodes.June:
								case GLOBAL.MonthCodes.July:
								case GLOBAL.MonthCodes.August:
								case GLOBAL.MonthCodes.September:
								case GLOBAL.MonthCodes.October:
								case GLOBAL.MonthCodes.November:
								case GLOBAL.MonthCodes.December:
									return UnderlyingSymbol.Substring(0, UnderlyingSymbol.Length - 2);
								default:
									break;
							}
						}
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return "";
			}
		}

		public double TickSize
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex <= DataGridView.Rows.Count - 1 && DataGridView.Columns.Contains("TickSize") && OwningRow.Cells["TickSize"].Value != null)
					{
						return (double)OwningRow.Cells["TickSize"].Value;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
				return 0.01;
			}
		}

		public decimal TickSizeDec
		{
			get
			{
				return Convert.ToDecimal(TickSize);
			}
		}

		public string SecType
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex <= DataGridView.Rows.Count - 1 && DataGridView.Columns.Contains("SecType") && OwningRow.Cells["SecType"].Value != null)
					{
						return OwningRow.Cells["SecType"].Value.ToString();
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
				return "";
			}
		}

		public double DisplayFactor
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex < DataGridView.Rows.Count - 1 && DataGridView.Columns.Contains("DisplayFactor") && OwningRow.Cells["DisplayFactor"] != null)
					{
						return (double)OwningRow.Cells["DisplayFactor"].Value;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
				return 1;
			}
		}

		private int decimalPlaces
		{
			get
			{
				if (owningColumn.MaxDecimal == 0)
				{
					return 0;
				}
				else
				{
					string[] strs = TickSizeDec.ToString().Split(new char[] { '.' }, StringSplitOptions.None);
					if (strs.Length > 1)
					{
						return strs[1].Trim().Length;
					}
					else
					{
						return 0;
					}
				}
			}
		}

		private Decimal _maximum = -1;
		public Decimal Maximum
		{
			get
			{
				if (_maximum == -1)
				{
					_maximum = owningColumn.DefaultMaximum;
				}
				return _maximum;
			}
			set
			{
				_maximum = value;
			}
		}

		private Decimal _minimum = -1;
		public Decimal Minimum
		{
			get
			{
				if (_minimum == -1)
				{
					_minimum = owningColumn.DefaultMinimum;
				}
				return _minimum;
			}
			set
			{
				_minimum = value;
			}
		}

		private Decimal _increment = 0;
		public Decimal Increment
		{
			get
			{
				if (_increment == 0)
				{
					_increment = owningColumn.DefaultIncrement;
				}
				return _increment;
			}
			set
			{
				_increment = value;
			}
		}

		private bool isInEditMode
		{
			get
			{
				try
				{
					if (this.RowIndex >= 0 &&
						this.IsInEditMode)
					{
						return true;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return false;
			}
		}

		[ThreadStatic]
		private Bitmap _renderingBitmap;
		private Bitmap renderingBitmap
		{
			get
			{
				if (_renderingBitmap == null)
				{
					_renderingBitmap = new Bitmap(owningColumn.DefaultRenderingBitmapWidth, owningColumn.DefaultRenderingBitmapHeight);
				}
				return _renderingBitmap;
			}
			set
			{
				_renderingBitmap = value;
			}
		}

		[ThreadStatic]
		private NumericUpDown _paintingNumericUpDown;
		private NumericUpDown paintingNumericUpDown
		{
			get
			{
				if (_paintingNumericUpDown == null)
				{
					_paintingNumericUpDown = new NumericUpDown();
					_paintingNumericUpDown.BorderStyle = BorderStyle.None;
					_paintingNumericUpDown.Maximum = Maximum;
					_paintingNumericUpDown.Minimum = Minimum;
					_paintingNumericUpDown.ThousandsSeparator = false;
				}
				return _paintingNumericUpDown;
			}
		}

		private Rectangle _valBounds;
		private Rectangle valBounds
		{
			get
			{
				return _valBounds;
			}
			set
			{
				_valBounds = value;
			}
		}

		// Type of this cell's editing control
		private static Type defaultEditType = typeof(DGVBaseNumberUpDownEditingControl);

		public override Type EditType
		{
			get
			{
				return defaultEditType; // the type is DataGridViewNumericUpDownEditingControl
			}
		}

		protected override Size GetPreferredSize(Graphics graphics, DataGridViewCellStyle cellStyle, int rowIndex, Size constraintSize)
		{
			if (this.DataGridView == null)
			{
				return new Size(-1, -1);
			}

			Size preferredSize = base.GetPreferredSize(graphics, cellStyle, rowIndex, constraintSize);
			if (constraintSize.Width == 0)
			{
				const int ButtonsWidth = 16; // Account for the width of the up/down buttons.
				const int ButtonMargin = 8;  // Account for some blank pixels between the text and buttons.
				preferredSize.Width += ButtonsWidth + ButtonMargin;
			}
			return preferredSize;
		}

		protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			try
			{
				bool selected = false;

				if (!isInEditMode && (cellState & DataGridViewElementStates.Visible) == DataGridViewElementStates.Visible)
				{
					if ((cellState & DataGridViewElementStates.Selected) == DataGridViewElementStates.Selected)
					{
						selected = true;
					}
					else
					{
						selected = false;
					}

					// First paint the borders and background of the cell.
					base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle,
							   paintParts & ~(DataGridViewPaintParts.ErrorIcon | DataGridViewPaintParts.ContentForeground));

					NumericUpDownCellImage(selected, graphics, cellBounds, formattedValue, cellStyle, advancedBorderStyle, paintParts);
				}
				else
				{
					base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
				}
			}
			catch (Exception ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}
			}
		}

		public override void InitializeEditingControl(int rowIndex, object initialFormattedValue, DataGridViewCellStyle dataGridViewCellStyle)
		{
			base.InitializeEditingControl(rowIndex, initialFormattedValue, dataGridViewCellStyle);
			NumericUpDown numericUpDown = this.DataGridView.EditingControl as NumericUpDown;

			if (numericUpDown != null)
			{
				numericUpDown.BorderStyle = BorderStyle.None;
				numericUpDown.DecimalPlaces = decimalPlaces;

				switch (ValueType.Name)
				{
					case "Decimal":
					case "Double":
						numericUpDown.Increment = TickSizeDec * owningColumn.IncrementMultiple;
						break;
					default:
						numericUpDown.Increment = Increment * owningColumn.IncrementMultiple;
						break;
				}
				numericUpDown.Maximum = Maximum;
				numericUpDown.Minimum = Minimum;
				numericUpDown.ThousandsSeparator = false;
				string initialFormattedValueStr = initialFormattedValue as string;
				if (initialFormattedValueStr == null || initialFormattedValueStr == "")
				{
					numericUpDown.Text = "0";
				}
				else
				{
					numericUpDown.Text = initialFormattedValueStr;
				}
			}
		}

		public override void PositionEditingControl(bool setLocation, bool setSize, Rectangle cellBounds, Rectangle cellClip, DataGridViewCellStyle cellStyle, bool singleVerticalBorderAdded, bool singleHorizontalBorderAdded, bool isFirstDisplayedColumn, bool isFirstDisplayedRow)
		{
			Rectangle editingControlBounds = PositionEditingPanel(cellBounds,
														cellClip,
														cellStyle,
														singleVerticalBorderAdded,
														singleHorizontalBorderAdded,
														isFirstDisplayedColumn,
														isFirstDisplayedRow);
			editingControlBounds = GetAdjustedEditingControlBounds(editingControlBounds, cellStyle);
			this.DataGridView.EditingControl.Location = new Point(editingControlBounds.X, editingControlBounds.Y);
			this.DataGridView.EditingControl.Size = new Size(editingControlBounds.Width, editingControlBounds.Height);
		}

		[EditorBrowsable(EditorBrowsableState.Advanced)]
		public override void DetachEditingControl()
		{
			try
			{
				DataGridView dataGridView = this.DataGridView;
				if (dataGridView == null || dataGridView.EditingControl == null)
				{
					return;
				}
				
				NumericUpDown numericUpDown = dataGridView.EditingControl as NumericUpDown;
				if (numericUpDown != null)
				{
					// Editing controls get recycled. Indeed, when a DGVBaseNumericUpDownCell cell gets edited
					// after another DGVBaseNumericUpDownCell cell, the same editing control gets reused for 
					// performance reasons (to avoid an unnecessary control destruction and creation). 
					// Here the undo buffer of the TextBox inside the NumericUpDown control gets cleared to avoid
					// interferences between the editing sessions.
					TextBox textBox = numericUpDown.Controls[1] as TextBox;
					if (textBox != null)
					{
						textBox.ClearUndo();
					}
				}
			}
			catch (Exception ex)
			{
				if (Debugger.IsAttached)
				{
					Debug.Print(ex.Message);
					Debug.Print(ex.StackTrace);
				}
			}

			base.DetachEditingControl();
		}

		public override string GetDisplayValue(object value)
		{
			if (value != null && value.ToString() != "")
			{
				switch (ValueType.Name)
				{
					case "Decimal":
					case "Double":
						return GetDoubleFormat(Convert.ToDouble(value));
					case "Int":
					case "Int64":
					case "Long":
						return GetLongFormat(Convert.ToInt64(value));
					case "String":
					default:
						break;
				}
			}

			return "";
		}

		#region - NumericUpDownCell -

		private void NumericUpDownCellImage(bool selected, Graphics graphics, Rectangle cellBounds, object formattedValue, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
		{
			try
			{
				if (_valBounds != cellBounds)
				{
					_valBounds = cellBounds;
					Rectangle borderWidths = BorderWidths(advancedBorderStyle);
					_valBounds.Offset(borderWidths.X, borderWidths.Y);
					_valBounds.Width -= borderWidths.Right;
					_valBounds.Height -= borderWidths.Bottom;
				}

				// Determine the NumericUpDown control location
				_valBounds = GetAdjustedEditingControlBounds(_valBounds, cellStyle);

				if (renderingBitmap.Width < valBounds.Width ||
					renderingBitmap.Height < valBounds.Height)
				{
					// The static bitmap is too small, a bigger one needs to be allocated.
					renderingBitmap.Dispose();
					renderingBitmap = new Bitmap(valBounds.Width, valBounds.Height);
				}
				// Make sure the NumericUpDown control is parented to a visible control
				if (paintingNumericUpDown.Parent == null || !paintingNumericUpDown.Parent.Visible)
				{
					paintingNumericUpDown.Parent = this.DataGridView;
				}
				// Set all the relevant properties
				paintingNumericUpDown.TextAlign = DGVBaseNumberUpDownCell.TranslateAlignment(cellStyle.Alignment);
				paintingNumericUpDown.DecimalPlaces = decimalPlaces;
				paintingNumericUpDown.Font = cellStyle.Font;
				paintingNumericUpDown.Width = _valBounds.Width;
				paintingNumericUpDown.Height = _valBounds.Height;
				paintingNumericUpDown.Location = new Point(0, -paintingNumericUpDown.Height - 100);
				paintingNumericUpDown.Text = formattedValue as string;
				//Debug.Print(paintingNumericUpDown.Text);

				Color backColor;
				Color foreColor;
				if (PartPainted(paintParts, DataGridViewPaintParts.SelectionBackground) && selected)
				{
					backColor = cellStyle.SelectionBackColor;
					foreColor = cellStyle.SelectionForeColor;
				}
				else
				{
					backColor = cellStyle.BackColor;
					foreColor = cellStyle.ForeColor;
				}
				if (PartPainted(paintParts, DataGridViewPaintParts.Background))
				{
					if (backColor.A < 255)
					{
						// The NumericUpDown control does not support transparent back colors
						backColor = Color.FromArgb(255, backColor);
					}
					if (foreColor.A < 255)
					{
						// The NumericUpDown control does not support transparent fore colors
						foreColor = Color.FromArgb(255, foreColor);
					}
					paintingNumericUpDown.BackColor = backColor;
					paintingNumericUpDown.ForeColor = foreColor;
				}
				// Finally paint the NumericUpDown control
				Rectangle srcRect = new Rectangle(0, 0, valBounds.Width, valBounds.Height);
				if (srcRect.Width > 0 && srcRect.Height > 0)
				{
					paintingNumericUpDown.DrawToBitmap(renderingBitmap, srcRect);
					graphics.DrawImage(renderingBitmap, new Rectangle(valBounds.Location, valBounds.Size), srcRect, GraphicsUnit.Pixel);
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(false, ex.Message, ex.StackTrace);
			}
		}

		#endregion

		#region - Support -

		private string GetLongFormat(long value)
		{
			if (value != 0)
			{
				if (owningColumn.DefaultCellStyle.Format == "F")
				{
					return value.ToString("F0");
				}
				else
				{
					return value.ToString("N0");
				}
			}

			return "";
		}

		private string GetDoubleFormat(double value)
		{
			string secType = SecType;
			double fromatedDouble = 0;
			string formatedValue = "";

			if (value != 0)
			{
				switch (secType)
				{
					case "E":
						#region - Stock -

						if (owningColumn.AllowResolutionChange)
						{
							fromatedDouble = Math.Round(value, owningColumn.MaxDecimal);
							if (owningColumn.DefaultCellStyle.Format == "F")
							{
								formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
							}
							else
							{
								formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
							}
						}
						else
						{
							fromatedDouble = Math.Round(value, 2);
							if (owningColumn.MaxDecimal < 2)
							{
								if (owningColumn.DefaultCellStyle.Format == "F")
								{
									formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
								}
								else
								{
									formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
								}
							}
							else
							{
								if (owningColumn.DefaultCellStyle.Format == "F")
								{
									formatedValue = fromatedDouble.ToString("F2");
								}
								else
								{
									formatedValue = fromatedDouble.ToString("N2");
								}
							}
						}

						#endregion
						break;
					default:
						bool normal = true;
						bool done = false;
						double tickSize = TickSize;

						double floorDouble = 0;
						double remainderDouble = 0;
						int decimalPlace = -1;

						#region - Convert To Fraction -

						if (owningColumn.IsFraction)
						{
							switch (tickSize.ToString())
							{
								case "0.5":
									break;
								case "0.125":			// 1/8
								case "0.0625":			// 1/16
								case "0.03125":			// 1/32
								case "0.015625":		// 1/64
								case "0.0078125":		// 1/128
									if (owningColumn.Is64Th ||
										(owningColumn.CheckFor64Th && GLOBAL.OptionOnFuture.ShowAs64TH.Contains(BaseSymbol)))
									{
										floorDouble = Math.Floor(Math.Abs(value));
										remainderDouble = Math.Abs(value) - floorDouble;
										remainderDouble = remainderDouble / 0.015625;
										formatedValue = string.Format("{0}'{1}", floorDouble.ToString("N0"), remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
										normal = false;
									}
									else
									{
										floorDouble = Math.Floor(Math.Abs(value));
										remainderDouble = Math.Abs(value) - floorDouble;
										remainderDouble = remainderDouble / 0.03125;
										formatedValue = string.Format("{0}'{1}", floorDouble.ToString("N0"), remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
										normal = false;
									}
									break;
								case "0.0025":			// 1/400
								case "0.00125":			// 1/800
									floorDouble = Math.Floor(Math.Abs(value) * 100);
									remainderDouble = (Math.Abs(value) * 100) - floorDouble;
									remainderDouble = remainderDouble - Math.Floor(remainderDouble);
									floorDouble = (floorDouble / 100);
									switch (remainderDouble.ToString("F3"))
									{
										case "0.125":
											formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 1);
											break;
										case "0.250":
											formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 2);
											break;
										case "0.375":
											formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 3);
											break;
										case "0.500":
											formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 4);
											break;
										case "0.625":
											formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 5);
											break;
										case "0.750":
											formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 6);
											break;
										case "0.875":
											formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 7);
											break;
										default:
											formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 0);
											break;
									}
									formatedValue = formatedValue.Replace(".", "'");
									normal = false;
									break;
								default:
									break;
							}
						}

						#endregion

						#region - Normal Tick Size -

						if (normal)
						{
							fromatedDouble = Math.Round(value, owningColumn.MaxDecimal);
							if (owningColumn.AllowResolutionChange)
							{
								if (owningColumn.DefaultCellStyle.Format == "F")
								{
									formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
								}
								else
								{
									formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
								}
							}
							else
							{
								//_fromatedDouble = _fromatedDouble * DisplayFactor;
								switch (owningColumn.MaxDecimal)
								{
									case 0:
									case 1:
									case 2:
									case 3:
									case 4:
										#region - 4 Decimal -
										if (owningColumn.DefaultCellStyle.Format == "F")
										{
											formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
										}
										else
										{
											formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
										}
										break;
										#endregion
									default:
										#region - If isFraction -
										if (owningColumn.IsFraction)
										{
											if (tickSize >= 1)
											{
												if (owningColumn.DefaultCellStyle.Format == "F")
												{
													formatedValue = fromatedDouble.ToString("F0");
												}
												else
												{
													formatedValue = fromatedDouble.ToString("N0");
												}
												done = true;
											}
											else if (tickSize == 0.5 || tickSize == 0.1)
											{
												if (owningColumn.DefaultCellStyle.Format == "F")
												{
													formatedValue = fromatedDouble.ToString("F1");
												}
												else
												{
													formatedValue = fromatedDouble.ToString("N1");
												}
												done = true;
											}
										}
										#endregion

										#region - If still not done by fraction conversion -
										if (!done)
										{
											if (owningColumn.DefaultCellStyle.Format == "F")
											{
												formatedValue = fromatedDouble.ToString("F7").TrimEnd(new char[] { '0' });
											}
											else
											{
												formatedValue = fromatedDouble.ToString("N7").TrimEnd(new char[] { '0' });
											}
											decimalPlace = formatedValue.IndexOf('.');
											decimalPlace = (formatedValue.Substring(decimalPlace + 1)).Length;

											switch (decimalPlace)
											{
												case 0:
													formatedValue = String.Concat(new object[] { formatedValue, "00" });
													break;
												case 1:
													formatedValue = String.Concat(new object[] { formatedValue, "0" });
													break;
												default:
													break;
											}
										}
										#endregion
										break;
								}
							}
						}

						#endregion
						break;
				}

				if (formatedValue != "")
				{
					if (owningColumn.IsPercent)
					{
						formatedValue = String.Concat(new object[] { formatedValue, "%" });
					}
					else if (owningColumn.IsCurrency)
					{
						if (value < 0)
						{
							formatedValue = string.Format("(${0})", formatedValue.Replace("-", ""));
						}
						else
						{
							formatedValue = string.Format("${0}", formatedValue);
						}
					}
				}
			}

			return formatedValue;
		}

		private Rectangle GetAdjustedEditingControlBounds(Rectangle editingControlBounds, DataGridViewCellStyle cellStyle)
		{
			// Add a 1 pixel padding on the left and right of the editing control
			editingControlBounds.X += 1;
			editingControlBounds.Width = Math.Max(0, editingControlBounds.Width - 2);

			// Adjust the vertical location of the editing control:
			int preferredHeight = cellStyle.Font.Height + 3;
			if (preferredHeight < editingControlBounds.Height)
			{
				switch (cellStyle.Alignment)
				{
					case DataGridViewContentAlignment.MiddleLeft:
					case DataGridViewContentAlignment.MiddleCenter:
					case DataGridViewContentAlignment.MiddleRight:
						editingControlBounds.Y += (editingControlBounds.Height - preferredHeight) / 2;
						break;
					case DataGridViewContentAlignment.BottomLeft:
					case DataGridViewContentAlignment.BottomCenter:
					case DataGridViewContentAlignment.BottomRight:
						editingControlBounds.Y += editingControlBounds.Height - preferredHeight;
						break;
				}
			}

			return editingControlBounds;
		}

		private static bool PartPainted(DataGridViewPaintParts paintParts, DataGridViewPaintParts paintPart)
		{
			return (paintParts & paintPart) != 0;
		}

		internal static HorizontalAlignment TranslateAlignment(DataGridViewContentAlignment align)
		{
			if ((align & anyRight) != 0)
			{
				return HorizontalAlignment.Right;
			}
			else if ((align & anyCenter) != 0)
			{
				return HorizontalAlignment.Center;
			}
			else
			{
				return HorizontalAlignment.Left;
			}
		}

		#endregion
	}
}
