using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace DataGridViewEx
{
	[Serializable]
	public class ROCLevel2ListProfile : DGVBaseProfile
	{
		public ROCLevel2ListProfile()
		{
		}

		public ROCLevel2ListProfile(ROCLevel2List grid)
		{
			Export(grid);
		}

		public ROCLevel2ListProfile(ROCLevel2List grid, ROCLevel2ListProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCLevel2List grid)
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
					case "Symbol":
					case "SymbolDetail":
					case "Tag":
					case "PartName":
					case "PartID":
					case "Price":
					case "Size":
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

		public void Import(ROCLevel2List grid, ROCLevel2ListProfile prof)
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
					case "Symbol":
					case "SymbolDetail":
					case "Tag":
					case "PartName":
					case "PartID":
					case "Price":
					case "Size":
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
				grid.Columns[val.ColumnName].DisplayIndex = val.ColumnDisplayIndex;
			}

			grid.LastSort = grid.RocGridTable.DefaultView.Sort;
			grid.LastFilter = grid.RocGridTable.DefaultView.RowFilter;
			grid.Invalidate();

			#endregion
		}
	}
}
