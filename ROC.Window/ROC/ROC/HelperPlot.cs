using System;
using System.Collections.Generic;
using System.Text;
using PlotEx;

namespace ROC
{
	public sealed class HelperPlot
	{
		public sealed class BarGraphValues
		{
			public BarGraphValues(string symbolDetail, double tickSize)
			{
				_symbolDetail = symbolDetail;
				_tickSize = tickSize;
			}

			private Dictionary<double, int> _priceToPointList;
			public Dictionary<double, int> PriceToPointList
			{
				get
				{
					if (_priceToPointList == null)
					{
						_priceToPointList = new Dictionary<double, int>();
					}
					return _priceToPointList;
				}
				set
				{
					_priceToPointList = value;
				}
			}

			private PointPairList _pointPairs;
			public PointPairList PointPairs
			{
				get
				{
					if (_pointPairs == null)
					{
						_pointPairs = new PointPairList();
					}
					return _pointPairs;
				}
				set
				{
					_pointPairs = value;
				}
			}

			private BarItem _curve;
			public BarItem Curve
			{
				get
				{
					return _curve;
				}
				set
				{
					_curve = value;
				}
			}

			private string _symbolDetail = "";
			public string SymbolDetail
			{
				get
				{
					return _symbolDetail;
				}
				set
				{
					_symbolDetail = value;
				}
			}

			private double _tickSize = 0.01;
			public double TickSize
			{
				get
				{
					return _tickSize;
				}
				set
				{
					_tickSize = value;
				}
			}

			// X - Price
			// Y - Volume
			public void AddUpdate(double price, double volume)
			{
				if (volume > 0)
				{
					price = Math.Round(price, 7);

					Update(price, volume);
					SetMinMax(price, volume);
				}
			}

			private void Update(double price, double volume)
			{
				if (PriceToPointList.ContainsKey(price))
				{
					int pointIndex = PriceToPointList[price];
					PointPairs[pointIndex].Y += volume;
					Curve[pointIndex].Y = PointPairs[pointIndex].Y;
				}
				else
				{
					PointPair pt = Convert(price, volume);
					PointPairs.Add(pt);
					Curve.AddPoint(pt);
					PriceToPointList.Add(price, PointPairs.Count - 1);
				}
			}

			private PointPair Convert(double price, double volume)
			{
				return new PointPair(price, volume);
			}

			private void SetMinMax(double price, double volume)
			{
				XMax = price;
				XMin = price;

				YMax = volume;
			}

			private double _xMax = 0;
			public double XMax
			{
				get
				{
					return _xMax;
				}
				set
				{
					if (_xMax < value || _xMax == 0)
					{
						_xMax = value;
					}
				}
			}

			private double _xMin = 0;
			public double XMin
			{
				get
				{
					return _xMin;
				}
				set
				{
					if (_xMin > value || _xMin == 0)
					{
						_xMin = value;
					}
				}
			}

			private double _yMax = 0;
			public double YMax
			{
				get
				{
					return _yMax;
				}
				set
				{
					if (_yMax < value || _yMax == 0)
					{
						_yMax = value;
					}
				}
			}

			private double _yMin = 0;
			public double YMin
			{
				get
				{
					return _yMin;
				}
				set
				{
					if (_yMin > value || _yMin == 0)
					{
						_yMin = value;
					}
				}
			}
		}

		public sealed class LineGraphValues
		{
			public LineGraphValues(string symbolDetail, double tickSize)
			{
				_symbolDetail = symbolDetail;
				_tickSize = tickSize;
			}

			private PointPairList _pointPairs;
			public PointPairList PointPairs
			{
				get
				{
					if (_pointPairs == null)
					{
						_pointPairs = new PointPairList();
					}
					return _pointPairs;
				}
				set
				{
					_pointPairs = value;
				}
			}

			private LineItem _priceCurve;
			public LineItem PriceCurve
			{
				get
				{
					return _priceCurve;
				}
				set
				{
					_priceCurve = value;
				}
			}

			private string _symbolDetail = "";
			public string SymbolDetail
			{
				get
				{
					return _symbolDetail;
				}
				set
				{
					_symbolDetail = value;
				}
			}

			private double _tickSize = 0.01;
			public double TickSize
			{
				get
				{
					return _tickSize;
				}
				set
				{
					_tickSize = value;
				}
			}

			private bool _showDataPoints = true;
			public bool ShowDataPoints
			{
				get
				{
					return _showDataPoints;
				}
				set
				{
					_showDataPoints = value;
				}
			}

			private bool _showLine = true;
			public bool ShowLine
			{
				get
				{
					return _showLine;
				}
				set
				{
					_showLine = value;
				}
			}

			public void AddUpdate(double price, double volume)
			{
				if (volume > 0)
				{
					price = Math.Round(price, 7);

					Update(price, volume);
					SetMinMax(price);
				}
			}

			private void Update(double price, double volume)
			{
				PointPair pt = new PointPair(XDate.DateTimeToXLDate(DateTime.Now), price, volume);
				PointPairs.Add(pt);
				PriceCurve.AddPoint(pt);
			}

			private void SetMinMax(double price)
			{
				XTimeMax = XDate.DateTimeToXLDate(DateTime.Now.AddSeconds(5));

				YPriceMax = price;
				YPriceMin = price;
			}

			private double _xTimeMax = 0;
			public double XTimeMax
			{
				get
				{
					return _xTimeMax;
				}
				set
				{
					if (_xTimeMax < value || _xTimeMax == 0)
					{
						_xTimeMax = value;
					}
				}
			}

			private double _xTimeMin = 0;
			public double XTimeMin
			{
				get
				{
					return _xTimeMin;
				}
				set
				{
					if (_xTimeMin > value || _xTimeMin == 0)
					{
						_xTimeMin = value;
					}
				}
			}

			private double _yPriceMax = 0;
			public double YPriceMax
			{
				get
				{
					return _yPriceMax;
				}
				set
				{
					if (_yPriceMax < value || _yPriceMax == 0)
					{
						_yPriceMax = value;
					}
				}
			}

			private double _yPriceMin = 0;
			public double YPriceMin
			{
				get
				{
					return _yPriceMin;
				}
				set
				{
					if (_yPriceMin > value || _yPriceMin == 0)
					{
						_yPriceMin = value;
					}
				}
			}
		}
	}
}
