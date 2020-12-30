using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

using DictionaryEx;
using System.Data;

namespace DataGridViewEx
{
	[Serializable]
	public class ROCWatchListProfile : DGVBaseProfile
	{
		public ROCWatchListProfile()
		{
		}

		public ROCWatchListProfile(ROCWatchList grid)
		{
			Export(grid);
		}

		public ROCWatchListProfile(ROCWatchList grid, ROCWatchListProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCWatchList grid)
		{
			Update(DGVProfileFieldID.Grid.ForeColor, grid.DefaultCellForeColor);
			Update(DGVProfileFieldID.Grid.SelectedBackColor, grid.DefaultSelectedBackgroundColor);
			Update(DGVProfileFieldID.Grid.BackColor, grid.BackgroundColor);
			Update(DGVProfileFieldID.Grid.LineColor, grid.GridColor);
			Update(DGVProfileFieldID.Grid.Font, grid.Font);

			Update(DGVProfileFieldID.Watch.RowColorType, (int)grid.RowColorType);

			#region - SymbolDetail List -

			for (int i = 0; i < grid.Rows.Count - 1; i++)
			{
				if (grid.Rows[i].Cells["SymbolDetail"].Value != null)
				{
					SymbolDetailList.Add(grid.Rows[i].Cells["SymbolDetail"].Value.ToString().ToUpper().Trim());
				}
			}

			#endregion

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (col.Name)
				{
					case "BidPrice":
					case "AskPrice":
					case "LastTraded":
						AddProfile((DGVWatchListNumberColumn)col, true);
						break;
					case "Tick":
						AddProfile((DGVWatchListTickColumn)col);
						break;
					case "NetChange":
					case "PctChange":
						AddProfile((DGVWatchListNumberColumn)col, false);
						break;
					case "Symbol":
					case "SymbolDetail":

					case "BidSize":
					case "BidPartID":
					case "BidHigh":
					case "BidLow":

					case "AskSize":
					case "AskPartID":
					case "AskHigh":
					case "AskLow":

					case "Volume":
					case "TradeVolume":
					case "TradePartID":

					case "High":
					case "Low":
					case "Open":
					case "Close":
					case "Vwap":
					case "SettlePrice":
					case "OpenInterest":
					case "Name":

					case "TickSize":
					case "DisplayFactor":

					case "SecType":
					case "SecurityStatus":

					case "LineTime":
					case "DecodeTime":
					case "OnMsgTime":
					case "ServerSentTime":
					case "ClientRecivedTime":
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		private void AddProfile(DGVWatchListNumberColumn col, bool withNotification)
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

		private void AddProfile(DGVWatchListTickColumn col)
		{
			AddProfile((DataGridViewColumn)col);

			ProfileColumns[col.Name].DefaultColor = col.DefaultColor;
			ProfileColumns[col.Name].UpColor = col.UpColor;
			ProfileColumns[col.Name].DownColor = col.DownColor;
			ProfileColumns[col.Name].NotifyType = col.NotifyType;
		}

		public void Import(ROCWatchList grid, ROCWatchListProfile prof)
		{
			lock (prof)
			{
				grid.DefaultCellForeColor = prof.GridForeColor;
				grid.DefaultSelectedBackgroundColor = prof.GridSelectedBackColor;
				grid.BackgroundColor = prof.GridBackColor;
				grid.GridColor = prof.GridColor;
				grid.Font = prof.GridFont;

				grid.RowColorType = prof.GridRowColorType;

				ProfileColumns = prof.ProfileColumns;
			}

			#region - SymbolDetail List -

			//if (prof.SymbolDetailList != null && prof.SymbolDetailList.Count > 0)
			//{
			//    grid.RocGridTable.BeginLoadData();

			//    // Check to see if the last item is null or not, if null remove from list
			//    while (prof.SymbolDetailList[prof.SymbolDetailList.Count - 1] == "")
			//    {
			//        prof.SymbolDetailList.RemoveAt(prof.SymbolDetailList.Count - 1);
			//    }

			//    DataRow row;
			//    foreach (string symbolDetail in prof.SymbolDetailList)
			//    {
			//        row = grid.RocGridTable.NewRow();
			//        row["SymbolDetail"] = symbolDetail;
			//        grid.RocGridTable.Rows.Add(row);
			//    }

			//    grid.RocGridTable.EndLoadData();
			//}

			#endregion

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
						SetProfile((DGVWatchListNumberColumn)grid.Columns[val.ColumnName], val, true);
						break;
					case "Tick":
						SetProfile((DGVWatchListTickColumn)grid.Columns[val.ColumnName], val);
						break;
					case "NetChange":
					case "PctChange":
						SetProfile((DGVWatchListNumberColumn)grid.Columns[val.ColumnName], val, false);
						break;
					case "Symbol":
					case "SymbolDetail":

					case "BidSize":
					case "BidPartID":
					case "BidHigh":
					case "BidLow":

					case "AskSize":
					case "AskPartID":
					case "AskHigh":
					case "AskLow":

					case "Volume":
					case "TradeVolume":
					case "TradePartID":

					case "High":
					case "Low":
					case "Open":
					case "Close":
					case "Vwap":
					case "SettlePrice":
					case "OpenInterest":
					case "Name":

					case "TickSize":
					case "DisplayFactor":

					case "SecType":
					case "SecurityStatus":

					case "LineTime":
					case "DecodeTime":
					case "OnMsgTime":
					case "ServerSentTime":
					case "ClientRecivedTime":
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

		private void SetProfile(DGVWatchListTickColumn col, DGVColumnProfile val)
		{
			if (col != null)
			{
				SetProfile((DataGridViewColumn)col, val);

				col.DefaultColor = val.ColumnForeColor;
				col.UpColor = val.UpColor;
				col.DownColor = val.DownColor;
				col.NotifyType = val.NotifyType;
			}
		}

		private void SetProfile(DGVWatchListNumberColumn col, DGVColumnProfile val, bool withNotification)
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

		public ROCWatchList.RowColorTypes GridRowColorType
		{
			get
			{
				if (!TryGet(DGVProfileFieldID.Watch.RowColorType, out long value))
					return ROCWatchList.RowColorTypes.None;
				else if (value == 1)
					return ROCWatchList.RowColorTypes.Performence;
				else
					return ROCWatchList.RowColorTypes.None;
			}
			set
			{
				Update(DGVProfileFieldID.Watch.RowColorType, (int)value);
			}
		}

		#endregion
	}
}
