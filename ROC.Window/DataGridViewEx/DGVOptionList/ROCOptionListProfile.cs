using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace DataGridViewEx
{
	[Serializable]
	public class ROCOptionListProfile : DGVBaseProfile
	{
		public ROCOptionListProfile()
		{
		}

		public ROCOptionListProfile(ROCOptionList grid)
		{
			Export(grid);
		}

		public ROCOptionListProfile(ROCOptionList grid, ROCOptionListProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCOptionList grid)
		{
			Update(DGVProfileFieldID.Grid.ForeColor, grid.DefaultCellForeColor);
			Update(DGVProfileFieldID.Grid.SelectedBackColor, grid.DefaultSelectedBackgroundColor);
			Update(DGVProfileFieldID.Grid.BackColor, grid.BackgroundColor);
			Update(DGVProfileFieldID.Grid.LineColor, grid.GridColor);
			Update(DGVProfileFieldID.Grid.Font, grid.Font);

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (col.Name)
				{
					case "PutSymbol":
					case "PutPartID":
					case "PutOpenQty":
					case "PutLastTraded":
					case "PutLastPartID":
					case "PutPctChanged":
					case "PutVolume":
					case "PutBidPartID":
					case "PutBidSize":
					case "PutBidPrice":
					case "PutAskSize":
					case "PutAskPrice":
					case "PutAskPartID":
					case "CallLastTraded":
					case "CallLastPartID":
					case "CallPctChanged":
					case "CallVolume":
					case "CallBidPartID":
					case "CallBidSize":
					case "CallBidPrice":
					case "CallAskSize":
					case "CallAskPrice":
					case "CallAskPartID":
					case "CallOpenQty":
					case "CallPartID":
					case "CallSymbol":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					case "Strike":
					case "Expiration":
					case "DisplayExpiration":
					case "OptionSymbol":
					case "SecType":
					case "PriceFlag":
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		public void Import(ROCOptionList grid, ROCOptionListProfile prof)
		{
			lock (prof)
			{
				grid.DefaultCellForeColor = prof.GridForeColor;
				grid.DefaultSelectedBackgroundColor = prof.GridSelectedBackColor;
				grid.BackgroundColor = prof.GridBackColor;
				grid.GridColor = prof.GridColor;
				grid.Font = prof.GridFont;

				ProfileColumns = prof.ProfileColumns;
			}

			#region - Profile Columns -

			foreach (DGVColumnProfile val in ProfileColumns.Values)
			{
				switch (val.ColumnName)
				{
					case "PutSymbol":
					case "PutPartID":
					case "PutOpenQty":
					case "PutLastTraded":
					case "PutLastPartID":
					case "PutPctChanged":
					case "PutVolume":
					case "PutBidPartID":
					case "PutBidSize":
					case "PutBidPrice":
					case "PutAskSize":
					case "PutAskPrice":
					case "PutAskPartID":
					case "CallLastTraded":
					case "CallLastPartID":
					case "CallPctChanged":
					case "CallVolume":
					case "CallBidPartID":
					case "CallBidSize":
					case "CallBidPrice":
					case "CallAskSize":
					case "CallAskPrice":
					case "CallAskPartID":
					case "CallOpenQty":
					case "CallPartID":
					case "CallSymbol":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					case "Strike":
					case "Expiration":
					case "DisplayExpiration":
					case "OptionSymbol":
					case "SecType":
					case "PriceFlag":
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
