using System;
using System.Drawing;

using DictionaryEx;

namespace DataGridViewEx
{
	[Serializable]
	public class DGVColumnProfile : MutiTypedUIDictionary
	{
		public bool IsFraction
		{
			get
			{
				if (Bools.ContainsKey(DGVColumnProfileFieldID.Column.IsFraction))
				{
					return Bools[DGVColumnProfileFieldID.Column.IsFraction];
				}
				else
				{
					return false;
				}
			}
		}

		public bool IsNotified
		{
			get
			{
				if (Bools.ContainsKey(DGVColumnProfileFieldID.Column.IsNotified))
				{
					return Bools[DGVColumnProfileFieldID.Column.IsNotified];
				}
				else
				{
					return false;
				}
			}
		}

		public bool IsPandN
		{
			get
			{
				if (Bools.ContainsKey(DGVColumnProfileFieldID.Column.IsPandN))
				{
					return Bools[DGVColumnProfileFieldID.Column.IsPandN];
				}
				else
				{
					return false;
				}
			}
		}

		public Color ColumnForeColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.Column.ForeColor))
				{
					return Colors[DGVColumnProfileFieldID.Column.ForeColor];
				}
				else
				{
					return Color.White;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.ForeColor, value);
			}
		}

		public Color ColumnBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.Column.BackColor))
				{
					return Colors[DGVColumnProfileFieldID.Column.BackColor];
				}
				else
				{
					return Color.Black;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.BackColor, value);
			}
		}

		public Font ColumnFont
		{
			get
			{
				if (Fonts.ContainsKey(DGVColumnProfileFieldID.Column.Font))
				{
					return Fonts[DGVColumnProfileFieldID.Column.Font];
				}
				else
				{
					return new Font("Microsoft Sans Serif", 9.00F, System.Drawing.FontStyle.Regular);
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.Font, value);
			}
		}

		public Color HeaderForeColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.Column.HeaderForeColor))
				{
					return Colors[DGVColumnProfileFieldID.Column.HeaderForeColor];
				}
				else
				{
					return Color.Black;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.HeaderForeColor, value);
			}
		}

		public Color HeaderBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.Column.HeaderBackColor))
				{
					return Colors[DGVColumnProfileFieldID.Column.HeaderBackColor];
				}
				else
				{
					return Color.Gray;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.HeaderBackColor, value);
			}
		}

		public Font HeaderFont
		{
			get
			{
				if (Fonts.ContainsKey(DGVColumnProfileFieldID.Column.HeaderFont))
				{
					return Fonts[DGVColumnProfileFieldID.Column.HeaderFont];
				}
				else
				{
					return new Font("Microsoft Sans Serif", 9.00F, System.Drawing.FontStyle.Regular);
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.HeaderFont, value);
			}
		}

		public int ColumnDisplayIndex
		{
			get
			{
				if (Longs.ContainsKey(DGVColumnProfileFieldID.Column.DisplayIndex))
				{
					return (int)Longs[DGVColumnProfileFieldID.Column.DisplayIndex];
				}
				else
				{
					return 0;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.DisplayIndex, value);
			}
		}

		public string ColumnName
		{
			get
			{
				if (Strings.ContainsKey(DGVColumnProfileFieldID.Column.Name))
				{
					return Strings[DGVColumnProfileFieldID.Column.Name];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.Name, value);
			}
		}

		public bool ColumnVisiable
		{
			get
			{
				if (Bools.ContainsKey(DGVColumnProfileFieldID.Column.Visiable))
				{
					return Bools[DGVColumnProfileFieldID.Column.Visiable];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.Visiable, value);
			}
		}

		public int ColumnWidth
		{
			get
			{
				if (Longs.ContainsKey(DGVColumnProfileFieldID.Column.Width))
				{
					return (int)Longs[DGVColumnProfileFieldID.Column.Width];
				}
				else
				{
					return 25;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.Width, value);
			}
		}

		public Color SelectedBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.Column.SelectedBackColor))
				{
					return Colors[DGVColumnProfileFieldID.Column.SelectedBackColor];
				}
				else
				{
					return Color.Blue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Column.SelectedBackColor, value);
			}
		}

		#region - Notify -

		public NotifyTypes NotifyType
		{
			get
			{
				if (Longs.ContainsKey(DGVColumnProfileFieldID.Notify.NotifyType))
				{
					switch (Longs[DGVColumnProfileFieldID.Notify.NotifyType])
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
				switch (value)
				{
					case NotifyTypes.Border:
						Update(DGVColumnProfileFieldID.Notify.NotifyType, 1);
						break;
					case NotifyTypes.BackGround:
						Update(DGVColumnProfileFieldID.Notify.NotifyType, 2);
						break;
					case NotifyTypes.ForeGround:
						Update(DGVColumnProfileFieldID.Notify.NotifyType, 3);
						break;
					case NotifyTypes.None:
					default:
						Update(DGVColumnProfileFieldID.Notify.NotifyType, 0);
						break;
				}
			}
		}

		public Color UpColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.Notify.UpColor))
				{
					return Colors[DGVColumnProfileFieldID.Notify.UpColor];
				}
				else
				{
					return Color.LimeGreen;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Notify.UpColor, value);
			}
		}

		public float UpBorderWidth
		{
			get
			{
				if (Doubles.ContainsKey(DGVColumnProfileFieldID.Notify.UpBorderWidth))
				{
					return (float)Doubles[DGVColumnProfileFieldID.Notify.UpBorderWidth];
				}
				else
				{
					return 1.5f;
				}
			}
		}

		public Color DownColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.Notify.DownColor))
				{
					return Colors[DGVColumnProfileFieldID.Notify.DownColor];
				}
				else
				{
					return Color.Red;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Notify.DownColor, value);
			}
		}

		public float DownBorderWidth
		{
			get
			{
				if (Doubles.ContainsKey(DGVColumnProfileFieldID.Notify.DownBorderWidth))
				{
					return (float)Doubles[DGVColumnProfileFieldID.Notify.DownBorderWidth];
				}
				else
				{
					return 1.5f;
				}
			}
		}

		public Color DefaultColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.Notify.DefaultColor))
				{
					return Colors[DGVColumnProfileFieldID.Notify.DefaultColor];
				}
				else
				{
					return Color.White;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.Notify.DefaultColor, value);
			}
		}

		#endregion

		#region - Order Status Color -

		public Color PartialFilledColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.ParitalFilledColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.ParitalFilledColor];
				}
				else
				{
					return Color.LightBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.ParitalFilledColor, value);
			}
		}

		public Color FilledColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.FilledColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.FilledColor];
				}
				else
				{
					return Color.LimeGreen;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.FilledColor, value);
			}
		}

		public Color DoneForDayColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.DoneForDayColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.DoneForDayColor];
				}
				else
				{
					return Color.CadetBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.DoneForDayColor, value);
			}
		}

		public Color CancelledColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.CancelledColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.CancelledColor];
				}
				else
				{
					return Color.OrangeRed;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.CancelledColor, value);
			}
		}

		public Color ReplacedColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.ReplacedColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.ReplacedColor];
				}
				else
				{
					return Color.LightBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.ReplacedColor, value);
			}
		}

		public Color PendingCancelColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.PendingCancelColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.PendingCancelColor];
				}
				else
				{
					return Color.LightBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.PendingCancelColor, value);
			}
		}

		public Color StoppedColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.StoppedColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.StoppedColor];
				}
				else
				{
					return Color.CadetBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.StoppedColor, value);
			}
		}

		public Color RejectedColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.RejectedColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.RejectedColor];
				}
				else
				{
					return Color.Red;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.RejectedColor, value);
			}
		}

		public Color CalculatedColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.CalculatedColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.CalculatedColor];
				}
				else
				{
					return Color.CadetBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.CalculatedColor, value);
			}
		}

		public Color ExpiredColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.ExpiredColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.ExpiredColor];
				}
				else
				{
					return Color.CadetBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.ExpiredColor, value);
			}
		}

		public Color OpenColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.OpenColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.OpenColor];
				}
				else
				{
					return Color.LightBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.OpenColor, value);
			}
		}

		public Color CancelRejectedColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.CancelRejectedColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.CancelRejectedColor];
				}
				else
				{
					return Color.LightBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.CancelRejectedColor, value);
			}
		}

		public Color CorrectedColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.CorrectedColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.CorrectedColor];
				}
				else
				{
					return Color.LightBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.CorrectedColor, value);
			}
		}

		public Color BustedColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.BustedColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.BustedColor];
				}
				else
				{
					return Color.Red;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.BustedColor, value);
			}
		}

		public Color ReplaceRejectedColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.ReplaceRejectedColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.ReplaceRejectedColor];
				}
				else
				{
					return Color.LightBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.ReplaceRejectedColor, value);
			}
		}

		public Color ReplacePendingColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.ReplacePendingColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.ReplacePendingColor];
				}
				else
				{
					return Color.LightBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.ReplacePendingColor, value);
			}
		}

		public Color OtherColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderStatus.OtherColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderStatus.OtherColor];
				}
				else
				{
					return Color.CadetBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderStatus.OtherColor, value);
			}
		}

		#endregion

		#region - Order Side Color -

		public Color SellColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderSide.SellColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderSide.SellColor];
				}
				else
				{
					return Color.DarkRed;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderSide.SellColor, value);
			}
		}

		public Color BuyColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderSide.BuyColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderSide.BuyColor];
				}
				else
				{
					return Color.DarkGreen;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderSide.BuyColor, value);
			}
		}

		public Color SellShortColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderSide.SellShortColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderSide.SellShortColor];
				}
				else
				{
					return Color.DarkOrange;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderSide.SellShortColor, value);
			}
		}

		public Color SSEColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.OrderSide.SSEColor))
				{
					return Colors[DGVColumnProfileFieldID.OrderSide.SSEColor];
				}
				else
				{
					return Color.DarkSlateBlue;
				}
			}
			set
			{
				Update(DGVColumnProfileFieldID.OrderSide.SSEColor, value);
			}
		}

		#endregion
	}
}
