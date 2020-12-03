using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	[ToolboxItem(false)]
	public partial class itemBaseSymbolTradePair : UserControl, INotifyPropertyChanged
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

		private sealed class TradeTickIcons
		{
			public const int Up = 0;
			public const int None = 1;
			public const int Down = 2;
		}

		#region - Properties with events -

		private string _symbolToAdd = "";
		[Browsable(false)]
		public string SymbolToAdd
		{
			get
			{
				return _symbolToAdd;
			}
			set
			{
				if (_symbolToAdd != value)
				{
					_symbolToAdd = value;
					onPropertyChanged(new PropertyChangedEventArgs("SymbolToAdd"));
				}
			}
		}

		private string _symbolToRemove = "";
		[Browsable(false)]
		public string SymbolToRemove
		{
			get
			{
				return _symbolToRemove;
			}
			set
			{
				if (_symbolToRemove != value)
				{
					_symbolToRemove = value;
					onPropertyChanged(new PropertyChangedEventArgs("SymbolToRemove"));
				}
			}
		}

		private string _symbolToSubscribe = "";
		[Browsable(false)]
		public string SymbolToSubscribe
		{
			get
			{
				return _symbolToSubscribe;
			}
			set
			{
				_symbolToSubscribe = value;
				if (_symbolToSubscribe != "")
				{
					onPropertyChanged(new PropertyChangedEventArgs("SymbolToSubscribe"));
				}
			}
		}

		#endregion

		#region - Properties -

		[Browsable(false)]
		public string TradeTick
		{
			set
			{
				if (value.Contains("+"))
				{
					picTradeTick.Image = imageListTradeTick.Images[TradeTickIcons.Up];
					dspPrice.ForeColor = dspPrice.UpColor;
				}
				else if (value.Contains("-"))
				{
					picTradeTick.Image = imageListTradeTick.Images[TradeTickIcons.Down];
					dspPrice.ForeColor = dspPrice.DownColor;
				}
				else
				{
					picTradeTick.Image = imageListTradeTick.Images[TradeTickIcons.None];
					dspPrice.ForeColor = dspPrice.ForeColor;
				}
			}
		}

		[Browsable(false)]
		public double TradePrice
		{
			set
			{
				dspPrice.Value = value;
			}
		}

		private DataTable _symbols;
		[Browsable(false)]
		public DataTable Symbols
		{
			get
			{
				if (_symbols == null)
				{
					_symbols = new DataTable();

					_symbols.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
					
					List<DataColumn> keyColumns = new List<DataColumn>();
					keyColumns.Add(_symbols.Columns["Symbol"]);
					_symbols.PrimaryKey = keyColumns.ToArray();

					_symbols.DefaultView.Sort = "Symbol";
				}
				return _symbols;
			}
			set
			{
				_symbols = value;
			}
		}

		#endregion

		// This would reset the combox after every Enter and Delete key stroke
		private bool _newSymbol = true;

		public itemBaseSymbolTradePair()
		{
			InitializeComponent();

			cboSymbol.DataSource = Symbols;
			cboSymbol.DisplayMember = "Symbol";
		}

		#region - Symbol -

		private void cboSymbol_SelectedIndexChanged(object sender, EventArgs e)
		{
			SymbolToSubscribe = cboSymbol.Text;
		}

		private void cboSymbol_KeyUp(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					if (cboSymbol.Text != "")
					{
						AddSymbol(cboSymbol.Text.ToUpper());
					}
					_newSymbol = true;
					break;
				default:
					_newSymbol = false;
					break;
			}
		}

		private void AddSymbol(string symbol)
		{
			if (Symbols.DefaultView.FindRows(symbol).Length == 0)
			{
				Symbols.Rows.Add(symbol);
				cboSymbol.Text = symbol;
				SymbolToAdd = symbol;
			}
			else
			{
				cboSymbol.Text = symbol;
			}

			SymbolToSubscribe = cboSymbol.Text;
		}

		private void cboSymbol_KeyDown(object sender, KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Delete:
					DeleteSymbol(cboSymbol.Text.ToUpper());
					break;
				default:
					if (_newSymbol)
					{
						// Set the Text to null
						cboSymbol.Text = "";
					}
					break;
			}
		}

		private void DeleteSymbol(string symbol)
		{
			DataRowView[] rows = Symbols.DefaultView.FindRows(symbol);
			
			foreach (DataRowView row in rows)
			{
				Symbols.Rows.Remove(row.Row);
				cboSymbol.Text = "";
				SymbolToRemove = symbol;
			}
		}

		#endregion
	}
}
