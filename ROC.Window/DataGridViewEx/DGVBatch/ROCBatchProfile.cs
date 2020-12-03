using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

using DictionaryEx;
using System.Runtime.Serialization;

namespace DataGridViewEx
{
	[Serializable]
	public class ROCBatchProfile : DGVBaseProfile
	{
		public ROCBatchProfile(ROCBatch grid)
		{
			Export(grid);
		}

		public ROCBatchProfile(ROCBatch grid, ROCBatchProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCBatch grid)
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
					case "Status":
						AddProfile((DGVBatchStatusColumn)col);
						break;
					case "Symbol":
					case "Qty":
					case "Side":
					case "Price":
					case "Exchange":
					case "Trader":
					case "Account":
					case "Display":
					case "ShortLender":
					case "Duration":
					case "OrderType":
					case "Instruction":
					case "ExecInstruction":

					case "ProgramTrade":
					case "Note":

					case "StopPrice":
					case "PegPrice":

					case "AlgoType":
					case "StartTime":
					case "EndTime":

					case "OrderID":
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		private void AddProfile(DGVBatchStatusColumn col)
		{
			AddProfile((DataGridViewColumn)col);

			ProfileColumns[col.Name].PartialFilledColor = col.PartialFilledColor;
			ProfileColumns[col.Name].FilledColor = col.FilledColor;
			ProfileColumns[col.Name].DoneForDayColor = col.DoneForDayColor;
			ProfileColumns[col.Name].CancelledColor = col.CancelledColor;
			ProfileColumns[col.Name].ReplacedColor = col.ReplacedColor;
			ProfileColumns[col.Name].StoppedColor = col.StoppedColor;
			ProfileColumns[col.Name].RejectedColor = col.RejectedColor;
			ProfileColumns[col.Name].CalculatedColor = col.CalculatedColor;
			ProfileColumns[col.Name].ExpiredColor = col.ExpiredColor;
			ProfileColumns[col.Name].OpenColor = col.OpenColor;
			ProfileColumns[col.Name].CancelRejectedColor = col.CancelRejectedColor;
			ProfileColumns[col.Name].CorrectedColor = col.CorrectedColor;
			ProfileColumns[col.Name].BustedColor = col.BustedColor;
			ProfileColumns[col.Name].ReplaceRejectedColor = col.ReplaceRejectedColor;
			ProfileColumns[col.Name].ReplacePendingColor = col.ReplacePendingColor;
			ProfileColumns[col.Name].OtherColor = col.OtherColor;
		}

		public void Import(ROCBatch grid, ROCBatchProfile prof)
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
					case "Status":
						SetProfile((DGVBatchStatusColumn)grid.Columns[val.ColumnName], val);
						break;
					case "Symbol":
					case "Qty":
					case "Side":
					case "Price":
					case "Exchange":
					case "Trader":
					case "Account":
					case "Display":
					case "ShortLender":
					case "Duration":
					case "OrderType":
					case "Instruction":
					case "ExecInstruction":
					
					case "ProgramTrade":
					case "Note":
					
					case "StopPrice":
					case "PegPrice":

					case "AlgoType":
					case "StartTime":
					case "EndTime":

					case "OrderID":
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

			grid.Invalidate();

			#endregion
		}

		private void SetProfile(DGVBatchStatusColumn col, DGVColumnProfile val)
		{
			SetProfile((DataGridViewColumn)col, val);

			col.PartialFilledColor = val.PartialFilledColor;
			col.FilledColor = val.FilledColor;
			col.DoneForDayColor = val.DoneForDayColor;
			col.CancelledColor = val.CancelledColor;
			col.ReplacedColor = val.ReplacedColor;
			col.PendingCancelColor = val.PendingCancelColor;
			col.StoppedColor = val.StoppedColor;
			col.RejectedColor = val.RejectedColor;
			col.CalculatedColor = val.CalculatedColor;
			col.ExpiredColor = val.ExpiredColor;
			col.OpenColor = val.OpenColor;
			col.CancelRejectedColor = val.CancelRejectedColor;
			col.CorrectedColor = val.CorrectedColor;
			col.BustedColor = val.BustedColor;
			col.ReplaceRejectedColor = val.ReplaceRejectedColor;
			col.ReplacePendingColor = val.ReplacePendingColor;
			col.OtherColor = val.OtherColor;
		}
	}
}
