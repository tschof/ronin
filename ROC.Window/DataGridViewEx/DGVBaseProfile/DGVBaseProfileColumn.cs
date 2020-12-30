using System;
using System.Drawing;

using DictionaryEx;

namespace DataGridViewEx
{
	[Serializable]
	public class DGVColumnProfile
	{
		private MultiTypedUIDictionary _data = new MultiTypedUIDictionary();

		public bool IsFraction
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.IsFraction, out bool value) ? value : false;
			set => _data.Set(DGVColumnProfileFieldID.Column.IsFraction, value);
		}

		public bool IsNotified
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.IsNotified, out bool value) ? value : false;
			set => _data.Set(DGVColumnProfileFieldID.Column.IsNotified, value);
		}

		public bool IsPandN
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.IsPandN, out bool value) ? value : false;
			set => _data.Set(DGVColumnProfileFieldID.Column.IsPandN, value);
		}

		public Color ColumnForeColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.ForeColor, out Color value) ? value : Color.White;
			set => _data.Set(DGVColumnProfileFieldID.Column.ForeColor, value);
		}

		public Color ColumnBackColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.BackColor, out Color value) ? value : Color.Black;
			set => _data.Set(DGVColumnProfileFieldID.Column.BackColor, value);
		}

		public Font ColumnFont
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.Font, out Font value) ? value : new Font("Microsoft Sans Serif", 9.00F, FontStyle.Regular);
			set => _data.Set(DGVColumnProfileFieldID.Column.Font, value);
		}

		public Color HeaderForeColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.HeaderForeColor, out Color value) ? value : Color.Black;
			set => _data.Set(DGVColumnProfileFieldID.Column.HeaderForeColor, value);
		}

		public Color HeaderBackColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.HeaderBackColor, out Color value) ? value : Color.Gray;
			set => _data.Set(DGVColumnProfileFieldID.Column.HeaderBackColor, value);
		}

		public Font HeaderFont
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.HeaderFont, out Font value) ? value : new Font("Microsoft Sans Serif", 9.00F, System.Drawing.FontStyle.Regular);
			set => _data.Set(DGVColumnProfileFieldID.Column.HeaderFont, value);
		}

		public int ColumnDisplayIndex
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.DisplayIndex, out long value) ? (int)value : 0;
			set => _data.Set(DGVColumnProfileFieldID.Column.DisplayIndex, value);
		}

		public string ColumnName
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.Name, out string value) ? value : "";
			set => _data.Set(DGVColumnProfileFieldID.Column.Name, value);
		}

		public bool ColumnVisiable
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.Visible, out bool value) ? value : true;
			set => _data.Set(DGVColumnProfileFieldID.Column.Visible, value);
		}

		public int ColumnWidth
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.Width, out long value) ? (int)value : 25;
			set => _data.Set(DGVColumnProfileFieldID.Column.Width, value);
		}

		public Color SelectedBackColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Column.Width, out Color value) ? value : Color.Blue;
			set => _data.Set(DGVColumnProfileFieldID.Column.Width, value);
		}

		#region - Notify -

		public NotifyTypes NotifyType
		{
			get
			{
				if (_data.TryGet(DGVColumnProfileFieldID.Notify.NotifyType, out long value))
				{
					switch (value)
					{
						case 0:
							return NotifyTypes.None;
						case 2:
							return NotifyTypes.BackGround;
						case 3:
							return NotifyTypes.ForeGround;
						case 1:
						default:
							return NotifyTypes.Border;
					}
				}
				else
				{
					return NotifyTypes.Border;
				}
			}
			set
			{
				int ival;
				switch (value)
				{
					case NotifyTypes.Border: ival = 1; break;
					case NotifyTypes.BackGround: ival = 2; break;
					case NotifyTypes.ForeGround: ival = 3; break;
					case NotifyTypes.None:
					default:
						ival = 0;
						break;
				}
				_data.Set(DGVColumnProfileFieldID.Notify.NotifyType, ival);
			}
		}

		public Color UpColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Notify.UpColor, out Color value) ? value : Color.LimeGreen;
			set => _data.Set(DGVColumnProfileFieldID.Notify.UpColor, value);
		}

		public float UpBorderWidth
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Notify.UpColor, out double value) ? (float)value : 1.5f;
		}

		public Color DownColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Notify.DownColor, out Color value) ? value : Color.Red;
			set => _data.Set(DGVColumnProfileFieldID.Notify.DownColor, value);
		}

		public float DownBorderWidth
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Notify.DownBorderWidth, out double value) ? (float)value : 1.5f;
		}

		public Color DefaultColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.Notify.DefaultColor, out Color value) ? value : Color.White;
			set => _data.Set(DGVColumnProfileFieldID.Notify.DefaultColor, value);
		}

		#endregion

		#region - Order Status Color -

		public Color PartialFilledColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.PartialFilledColor, out Color value) ? value : Color.LightBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.PartialFilledColor, value);
		}

		public Color FilledColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.FilledColor, out Color value) ? value : Color.LimeGreen;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.FilledColor, value);
		}

		public Color DoneForDayColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.DoneForDayColor, out Color value) ? value : Color.CadetBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.DoneForDayColor, value);
		}

		public Color CancelledColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.CancelledColor, out Color value) ? value : Color.OrangeRed;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.CancelledColor, value);
		}

		public Color ReplacedColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.ReplacedColor, out Color value) ? value : Color.LightBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.ReplacedColor, value);
		}

		public Color PendingCancelColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.PendingCancelColor, out Color value) ? value : Color.LightBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.PendingCancelColor, value);
		}

		public Color StoppedColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.StoppedColor, out Color value) ? value : Color.CadetBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.StoppedColor, value);
		}

		public Color RejectedColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.RejectedColor, out Color value) ? value : Color.Red;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.RejectedColor, value);
		}

		public Color CalculatedColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.CalculatedColor, out Color value) ? value : Color.CadetBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.CalculatedColor, value);
		}

		public Color ExpiredColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.ExpiredColor, out Color value) ? value : Color.CadetBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.ExpiredColor, value);
		}

		public Color OpenColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.OpenColor, out Color value) ? value : Color.LightBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.OpenColor, value);
		}

		public Color CancelRejectedColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.CancelRejectedColor, out Color value) ? value : Color.LightBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.CancelRejectedColor, value);
		}

		public Color CorrectedColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.CorrectedColor, out Color value) ? value : Color.LightBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.CorrectedColor, value);
		}

		public Color BustedColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.BustedColor, out Color value) ? value : Color.Red;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.BustedColor, value);
		}

		public Color ReplaceRejectedColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.ReplaceRejectedColor, out Color value) ? value : Color.LightBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.ReplaceRejectedColor, value);
		}

		public Color ReplacePendingColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.ReplacePendingColor, out Color value) ? value : Color.LightBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.ReplacePendingColor, value);
		}

		public Color OtherColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderStatus.OtherColor, out Color value) ? value : Color.CadetBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderStatus.OtherColor, value);
		}

		#endregion

		#region - Order Side Color -

		public Color SellColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderSide.SellColor, out Color value) ? value : Color.DarkRed;
			set => _data.Set(DGVColumnProfileFieldID.OrderSide.SellColor, value);
		}

		public Color BuyColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderSide.BuyColor, out Color value) ? value : Color.DarkGreen;
			set => _data.Set(DGVColumnProfileFieldID.OrderSide.BuyColor, value);
		}

		public Color SellShortColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderSide.SellShortColor, out Color value) ? value : Color.DarkOrange;
			set => _data.Set(DGVColumnProfileFieldID.OrderSide.SellShortColor, value);
		}

		public Color SSEColor
		{
			get => _data.TryGet(DGVColumnProfileFieldID.OrderSide.SSEColor, out Color value) ? value : Color.DarkSlateBlue;
			set => _data.Set(DGVColumnProfileFieldID.OrderSide.SSEColor, value);
		}

		#endregion
	}
}
