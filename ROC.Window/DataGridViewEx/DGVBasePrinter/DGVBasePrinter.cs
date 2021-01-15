using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Printing;
using System.Globalization;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBasePrinter
	{
		class PageDef
		{
			public PageDef(Margins m, int count)
			{
				colstoprint = new List<object>(count);
				colwidths = new List<float>(count);
				colwidthsoverride = new List<float>(count);
				coltotalwidth = 0;
				margins = (Margins)m.Clone();
			}

			public System.Collections.IList colstoprint;
			public List<float> colwidths;
			public List<float> colwidthsoverride;
			public float coltotalwidth;
			public Margins margins;
		}
		IList<PageDef> pagesets;
		int currentpageset = 0;

		public class PrintDialogSettingsClass
		{
			public bool AllowSelection = true;
			public bool AllowSomePages = true;
			public bool AllowCurrentPage = true;
			public bool AllowPrintToFile = false;
			public bool ShowHelp = true;
			public bool ShowNetwork = true;
		}

		#region - Global Variables -

		// the data grid view we're printing
		DataGridView _dgv = null;

		// print document
		PrintDocument printDoc = null;

		// print status items
		private List<DataGridViewRow> _rowsToPrint;
		private List<DataGridViewColumn> _colsToPrint;          // divided into pagesets for printing
		private int _lastRowPrinted = -1;
		private int _fromPage = 0;
		private int _toPage = -1;

		// page formatting options
		private int pageHeight = 0;
		private int pageWidth = 0;
		private int printWidth = 0;
		private float rowheaderwidth = 0;
		private int CurrentPage = 0;
		private PrintRange printRange;

		// calculated values
		private float headerHeight = 0;
		private float footerHeight = 0;
		private float pagenumberHeight = 0;
		private float colheaderheight = 0;
		private List<float> rowheights;
		private List<float> colwidths;
		#endregion

		#region - Properties -

		#region - Global Properties -

		private Form _owner;
		public Form Owner
		{
			get
			{
				return _owner;
			}
			set
			{
				_owner = value;
			}
		}

		private double _printPreviewZoom = 1;
		public Double PrintPreviewZoom
		{ 
			get
			{
				return _printPreviewZoom;
			}
			set
			{
				_printPreviewZoom = value;
			}
		}

		public PrinterSettings PrintSettings
		{
			get 
			{
				return printDoc.PrinterSettings; 
			}
		}

		private PrintDialogSettingsClass printDialogSettings = new PrintDialogSettingsClass();
		public PrintDialogSettingsClass PrintDialogSettings
		{
			get { return printDialogSettings; }
		}

		private String printerName;
		public String PrinterName
		{
			get { return printerName; }
			set { printerName = value; }
		}

		public PrintDocument printDocument
		{
			get { return printDoc; }
		}

		private Icon ppvIcon = null;
		public Icon PreviewDialogIcon
		{
			get { return ppvIcon; }
			set { ppvIcon = value; }
		}

		private bool _blackWhite = true;
		public bool BlackWhite
		{
			get
			{
				return _blackWhite;
			}
			set
			{
				_blackWhite = value;
			}
		}

		#endregion

		#region - Title Properties -

		// override flag
		bool overridetitleformat = false;

		private String title;
		public String Title
		{
			get { return title; }
			set
			{
				title = value;
				if (docName == null)
				{
					printDoc.DocumentName = value;
				}
			}
		}

		private String docName;
		public String DocName
		{
			get { return docName; }
			set { printDoc.DocumentName = value; docName = value; }
		}

		private Font titlefont;
		public Font TitleFont
		{
			get { return titlefont; }
			set { titlefont = value; }
		}

		private Color titlecolor;
		public Color TitleColor
		{
			get { return titlecolor; }
			set { titlecolor = value; }
		}

		private StringFormat titleformat;
		public StringFormat TitleFormat
		{
			get { return titleformat; }
			set { titleformat = value; overridetitleformat = true; }
		}

		public StringAlignment TitleAlignment
		{
			get { return titleformat.Alignment; }
			set
			{
				titleformat.Alignment = value;
				overridetitleformat = true;
			}
		}

		public StringFormatFlags TitleFormatFlags
		{
			get { return titleformat.FormatFlags; }
			set
			{
				titleformat.FormatFlags = value;
				overridetitleformat = true;
			}
		}

		#endregion

		#region - Subtitle Properties -

		// override flat
		bool overridesubtitleformat = false;

		private String subtitle;
		public String SubTitle
		{
			get { return subtitle; }
			set { subtitle = value; }
		}

		private Font subtitlefont;
		public Font SubTitleFont
		{
			get { return subtitlefont; }
			set { subtitlefont = value; }
		}

		private Color subtitlecolor;
		public Color SubTitleColor
		{
			get { return subtitlecolor; }
			set { subtitlecolor = value; }
		}

		private StringFormat subtitleformat;
		public StringFormat SubTitleFormat
		{
			get { return subtitleformat; }
			set { subtitleformat = value; overridesubtitleformat = true; }
		}

		public StringAlignment SubTitleAlignment
		{
			get { return subtitleformat.Alignment; }
			set
			{
				subtitleformat.Alignment = value;
				overridesubtitleformat = true;
			}
		}

		public StringFormatFlags SubTitleFormatFlags
		{
			get { return subtitleformat.FormatFlags; }
			set
			{
				subtitleformat.FormatFlags = value;
				overridesubtitleformat = true;
			}
		}

		#endregion

		#region - Footer Properties -

		// override flag
		bool overridefooterformat = false;

		private String footer;
		public String Footer
		{
			get { return footer; }
			set { footer = value; }
		}

		private Font footerfont;
		public Font FooterFont
		{
			get { return footerfont; }
			set { footerfont = value; }
		}

		private Color footercolor;
		public Color FooterColor
		{
			get { return footercolor; }
			set { footercolor = value; }
		}

		private StringFormat footerformat;
		public StringFormat FooterFormat
		{
			get { return footerformat; }
			set { footerformat = value; overridefooterformat = true; }
		}

		public StringAlignment FooterAlignment
		{
			get { return footerformat.Alignment; }
			set
			{
				footerformat.Alignment = value;
				overridefooterformat = true;
			}
		}

		public StringFormatFlags FooterFormatFlags
		{
			get { return footerformat.FormatFlags; }
			set
			{
				footerformat.FormatFlags = value;
				overridefooterformat = true;
			}
		}

		private float footerspacing;
		public float FooterSpacing
		{
			get { return footerspacing; }
			set { footerspacing = value; }
		}

		#endregion

		#region - Page Number Properties -

		// override flag
		bool overridepagenumberformat = false;

		private bool pageno = true;
		public bool PageNumbers
		{
			get { return pageno; }
			set { pageno = value; }
		}

		private Font pagenofont;
		public Font PageNumberFont
		{
			get { return pagenofont; }
			set { pagenofont = value; }
		}

		private Color pagenocolor;
		public Color PageNumberColor
		{
			get { return pagenocolor; }
			set { pagenocolor = value; }
		}

		private StringFormat pagenumberformat;
		public StringFormat PageNumberFormat
		{
			get { return pagenumberformat; }
			set { pagenumberformat = value; overridepagenumberformat = true; }
		}

		public StringAlignment PageNumberAlignment
		{
			get { return pagenumberformat.Alignment; }
			set
			{
				pagenumberformat.Alignment = value;
				overridepagenumberformat = true;
			}
		}

		public StringFormatFlags PageNumberFormatFlags
		{
			get { return pagenumberformat.FormatFlags; }
			set
			{
				pagenumberformat.FormatFlags = value;
				overridepagenumberformat = true;
			}
		}

		private bool pagenumberontop = false;
		public bool PageNumberInHeader
		{
			get { return pagenumberontop; }
			set { pagenumberontop = value; }
		}

		private bool pagenumberonseparateline = false;
		public bool PageNumberOnSeparateLine
		{
			get { return pagenumberonseparateline; }
			set { pagenumberonseparateline = value; }
		}

		private int totalpages;
		private bool showtotalpagenumber = false;
		public bool ShowTotalPageNumber
		{
			get { return showtotalpagenumber; }
			set { showtotalpagenumber = value; }
		}

		private String pageseparator = " of ";
		public String PageSeparator
		{
			get { return pageseparator; }
			set { pageseparator = value; }
		}

		private String pagetext = "Page ";
		public String PageText
		{
			get { return pagetext; }
			set { pagetext = value; }
		}

		private String parttext = " - Part ";
		public String PartText
		{
			get { return parttext; }
			set { parttext = value; }
		}

		#endregion

		#region - Header Cell Printing Properties -

		private StringFormat headercellformat = null;
		public StringFormat GetHeaderCellFormat(DataGridView grid)
		{
			// get default values from provided data grid view, but only
			// if we don't already have a header cell format
			if ((null != grid) && (null == headercellformat))
			{
				buildstringformat(
					ref headercellformat, 
					grid.Columns[0].HeaderCell.InheritedStyle,
					headercellalignment, 
					StringAlignment.Near, 
					headercellformatflags,
					StringTrimming.Word);
			}

			// if we still don't have a header cell format, create an empty
			if (null == headercellformat)
				headercellformat = new StringFormat(headercellformatflags);

			return headercellformat;
		}

		private StringAlignment headercellalignment;
		public StringAlignment HeaderCellAlignment
		{
			get { return headercellalignment; }
			set { headercellalignment = value; }
		}

		private StringFormatFlags headercellformatflags;
		public StringFormatFlags HeaderCellFormatFlags
		{
			get { return headercellformatflags; }
			set { headercellformatflags = value; }
		}

		#endregion

		#region - Cell Properties -

		private StringFormat _cellformat = null;
		public StringFormat GetCellFormat(DataGridView grid)
		{
			// get default values from provided data grid view, but only
			// if we don't already have a cell format
			if ((null != grid) && (null == _cellformat))
			{
				buildstringformat(ref _cellformat, grid.Rows[0].Cells[0].InheritedStyle,
					_cellalignment, StringAlignment.Near, _cellformatflags,
					StringTrimming.Word);
			}

			// if we still don't have a cell format, create an empty
			if (null == _cellformat)
				_cellformat = new StringFormat(_cellformatflags);

			return _cellformat;
		}

		private StringAlignment _cellalignment;
		public StringAlignment CellAlignment
		{
			get { return _cellalignment; }
			set { _cellalignment = value; }
		}

		private StringFormatFlags _cellformatflags;
		public StringFormatFlags CellFormatFlags
		{
			get { return _cellformatflags; }
			set { _cellformatflags = value; }
		}

		private List<float> _colwidthsoverride = new List<float>();
		private Dictionary<string, float> _publicwidthoverrides = new Dictionary<string, float>();
		public Dictionary<string, float> ColumnWidths
		{
			get { return _publicwidthoverrides; }
		}

		private Dictionary<string, DataGridViewCellStyle> _colstyles = new Dictionary<string, DataGridViewCellStyle>();
		public Dictionary<string, DataGridViewCellStyle> ColumnStyles
		{
			get { return _colstyles; }
		}

		#endregion

		#region - Page Level Properties -

		private Margins _printmargins;
		public Margins PrintMargins
		{
			get { return _printmargins; }
			set { _printmargins = value; }
		}

		public PageSettings PageSettings
		{
			get { return printDoc.DefaultPageSettings; }
		}

		private bool _porportionalcolumns = false;
		public bool PorportionalColumns
		{
			get { return _porportionalcolumns; }
			set { _porportionalcolumns = value; }
		}

		public enum Alignment { NotSet, Left, Right, Center }
		private Alignment _tablealignment = Alignment.NotSet;
		public Alignment TableAlignment
		{
			get { return _tablealignment; }
			set { _tablealignment = value; }
		}

		#endregion

		#region - Utility Functions -

		private int PreviewDisplayWidth()
		{
			double displayWidth = printDoc.DefaultPageSettings.Bounds.Width
				+ 3 * printDoc.DefaultPageSettings.HardMarginY;
			return (int)(displayWidth * PrintPreviewZoom);
		}

		private int PreviewDisplayHeight()
		{
			double displayHeight = printDoc.DefaultPageSettings.Bounds.Height
				+ 3 * printDoc.DefaultPageSettings.HardMarginX;

			return (int)(displayHeight * PrintPreviewZoom);
		}

		#endregion

		#endregion

		public DGVBasePrinter()
		{
			// create print document
			printDoc = new PrintDocument();
			printDoc.PrintPage += new PrintPageEventHandler(printDoc_PrintPage);
			printDoc.BeginPrint += new PrintEventHandler(printDoc_BeginPrint);
			_printmargins = new Margins(60, 60, 40, 40);			

			// set default fonts
			pagenofont = new Font("Tahoma", 8, FontStyle.Regular, GraphicsUnit.Point);
			pagenocolor = Color.Black;
			titlefont = new Font("Tahoma", 18, FontStyle.Bold, GraphicsUnit.Point);
			titlecolor = Color.Black;
			subtitlefont = new Font("Tahoma", 12, FontStyle.Bold, GraphicsUnit.Point);
			subtitlecolor = Color.Black;
			footerfont = new Font("Tahoma", 10, FontStyle.Bold, GraphicsUnit.Point);
			footercolor = Color.Black;

			// Create string formatting objects
			buildstringformat(ref titleformat, null, StringAlignment.Center, StringAlignment.Center,
				StringFormatFlags.NoWrap | StringFormatFlags.LineLimit | StringFormatFlags.NoClip, StringTrimming.Word);
			buildstringformat(ref subtitleformat, null, StringAlignment.Center, StringAlignment.Center,
				StringFormatFlags.NoWrap | StringFormatFlags.LineLimit | StringFormatFlags.NoClip, StringTrimming.Word);
			buildstringformat(ref footerformat, null, StringAlignment.Center, StringAlignment.Center,
				StringFormatFlags.NoWrap | StringFormatFlags.LineLimit | StringFormatFlags.NoClip, StringTrimming.Word);
			buildstringformat(ref pagenumberformat, null, StringAlignment.Far, StringAlignment.Center,
				StringFormatFlags.NoWrap | StringFormatFlags.LineLimit | StringFormatFlags.NoClip, StringTrimming.Word);

			// Set these formatting objects to null to flag whether or not they were set by the caller
			headercellformat = null;
			_cellformat = null;

			// Print Preview properties
			Owner = null;
			PrintPreviewZoom = 1;

			// Deprecated properties - retain for backwards compatibility
			headercellalignment = StringAlignment.Near;
			headercellformatflags = StringFormatFlags.LineLimit | StringFormatFlags.NoClip;
			_cellalignment = StringAlignment.Near;
			_cellformatflags = StringFormatFlags.LineLimit | StringFormatFlags.NoClip;
		}

		public void PrintDataGridView(DataGridView dgv)
		{
			if (dgv.Rows.Count > 0)
			{
				// save the datagridview we're printing
				_dgv = dgv;

				// display dialog and print
				if (DialogResult.OK == DisplayPrintDialog())
				{
					SetupPrint();
					printDoc.Print();
				}
			}
			else
			{
				MessageBox.Show("Nothing To Print");
			}
		}

		public void PrintPreviewDataGridView(DataGridView dgv)
		{
			PrintPreviewDataGridView(dgv, false);
		}
		public void PrintPreviewDataGridView(DataGridView dgv, bool landscape)
		{
			// save the datagridview we're printing
			if (dgv.Rows.Count > 0)
			{
				_dgv = dgv;

				printDoc.DefaultPageSettings.Landscape = landscape;

				// display dialog and print
				if (DialogResult.OK == DisplayPrintDialog())
				{
					PrintPreviewNoDisplay(dgv);
				}
			}
			else
			{
				MessageBox.Show("Nothing To Print");
			}
		}

		public DialogResult DisplayPrintDialog()
		{
			// create new print dialog and set options
			PrintDialog pd = new PrintDialog();
			pd.AllowSelection = printDialogSettings.AllowSelection;
			pd.AllowSomePages = printDialogSettings.AllowSomePages;
			pd.AllowCurrentPage = printDialogSettings.AllowCurrentPage;
			pd.AllowPrintToFile = printDialogSettings.AllowPrintToFile;
			pd.ShowHelp = printDialogSettings.ShowHelp;
			pd.ShowNetwork = printDialogSettings.ShowNetwork;

			// setup print dialog with internal setttings
			pd.Document = printDoc;
			printDoc.DefaultPageSettings.Margins = _printmargins;
			if (!String.IsNullOrEmpty(printerName))
				printDoc.PrinterSettings.PrinterName = printerName;

			// show the dialog and display the result
			return pd.ShowDialog();
		}

		public void PrintNoDisplay(DataGridView dgv)
		{
			// save the grid we're printing
			this._dgv = dgv;

			// setup and do printing
			SetupPrint();
			printDoc.Print();
		}

		public void PrintPreviewNoDisplay(DataGridView dgv)
		{
			// save the grid we're printing
			this._dgv = dgv;

			// display the preview dialog
			SetupPrint();
			PrintPreviewDialog ppdialog = new PrintPreviewDialog();
			ppdialog.Document = printDoc;
			ppdialog.UseAntiAlias = true;
			ppdialog.Owner = Owner;
			ppdialog.PrintPreviewControl.Zoom = PrintPreviewZoom;
			ppdialog.Width = PreviewDisplayWidth();
			ppdialog.Height = PreviewDisplayHeight();

			if (null != ppvIcon)
				ppdialog.Icon = ppvIcon;
			ppdialog.ShowDialog();
		}

		private void SetupPrint()
		{
			//-----------------------------------------------------------------
			// Set headercell and normal cell print formats if they were not
			// explicitly set by the caller
			//-----------------------------------------------------------------
			if (null == headercellformat)
			{
				buildstringformat(
					ref headercellformat, 
					_dgv.Columns[0].HeaderCell.InheritedStyle,
					headercellalignment, 
					StringAlignment.Near, 
					headercellformatflags,
					StringTrimming.Word);
			}
			if (null == _cellformat)
			{
				buildstringformat(ref _cellformat, 
					_dgv.DefaultCellStyle,
					_cellalignment, 
					StringAlignment.Near, 
					_cellformatflags,
					StringTrimming.Word);
			}

			//-----------------------------------------------------------------
			// get info on the limits of the printer's actual print area available. Convert
			// to int's to work with margins.
			//-----------------------------------------------------------------
			int hardx = (int)Math.Round(printDoc.DefaultPageSettings.HardMarginX);
			int hardy = (int)Math.Round(printDoc.DefaultPageSettings.HardMarginY);
			int printareawidth;
			if (printDoc.DefaultPageSettings.Landscape)
				printareawidth = (int)Math.Round(printDoc.DefaultPageSettings.PrintableArea.Height);
			else
				printareawidth = (int)Math.Round(printDoc.DefaultPageSettings.PrintableArea.Width);

			//-----------------------------------------------------------------
			// set the print area we're working within
			//-----------------------------------------------------------------

			pageHeight = printDoc.DefaultPageSettings.Bounds.Height;
			pageWidth = printDoc.DefaultPageSettings.Bounds.Width;

			//-----------------------------------------------------------------
			// Set the printable area: margins and pagewidth
			//-----------------------------------------------------------------

			// Set initial printer margins 
			_printmargins = printDoc.DefaultPageSettings.Margins;

			// adjust for when the margins are less than the printer's hard x/y limits
			_printmargins.Right = (hardx > _printmargins.Right) ? hardx : _printmargins.Right;
			_printmargins.Left = (hardx > _printmargins.Left) ? hardx : _printmargins.Left;
			_printmargins.Top = (hardy > _printmargins.Top) ? hardy : _printmargins.Top;
			_printmargins.Bottom = (hardy > _printmargins.Bottom) ? hardy : _printmargins.Bottom;

			// Now, we can calc default print width, again, respecting the printer's limitations
			printWidth = pageWidth - _printmargins.Left - _printmargins.Right;
			printWidth = (printWidth > printareawidth) ? printareawidth : printWidth;

			//-----------------------------------------------------------------
			// Figure out which pages / rows to print
			//-----------------------------------------------------------------

			// save print range 
			printRange = printDoc.PrinterSettings.PrintRange;

			// pages to print handles "some pages" option
			if (PrintRange.SomePages == printRange)
			{
				// set limits to only print some pages
				_fromPage = printDoc.PrinterSettings.FromPage;
				_toPage = printDoc.PrinterSettings.ToPage;
			}
			else
			{
				// set extremes so that we'll print all pages
				_fromPage = 0;
				_toPage = 2147483647;
			}

			//-----------------------------------------------------------------
			// set up the rows and columns to print
			//
			// Note: The "Selectedxxxx" lists in the datagridview are 'stacks' that
			//  have the selected items pushed in the *in the order they were selected*
			//  i.e. not the order you want to print them in!
			//-----------------------------------------------------------------

			// rows to print (handles "selection" and "current page" options
			if (PrintRange.Selection == printRange)
			{
				//if DGV has rows selected, it's easy, selected rows and all visible columns
				if (0 != _dgv.SelectedRows.Count)
				{
					// sort the rows into index order
					_rowsToPrint = new List<DataGridViewRow>(_dgv.SelectedRows.Count);
					foreach (DataGridViewRow row in _dgv.SelectedRows)
						_rowsToPrint.Add(row);
					_rowsToPrint.Sort((a, b) => a.Index.CompareTo(b.Index));

					_colsToPrint = new List<DataGridViewColumn>(_dgv.Columns.Count);
					foreach (DataGridViewColumn col in _dgv.Columns) {
						if (col.Visible)
							_colsToPrint.Add(col);
					}
				}
				// if selected columns, then all rows, and selected columns
				else if (0 != _dgv.SelectedColumns.Count)
				{
					_rowsToPrint = new List<DataGridViewRow>(_dgv.Rows.Count);
					foreach (DataGridViewRow row in _dgv.Rows)
						_rowsToPrint.Add(row);

					_colsToPrint = new List<DataGridViewColumn>(_dgv.SelectedColumns.Count);
					foreach (DataGridViewColumn col in _dgv.SelectedColumns)
						_colsToPrint.Add(col);
					_colsToPrint.Sort((a, b) => a.Index.CompareTo(b.Index));
				}
				// we just have a bunch of selected cells so we have to do some work
				else
				{
					// set up sorted lists. the selectedcells method does not guarantee
					// that the cells will always be in left-right top-bottom order. 
					_rowsToPrint = new List<DataGridViewRow>();
					_colsToPrint = new List<DataGridViewColumn>();

					// for each selected cell, add unique rows and columns
					int colindex, rowindex;
					foreach (DataGridViewCell cell in _dgv.SelectedCells)
					{
						colindex = cell.ColumnIndex;
						rowindex = cell.RowIndex;

						// add unique rows
						if (!_rowsToPrint.Exists(n => n.Index == rowindex))
							_rowsToPrint.Add(_dgv.Rows[rowindex]);

						// add unique columns
						if (!_colsToPrint.Exists(n => n.Index == colindex))
							_colsToPrint.Add(_dgv.Columns[colindex]);
					}

					_rowsToPrint.Sort((a, b) => a.Index.CompareTo(b.Index));
					_colsToPrint.Sort((a, b) => a.Index.CompareTo(b.Index));
				}
			}
			// if current page was selected, print visible columns for the
			// displayed rows                
			else if (PrintRange.CurrentPage == printRange)
			{
				// create lists
				_rowsToPrint = new List<DataGridViewRow>(_dgv.DisplayedRowCount(true));
				_colsToPrint = new List<DataGridViewColumn>(_dgv.Columns.Count);

				// select all visible rows on displayed page
				for (int i = _dgv.FirstDisplayedScrollingRowIndex;
					i < _dgv.FirstDisplayedScrollingRowIndex + _dgv.DisplayedRowCount(true);
					++i)
				{
					DataGridViewRow row = _dgv.Rows[i];
					if (row.Visible) 
						_rowsToPrint.Add(row);
				}

				// select all visible columns
				foreach (DataGridViewColumn col in _dgv.Columns) {
					if (col.Visible) 
						_colsToPrint.Add(col);
				}
			}
			// this is the default for print all - everything marked visible will be printed
			else
			{
				// select all visible rows and all visible columns
				_rowsToPrint = new List<DataGridViewRow>(_dgv.Rows.Count);
				foreach (DataGridViewRow row in _dgv.Rows) if (row.Visible) _rowsToPrint.Add(row);

				_colsToPrint = new List<DataGridViewColumn>(_dgv.Columns.Count);
				foreach (DataGridViewColumn col in _dgv.Columns) if (col.Visible) _colsToPrint.Add(col);
			}

			// Reorder columns based on Display Index (if the programmer or user has
			// changed the column display order we want to respect it in the printout)
			_colsToPrint.Sort((a, b) => a.DisplayIndex.CompareTo(b.DisplayIndex));

			// Adjust override list to have the same number of entries as colstoprint
			foreach (DataGridViewColumn col in _colsToPrint)
				if (_publicwidthoverrides.TryGetValue(col.Name, out float value))
					_colwidthsoverride.Add(value);
				else
					_colwidthsoverride.Add(-1);

			//-----------------------------------------------------------------
			// Now that we know what we're printing, measure the print area and
			// count the pages.
			//-----------------------------------------------------------------

			// Measure the print area
			measureprintarea(printDoc.PrinterSettings.CreateMeasurementGraphics());

			// Count the pages
			totalpages = TotalPages();

		}

		private void buildstringformat(
			ref StringFormat format, 
			DataGridViewCellStyle controlstyle,
			StringAlignment alignment, 
			StringAlignment linealignment, 
			StringFormatFlags flags,
			StringTrimming trim)
		{
			// allocate format if it doesn't already exist
			if (null == format)
				format = new StringFormat();

			// Set defaults
			format.Alignment = alignment;
			format.LineAlignment = linealignment;
			format.FormatFlags = flags;
			format.Trimming = trim;

			// use cell alignment to override defaulted alignments
			if (null != controlstyle)
			{
				// Adjust the format based on the control settings, bias towards centered
				DataGridViewContentAlignment cellalign = controlstyle.Alignment;
				if (cellalign.ToString().Contains("Center")) format.Alignment = StringAlignment.Center;
				else if (cellalign.ToString().Contains("Left")) format.Alignment = StringAlignment.Near;
				else if (cellalign.ToString().Contains("Right")) format.Alignment = StringAlignment.Far;

				if (cellalign.ToString().Contains("Top")) format.LineAlignment = StringAlignment.Near;
				else if (cellalign.ToString().Contains("Middle")) format.LineAlignment = StringAlignment.Center;
				else if (cellalign.ToString().Contains("Bottom")) format.LineAlignment = StringAlignment.Far;
			}
		}

		private void measureprintarea(Graphics g)
		{
			int i, j;
			rowheights = new List<float>(_rowsToPrint.Count);
			colwidths = new List<float>(_colsToPrint.Count);
			headerHeight = 0;
			footerHeight = 0;

			// temp variables
			DataGridViewColumn col;
			DataGridViewRow row;

			//-----------------------------------------------------------------
			// measure the page headers and footers, including the grid column header cells
			//-----------------------------------------------------------------

			// measure the column headers
			Font headerfont = _dgv.ColumnHeadersDefaultCellStyle.Font;
			if (null == headerfont)
				headerfont = _dgv.DefaultCellStyle.Font;

			// set initial column sizes based on column titles
			for (i = 0; i < _colsToPrint.Count; i++)
			{
				col = (DataGridViewColumn)_colsToPrint[i];

				// deal with overridden col widths
				float usewidth = 0;
				if (0 < _colwidthsoverride[i])
					usewidth = _colwidthsoverride[i];
				else
					usewidth = printWidth;

				// measure the title for each column, keep widths and biggest height
				SizeF size = g.MeasureString(col.HeaderText, headerfont, new SizeF(usewidth, 2147483647),
					headercellformat);
				colwidths.Add(size.Width);
				colheaderheight = (colheaderheight < size.Height ? size.Height : colheaderheight);
			}

			//-----------------------------------------------------------------
			// measure the page number
			//-----------------------------------------------------------------

			if (pageno)
			{
				pagenumberHeight = (g.MeasureString("Page", pagenofont, printWidth, pagenumberformat)).Height;
			}


			//-----------------------------------------------------------------
			// Calc height of header.
			// Header height is height of page number, title, subtitle and height of column headers
			//-----------------------------------------------------------------

			// note that we dont count the page number height if it's not on a separate line
			if (pagenumberontop && !pagenumberonseparateline)
			{
				headerHeight += pagenumberHeight;
			}

			if (!String.IsNullOrEmpty(title))
			{
				headerHeight += (g.MeasureString(title, titlefont, printWidth, titleformat)).Height;
			}

			if (!String.IsNullOrEmpty(subtitle))
			{
				headerHeight += (g.MeasureString(subtitle, subtitlefont, printWidth, subtitleformat)).Height;
			}

			headerHeight += colheaderheight;

			//-----------------------------------------------------------------
			// measure the footer, if one is provided. Include the page number if we're printing
			// it on the bottom
			//-----------------------------------------------------------------

			if (!String.IsNullOrEmpty(footer))
			{
				footerHeight += (g.MeasureString(footer, footerfont, printWidth, footerformat)).Height;
			}

			// note we don't count the page number height if it's not on a separate line
			if (!pagenumberontop && pagenumberonseparateline)
			{
				footerHeight += pagenumberHeight;
			}

			footerHeight += footerspacing;

			//-----------------------------------------------------------------
			// measure the grid to be printed ... this gets us all the row heights
			// and an accurate measure of column widths for the printed area
			//-----------------------------------------------------------------

			for (i = 0; i < _rowsToPrint.Count; i++)
			{
				row = (DataGridViewRow)_rowsToPrint[i];
				rowheights.Add(0);

				// add row headers if they're visible
				if (_dgv.RowHeadersVisible)
				{
					SizeF rhsize = g.MeasureString(row.HeaderCell.EditedFormattedValue.ToString(),
						headerfont);
					rowheaderwidth = (rowheaderwidth < rhsize.Width) ? rhsize.Width : rowheaderwidth;
				}

				// calculate widths for each column. We're looking for the largest width needed for
				// all the rows of data.
				for (j = 0; j < _colsToPrint.Count; j++)
				{
					col = (DataGridViewColumn)_colsToPrint[j];

					// access the data to be printed - weird bug: had to move this up here since
					// doing this access actually changes the cell's style. ???
					String datastr = row.Cells[col.Index].EditedFormattedValue.ToString();

					// get gridview style, and override if we have a set style for this column
					StringFormat currentformat = null;
					DataGridViewCellStyle colstyle;
					if (_colstyles.TryGetValue(col.Name, out var style))
					{
						colstyle = style;

						// build the cell style and font 
						buildstringformat(ref currentformat, colstyle, _cellformat.Alignment, _cellformat.LineAlignment,
							_cellformat.FormatFlags, _cellformat.Trimming);
					}
					else if ((col.HasDefaultCellStyle) || (row.Cells[col.Index].HasStyle))
					{
						colstyle = row.Cells[col.Index].InheritedStyle;

						// build the cell style and font 
						buildstringformat(ref currentformat, colstyle, _cellformat.Alignment, _cellformat.LineAlignment,
							_cellformat.FormatFlags, _cellformat.Trimming);
					}
					else
					{
						currentformat = _cellformat;
						colstyle = _dgv.DefaultCellStyle;
					}

					// get the raw size of the string. 
					SizeF size = g.MeasureString(datastr, colstyle.Font);

					// Handle fixed size cells and > printwidth cells where the width of the
					// data won't fit. (I.E. need to stretch the row down the page) 
					if ((0 < _colwidthsoverride[j]) || (size.Width > printWidth))
					{
						// set column width
						if (0 < _colwidthsoverride[j])
							colwidths[j] = _colwidthsoverride[j];
						else if (size.Width > printWidth)
							colwidths[j] = printWidth;

						// remeasure the string with the new limits and proper formatting for wrapping. 
						// Use an absurd height value so that we can get the real number of lines printed
						int chars, lines;
						size = g.MeasureString(datastr, colstyle.Font, new SizeF(colwidths[j], 2147483647),
							currentformat, out chars, out lines);

						// set row height
						float tempheight = size.Height; // lines * size.Height;
						rowheights[i] = (rowheights[i] < tempheight ? tempheight : rowheights[i]);
					}
					else
					{
						colwidths[j] = (colwidths[j] < size.Width ? size.Width : colwidths[j]);
						rowheights[i] = (rowheights[i] < size.Height ? size.Height : rowheights[i]);
					}
				}
			}

			//-----------------------------------------------------------------
			// Break the columns accross page sets. This is the key to printing
			// where the total width is wider than one page.
			//-----------------------------------------------------------------

			// assume everything will fit on one page
			pagesets = new List<PageDef>();
			pagesets.Add(new PageDef(_printmargins, _colsToPrint.Count));
			int pset = 0;

			// Account for row headers 
			pagesets[pset].coltotalwidth = rowheaderwidth;

			// split columns into page sets
			float columnwidth;
			for (i = 0; i < _colsToPrint.Count; i++)
			{
				// get initial column width
				columnwidth = (_colwidthsoverride[i] >= 0)
					? _colwidthsoverride[i] : colwidths[i];

				// See if the column width takes us off the page - Except for the 
				// first column. This will prevent printing an empty page!! Otherwise,
				// columns longer than the page width are printed on their own page
				if (printWidth < (pagesets[pset].coltotalwidth + columnwidth) && i != 0)
				{
					pagesets.Add(new PageDef(_printmargins, _colsToPrint.Count));
					pset++;

					// Account for row headers 
					pagesets[pset].coltotalwidth = rowheaderwidth;
				}

				// update page set definition 
				pagesets[pset].colstoprint.Add(_colsToPrint[i]);
				pagesets[pset].colwidths.Add(colwidths[i]);
				pagesets[pset].colwidthsoverride.Add(_colwidthsoverride[i]);
				pagesets[pset].coltotalwidth += columnwidth;
			}

			//-----------------------------------------------------------------
			// Adjust column widths and table margins for each page
			//-----------------------------------------------------------------
			for (i = 0; i < pagesets.Count; i++)
				AdjustPageSets(g, pagesets[i]);
		}

		private void AdjustPageSets(
			Graphics g, 
			PageDef pageset)
		{
			int i;
			float fixedcolwidth = rowheaderwidth;
			float remainingcolwidth = 0;
			float ratio;

			//-----------------------------------------------------------------
			// Adjust the column widths in the page set to their final values,
			// accounting for overridden widths and porportional column stretching
			//-----------------------------------------------------------------

			// calculate the amount of space reserved for fixed width columns
			for (i = 0; i < pageset.colwidthsoverride.Count; i++)
				if (pageset.colwidthsoverride[i] >= 0)
					fixedcolwidth += pageset.colwidthsoverride[i];

			// calculate the amount space for non-overridden columns
			for (i = 0; i < pageset.colwidths.Count; i++)
				if (pageset.colwidthsoverride[i] < 0)
					remainingcolwidth += pageset.colwidths[i];

			// calculate the ratio for porportional colums, use 1 for no 
			// non-overridden columns or not porportional
			if (_porportionalcolumns && 0 < remainingcolwidth)
				ratio = ((float)printWidth - fixedcolwidth) / (float)remainingcolwidth;
			else
				ratio = (float)1.0;

			// reset all column widths for override and/or porportionality. coltotalwidth
			// for each pageset should be <= pageWidth
			pageset.coltotalwidth = rowheaderwidth;
			for (i = 0; i < pageset.colwidths.Count; i++)
			{
				if (pageset.colwidthsoverride[i] >= 0)
					pageset.colwidths[i] = pageset.colwidthsoverride[i];
				else
					pageset.colwidths[i] = pageset.colwidths[i] * ratio;

				pageset.coltotalwidth += pageset.colwidths[i];
			}

			//-----------------------------------------------------------------
			// Table Alignment - now that we have the column widths established
			// we can reset the table margins to get left, right and centered
			// for the table on the page
			//-----------------------------------------------------------------

			// Reset Print Margins based on table alignment
			if (Alignment.Left == _tablealignment)
			{
				// Bias table to the left by setting "right" value
				pageset.margins.Right = pageWidth - pageset.margins.Left - (int)pageset.coltotalwidth;
				if (0 > pageset.margins.Right) pageset.margins.Right = 0;
			}
			else if (Alignment.Right == _tablealignment)
			{
				// Bias table to the right by setting "left" value
				pageset.margins.Left = pageWidth - pageset.margins.Right - (int)pageset.coltotalwidth;
				if (0 > pageset.margins.Left) pageset.margins.Left = 0;
			}
			else if (Alignment.Center == _tablealignment)
			{
				// Bias the table to the center by setting left and right equal
				pageset.margins.Left = (pageWidth - (int)pageset.coltotalwidth) / 2;
				if (0 > pageset.margins.Left) pageset.margins.Left = 0;
				pageset.margins.Right = pageset.margins.Left;
			}
		}

		private int TotalPages()
		{
			int pages = 1;
			float pos = 0;
			float printablearea = pageHeight - headerHeight - footerHeight -
				_printmargins.Top - _printmargins.Bottom;

			for (int i = 0; i < (rowheights.Count); i++)
			{
				if (pos + rowheights[i] > printablearea)
				{
					// count the page
					pages++;
					// reset the counter
					pos = 0;
				}

				// add space
				pos += rowheights[i];
			}

			return pages;
		}

		private bool HasMorePages()
		{
			currentpageset++;
			if (currentpageset < pagesets.Count)
			{
				//currentpageset--;   // decrement back to a valid pageset number
				return true;        // tell the caller we're through.
			}
			else
				return false;
		}

		void printDoc_BeginPrint(
			object sender, 
			PrintEventArgs e)
		{
			// reset counters since we'll go through this twice if we print from preview
			currentpageset = 0;
			_lastRowPrinted = -1;
			CurrentPage = 0;
		}

		private void printDoc_PrintPage(
			object sender, 
			PrintPageEventArgs e)
		{
			// flag for handling printing some pages rather than all
			bool printthispage = false;

			// current printing position within one page
			float printpos = pagesets[currentpageset].margins.Top;

			// increment page number & check page range
			CurrentPage++;
			if ((CurrentPage >= _fromPage) && (CurrentPage <= _toPage))
				printthispage = true;

			// calculate the static vertical space available - this is where we stop printing rows
			float staticheight = pageHeight - footerHeight - pagesets[currentpageset].margins.Bottom;

			// holder for one-row height lookahead to see if the row will fit on the page
			float nextrowheight;

			//-----------------------------------------------------------------
			// scan down heights until we're off this (non-printing) page
			//-----------------------------------------------------------------

			while (!printthispage)
			{
				// calculate and increment over the page we're not printing
				printpos = pagesets[currentpageset].margins.Top + headerHeight;

				// are we done with this page?
				bool pagecomplete = false;

				// do one row look-ahead to see if we have room on the page
				nextrowheight = (_lastRowPrinted < rowheights.Count) ? rowheights[_lastRowPrinted + 1] : 0;
				while (!pagecomplete)
				{
					// this page is complete if we run out of data
					if (_lastRowPrinted >= _rowsToPrint.Count - 1)
						pagecomplete = true;
					// ... or off the bottom of the page
					else if ((printpos + nextrowheight) >= staticheight)
						pagecomplete = true;
					else
					{
						// not done yet so consume space
						_lastRowPrinted++;
						printpos += rowheights[_lastRowPrinted];
						nextrowheight = (_lastRowPrinted + 1 < rowheights.Count) ? rowheights[_lastRowPrinted + 1] : 0;
					}
				}

				// skip to the next page & see if it's in the print range
				CurrentPage++;
				if ((CurrentPage >= _fromPage) && (CurrentPage <= _toPage))
					printthispage = true;

				// bottom check~ out of data or out of pages
				if ((_lastRowPrinted >= _rowsToPrint.Count - 1) || (CurrentPage > _toPage))
				{
					// reset for next pageset or tell the caller we're complete
					e.HasMorePages = HasMorePages();

					// reset counters since we'll go through this twice if we print from preview
					_lastRowPrinted = -1;
					CurrentPage = 0;

					return;
				}
			}

			//-----------------------------------------------------------------
			// print headers
			//-----------------------------------------------------------------

			// reset printpos as it may have changed during the 'skip pages' routine just above.
			printpos = pagesets[currentpageset].margins.Top;

			// print page number if user selected it
			if (pagenumberontop)
			{
				// if we have a page number to print
				if (pageno)
				{
					String pagenumber = pagetext + CurrentPage.ToString(CultureInfo.CurrentCulture);
					if (showtotalpagenumber)
					{
						pagenumber += pageseparator + totalpages.ToString();
					}
					if (1 < pagesets.Count)
						pagenumber += parttext + (currentpageset + 1).ToString(CultureInfo.CurrentCulture);

					// ... then print it
					printsection(e.Graphics, ref printpos,
						pagenumber, pagenofont, pagenocolor, pagenumberformat,
						overridepagenumberformat, pagesets[currentpageset].margins);

					// if the page number is not on a separate line, don't "use up" it's vertical space
					if (!pagenumberonseparateline)
						printpos -= pagenumberHeight;
				}
			}

			// print title if provided
			if (!String.IsNullOrEmpty(title))
				printsection(e.Graphics, ref printpos, title, titlefont,
					titlecolor, titleformat, overridetitleformat,
					pagesets[currentpageset].margins);

			// print subtitle if provided
			if (!String.IsNullOrEmpty(subtitle))
				printsection(e.Graphics, ref printpos, subtitle, subtitlefont,
					subtitlecolor, subtitleformat, overridesubtitleformat,
					pagesets[currentpageset].margins);

			// print column headers
			printcolumnheaders(e.Graphics, ref printpos, pagesets[currentpageset]);

			//-----------------------------------------------------------------
			// print rows until the page is complete
			//-----------------------------------------------------------------

			// do one row look-ahead to see if we have room on the page
			nextrowheight = (_lastRowPrinted < rowheights.Count) ? rowheights[_lastRowPrinted + 1] : 0;
			while ((printpos + nextrowheight) < staticheight)
			{
				_lastRowPrinted++;
				printrow(e.Graphics, ref printpos, (DataGridViewRow)(_rowsToPrint[_lastRowPrinted]),
					pagesets[currentpageset]);

				// bottom check, we're really really done when there's no more data
				if (_lastRowPrinted >= _rowsToPrint.Count - 1)
				{
					// print a footer for this page
					printfooter(e.Graphics, ref printpos, pagesets[currentpageset].margins);

					// check on more page sets or set no more pages flag
					e.HasMorePages = HasMorePages();

					// reset counters since we'll go through this twice if we print from preview
					_lastRowPrinted = -1;
					CurrentPage = 0;

					// return
					return;
				}
				else
					nextrowheight = (_lastRowPrinted < rowheights.Count) ? rowheights[_lastRowPrinted + 1] : 0;
			}

			//-----------------------------------------------------------------
			// print footer
			//-----------------------------------------------------------------
			printfooter(e.Graphics, ref printpos, pagesets[currentpageset].margins);

			//-----------------------------------------------------------------
			// bottom check, see if this is the last page to print
			//-----------------------------------------------------------------

			if (CurrentPage >= _toPage)
			{
				// reset for next pageset or tell the caller we're complete
				e.HasMorePages = HasMorePages();

				// reset counters since we'll go through this twice if we print from preview
				_lastRowPrinted = -1;
				CurrentPage = 0;
			}
			else
			{
				// we're not done yet
				e.HasMorePages = true;
			}

			return;
		}

		private void printsection(
			Graphics g, 
			ref float pos, 
			string text, 
			Font font, 
			Color color, 
			StringFormat format, 
			bool useroverride, 
			Margins margins)
		{
			// measure string
			SizeF printsize = g.MeasureString(text, font, printWidth, format);

			// build area to print within
			RectangleF printarea = new RectangleF((float)margins.Left, pos, (float)printWidth,
			   printsize.Height);

			// do the actual print
			g.DrawString(text, font, new SolidBrush(color), printarea, format);

			// track "used" vertical space
			pos += printsize.Height;
		}

		private void printfooter(
			Graphics g, 
			ref float pos, 
			Margins margins)
		{
			// print last footer. Note: need to force printpos to the bottom of the page
			// as we may have run out of data anywhere on the page
			pos = pageHeight - footerHeight - margins.Bottom;  // - margins.Top

			// add spacing
			pos += footerspacing;

			// print the footer
			printsection(g, ref pos, footer, footerfont, footercolor, footerformat,
				overridefooterformat, margins);

			// print the page number if it's on the bottom.
			if (!pagenumberontop)
			{
				if (pageno)
				{
					String pagenumber = pagetext + CurrentPage.ToString(CultureInfo.CurrentCulture);
					if (showtotalpagenumber)
					{
						pagenumber += pageseparator + totalpages.ToString();
					}
					if (1 < pagesets.Count)
						pagenumber += parttext + (currentpageset + 1).ToString(CultureInfo.CurrentCulture);

					// if the pageno is not on a separate line, push the print location up by its height.
					if (!pagenumberonseparateline)
						pos = pos - pagenumberHeight;

					// print the page number
					printsection(g, ref pos, pagenumber, pagenofont, pagenocolor, pagenumberformat,
						overridepagenumberformat, margins);
				}
			}
		}

		private void printcolumnheaders(
			Graphics g, 
			ref float pos, 
			PageDef pageset)
		{
			// track printing location accross the page. start position is hard left,
			// adjusted for the row headers. Note rowheaderwidth is 0 if row headers are not printed
			float xcoord = pageset.margins.Left + rowheaderwidth;

			// set the pen for drawing the grid lines
			Pen lines = new Pen(_dgv.GridColor, 1);

			//-----------------------------------------------------------------
			// Print the column headers
			//-----------------------------------------------------------------
			DataGridViewColumn col;
			for (int i = 0; i < pageset.colstoprint.Count; i++)
			{
				col = (DataGridViewColumn)pageset.colstoprint[i];

				// calc cell width, account for columns larger than the print area!
				float cellwidth = (pageset.colwidths[i] > printWidth - rowheaderwidth ?
					printWidth - rowheaderwidth : pageset.colwidths[i]);

				// get column style
				DataGridViewCellStyle style = col.HeaderCell.InheritedStyle;

				// set print area for this individual cell, account for cells larger
				// than the print area!
				RectangleF cellprintarea = new RectangleF(xcoord, pos, cellwidth, colheaderheight);

				// print column header background
				g.FillRectangle(new SolidBrush(style.BackColor), cellprintarea);

				// draw the text
				g.DrawString(col.HeaderText, style.Font, new SolidBrush(style.ForeColor), cellprintarea,
					headercellformat);

				// draw the borders - default to the dgv's border setting; account for 
				// columns larger than the print width
				if (_dgv.ColumnHeadersBorderStyle != DataGridViewHeaderBorderStyle.None)
					g.DrawRectangle(lines, xcoord, pos, cellwidth, colheaderheight);

				xcoord += pageset.colwidths[i];
			}

			// all done, consume "used" vertical space, including space for border lines
			pos += colheaderheight +
				(_dgv.ColumnHeadersBorderStyle != DataGridViewHeaderBorderStyle.None ? lines.Width : 0);
		}

		private void printrow(
			Graphics g, 
			ref float pos, 
			DataGridViewRow row, 
			PageDef pageset)
		{
			// track printing location accross the page
			float xcoord = pageset.margins.Left;

			// set the pen for drawing the grid lines
			Pen lines;
			if (BlackWhite)
			{
				lines = new Pen(Color.Black, 1);
			}
			else
			{
				lines = new Pen(_dgv.GridColor, 1);
			}

			//-----------------------------------------------------------------
			// Print Row background
			//-----------------------------------------------------------------

			// get current row style, start with header style
			DataGridViewCellStyle rowstyle = row.InheritedStyle;

			// calc row width, account for columns larger than the print area!
			float rowwidth = (pageset.coltotalwidth > printWidth ? printWidth : pageset.coltotalwidth);

			// define print rectangle
			RectangleF printarea = new RectangleF(
				xcoord, 
				pos, 
				rowwidth,
				rowheights[_lastRowPrinted]);

			// fill in the row background as the default color
			if (BlackWhite)
			{
				if (row.Index % 2 == 0)
				{
					g.FillRectangle(new SolidBrush(Color.White), printarea);
				}
				else
				{
					g.FillRectangle(new SolidBrush(Color.LightGray), printarea);
				}
			}
			else
			{
				g.FillRectangle(new SolidBrush(rowstyle.BackColor), printarea);
			}

			//-----------------------------------------------------------------
			// Print the Row Headers, if they are visible
			//-----------------------------------------------------------------
			if (_dgv.RowHeadersVisible)
			{
				// get current row style, start with header style
				DataGridViewCellStyle headerstyle = row.HeaderCell.InheritedStyle;

				// set print area for this individual cell
				RectangleF headercellprintarea = new RectangleF(xcoord, pos, rowheaderwidth,
					rowheights[_lastRowPrinted]);

				// fill in the row header background
				g.FillRectangle(new SolidBrush(headerstyle.BackColor), headercellprintarea);

				// draw the text for the row header cell
				g.DrawString(row.HeaderCell.EditedFormattedValue.ToString(),
					headerstyle.Font, new SolidBrush(headerstyle.ForeColor), headercellprintarea,
					headercellformat);

				// draw the borders - default to the dgv's border setting
				if (_dgv.RowHeadersBorderStyle != DataGridViewHeaderBorderStyle.None)
					g.DrawRectangle(lines, xcoord, pos, rowheaderwidth, rowheights[_lastRowPrinted]);

				// track horizontal space used
				xcoord += rowheaderwidth;
			}

			//-----------------------------------------------------------------
			// Print the row: write and draw each cell
			//-----------------------------------------------------------------
			DataGridViewColumn col;
			for (int i = 0; i < pageset.colstoprint.Count; i++)
			{
				// access the column being printed
				col = (DataGridViewColumn)pageset.colstoprint[i];

				// access the data to be printed - weird bug: had to move this up here since
				// doing this access actually changes the cell's style. ???
				String datastr = row.Cells[col.Index].EditedFormattedValue.ToString();

				// calc cell width, account for columns larger than the print area!
				float cellwidth = (pageset.colwidths[i] > printWidth - rowheaderwidth ?
					printWidth - rowheaderwidth : pageset.colwidths[i]);

				// get DGV column style and see if we have an override for this column
				StringFormat finalformat = null;

				if (ColumnStyles.TryGetValue(col.Name, out var colstyle))
				{
					// set string format
					buildstringformat(ref finalformat, colstyle, _cellformat.Alignment, _cellformat.LineAlignment,
						_cellformat.FormatFlags, _cellformat.Trimming);
				}
				else if ((col.HasDefaultCellStyle) || (row.Cells[col.Index].HasStyle))
				{
					colstyle = row.Cells[col.Index].InheritedStyle;

					// set string format
					buildstringformat(ref finalformat, colstyle, _cellformat.Alignment, _cellformat.LineAlignment,
						_cellformat.FormatFlags, _cellformat.Trimming);
				}
				else
				{
					finalformat = _cellformat;

					// inherited style == default style (mostly) if no style was ever set.
					colstyle = row.Cells[col.Index].InheritedStyle;
				}

				// set print area for this individual cell
				RectangleF cellprintarea = new RectangleF(
					xcoord, 
					pos, 
					cellwidth,
					rowheights[_lastRowPrinted]);

				// fill in the cell background - using the selected style
				if (BlackWhite)
				{
					if (row.Index % 2 == 0)
					{
						g.FillRectangle(new SolidBrush(Color.White), cellprintarea);
					}
					else
					{
						g.FillRectangle(new SolidBrush(Color.LightGray), cellprintarea);
					}
				}
				else
				{
					g.FillRectangle(new SolidBrush(colstyle.BackColor), cellprintarea);
				}

				// draw the text for the cell at the row / col intersection
				if (BlackWhite)
				{
					g.DrawString(datastr, colstyle.Font, new SolidBrush(Color.Black),
						cellprintarea, finalformat);
				}
				else
				{
					g.DrawString(datastr, colstyle.Font, new SolidBrush(colstyle.ForeColor),
						cellprintarea, finalformat);
				}

				// draw the borders - default to the dgv's border setting
				if (_dgv.CellBorderStyle != DataGridViewCellBorderStyle.None)
					g.DrawRectangle(lines, xcoord, pos, cellwidth, rowheights[_lastRowPrinted]);

				// track horizontal space used
				xcoord += pageset.colwidths[i];
			}

			//-----------------------------------------------------------------
			// All done with this row, consume "used" vertical space
			//-----------------------------------------------------------------
			pos += rowheights[_lastRowPrinted];
		}
	}
}
