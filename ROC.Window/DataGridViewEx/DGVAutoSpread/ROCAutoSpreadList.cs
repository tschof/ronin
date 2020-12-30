using System;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;

using Price = Common.Price;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCAutoSpreadList : ROCQuickList
	{
		#region - Event -

		[Description("Event raised when the value of the Refeshing Ration property is changed."), Category("Property Changed")]
		public event EventHandler RefreshingLadder;

		#endregion

		#region - OnEvent -

		protected virtual void OnRefreshingLadder(EventArgs e)
		{
			if (RefreshingLadder != null)
			{
				RefreshingLadder(this, e);
			}
		}

		#endregion

		#region - Properties -

		private bool _refreshLadder = false;
		[Browsable(false)]
		public bool RefreshLadder
		{
			get
			{
				return _refreshLadder;
			}
			set
			{
				_refreshLadder = value;
				if (_refreshLadder)
				{
					OnRefreshingLadder(EventArgs.Empty);
				}
			}
		}

		public Nullable<double> TopOfBookAskPrice
		{
			get
			{
				try
				{
					DataRow[] rows = RocGridTable.Select("AskSize > 0", "Price DESC");
					if (rows.Length > 0)
					{
						return (double)rows[rows.Length - 1]["Price"];
					}
				}
				catch (Exception ex)
				{
					Debug.Print(ex.Message);
				}
				return null;
			}
		}

		public Nullable<double> TopOfBookBidPrice
		{
			get
			{
				try
				{
					DataRow[] rows = RocGridTable.Select("BidSize > 0", "Price DESC");
					if (rows.Length > 0)
					{
						return (double)rows[0]["Price"];
					}
				}
				catch (Exception ex)
				{
					Debug.Print(ex.Message);
				}
				return null;
			}
		}

		#endregion

		#region - Auto Spread Function -

		public override void SetBookSize(Price price, long size, string side)
		{
			DataRowView[] rows = new DataRowView[0];

			lock (RocGridTable)
			{
				if (RocGridTable.Rows.Count > 0)
				{
					rows = PriceSearchView.FindRows(price);

					if (rows.Length > 0)
					{
						if (size == 0)
						{
							rows[0][side + "Size"] = DBNull.Value;
						}
						else
						{
							rows[0][side + "Size"] = size;
						}
					}

					SetMaxBookSize(size, side);

					switch (side)
					{
						case "Bid":
							if (!bidBookPrices.Contains(price))
							{
								bidBookPrices.Add(price);
							}
							break;
						case "Ask":
							if (!askBookPrices.Contains(price))
							{
								askBookPrices.Add(price);
							}
							break;
					}
				}
			}
		}

		public void FlagAskPriceFromSpredPrice(Price oldPrice, Price newPrice)
		{
			FlagAskPrice(oldPrice, CalTickPriceFromSpreadPrice(newPrice, "Ask"));
		}

		public void FlagBidPriceFromSpredPrice(Price oldPrice, Price newPrice)
		{
			FlagBidPrice(oldPrice, CalTickPriceFromSpreadPrice(newPrice, "Bid"));
		}

		public void FlagTradedPriceFromSpredPrice(Price oldPrice, Price newPrice, string tick)
		{
			if (tick == "+")
			{
				FlagTradePrice(oldPrice, CalTickPriceFromSpreadPrice(newPrice, "Ask"), tick);
			}
			else
			{
				FlagTradePrice(oldPrice, CalTickPriceFromSpreadPrice(newPrice, "Bid"), tick);
			}
		}

		public Price CalTickPriceFromSpreadPrice(Price price, string side)
		{
			Price result = price;

			switch (side)
			{
				case "Bid":
					price = RoundDown(price);
					break;
				case "Ask":
					price = RoundUp(price);
					break;
			}

			if (RocGridTable.Rows.Count > 0) {
				Price startingPrice = (double)RocGridTable.Rows[0]["Price"];
				if (startingPrice > price) {
					int rowIndex = Convert.ToInt32((startingPrice - price).Value / TickSize);

					result = startingPrice - (rowIndex * TickSize);
				}
			}

			return result.Round(2);
		}

		private Price RoundDown(Price price)
		{
			Price roundedValue = price.Round(2);
			return (roundedValue > price) ? (roundedValue - TickSize) : roundedValue;
		}

		private Price RoundUp(Price price)
		{
			Price roundedValue = price.Round(2);
			return (roundedValue < price) ? (roundedValue + TickSize) : roundedValue;
		}

		#endregion
	}
}
