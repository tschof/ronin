using System.Collections.Generic;

namespace Common
{
	// Market data providers represent prices as doubles.  However, bid/ask levels
	// are ordered by price, and comparing doubles has problems with precision.
	// This class wraps double values with a comparison safe implementation, which
	// permits Price to be safely used as a Dictionary key (unlike type double).
	public struct Price : System.IComparable<Price>, System.IEquatable<Price>, System.IEquatable<double>
	{
		private const double EPSILON = 0.00000001; // Max precision.

		public const double WHOLE_TICK = 0.0625;
		public const double HALF_TICK = 0.03125;
		public const double QUARTER_TICK = 0.015625;
		public const double EIGHTH_TICK = 0.0078125;

		public static Price UnsetPrice { get; private set; } = new Price(double.NaN);

		private double _value;

		// Don't define a converion operator; we want conversions to be
		// explicit in a performance critical application like market data.
		public double Value { get { return _value; } }

		public bool IsZero { get { return IsZeroValue(_value); } } // Can't have a property with same name as static method.  WTF?
		public bool HasValue { get { return !double.IsNaN(_value); } }

		public static bool operator <(Price a, Price b) => a.CompareTo(b) < 0;
		public static bool operator >(Price a, Price b) => a.CompareTo(b) > 0;
		public static bool operator ==(Price a, Price b) => a.CompareTo(b) == 0;
		public static bool operator !=(Price a, Price b) => a.CompareTo(b) != 0;
		public static bool operator >=(Price a, Price b) => a.CompareTo(b) >= 0;
		public static bool operator <=(Price a, Price b) => a.CompareTo(b) <= 0;

		public static Price operator +(Price price, double value) => new Price(price._value + value);
		public static Price operator +(Price price, Price other) => new Price(price._value + other._value);
		public static Price operator -(Price price, double value) => new Price(price._value - value);
		public static Price operator -(Price price, Price other) => new Price(price._value - other._value);

		// Allow implicit conversion from double to Price.
		public static implicit operator Price(double d) => new Price(d);

		public Price(double value)
		{
			_value = value;
		}

		public Price(Price other)
		{
			_value = other._value;
		}

		public Price Round(int digits)
		{
			return new Price(System.Math.Round(_value, digits));
		}

		public int CompareTo(Price other)
		{
			if (_value < other._value) {
				return ((other._value - _value) < EPSILON) ? 0 : -1;
			} else {
				return ((_value - other._value) < EPSILON) ? 0 : 1;
			}
		}

		public bool Equals(Price other) => Same(_value, other._value);
		public bool Equals(double other) => Same(_value, other);
		public override bool Equals(object obj)
		{
			switch (obj) {
				case Price pval:
					return Same(_value, pval._value);
				case double dval:
					return Same(_value, dval);
				default:
					return false;
			}
		}

		public override int GetHashCode()
		{
			return _value.GetHashCode();
		}

		public override string ToString()
		{
			return _value.ToString();
		}

		public static bool IsZeroValue(double value)
		{
			return (value < 0) ? (value > -EPSILON) : (value < EPSILON);
		}

		public static bool Same(double a, double b)
		{
			if (a < b)
				return (b - a) < EPSILON;
			return (a - b) < EPSILON;
		}

		public enum SortOrder : byte
		{
			Ask, // ascending (lowest ask is top)
			Bid  // descending (highest bid is top)
		};

		public static void Sort(List<Price> list, SortOrder order)
		{
			if (list != null) {
				SortByPrice compare = new SortByPrice(order);
				list.Sort(compare);
			}
		}

		public static Price FromObject(object value)
		{
			switch (value) {
				case Price p:
					return new Price(p);
				case double d:
					return new Price(d);
				case int i:
					return new Price(i);
				case long l:
					return new Price(l);
				case decimal n:
					return new Price(decimal.ToDouble(n));
				case string s:
					return double.TryParse(s, out double converted) ? new Price(converted) : Price.UnsetPrice;
				default:
					return Price.UnsetPrice;
			}
		}

		#region comparers

		private class SortByPrice : IComparer<Price>
		{
			private readonly SortOrder _order;

			internal SortByPrice(SortOrder order)
			{
				_order = order;
			}

			public int Compare(Price x, Price y)
			{
				if (_order == SortOrder.Ask)
					return x.CompareTo(y);
				else
					return y.CompareTo(x);
			}
		}

		#endregion // comparers
	}
}