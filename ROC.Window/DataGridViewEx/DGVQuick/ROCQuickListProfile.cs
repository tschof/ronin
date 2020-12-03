using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace DataGridViewEx
{
	[Serializable]
	public class ROCQuickListProfile : DGVBaseProfile
	{
		public ROCQuickListProfile()
		{
		}

		public ROCQuickListProfile(ROCQuickList grid)
		{
			Export(grid);
		}

		public ROCQuickListProfile(ROCQuickList grid, ROCQuickListProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCQuickList grid)
		{
			Update(DGVProfileFieldID.Grid.ForeColor, grid.DefaultCellForeColor);
			Update(DGVProfileFieldID.Grid.SelectedBackColor, grid.DefaultSelectedBackgroundColor);
			Update(DGVProfileFieldID.Grid.BackColor, grid.BackgroundColor);
			Update(DGVProfileFieldID.Grid.LineColor, grid.GridColor);
			Update(DGVProfileFieldID.Grid.Font, grid.Font);

			Update(DGVProfileFieldID.Grid.AutoCenterOnTraded, grid.AutoCenterOnTraded);
			Update(DGVProfileFieldID.Grid.SelectedStopLimitBackColor, grid.SelectedStopLimitBackColor);
			Update(DGVProfileFieldID.Grid.AutoCenterRange, grid.AutoCenterRange);

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (col.Name)
				{
					case "BuyTags":
					case "BuyOpenQty":
					case "BidQty":
					case "BidSize":
					case "Price":
					case "Volume":
					case "TradedVolume":
					case "AskSize":
					case "AskQty":
					case "SellOpenQty":
					case "SellTags":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					case "PriceFlag":
					case "PriceHighFlag":
					case "PriceLowFlag":
					case "BuyStopLimitFlag":
					case "SellStopLimitFlag":
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		public void Import(ROCQuickList grid, ROCQuickListProfile prof)
		{
			lock (prof)
			{
				grid.DefaultCellForeColor = prof.GridForeColor;
				grid.DefaultSelectedBackgroundColor = prof.GridSelectedBackColor;
				grid.BackgroundColor = prof.GridBackColor;
				grid.GridColor = prof.GridColor;
				grid.Font = prof.GridFont;

				grid.AutoCenterOnTraded = prof.GridAutoCenterOnTraded;
				grid.SelectedStopLimitBackColor = prof.SelectedStopLimitBackColor;
				grid.AutoCenterRange = prof.GridAutoCenterRange;

				ProfileColumns = prof.ProfileColumns;
			}

			#region - Profile Columns -

			foreach (DGVColumnProfile val in ProfileColumns.Values)
			{
				switch (val.ColumnName)
				{
					case "BuyTags":
					case "BuyOpenQty":
					case "BidQty":
					case "BidSize":
					case "Price":
					case "Volume":
					case "TradedVolume":
					case "AskSize":
					case "AskQty":
					case "SellOpenQty":
					case "SellTags":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					case "PriceFlag":
					case "PriceHighFlag":
					case "PriceLowFlag":
					case "BuyStopLimitFlag":
					case "SellStopLimitFlag":
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

			grid.LastSort = grid.RocGridTable.DefaultView.Sort;
			grid.LastFilter = grid.RocGridTable.DefaultView.RowFilter;
			grid.Invalidate();

			#endregion
		}
	}
}
