using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

using FormEx;
using DataGridViewEx;

namespace FontDialogEx
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmOrdersDisplay : VistaWindowBorderless, INotifyPropertyChanged
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

		public int RowColorType
		{
			get
			{
				return cboRowType.SelectedIndex;
			}
		}

		public frmOrdersDisplay(ROCOrdersProfile prof)
		{
			InitializeComponent();

			itemColorSelectedBackground.ItemColor = prof.GridSelectedBackColor;
			itemColorBackground.ItemColor = prof.GridBackColor;
			itemColorGridlines.ItemColor = prof.GridColor;
			itemColorText.ItemColor = prof.GridForeColor;

			itemColumnOrderID.ItemColor = prof.ProfileColumns["OrderID"].ColumnForeColor;
			itemColumnSymbol.ItemColor = prof.ProfileColumns["Symbol"].ColumnForeColor;
			itemColumnQty.ItemColor = prof.ProfileColumns["Qty"].ColumnForeColor;
			itemColumnLeaveQty.ItemColor = prof.ProfileColumns["Leave"].ColumnForeColor;
			itemColumnPrice.ItemColor = prof.ProfileColumns["Price"].ColumnForeColor;
			itemColumnFilledQty.ItemColor = prof.ProfileColumns["Filled"].ColumnForeColor;
			itemColumnAvgPrice.ItemColor = prof.ProfileColumns["AvgPrice"].ColumnForeColor;
			itemColumnType.ItemColor = prof.ProfileColumns["Type"].ColumnForeColor;
			itemColumnDuration.ItemColor = prof.ProfileColumns["Duration"].ColumnForeColor;
			itemColumnExchange.ItemColor = prof.ProfileColumns["Exchange"].ColumnForeColor;
			itemColumnTime.ItemColor = prof.ProfileColumns["Time"].ColumnForeColor;
			itemColumnAccount.ItemColor = prof.ProfileColumns["Account"].ColumnForeColor;

			itemStatusBusted.ItemColor = prof.ProfileColumns["Status"].BustedColor;
			itemStatusCalculated.ItemColor = prof.ProfileColumns["Status"].CalculatedColor;
			itemStatusCancelled.ItemColor = prof.ProfileColumns["Status"].CancelledColor;
			itemStatusCancelRejected.ItemColor = prof.ProfileColumns["Status"].CancelRejectedColor;
			itemStatusCorrected.ItemColor = prof.ProfileColumns["Status"].CorrectedColor;
			itemStatusDoneForDay.ItemColor = prof.ProfileColumns["Status"].DoneForDayColor;
			itemStatusExpired.ItemColor = prof.ProfileColumns["Status"].ExpiredColor;
			itemStatusFilled.ItemColor = prof.ProfileColumns["Status"].FilledColor;
			itemStatusOpen.ItemColor = prof.ProfileColumns["Status"].OpenColor;
			itemStatusOther.ItemColor = prof.ProfileColumns["Status"].OtherColor;
			itemStatusPartialFilled.ItemColor = prof.ProfileColumns["Status"].PartialFilledColor;
			itemStatusPendingCancel.ItemColor = prof.ProfileColumns["Status"].PendingCancelColor;
			itemStatusRejected.ItemColor = prof.ProfileColumns["Status"].RejectedColor;
			itemStatusReplaced.ItemColor = prof.ProfileColumns["Status"].ReplacedColor;
			itemStatusReplacePending.ItemColor = prof.ProfileColumns["Status"].ReplacePendingColor;
			itemStatusReplaceRejected.ItemColor = prof.ProfileColumns["Status"].ReplaceRejectedColor;
			itemStatusStopped.ItemColor = prof.ProfileColumns["Status"].StoppedColor;

			itemSideBuy.ItemColor = prof.ProfileColumns["Side"].BuyColor;
			itemSideSell.ItemColor = prof.ProfileColumns["Side"].SellColor;
			itemSideSellShort.ItemColor = prof.ProfileColumns["Side"].SellShortColor;

			cboRowType.SelectedIndex = (int)prof.GridRowColorType;

			itemFontText.ItemFont = prof.GridFont;
		}

		public ROCOrdersProfile GetProfile(ROCOrdersProfile prof)
		{
			prof.GridSelectedBackColor = itemColorSelectedBackground.ItemColor;
			prof.GridBackColor = itemColorBackground.ItemColor;
			prof.GridColor = itemColorGridlines.ItemColor;
			prof.GridForeColor = itemColorText.ItemColor;

			prof.ProfileColumns["OrderID"].ColumnForeColor = itemColumnOrderID.ItemColor;
			prof.ProfileColumns["Symbol"].ColumnForeColor = itemColumnSymbol.ItemColor;
			prof.ProfileColumns["Qty"].ColumnForeColor = itemColumnQty.ItemColor;
			prof.ProfileColumns["Leave"].ColumnForeColor = itemColumnLeaveQty.ItemColor;
			prof.ProfileColumns["Price"].ColumnForeColor = itemColumnPrice.ItemColor;
			prof.ProfileColumns["Filled"].ColumnForeColor = itemColumnFilledQty.ItemColor;
			prof.ProfileColumns["AvgPrice"].ColumnForeColor = itemColumnAvgPrice.ItemColor;
			prof.ProfileColumns["Type"].ColumnForeColor = itemColumnType.ItemColor;
			prof.ProfileColumns["Duration"].ColumnForeColor = itemColumnDuration.ItemColor;
			prof.ProfileColumns["Exchange"].ColumnForeColor = itemColumnExchange.ItemColor;
			prof.ProfileColumns["Time"].ColumnForeColor = itemColumnTime.ItemColor;
			prof.ProfileColumns["Account"].ColumnForeColor = itemColumnAccount.ItemColor;

			prof.ProfileColumns["Status"].BustedColor = itemStatusBusted.ItemColor;
			prof.ProfileColumns["Status"].CalculatedColor = itemStatusCalculated.ItemColor;
			prof.ProfileColumns["Status"].CancelledColor = itemStatusCancelled.ItemColor;
			prof.ProfileColumns["Status"].CancelRejectedColor = itemStatusCancelRejected.ItemColor;
			prof.ProfileColumns["Status"].CorrectedColor = itemStatusCorrected.ItemColor;
			prof.ProfileColumns["Status"].DoneForDayColor = itemStatusDoneForDay.ItemColor;
			prof.ProfileColumns["Status"].ExpiredColor = itemStatusExpired.ItemColor;
			prof.ProfileColumns["Status"].FilledColor = itemStatusFilled.ItemColor;
			prof.ProfileColumns["Status"].OpenColor = itemStatusOpen.ItemColor;
			prof.ProfileColumns["Status"].OtherColor = itemStatusOther.ItemColor;
			prof.ProfileColumns["Status"].PartialFilledColor = itemStatusPartialFilled.ItemColor;
			prof.ProfileColumns["Status"].PendingCancelColor = itemStatusPendingCancel.ItemColor;
			prof.ProfileColumns["Status"].RejectedColor = itemStatusRejected.ItemColor;
			prof.ProfileColumns["Status"].ReplacedColor = itemStatusReplaced.ItemColor;
			prof.ProfileColumns["Status"].ReplacePendingColor = itemStatusReplacePending.ItemColor;
			prof.ProfileColumns["Status"].ReplaceRejectedColor = itemStatusReplaceRejected.ItemColor;
			prof.ProfileColumns["Status"].StoppedColor = itemStatusStopped.ItemColor;

			prof.ProfileColumns["Side"].BuyColor = itemSideBuy.ItemColor;
			prof.ProfileColumns["Side"].SellColor = itemSideSell.ItemColor;
			prof.ProfileColumns["Side"].SellShortColor = itemSideSellShort.ItemColor;

			switch (cboRowType.SelectedIndex)
			{
				case 0:
					prof.GridRowColorType = ROCOrderList.RowColorTypes.None;
					break;
				case 1:
					prof.GridRowColorType = ROCOrderList.RowColorTypes.Status;
					break;
				case 2:
					prof.GridRowColorType = ROCOrderList.RowColorTypes.Side;
					break;
				default:
					prof.GridRowColorType = ROCOrderList.RowColorTypes.Status;
					break;
			}

			prof.GridFont = itemFontText.ItemFont;

			return prof;
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void cmdApply_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Apply"));
			Close();
		}

		private void cmdApplyToAll_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("ApplyToAll"));
			Close();
		}

		private void cboRowType_SelectedIndexChanged(object sender, EventArgs e)
		{
			switch (cboRowType.SelectedIndex)
			{
				case 0:	// None
					grpColumn.Enabled = true;
					break;
				default:
					grpColumn.Enabled = false;
					break;
			}
		}

		private void cmdDefaultColor_Click(object sender, EventArgs e)
		{
			itemColumnOrderID.ItemColor = itemColorText.ItemColor;
			itemColumnSymbol.ItemColor = itemColorText.ItemColor;
			itemColumnQty.ItemColor = itemColorText.ItemColor;
			itemColumnLeaveQty.ItemColor = itemColorText.ItemColor;
			itemColumnPrice.ItemColor = itemColorText.ItemColor;
			itemColumnFilledQty.ItemColor = itemColorText.ItemColor;
			itemColumnAvgPrice.ItemColor = itemColorText.ItemColor;
			itemColumnType.ItemColor = itemColorText.ItemColor;
			itemColumnDuration.ItemColor = itemColorText.ItemColor;
			itemColumnExchange.ItemColor = itemColorText.ItemColor;
			itemColumnTime.ItemColor = itemColorText.ItemColor;
			itemColumnAccount.ItemColor = itemColorText.ItemColor;

		}

		#region - Show -

		public void Display(Control control)
		{
			Display(control, control.ClientRectangle);
		}
		public void Display(Form f, Point p)
		{
			Display(f, new Rectangle(p, new Size(0, 0)));
		}
		private void Display(Control control, Rectangle area)
		{
			if (control == null)
				throw new ArgumentNullException("control can not be null");

			Point location = control.PointToScreen(new Point(area.Left, area.Top + area.Height));

			Rectangle screen = Screen.FromControl(control).WorkingArea;

			if (location.X + Size.Width > (screen.Left + screen.Width))
				location.X = (screen.Left + screen.Width) - Size.Width;

			if (location.Y + Size.Height > (screen.Top + screen.Height))
				location.Y -= Size.Height + area.Height;

			//location = control.PointToClient(location);

			Location = location;
			ShowDialog(control);
		}

		#endregion
	}
}
