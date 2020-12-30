using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Collections.Generic;
using System.Data;

using BindingListEx;
using DataSetEx;
using System.Diagnostics;

namespace DataGridViewEx
{
	public enum NotifyTypes
	{
		None,
		Border,
		BackGround,
		ForeGround,
	}

	public sealed class PriceFlages
	{
		public const int stopLimit = 21;
		public const int stop = 22;

		public const int tradeUp = 10;
		public const int tradeDown = 11;
		public const int high = 12;
		public const int low = 13;
		public const int bid = 14;
		public const int ask = 15;
	}

	[System.ComponentModel.DesignerCategory("Code")]
	public class DGVBase : DataGridView
	{
		#region - Event -

		[Description("Event raised when the value of the Processing property is changed."), Category("Property Changed")]
		public event EventHandler ProcessingChanged;

		[Description("Event raised when the value of the Should Scroll To Last Row property is changed."), Category("Property Changed")]
		public event EventHandler ShouldScrollToLastRowChanged;

		[Description("Event raised when the value of the Refresh Aggragation is changed."), Category("Property Changed")]
		public event EventHandler RefreshAggragationChanged;

		[Description("Event raised when rows are shared after load and sort."), Category("Property Changed")]
		public event EventHandler RefreshSharedRowsChanged;

		#endregion

		#region - OnEvent -

		protected virtual void OnProcessingChanged(EventArgs e)
		{
			if (ProcessingChanged != null)
			{
				ProcessingChanged(this, e);
			}
		}

		protected virtual void OnShouldScrollToLastRowChanged(EventArgs e)
		{
			if (ShouldScrollToLastRowChanged != null)
			{
				ShouldScrollToLastRowChanged(this, e);
			}
		}

		protected virtual void OnRefreshAggragationChanged(EventArgs e)
		{
			if (RefreshAggragationChanged != null)
			{
				RefreshAggragationChanged(this, e);
			}
		}

		protected virtual void OnRefreshSharedRowsChanged(EventArgs e)
		{
			if (RefreshSharedRowsChanged != null)
			{
				RefreshSharedRowsChanged(this, e);
			}
		}

		#endregion

		#region - Property With Events -

		private bool _processing = false;
		[Browsable(false)]
		public bool Processing
		{
			get
			{
				return _processing;
			}
			set
			{
				_processing = value;
				OnProcessingChanged(EventArgs.Empty);
			}
		}

		private bool _shouldScrollToLastRow = false;
		[Browsable(false)]
		public bool ShouldScrollToLastRow
		{
			get
			{
				return _shouldScrollToLastRow;
			}
			set
			{
				_shouldScrollToLastRow = value;
				if (_shouldScrollToLastRow)
				{
					OnShouldScrollToLastRowChanged(EventArgs.Empty);
				}
			}
		}

		private bool _refreshAggragation = true;
		[Browsable(false)]
		public bool RefreshAggragation
		{
			get
			{
				return _refreshAggragation;
			}
			set
			{
				_refreshAggragation = value;
				if (_refreshAggragation)
				{
					OnRefreshAggragationChanged(EventArgs.Empty);
				}
			}
		}

		private bool _refreshSharedRows = false;
		[Browsable(false)]
		public bool RefreshSharedRows
		{
			get
			{
				return _refreshSharedRows;
			}
			set
			{
				_refreshSharedRows = value;
				if (_refreshSharedRows)
				{
					OnRefreshSharedRowsChanged(EventArgs.Empty);
				}
			}
		}

		#endregion

		#region - Property -

		private int _redDefault = 233;
		[Browsable(false)]
		public int RedDefault
		{
			get
			{
				return _redDefault;
			}
			set
			{
				_redDefault = value;
			}
		}

		private int _greenDefault = 243;
		[Browsable(false)]
		public int GreenDefault
		{
			get
			{
				return _greenDefault;
			}
			set
			{
				_greenDefault = value;
			}
		} 

		private int _blueDefault = 228;
		[Browsable(false)]
		public int BlueDefault
		{
			get
			{
				return _blueDefault;
			}
			set
			{
				_blueDefault = value;
			}
		}

		private Color _headerColor = Color.Empty;
		[Browsable(false)]
		public Color HeaderBackColor
		{
			get
			{
				if (_headerColor == Color.Empty)
				{
					_headerColor= Color.FromArgb(RedDefault, GreenDefault, BlueDefault);
				}
				return _headerColor;
			}
			set
			{
				_headerColor = value;
			}
		}

		private Color _defaultCellForeColor = Color.White;
		[Browsable(false), DefaultValue(typeof(Color), "White"), Category("Appearance")]
		public Color DefaultCellForeColor
		{
			get
			{
				return _defaultCellForeColor;
			}
			set
			{
				_defaultCellForeColor = value;
			}
		}

		private Color _defaultCellErrorForeColor = Color.Red;
		[Browsable(false), DefaultValue(typeof(Color), "Red"), Category("Appearance")]
		public Color DefaultCellErrorForeColor
		{
			get
			{
				return _defaultCellErrorForeColor;
			}
			set
			{
				_defaultCellErrorForeColor = value;
			}
		}

		private Color _defaultSelectedBackgroundColor = Color.Blue;
		[Browsable(false), DefaultValue(typeof(Color), "Blue"), Category("Appearance")]
		public Color DefaultSelectedBackgroundColor
		{
			get
			{
				return _defaultSelectedBackgroundColor;
			}
			set
			{
				if (_defaultSelectedBackgroundColor != value)
				{
					_defaultSelectedBackgroundColor = value;
				}
			}
		}

		private string _lastSort = "";
		[Browsable(false)]
		public string LastSort
		{
			get
			{
				return _lastSort;
			}
			set
			{
				_lastSort = value;
			}
		}

		private string _lastFilter = "";
		[Browsable(false)]
		public string LastFilter
		{
			get
			{
				return _lastFilter;
			}
			set
			{
				_lastFilter = value;
			}
		}

		private DGVBasePrinter _printer;
		[Browsable(false)]
		public DGVBasePrinter Printer
		{
			get
			{
				if (_printer == null)
				{
					_printer = new DGVBasePrinter();
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

		private ROCBindingList<string> _symbols;
		[Browsable(false)]
		public ROCBindingList<string> Symbols
		{
			get
			{
				if (_symbols == null)
				{
					_symbols = new ROCBindingList<string>();
					_symbols.Add("");
				}
				return _symbols;
			}
			set
			{
				_symbols = value;
			}
		}

		private ROCBindingList<string> _details;
		[Browsable(false)]
		public ROCBindingList<string> Details
		{
			get
			{
				if (_details == null)
				{
					_details = new ROCBindingList<string>();
					_details.Add("");
				}
				return _details;
			}
			set
			{
				_details = value;
			}
		}

		private List<string> _gridKeys;
		[Browsable(false)]
		public List<string> GridKeys
		{
			get
			{
				if (_gridKeys == null)
				{
					_gridKeys = new List<string>();
				}
				return _gridKeys;
			}
			set
			{
				_gridKeys = value;
			}
		}

		private bool _importing = false;
		[Browsable(false)]
		public bool Importing
		{
			get
			{
				return _importing;
			}
			set
			{
				_importing = value;
			}
		}

		private Dictionary<string, double> _tickSizes;
		[Browsable(false)]
		public Dictionary<string, double> TickSizes
		{
			get
			{
				if (_tickSizes == null)
				{
					_tickSizes = new Dictionary<string, double>();
				}
				return _tickSizes;
			}
			set
			{
				_tickSizes = value;
			}
		}

		private Dictionary<string, double> _displayFactors;
		[Browsable(false)]
		public Dictionary<string, double> DisplayFactors
		{
			get
			{
				if (_displayFactors == null)
				{
					_displayFactors = new Dictionary<string, double>();
				}
				return _displayFactors;
			}
			set
			{
				_displayFactors = value;
			}
		}

		private BindingSource _bindingDS;
		[Browsable(false)]
		internal BindingSource BindingDS
		{
			get
			{
				if (_bindingDS == null)
				{
					_bindingDS = new BindingSource();
				}
				return _bindingDS;
			}
			set
			{
				_bindingDS = value;
			}
		}

		private int _editingColumn = -1;
		[Browsable(false)]
		internal int EditingColumn
		{
			get
			{
				return _editingColumn;
			}
		}

		private int _editingRow = -1;
		[Browsable(false)]
		internal int EditingRow
		{
			get
			{
				return _editingRow;
			}
		}

		#endregion

		#region - Local Variables -

		private const string _indexSplit = "-";
		private const string _rowSplit = "\r\n";
		public string RowSplit
		{
			get
			{
				return _rowSplit;
			}
		}
		private const string _colSplit = "\t";
		public string ColSplit
		{
			get
			{
				return _colSplit;
			}
		}

		private readonly string[] _sepecialIndexSplits = new string[] { _indexSplit };
		private readonly string[] _rowSplits = new string[] { _rowSplit };
		private readonly string[] _colSplits = new string[] { _colSplit };

		#endregion

		public DGVBase()
		{
			SetStyle(
				ControlStyles.AllPaintingInWmPaint |
				ControlStyles.CacheText |
				ControlStyles.OptimizedDoubleBuffer |
				ControlStyles.ResizeRedraw |
				ControlStyles.UserPaint, true);

			Controls[1].MouseDown += new MouseEventHandler(VBar_MouseDown);
			Controls[1].MouseUp += new MouseEventHandler(VBar_MouseLeave);
			Controls[1].MouseLeave += new EventHandler(VBar_MouseLeave);
		}

		#region - Refresh Selected Rows -
	
		public void RefreshSelectedRows()
		{
			if (SelectedRows != null)
			{
				//bool editing = false;

				foreach (DataGridViewRow row in SelectedRows)
				{
					if (row.Index != EditingRow && row.Index >= 0)
					{
						InvalidateRow(row.Index);
					}
				}
			}
		}

		#endregion

		#region - Scroll Bar -

		private bool _scrolling = false;
		public bool Scrolling
		{
			get
			{
				return _scrolling;
			}
		}

		private void VBar_MouseDown(object sender, MouseEventArgs e)
		{
			_scrolling = true;
		}

		private void VBar_MouseLeave(object sender, EventArgs e)
		{
			if (_scrolling)
			{
				_scrolling = false;
				Invalidate();
			}
		}

		#endregion

		#region - Override Events -

		protected override void OnPaint(PaintEventArgs e)
		{
			if (!Scrolling)
			{
				base.OnPaint(e);
			}
		}

		protected override void OnInvalidated(InvalidateEventArgs e)
		{
			if (!Scrolling)
			{
				base.OnInvalidated(e);
			}
		}

		protected override void OnColumnHeaderMouseClick(DataGridViewCellMouseEventArgs e)
		{
			base.OnColumnHeaderMouseClick(e);
			RefreshSharedRows = true;
		}

		protected override void OnSorted(EventArgs e)
		{
			base.OnSorted(e);
			UnshareRows();
		}

		protected override void OnPaintBackground(PaintEventArgs pevent)
		{
			//base.OnPaintBackground(pevent);
		}

		protected override void OnEditingControlShowing(DataGridViewEditingControlShowingEventArgs e)
		{
			base.OnEditingControlShowing(e);
			GetEditingCell();
		}

		protected override void OnCellEndEdit(DataGridViewCellEventArgs e)
		{
			base.OnCellEndEdit(e);
			GetEditingCell();
		}

		private void GetEditingCell()
		{
			if (EditingControl != null)
			{
				// Get info on where user clicked
				DataGridView.HitTestInfo hitInfo = HitTest(EditingControl.Parent.Location.X, EditingControl.Parent.Location.Y);
				_editingRow = hitInfo.RowIndex;
				_editingColumn = hitInfo.ColumnIndex;

				//Debug.Print("EditingControl " + _editingRow.ToString() + " " + _editingColumn.ToString());
			}
			else
			{
				_editingRow = -1;
				_editingColumn = -1;

				//Debug.Print("No EditingControl");
			}
		}

		#endregion

		#region - Copy & Paste -

		private string _clipboardString;
		private string[] _rows;
		private string[] _cells;

		public void CopyToClipboard(bool withHeader)
		{
			CopyToClipboard(withHeader, -1);
		}
		public void CopyToClipboard(bool withHeader, int specialIndex)
		{
			if (specialIndex > 0)
			{
				_specialIndex = specialIndex - 1;
			}
			else
			{
				_specialIndex = specialIndex;
			}

			if (Rows.Count > 0)
			{
				if (withHeader)
				{
					ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableWithAutoHeaderText;
				}
				else
				{
					ClipboardCopyMode = DataGridViewClipboardCopyMode.EnableWithoutHeaderText;
				}

				MultiSelect = true;
				if (AllowUserToAddRows)
				{
					// Skip the last blank row
					for (int i = 0; i < Rows.Count - 1; i++)
					{
						Rows[i].Selected = true;
					}
				}
				else
				{
					SelectAll();
				}

				if (GetCellCount(DataGridViewElementStates.Selected) > 0)
				{
					try
					{
						Clipboard.SetDataObject(GetClipboardContent());

						_clipboardString = Clipboard.GetText();

						RemakeClipBoardStringBySpecialIndex();
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

		public void PasteFromClipboard()
		{
			ExitRowEdit();

			_clipboardString = Clipboard.GetText();

			_rows = _clipboardString.Split(_rowSplits, StringSplitOptions.None);
			foreach (string row in _rows)
			{
				if (row != "")
				{
					_cells = row.Split(_colSplits, StringSplitOptions.None);
					// Unbound Grid Only
					if (DataSource == null)
					{
						try
						{
							Rows.Add(_cells);
						}
						catch (Exception ex)
						{
							Debug.Print(ex.Message, " ", ex.StackTrace);
						}
					}
				}
			}
		}

		public void InsertFromCSVFile(string[] csvItems)
		{
			if (csvItems.Length > 0)
			{
				ExitRowEdit();

				foreach (string csvItem in csvItems)
				{
					if (csvItem != "")
					{
						_cells = csvItem.Split(new string[] { "," }, StringSplitOptions.None);
						// Unbound Grid Only
						if (DataSource == null)
						{
							try
							{
								Rows.Add(_cells);
							}
							catch (Exception ex)
							{
								Debug.Print(ex.Message, " ", ex.StackTrace);
							}
						}
					}
				}
			}
		}

		private void ExitRowEdit()
		{
			if (SelectedRows.Contains(Rows[NewRowIndex]))
			{
				Rows[NewRowIndex].Selected = false;
				CancelEdit();
			}
		}

		#endregion

		#region - Save -

		private SaveFileDialog _saveDialog;
		[Browsable(false)]
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
			Save(-1);
		}
		public void Save(int specialIndex)
		{
			if (specialIndex > 0)
			{
				_specialIndex = specialIndex - 1;
			}
			else
			{
				_specialIndex = specialIndex;
			}

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
						
						_clipboardString = Clipboard.GetText();
						RemakeClipBoardStringBySpecialIndex();

						_csvString = _clipboardString.Replace(",", "");
						_csvString = _csvString.Replace(_colSplit, ",");
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

		#region - OrderID Speical Index -

		private int _specialIndex = -1;
		private List<string> _specialRows;

		private void RemakeClipBoardStringBySpecialIndex()
		{
			if (_specialIndex >= 0)
			{
				_rows = _clipboardString.Split(_rowSplits, StringSplitOptions.None);
				_specialRows = new List<string>();
				foreach (string txtRow in _rows)
				{
					if (txtRow != "")
					{
						SpeicalColumnUpdate(txtRow.Split(_colSplits, StringSplitOptions.None), _specialIndex);
					}
				}

				if (_specialRows.Count > 0)
				{
					_clipboardString = string.Join(_rowSplit, _specialRows.ToArray());
				}

				// reset
				_specialIndex = -1;
			}
		}
		private void SpeicalColumnUpdate(string[] txtCells, int cellIndex)
		{
			if (txtCells.Length > cellIndex)
			{
				if (txtCells[cellIndex].Contains(_indexSplit))
				{
					string[] speicals = txtCells[cellIndex].Split(_sepecialIndexSplits, StringSplitOptions.RemoveEmptyEntries);

					if (speicals.Length > 1)
					{
						txtCells[cellIndex] = speicals[1];
					}
				}
			}

			_specialRows.Add(string.Join(_colSplit, txtCells));
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

		public void ScrollByWheel(int delta)
		{
			if (Rows.Count > 0)
			{
				try
				{
					if (delta > 0)
					{
						if (FirstDisplayedScrollingRowIndex - delta >= 0)
						{
							FirstDisplayedScrollingRowIndex = FirstDisplayedScrollingRowIndex - delta;
						}
					}
					else if (delta < 0)
					{
						if (FirstDisplayedScrollingRowIndex - delta < Rows.Count)
						{
							FirstDisplayedScrollingRowIndex = FirstDisplayedScrollingRowIndex - delta;
						}
					}
				}
				catch (Exception ex)
				{
					Debug.Print(ex.Message, " ", ex.StackTrace);
				}
			}
		}

		public void ScrollToFirstRow()
		{
			if (Rows.Count > 0)
			{
				FirstDisplayedScrollingRowIndex = 0;
				Rows[0].Selected = true;
			}
		}

		#endregion

		#region - Filter -

		public string BuildRowFilterString(string filter, string input)
		{
			return BuildRowFilterString(filter, input, false);
		}
		public string BuildRowFilterString(string filter, string input, bool useOr)
		{
			if (filter == "")
			{
				filter = input;
			}
			else
			{
				if (useOr)
				{
					filter = String.Concat(new object[] { filter, " or ", input });
				}
				else
				{
					filter = String.Concat(new object[] { filter, " and ", input });
				}
			}

			return filter;
		}

		#endregion

		#region - Unshare Rows -

		public void UnshareRows()
		{
			if (RefreshSharedRows)
			{
				RefreshSharedRows = false;
				lock (this)
				{
					this.SuspendLayout();
					foreach (DataGridViewRow row in this.Rows)
					{
						row.Selected = true;
						row.Selected = false;
					}
					this.ResumeLayout(true);
				}
			}
		}

		#endregion

		#region - Symbol -

		public void UpdateSymbol(string symbol)
		{
			if (!Symbols.Contains(symbol))
			{
				Symbols.Add(symbol);
			}
		}

		#endregion

		#region - Tick Sizes -

		public void UpdateTickSize(string symbol, double tickSize)
		{
			if (TickSizes.ContainsKey(symbol))
			{
				TickSizes[symbol] = tickSize;
			}
			else
			{
				TickSizes.Add(symbol, tickSize);
			}
		}

		#endregion

		#region - Display Factors -

		public void UpdateDisplayFactor(string symbol, double displayFactor)
		{
			if (DisplayFactors.ContainsKey(symbol))
			{
				DisplayFactors[symbol] = displayFactor;
			}
			else
			{
				DisplayFactors.Add(symbol, displayFactor);
			}
		}

		#endregion

		#region - Binding -

		public void SuspendBinding()
		{
			BindingDS.SuspendBinding();
		}

		public void ResumeBinding()
		{
			BindingDS.ResumeBinding();
		}

		public void ResetBinding()
		{
			BindingDS.ResetBindings(true);
		}

		#endregion

		#region - Single Click Functions -

		private struct RECT
		{
			public System.Int32 Left;
			public System.Int32 Top;
			public System.Int32 Right;
			public System.Int32 Bottom;
		}

		// API Constants
		const int WM_LBUTTONDOWN = 0x0201;
		const int WM_LBUTTONUP = 0x0202;
		[System.Runtime.InteropServices.DllImport("user32.dll")]
		private static extern bool SendMessage(IntPtr hWnd, Int32 msg, Int32 wParam, Int32 lParam);
		[System.Runtime.InteropServices.DllImport("user32.dll")]
		private static extern IntPtr ChildWindowFromPoint(IntPtr hwndParent, int x, int y);
		[System.Runtime.InteropServices.DllImport("user32.dll")]
		private static extern bool GetWindowRect(System.IntPtr hWnd, out RECT lpRect);

		// Properties
		private List<DataGridViewColumn> _singleClickColumns;
		internal List<DataGridViewColumn> SingleClickColumns
		{
			get
			{
				if (_singleClickColumns == null)
				{
					_singleClickColumns = new List<DataGridViewColumn>();
				}
				return _singleClickColumns;
			}
			set
			{
				_singleClickColumns = value;
			}
		}

		// Setup
		internal void DefineSingleClickColumns(params DataGridViewColumn[] columns)
		{
			SingleClickColumns.Clear();
			foreach (DataGridViewColumn column in columns)
			{
				if (this.Columns.IndexOf(column) >= 0)
				{
					SingleClickColumns.Add(column);
				}
			}
		}
		internal void DefineSingleClickColumns(params int[] columnIndexes)
		{
			SingleClickColumns.Clear();
			foreach (int columnIndex in columnIndexes)
			{
				if (columnIndex >= 0 || columnIndex < this.Columns.Count)
				{
					SingleClickColumns.Add(this.Columns[columnIndex]);
				}
			}
		}

		// On Single Click Event
		internal void OnSingleClick(MouseEventArgs e)
		{
			if (SingleClickColumns.Count == 0)
			{
				base.OnMouseDown(e);
				return;
			}

			// If primary mouse button not down, do standard processing
			if (e.Button != MouseButtons.Left)
			{
				base.OnMouseDown(e);
				return;
			}

			// Get info on where user clicked
			DataGridView.HitTestInfo hitInfo = HitTest(e.X, e.Y);

			// If a cell wasn't clicked or column is read only, do standard processing
			if (hitInfo.Type != DataGridViewHitTestType.Cell || this.Columns[hitInfo.ColumnIndex].ReadOnly)
			{
				base.OnMouseDown(e);
				return;
			}

			// Get column clicked
			DataGridViewColumn column = this.Columns[hitInfo.ColumnIndex];
			// If column type already works with a single click, do standard processing
			if (column is DataGridViewCheckBoxColumn || 
				column is DataGridViewButtonColumn ||
				column is DataGridViewLinkColumn ||
				column is DataGridViewImageColumn)
			{
				base.OnMouseDown(e);
				return;
			}

			// If functionality enabled for specific columns and column clicked is not
			// one of these, do standard processing
			if (SingleClickColumns.Count >= 1 && SingleClickColumns.IndexOf(column) == -1)
			{
				base.OnMouseDown(e);
				return;
			}

			// Get clicked cell
			DataGridViewCell clickedCell = this[hitInfo.ColumnIndex, hitInfo.RowIndex];

			// If cell not current, try and make it so
			if (this.CurrentCell != clickedCell)
			{
				// Allow standard processing make clicked cell current
				base.OnMouseDown(e);

				// If this didn't happen (validation failed etc), abort
				if (this.CurrentCell != clickedCell)
				{
					return;
				}
			}

			// If already in edit mode, do standard processing (will position caret)
			if (this.CurrentCell.IsInEditMode)
			{
				base.OnMouseDown(e);
				return;
			}

			// Enter edit mode
			this.BeginEdit(false);

			// For editing controls derived from TextBoxBase, ensure text is scrolled to the left
			if (this.EditingControl is TextBoxBase)
			{
				TextBoxBase textBox = (TextBoxBase)this.EditingControl;
				textBox.SelectionStart = 0;
				textBox.ScrollToCaret();
			}

			// Calculate co-ordinates of where user clicked within editing control
			System.Drawing.Point clickPoint = new System.Drawing.Point(
				e.X - hitInfo.ColumnX - this.EditingControl.Left,
				e.Y - hitInfo.RowY - this.EditingControl.Top);

			// Simulate mouse click within control
			SimulateMouseClick(clickPoint);
		}

		// Implementation
		private void SimulateMouseClick(System.Drawing.Point clickPoint)
		{
			// Check for child control within editing control
			IntPtr childHwnd = ChildWindowFromPoint(this.EditingControl.Handle, clickPoint.X, clickPoint.Y);

			// If no child window, simulate mouse click within editing control
			if (childHwnd == this.EditingControl.Handle)
			{
				SimulateMouseClick(this.EditingControl.Handle, clickPoint);
			}
			// Otherwise, simulate mouse click within child control
			else
			{
				// Get screen co-ordinates of parent (edit control) and child
			    RECT parentRect, childRect;
				if (!GetWindowRect(this.EditingControl.Handle, out parentRect))
				{
					return;
				}

				if (!GetWindowRect(childHwnd, out childRect))
				{
					return;
				}

				// Determine co-ordinates of where user clicked within child control
				System.Drawing.Point childClickPoint = new System.Drawing.Point(
					clickPoint.X - (childRect.Left - parentRect.Left),
					clickPoint.Y - (childRect.Top - parentRect.Top));

				// Simulate mouse click within child control
				SimulateMouseClick(childHwnd, childClickPoint);
			}

		}
		private void SimulateMouseClick(IntPtr hWnd, System.Drawing.Point clickPoint)
		{
			Int32 lParam = MakeLong(clickPoint.X, clickPoint.Y);
			SendMessage(hWnd, WM_LBUTTONDOWN, 0, lParam);
			SendMessage(hWnd, WM_LBUTTONUP, 0, lParam);
		}
		private int MakeLong(int loWord, int hiWord)
		{
			return (hiWord << 16) | (loWord & 0xffff);
		}

		#endregion

		#region - Distructor -

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				DataSource = null;
				Rows.Clear();
				Columns.Clear();
				base.Dispose(disposing);
			}
		}

		#endregion
	}
}
