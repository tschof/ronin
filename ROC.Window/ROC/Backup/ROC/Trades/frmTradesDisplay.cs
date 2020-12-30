using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

using FormEx;
using DataGridViewEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmTradesDisplay : VistaWindowBorderless, INotifyPropertyChanged
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

		public frmTradesDisplay(ROCTradeProfile prof)
		{
			InitializeComponent();

			itemColorSelectedBackground.ItemColor = prof.GridSelectedBackColor;
			itemColorBackground.ItemColor = prof.GridBackColor;
			itemColorGridlines.ItemColor = prof.GridColor;
			itemColorText.ItemColor = prof.GridForeColor;

			itemColumnExecID.ItemColor = prof.ProfileColumns["ExecID"].ColumnForeColor;
			itemColumnSymbol.ItemColor = prof.ProfileColumns["SymbolDetail"].ColumnForeColor;
			itemColumnDisplay.ItemColor = prof.ProfileColumns["SymbolDisplay"].ColumnForeColor;
			itemColumnQty.ItemColor = prof.ProfileColumns["Qty"].ColumnForeColor;
			itemColumnPrice.ItemColor = prof.ProfileColumns["Price"].ColumnForeColor;
			itemColumnValue.ItemColor = prof.ProfileColumns["Value"].ColumnForeColor;
			itemColumnExchange.ItemColor = prof.ProfileColumns["Exchange"].ColumnForeColor;
			itemColumnTime.ItemColor = prof.ProfileColumns["Time"].ColumnForeColor;
			itemColumnAccount.ItemColor = prof.ProfileColumns["Account"].ColumnForeColor;

			itemSideBuy.ItemColor = prof.ProfileColumns["Side"].BuyColor;
			itemSideSell.ItemColor = prof.ProfileColumns["Side"].SellColor;
			itemSideSellShort.ItemColor = prof.ProfileColumns["Side"].SellShortColor;

			cboRowType.SelectedIndex = (int)prof.GridRowColorType;

			itemFontText.ItemFont = prof.GridFont;
		}

		public ROCTradeProfile GetProfile(ROCTradeProfile prof)
		{
			prof.GridSelectedBackColor = itemColorSelectedBackground.ItemColor;
			prof.GridBackColor = itemColorBackground.ItemColor;
			prof.GridColor = itemColorGridlines.ItemColor;
			prof.GridForeColor = itemColorText.ItemColor;

			prof.ProfileColumns["ExecID"].ColumnForeColor = itemColumnExecID.ItemColor;
			prof.ProfileColumns["SymbolDetail"].ColumnForeColor = itemColumnSymbol.ItemColor;
			prof.ProfileColumns["SymbolDisplay"].ColumnForeColor = itemColumnDisplay.ItemColor;
			prof.ProfileColumns["Qty"].ColumnForeColor = itemColumnQty.ItemColor;
			prof.ProfileColumns["Price"].ColumnForeColor = itemColumnPrice.ItemColor;
			prof.ProfileColumns["Value"].ColumnForeColor = itemColumnValue.ItemColor;
			prof.ProfileColumns["Exchange"].ColumnForeColor = itemColumnExchange.ItemColor;
			prof.ProfileColumns["Time"].ColumnForeColor = itemColumnTime.ItemColor;
			prof.ProfileColumns["Account"].ColumnForeColor = itemColumnAccount.ItemColor;

			prof.ProfileColumns["Side"].BuyColor = itemSideBuy.ItemColor;
			prof.ProfileColumns["Side"].SellColor = itemSideSell.ItemColor;
			prof.ProfileColumns["Side"].SellShortColor = itemSideSellShort.ItemColor;

			switch (cboRowType.SelectedIndex)
			{
				case 0:
					prof.GridRowColorType = ROCTradeList.RowColorTypes.None;
					break;
				case 1:
					prof.GridRowColorType = ROCTradeList.RowColorTypes.Side;
					break;
				default:
					prof.GridRowColorType = ROCTradeList.RowColorTypes.None;
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
			itemColumnExecID.ItemColor = itemColorText.ItemColor;
			itemColumnSymbol.ItemColor = itemColorText.ItemColor;
			itemColumnQty.ItemColor = itemColorText.ItemColor;
			itemColumnPrice.ItemColor = itemColorText.ItemColor;
			itemColumnValue.ItemColor = itemColorText.ItemColor;
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