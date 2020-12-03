using System;
using System.Drawing;
using System.Windows.Forms;
using System.Collections.Generic;

using DictionaryEx;
using System.Runtime.Serialization;

namespace DataGridViewEx
{
	[Serializable]
	public class ROCOrderProfile : DGVBaseProfile
	{
		public ROCOrderProfile(ROCOrderList grid)
		{
			Export(grid);
		}

		public ROCOrderProfile(ROCOrderList grid, ROCOrderProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCOrderList grid)
		{
			Update(DGVProfileFieldID.Grid.ForeColor, grid.DefaultCellForeColor);
			Update(DGVProfileFieldID.Grid.SelectedBackColor, grid.DefaultSelectedBackgroundColor);
			Update(DGVProfileFieldID.Grid.BackColor, grid.BackgroundColor);
			Update(DGVProfileFieldID.Grid.LineColor, grid.GridColor);
			Update(DGVProfileFieldID.Grid.Font, grid.Font);

			Update(DGVProfileFieldID.Order.RowColorType, (int)grid.RowColorType);

			Update(DGVProfileFieldID.Order.FilterOutStock, grid.FilterOutStock);
			Update(DGVProfileFieldID.Order.FilterOutFuture, grid.FilterOutFuture);
			Update(DGVProfileFieldID.Order.FilterOutOption, grid.FilterOutOption);

			Update(DGVProfileFieldID.Order.FilterOutFilled, grid.FilterOutFilled);
			Update(DGVProfileFieldID.Order.FilterOutPartialFilled, grid.FilterOutPartialFilled);
			Update(DGVProfileFieldID.Order.FilterOutReplaced, grid.FilterOutReplaced);
			Update(DGVProfileFieldID.Order.FilterOutOpen, grid.FilterOutOpen);
			Update(DGVProfileFieldID.Order.FilterOutCancelled, grid.FilterOutCanceled);
			Update(DGVProfileFieldID.Order.FilterOutRejected, grid.FilterOutRejected);

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (col.Name)
				{
					case "Status":
						AddProfile((DGVOrderStatusColumn)col);
						break;
					case "Side":
						AddProfile((DGVOrderSideColumn)col);
						break;
					case "OrderID":
					case "Symbol":
					case "SymbolDetail":
					case "SymbolDisplay":
					case "Qty":
					case "Leave":
					case "Price":
					case "StopPrice":
					case "PegPrice":
					case "Filled":
					case "AvgPrice":
					case "Type":
					case "Duration":
					case "AlgoType":
					case "AlgoEndTime":
					case "Exchange":
					case "Time":
					case "Account":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					case "ParentTag":
					case "Echo":
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		private void AddProfile(DGVOrderStatusColumn col)
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

		private void AddProfile(DGVOrderSideColumn col)
		{
			AddProfile((DataGridViewColumn)col);

			ProfileColumns[col.Name].SellColor = col.SellColor;
			ProfileColumns[col.Name].BuyColor = col.BuyColor;
			ProfileColumns[col.Name].SellShortColor = col.SellShortColor;
			ProfileColumns[col.Name].SSEColor = col.SSEColor;
		}

		public void Import(ROCOrderList grid, ROCOrderProfile prof)
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

				grid.FilterOutFilled = prof.GridFilterOutFilled;
				grid.FilterOutPartialFilled = prof.GridFilterOutPartialFilled;
				grid.FilterOutReplaced = prof.GridFilterOutReplaced;
				grid.FilterOutOpen = prof.GridFilterOutOpen;
				grid.FilterOutCanceled = prof.GridFilterOutCanceled;
				grid.FilterOutRejected = prof.GridFilterOutRejected;

				ProfileColumns = prof.ProfileColumns;
			}

			#region - Profile Columns -

			foreach (DGVColumnProfile val in ProfileColumns.Values)
			{
				// Make sure the Column BackColor == GridBackColor
				val.ColumnBackColor = prof.GridBackColor;

				switch (val.ColumnName)
				{
					case "Status":
						SetProfile((DGVOrderStatusColumn)grid.Columns[val.ColumnName], val);
						break;
					case "Side":
						SetProfile((DGVOrderSideColumn)grid.Columns[val.ColumnName], val);
						break;
					case "OrderID":
					case "Symbol":
					case "SymbolDetail":
					case "SymbolDisplay":
					case "Detail":
					case "Qty":
					case "Leave":
					case "Price":
					case "StopPrice":
					case "PegPrice":
					case "Filled":
					case "AvgPrice":
					case "Type":
					case "Duration":
					case "AlgoType":
					case "AlgoEndTime":
					case "Exchange":
					case "Time":
					case "Account":
					case "ContractSize":
					case "TickSize":
					case "DisplayFactor":
					case "ParentTag":
					case "Echo":
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

		private void SetProfile(DGVOrderStatusColumn col, DGVColumnProfile val)
		{
			if (col != null)
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

		private void SetProfile(DGVOrderSideColumn col, DGVColumnProfile val)
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

		public ROCOrderList.RowColorTypes GridRowColorType
		{
			get
			{
				if (Longs.ContainsKey(DGVProfileFieldID.Order.RowColorType))
				{
					switch (Longs[DGVProfileFieldID.Order.RowColorType])
					{
						case 0:
							return ROCOrderList.RowColorTypes.None;
						case 2:
							return ROCOrderList.RowColorTypes.Side;
						case 1:
						default:
							return ROCOrderList.RowColorTypes.Status;
					}
				}
				else
				{
					return ROCOrderList.RowColorTypes.Status;
				}
			}
			set
			{
				Update(DGVProfileFieldID.Order.RowColorType, (int)value);
			}
		}

		public bool GridFilterOutStock
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutStock))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutStock];
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
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutFuture))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutFuture];
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
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutOption))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutOption];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutFilled
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutFilled))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutFilled];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutPartialFilled
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutPartialFilled))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutPartialFilled];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutReplaced
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutReplaced))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutReplaced];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutOpen
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutOpen))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutOpen];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutCanceled
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutCancelled))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutCancelled];
				}
				else
				{
					return false;
				}
			}
		}

		public bool GridFilterOutRejected
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Order.FilterOutRejected))
				{
					return Bools[DGVProfileFieldID.Order.FilterOutRejected];
				}
				else
				{
					return false;
				}
			}
		}

		#endregion
	}
}
