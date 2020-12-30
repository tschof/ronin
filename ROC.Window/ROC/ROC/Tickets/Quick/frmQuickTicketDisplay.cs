using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using FormEx;
using DataGridViewEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	internal partial class frmQuickTicketDisplay : VistaWindowBorderless, INotifyPropertyChanged
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

		internal frmQuickTicketDisplay(ROCQuickListProfile grid, ROCTicketProfile ticket)
		{
			InitializeComponent();

			itemColorGridlines.ItemColor = grid.GridColor;

			itemColumnForeBuyOpenQty.ItemColor = grid.ProfileColumns["BuyOpenQty"].ColumnForeColor;
			itemColumnForeBidQty.ItemColor = grid.ProfileColumns["BidQty"].ColumnForeColor;
			itemColumnForeBidSize.ItemColor = grid.ProfileColumns["BidSize"].ColumnForeColor;
			itemColumnForePrice.ItemColor = grid.ProfileColumns["Price"].ColumnForeColor;
			itemColumnForeAskSize.ItemColor = grid.ProfileColumns["AskSize"].ColumnForeColor;
			itemColumnForeAskQty.ItemColor = grid.ProfileColumns["AskQty"].ColumnForeColor;
			itemColumnForeSellOpenQty.ItemColor = grid.ProfileColumns["SellOpenQty"].ColumnForeColor;

			itemColumnBackBuyOpenQty.ItemColor = grid.ProfileColumns["BuyOpenQty"].ColumnBackColor;
			itemColumnBackBidQty.ItemColor = grid.ProfileColumns["BidQty"].ColumnBackColor;
			itemColumnBackBidSize.ItemColor = grid.ProfileColumns["BidSize"].ColumnBackColor;
			itemColumnBackPrice.ItemColor = grid.ProfileColumns["Price"].ColumnBackColor;
			itemColumnBackAskSize.ItemColor = grid.ProfileColumns["AskSize"].ColumnBackColor;
			itemColumnBackAskQty.ItemColor = grid.ProfileColumns["AskQty"].ColumnBackColor;
			itemColumnBackSellOpenQty.ItemColor = grid.ProfileColumns["SellOpenQty"].ColumnBackColor;

			itemFontText.ItemFont = grid.GridFont;

			if (ticket.TryGet(ROCTicketProfileFieldID.BackColor, out Color color))
				itemColorTicketBackColor.ItemColor = color;
			if (ticket.TryGet(ROCTicketProfileFieldID.Opacity, out double opacity))
				itemOpacity.Value = Convert.ToInt32(opacity * 100);
			itemColorSelectedStopLimitBackColor.ItemColor = grid.SelectedStopLimitBackColor;

			chkAutoCenter.Checked = grid.GridAutoCenterOnTraded;
			numAutoCenterRange.Value = grid.GridAutoCenterRange;

			bool bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowBidPrice, out bval))
				chkShowBidPrice.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowBidSize, out bval))
				chkShowBidSize.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowAskPrice, out bval))
				chkShowAskPrice.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowAskSize, out bval))
				chkShowAskSize.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowNetChange, out bval))
				chkShowNetChange.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowPctChange, out bval))
				chkShowPctChange.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowTotalVolume, out bval))
				chkShowTotalVolume.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowTradedVolume, out bval))
				chkShowTradedVolume.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowLowPrice, out bval))
				chkShowLowPrice.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowHighPrice, out bval))
				chkShowHighPrice.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowPrevClosePrice, out bval))
				chkShowPrevClosePrice.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowQty, out bval))
				chkShowQty.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowOrder, out bval))
				chkShowOrderType.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowLimitPrice, out bval))
				chkShowLimitPrice.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowStopPrice, out bval))
				chkShowStopPrice.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowDuration, out bval))
				chkShowDuration.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowExchange, out bval))
				chkShowExchange.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowOrderInfo, out bval))
				chkShowOrderInfo.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowCommand, out bval))
				chkShowCommand.Checked = bval;
			if (ticket.TryGet(ROCTicketProfileFieldID.ShowSelectedAccountOnly, out bval))
				chkShowSelectedAccountOnly.Checked = bval;
		}

		internal ROCQuickListProfile GetProfile(ROCQuickListProfile prof)
		{
			prof.GridColor = itemColorGridlines.ItemColor;

			prof.ProfileColumns["BuyOpenQty"].ColumnForeColor = itemColumnForeBuyOpenQty.ItemColor;
			prof.ProfileColumns["BidQty"].ColumnForeColor = itemColumnForeBidQty.ItemColor;
			prof.ProfileColumns["BidSize"].ColumnForeColor = itemColumnForeBidSize.ItemColor;
			prof.ProfileColumns["Price"].ColumnForeColor = itemColumnForePrice.ItemColor;
			prof.ProfileColumns["AskSize"].ColumnForeColor = itemColumnForeAskSize.ItemColor;
			prof.ProfileColumns["AskQty"].ColumnForeColor = itemColumnForeAskQty.ItemColor;
			prof.ProfileColumns["SellOpenQty"].ColumnForeColor = itemColumnForeSellOpenQty.ItemColor;

			prof.ProfileColumns["BuyOpenQty"].ColumnBackColor = itemColumnBackBuyOpenQty.ItemColor;
			prof.ProfileColumns["BidQty"].ColumnBackColor = itemColumnBackBidQty.ItemColor;
			prof.ProfileColumns["BidSize"].ColumnBackColor = itemColumnBackBidSize.ItemColor;
			prof.ProfileColumns["Price"].ColumnBackColor = itemColumnBackPrice.ItemColor;
			prof.ProfileColumns["AskSize"].ColumnBackColor = itemColumnBackAskSize.ItemColor;
			prof.ProfileColumns["AskQty"].ColumnBackColor = itemColumnBackAskQty.ItemColor;
			prof.ProfileColumns["SellOpenQty"].ColumnBackColor = itemColumnBackSellOpenQty.ItemColor;

			prof.GridFont = itemFontText.ItemFont;

			prof.SelectedStopLimitBackColor = itemColorSelectedStopLimitBackColor.ItemColor;

			prof.GridAutoCenterOnTraded = chkAutoCenter.Checked;
			prof.GridAutoCenterRange = (long)numAutoCenterRange.Value;

			return prof;
		}

		internal ROCTicketProfile GetProfile(ROCTicketProfile prof)
		{
			prof.Set(ROCTicketProfileFieldID.Opacity, itemOpacity.Value / 100F);
			prof.Set(ROCTicketProfileFieldID.BackColor, itemColorTicketBackColor.ItemColor);
			prof.Set(ROCTicketProfileFieldID.ShowBidPrice, chkShowBidPrice.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowBidSize, chkShowBidSize.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowAskPrice, chkShowAskPrice.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowAskSize, chkShowAskSize.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowNetChange, chkShowNetChange.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowPctChange, chkShowPctChange.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowTotalVolume, chkShowTotalVolume.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowTradedVolume, chkShowTradedVolume.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowLowPrice, chkShowLowPrice.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowHighPrice, chkShowHighPrice.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowPrevClosePrice, chkShowPrevClosePrice.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowQty, chkShowQty.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowOrder, chkShowOrderType.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowLimitPrice, chkShowLimitPrice.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowStopPrice, chkShowStopPrice.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowDuration, chkShowDuration.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowExchange, chkShowExchange.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowOrderInfo, chkShowOrderInfo.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowCommand, chkShowCommand.Checked);
			prof.Set(ROCTicketProfileFieldID.ShowSelectedAccountOnly, chkShowSelectedAccountOnly.Checked);

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

		private void cmdDefaultForeColor_Click(object sender, EventArgs e)
		{
			itemColumnForeBuyOpenQty.ItemColor = Color.LimeGreen;
			itemColumnForeBidQty.ItemColor = Color.White;
			itemColumnForeBidSize.ItemColor = Color.LimeGreen;
			itemColumnForePrice.ItemColor = Color.Black;
			itemColumnForeAskSize.ItemColor = Color.Red;
			itemColumnForeAskQty.ItemColor = Color.White;
			itemColumnForeSellOpenQty.ItemColor = Color.Red;
		}

		private void cmdDefaultBackColor_Click(object sender, EventArgs e)
		{
			itemColumnBackBuyOpenQty.ItemColor = Color.Black;
			itemColumnBackBidQty.ItemColor = Color.LimeGreen;
			itemColumnBackBidSize.ItemColor = Color.Black;
			itemColumnBackPrice.ItemColor = Color.WhiteSmoke;
			itemColumnBackAskSize.ItemColor = Color.Black;
			itemColumnBackAskQty.ItemColor = Color.Red;
			itemColumnBackSellOpenQty.ItemColor = Color.Black;
		}

		private void cmdDefaultGridColor_Click(object sender, EventArgs e)
		{
			itemColorGridlines.ItemColor = Color.WhiteSmoke;
		}

		private void cmdDefaultTicketColor_Click(object sender, EventArgs e)
		{
			itemOpacity.Value = 100;
			itemColorTicketBackColor.ItemColor = Color.LightSteelBlue;
		}

		#region - Show -

		internal void Display(Control control)
		{
			Display(control, control.ClientRectangle);
		}
		internal void Display(Form f, Point p)
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