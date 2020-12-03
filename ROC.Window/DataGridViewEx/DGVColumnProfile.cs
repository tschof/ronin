using System;
using System.Drawing;

namespace DataGridViewEx
{
	[Serializable]
	public class DGVColumnProfile
	{
		private bool _isFraction;
		private bool _isNotified;
		private Color _defaultForeColor;
		private NotifyTypes _notifyType;
		private Color _notifyPositiveColor;
		private float _notifyPositiveBorderWidth;
		private Color _notifyNegativeColor;
		private float _notifyNegativeBorderWidth;
		private Font _columnFont;
		private int _columnDisplayIndex;
		private string _columnName;
		private bool _columnVisiable;
		private int _columnWidth;

		public DGVColumnProfile(
			Color defaultForeColor,
			Font columnFont,
			int columnDisplayIndex,
			string columnName,
			bool columnVisiable,
			int columnWidth)
		{
			Initializ(
				false,
				false,
				defaultForeColor,
				NotifyTypes.None,
				Color.Empty,
				1.5f,
				Color.Empty,
				1.5f,
				columnFont,
				columnDisplayIndex,
				columnName,
				columnVisiable,
				columnWidth);
		}

		public DGVColumnProfile(
			bool isFraction,
			Color defaultForeColor,
			Font columnFont,
			int columnDisplayIndex,
			string columnName,
			bool columnVisiable,
			int columnWidth)
		{
			Initializ(
				isFraction,
				false,
				defaultForeColor,
				NotifyTypes.None,
				Color.Empty,
				1.5f,
				Color.Empty,
				1.5f,
				columnFont,
				columnDisplayIndex,
				columnName,
				columnVisiable,
				columnWidth);
		}

		public DGVColumnProfile(
			bool isFraction,
			bool isNotified,
			Color defaultForeColor,
			NotifyTypes notifyType,
			Color notifyPositiveColor,
			float notifyPositiveBorderWidth,
			Color notifyNegativeColor,
			float notifyNegativeBorderWidth,
			Font columnFont,
			int columnDisplayIndex,
			string columnName,
			bool columnVisiable,
			int columnWidth)
		{
			Initializ(
				isFraction,
				isNotified,
				defaultForeColor,
				notifyType,
				notifyPositiveColor,
				notifyPositiveBorderWidth,
				notifyNegativeColor,
				notifyNegativeBorderWidth,
				columnFont,
				columnDisplayIndex,
				columnName,
				columnVisiable,
				columnWidth);
		}

		private void Initializ(
			bool isFraction,
			bool isNotified,
			Color defaultForeColor,
			NotifyTypes notifyType,
			Color notifyPositiveColor,
			float notifyPositiveBorderWidth,
			Color notifyNegativeColor,
			float notifyNegativeBorderWidth,
			Font columnFont,
			int columnDisplayIndex,
			string columnName,
			bool columnVisiable,
			int columnWidth)
		{
			_isFraction = isFraction;
			_isNotified = isNotified;
			_defaultForeColor = defaultForeColor;
			_notifyType = notifyType;
			_notifyPositiveColor = notifyPositiveColor;
			_notifyPositiveBorderWidth = notifyPositiveBorderWidth;
			_notifyNegativeColor = notifyNegativeColor;
			_notifyNegativeBorderWidth = notifyNegativeBorderWidth;
			_columnFont = columnFont;
			_columnDisplayIndex = columnDisplayIndex;
			_columnName = columnName;
			_columnVisiable = columnVisiable;
			_columnWidth = columnWidth;
		}

		public bool IsFraction
		{
			get
			{
				return _isFraction;
			}
		}

		public bool IsNotified
		{
			get
			{
				return _isNotified;
			}
		}

		public Color DefaultForeColor
		{
			get
			{
				return _defaultForeColor;
			}
		}

		public NotifyTypes NotifyType
		{
			get
			{
				return _notifyType;
			}
		}

		public Color NotifyPositiveColor
		{
			get
			{
				return _notifyPositiveColor;
			}
		}

		public float NotifyPositiveBorderWidth
		{
			get
			{
				return _notifyPositiveBorderWidth;
			}
		}

		public Color NotifyNegativeColor
		{
			get
			{
				return _notifyNegativeColor;
			}
		}

		public float NotifyNegativeBorderWidth
		{
			get
			{
				return _notifyNegativeBorderWidth;
			}
		}

		public Font ColumnFont
		{
			get
			{
				return _columnFont;
			}
		}

		public int ColumnDisplayIndex
		{
			get
			{
				return _columnDisplayIndex;
			}
		}

		public string ColumnName
		{
			get
			{
				return _columnName;
			}
		}

		public bool ColumnVisiable
		{
			get
			{
				return _columnVisiable;
			}
		}

		public int ColumnWidth
		{
			get
			{
				return _columnWidth;
			}
		}
	}
}
