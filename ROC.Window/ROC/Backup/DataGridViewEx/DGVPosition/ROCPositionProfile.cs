using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

using DictionaryEx;


namespace DataGridViewEx
{
	[Serializable]
	public class ROCPositionProfile : DGVBaseProfile
	{
		public ROCPositionProfile()
		{
		}

		public ROCPositionProfile(ROCPositionList grid)
		{
			Export(grid);
		}

		public ROCPositionProfile(ROCPositionList grid, ROCPositionProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCPositionList grid)
		{
			Update(DGVProfileFieldID.Grid.ForeColor, grid.DefaultCellForeColor);
			Update(DGVProfileFieldID.Grid.SelectedBackColor, grid.DefaultSelectedBackgroundColor);
			Update(DGVProfileFieldID.Grid.BackColor, grid.BackgroundColor);
			Update(DGVProfileFieldID.Grid.LineColor, grid.GridColor);
			Update(DGVProfileFieldID.Grid.Font, grid.Font);

			Update(DGVProfileFieldID.Position.RowColorType, (int)grid.RowColorType);

			Update(DGVProfileFieldID.Position.FilterOutStock, grid.FilterOutStock);
			Update(DGVProfileFieldID.Position.FilterOutFuture, grid.FilterOutFuture);
			Update(DGVProfileFieldID.Position.FilterOutOption, grid.FilterOutOption);

			Update(DGVProfileFieldID.Position.FilterOutTPOS, grid.FilterOutTPOS);
			Update(DGVProfileFieldID.Position.FilterOutROC, grid.FilterOutROC);

			Update(DGVProfileFieldID.Position.FilterOutAccounts, grid.FilterOutAccounts);
			Update(DGVProfileFieldID.Position.FilterOutSymbols, grid.FilterOutSymbols);

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (col.Name)
				{
					case "BidPrice":
					case "AskPrice":
					case "LastTraded":
						AddProfile((DGVPositionNumberColumn)col, true);
						break;
					case "NetChange":
					case "PctChange":
					case "OpenQty":
					case "CurrentQty":
					case "OpenPnL":
					case "DayPnL":
					case "TotalPnL":
					case "DayRealizedPnL":
					case "MarketValue":
						AddProfile((DGVPositionNumberColumn)col, false);
						break;
					case "BidSize":
					case "AskSize":
					case "PositionKey":
					case "Symbol":
					case "SecType":
					case "SymbolDetail":
					case "SymbolDisplay":
					case "Volume":
					case "ClearingAcct":
					case "Trader":
					case "OpenAvg":
					case "BuyQty":
					case "BuyAvg":
					case "SellQty":
					case "SellAvg":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					case "SettlePrice":
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		private void AddProfile(DGVPositionNumberColumn col, bool withNotification)
		{
			AddProfile((DataGridViewColumn)col);

			ProfileColumns[col.Name].DefaultColor = col.DefaultColor;
			ProfileColumns[col.Name].UpColor = col.UpColor;
			ProfileColumns[col.Name].DownColor = col.DownColor;
			if (withNotification)
			{
				ProfileColumns[col.Name].NotifyType = col.NotifyType;
			}
		}

		public void Import(ROCPositionList grid, ROCPositionProfile prof)
		{
			lock (prof)
			{
				grid.DefaultCellForeColor = prof.GridForeColor;
				grid.DefaultSelectedBackgroundColor = prof.GridSelectedBackColor;
				grid.BackgroundColor = prof.GridBackColor;
				grid.GridColor = prof.GridColor;
				grid.Font = prof.GridFont;

				grid.RowColorType = prof.GridRowColorType;

				grid.FilterOutStock = prof.GridFilterOutStock;
				grid.FilterOutFuture = prof.GridFilterOutFuture;
				grid.FilterOutOption = prof.GridFilterOutOption;

				grid.FilterOutTPOS = prof.GridFilterOutTPOS;
				grid.FilterOutROC = prof.GridFilterOutROC;

				grid.FilterOutAccounts = prof.GridFilterOutAccounts;
				grid.FilterOutSymbols = prof.GridFilterOutSymbols;

				ProfileColumns = prof.ProfileColumns;
			}

			#region - Profile Columns -

			foreach (DGVColumnProfile val in ProfileColumns.Values)
			{
				// Make sure the Column BackColor == GridBackColor
				val.ColumnBackColor = prof.GridBackColor;

				switch (val.ColumnName)
				{
					case "BidPrice":
					case "AskPrice":
					case "LastTraded":
						SetProfile((DGVPositionNumberColumn)grid.Columns[val.ColumnName], val, true);
						break;
					case "NetChange":
					case "PctChange":
					case "OpenQty":
					case "CurrentQty":
					case "OpenPnL":
					case "DayPnL":
					case "TotalPnL":
					case "DayRealizedPnL":
					case "MarketValue":
						SetProfile((DGVPositionNumberColumn)grid.Columns[val.ColumnName], val, false);
						break;
					case "BidSize":
					case "AskSize":
					case "PositionKey":
					case "Symbol":
					case "SecType":
					case "SymbolDetail":
					case "SymbolDisplay":
					case "Volume":
					case "ClearingAcct":
					case "Trader":
					case "OpenAvg":
					case "BuyQty":
					case "BuyAvg":
					case "SellQty":
					case "SellAvg":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					case "SettlePrice":
					default:
						SetProfile((DataGridViewColumn)grid.Columns[val.ColumnName], val);
						break;
				}
			}

			// TODO MSFT Behavior, when display index is duplicated, it defaults to index
			// http://connect.microsoft.com/VisualStudio/feedback/ViewFeedback.aspx?FeedbackID=115233
			// Need to set displayindex twice to really set it.
			foreach (DGVColumnProfile val in ProfileColumns.Values)
			{
				if (grid.Columns.Contains(val.ColumnName))
				{
					grid.Columns[val.ColumnName].DisplayIndex = val.ColumnDisplayIndex;
				}
			}

			grid.Importing = false;
			grid.ImportDone = true;
			grid.LastSort = grid.RocGridTable.DefaultView.Sort;
			grid.LastFilter = grid.RocGridTable.DefaultView.RowFilter;
			grid.Invalidate();

			#endregion
		}

		private void SetProfile(DGVPositionNumberColumn col, DGVColumnProfile val, bool withNotification)
		{
			if (col != null)
			{
				SetProfile((DataGridViewColumn)col, val);

				col.DefaultColor = val.ColumnForeColor;
				col.UpColor = val.UpColor;
				col.DownColor = val.DownColor;
				if (withNotification)
				{
					col.NotifyType = val.NotifyType;
				}
			}
		}

		#region - Filter -

		public ROCPositionList.RowColorTypes GridRowColorType
		{
			get
			{
				if (Longs.ContainsKey(DGVProfileFieldID.Position.RowColorType))
				{
					switch (Longs[DGVProfileFieldID.Position.RowColorType])
					{
						case 1:
							return ROCPositionList.RowColorTypes.CurrentQty;
						case 2:
							return ROCPositionList.RowColorTypes.TotalPnL;
						case 0:
						default:
							return ROCPositionList.RowColorTypes.None;
					}
				}
				else
				{
					return ROCPositionList.RowColorTypes.None;
				}
			}
			set
			{
				Update(DGVProfileFieldID.Position.RowColorType, (int)value);
			}
		}

		public bool GridFilterOutStock
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Position.FilterOutStock))
				{
					return Bools[DGVProfileFieldID.Position.FilterOutStock];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutFuture
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Position.FilterOutFuture))
				{
					return Bools[DGVProfileFieldID.Position.FilterOutFuture];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutOption
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Position.FilterOutOption))
				{
					return Bools[DGVProfileFieldID.Position.FilterOutOption];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutTPOS
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Position.FilterOutTPOS))
				{
					return Bools[DGVProfileFieldID.Position.FilterOutTPOS];
				}
				else
				{
					return true;
				}
			}
		}

		public bool GridFilterOutROC
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Position.FilterOutROC))
				{
					return Bools[DGVProfileFieldID.Position.FilterOutROC];
				}
				else
				{
					return true;
				}
			}
		}

		public string GridFilterOutAccounts
		{
			get
			{
				if (Strings.ContainsKey(DGVProfileFieldID.Position.FilterOutAccounts))
				{
					return Strings[DGVProfileFieldID.Position.FilterOutAccounts];
				}
				else
				{
					return "";
				}
			}
		}

		public string GridFilterOutSymbols
		{
			get
			{
				if (Strings.ContainsKey(DGVProfileFieldID.Position.FilterOutSymbols))
				{
					return Strings[DGVProfileFieldID.Position.FilterOutSymbols];
				}
				else
				{
					return "";
				}
			}
		}

		#endregion
	}
}
