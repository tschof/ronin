using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVBaseStatusColumn : DataGridViewColumn
	{		
		private Color _partialFilledColor = Color.LightBlue;
		public Color PartialFilledColor
		{
			get
			{
				return _partialFilledColor;
			}
			set
			{
				_partialFilledColor = value;
			}
		}

		private Color _filledColor = Color.LimeGreen;
		public Color FilledColor
		{
			get
			{
				return _filledColor;
			}
			set
			{
				_filledColor = value;
			}
		}

		private Color _doneForDayColor = Color.CadetBlue;
		public Color DoneForDayColor
		{
			get
			{
				return _doneForDayColor;
			}
			set
			{
				_doneForDayColor = value;
			}
		}

		private Color _cancelledColor = Color.OrangeRed;
		public Color CancelledColor
		{
			get
			{
				return _cancelledColor;
			}
			set
			{
				_cancelledColor = value;
			}
		}

		private Color _replacedColor = Color.LightBlue;
		public Color ReplacedColor
		{
			get
			{
				return _replacedColor;
			}
			set
			{
				_replacedColor = value;
			}
		}

		private Color _pendingCancelColor = Color.LightBlue;
		public Color PendingCancelColor
		{
			get
			{
				return _pendingCancelColor;
			}
			set
			{
				_pendingCancelColor = value;
			}
		}

		private Color _stoppedColor = Color.CadetBlue;
		public Color StoppedColor
		{
			get
			{
				return _stoppedColor;
			}
			set
			{
				_stoppedColor = value;
			}
		}

		private Color _rejectedColor = Color.Red;
		public Color RejectedColor
		{
			get
			{
				return _rejectedColor;
			}
			set
			{
				_rejectedColor = value;
			}
		}

		private Color _calculatedColor = Color.CadetBlue;
		public Color CalculatedColor
		{
			get
			{
				return _calculatedColor;
			}
			set
			{
				_calculatedColor = value;
			}
		}

		private Color _expiredColor = Color.CadetBlue;
		public Color ExpiredColor
		{
			get
			{
				return _expiredColor;
			}
			set
			{
				_expiredColor = value;
			}
		}

		private Color _openColor = Color.LightBlue;
		public Color OpenColor
		{
			get
			{
				return _openColor;
			}
			set
			{
				_openColor = value;
			}
		}

		private Color _cancelRejectedColor = Color.LightBlue;
		public Color CancelRejectedColor
		{
			get
			{
				return _cancelRejectedColor;
			}
			set
			{
				_cancelRejectedColor = value;
			}
		}

		private Color _correctedColor = Color.LightBlue;
		public Color CorrectedColor
		{
			get
			{
				return _correctedColor;
			}
			set
			{
				_correctedColor = value;
			}
		}

		private Color _bustedColor = Color.Red;
		public Color BustedColor
		{
			get
			{
				return _bustedColor;
			}
			set
			{
				_bustedColor = value;
			}
		}

		private Color _replaceRejectedColor = Color.LightBlue;
		public Color ReplaceRejectedColor
		{
			get
			{
				return _replaceRejectedColor;
			}
			set
			{
				_replaceRejectedColor = value;
			}
		}

		private Color _replacePendingColor = Color.LightBlue;
		public Color ReplacePendingColor
		{
			get
			{
				return _replacePendingColor;
			}
			set
			{
				_replacePendingColor = value;
			}
		}

		private Color _otherColor = Color.CadetBlue;
		public Color OtherColor
		{
			get
			{
				return _otherColor;
			}
			set
			{
				_otherColor = value;
			}
		}

		public DGVBaseStatusColumn()
		{
			base.CellTemplate = new DGVBaseStatusCell();
		}
	}
}
