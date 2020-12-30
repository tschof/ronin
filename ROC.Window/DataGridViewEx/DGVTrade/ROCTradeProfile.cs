using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;

using DictionaryEx;

namespace DataGridViewEx
{
	[Serializable]
	public class ROCTradeProfile : DGVBaseProfile
	{
		public ROCTradeProfile()
		{
		}

		public ROCTradeProfile(ROCTradeList grid)
		{
			Export(grid);
		}

		public ROCTradeProfile(ROCTradeList grid, ROCTradeProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCTradeList grid)
		{
			Update(DGVProfileFieldID.Grid.ForeColor, grid.DefaultCellForeColor);
			Update(DGVProfileFieldID.Grid.SelectedBackColor, grid.DefaultSelectedBackgroundColor);
			Update(DGVProfileFieldID.Grid.BackColor, grid.BackgroundColor);
			Update(DGVProfileFieldID.Grid.LineColor, grid.GridColor);
			Update(DGVProfileFieldID.Grid.Font, grid.Font);

			Update(DGVProfileFieldID.Trade.RowColorType, (int)grid.RowColorType);

			Update(DGVProfileFieldID.Trade.FilterOutStock, grid.FilterOutStock);
			Update(DGVProfileFieldID.Trade.FilterOutFuture, grid.FilterOutFuture);
			Update(DGVProfileFieldID.Trade.FilterOutOption, grid.FilterOutOption);

			Update(DGVProfileFieldID.Trade.FilterOutTPOS, grid.FilterOutTPOS);
			Update(DGVProfileFieldID.Trade.FilterOutROC, grid.FilterOutROC);

			Update(DGVProfileFieldID.Trade.FilterOutAccounts, grid.FilterOutAccounts);
			Update(DGVProfileFieldID.Trade.FilterOutSymbols, grid.FilterOutSymbols);

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (col.Name)
				{
					case "Side":
						AddProfile((DGVTradeSideColumn)col);
						break;
					case "ExecID":
					case "Symbol":
					case "SymbolDetail":
					case "SymbolDisplay":
					case "Qty":
					case "Price":
					case "TradeValue":
					case "Exchange":
					case "Time":
					case "Account":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		private void AddProfile(DGVTradeSideColumn col)
		{
			AddProfile((DataGridViewColumn)col);

			ProfileColumns[col.Name].SellColor = col.SellColor;
			ProfileColumns[col.Name].BuyColor = col.BuyColor;
			ProfileColumns[col.Name].SellShortColor = col.SellShortColor;
			ProfileColumns[col.Name].SSEColor = col.SSEColor;
		}

		public void Import(ROCTradeList grid, ROCTradeProfile prof)
		{
			grid.Importing = true;

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
					case "Side":
						SetProfile((DGVTradeSideColumn)grid.Columns[val.ColumnName], val);
						break;
					case "ExecID":
					case "Symbol":
					case "SymbolDetail":
					case "SymbolDisplay":
					case "Qty":
					case "Price":
					case "TradeValue":
					case "Exchange":
					case "Time":
					case "Account":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
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

		private void SetProfile(DGVTradeSideColumn col, DGVColumnProfile val)
		{
			if (col != null)
			{
				SetProfile((DataGridViewColumn)col, val);

				col.BuyColor = val.BuyColor;
				col.SellColor = val.SellColor;
				col.SellShortColor = val.SellShortColor;
				col.SSEColor = val.SSEColor;
			}
		}

		#region - Filter -

		public ROCTradeList.RowColorTypes GridRowColorType
		{
			get
			{
				if (!TryGet(DGVProfileFieldID.Trade.RowColorType, out long value))
					return ROCTradeList.RowColorTypes.Side;
				else if (value == 1)
					return ROCTradeList.RowColorTypes.Side;
				else
					return ROCTradeList.RowColorTypes.None;
			}
			set
			{
				Update(DGVProfileFieldID.Trade.RowColorType, (int)value);
			}
		}

		public bool GridFilterOutStock
		{
			get => TryGet(DGVProfileFieldID.Trade.FilterOutStock, out bool value) && value;
		}

		public bool GridFilterOutFuture
		{
			get => TryGet(DGVProfileFieldID.Trade.FilterOutFuture, out bool value) && value;
		}

		public bool GridFilterOutOption
		{
			get => TryGet(DGVProfileFieldID.Trade.FilterOutOption, out bool value) && value;
		}

		public bool GridFilterOutTPOS
		{
			get => TryGet(DGVProfileFieldID.Trade.FilterOutTPOS, out bool value) ? value : true;
		}

		public bool GridFilterOutROC
		{
			get => TryGet(DGVProfileFieldID.Trade.FilterOutROC, out bool value) ? value : true;
		}

		public string GridFilterOutAccounts
		{
			get => TryGet(DGVProfileFieldID.Trade.FilterOutAccounts, out string value) ? value : "";
		}

		public string GridFilterOutSymbols
		{
			get => TryGet(DGVProfileFieldID.Trade.FilterOutSymbols, out string value) ? value : "";
		}

		#endregion
	}
}
