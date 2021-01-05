using System.Collections.Generic;
using System.Collections;

namespace RDSEx
{
	public class DestinationMap
	{
		public enum DestinationFieldIDs : int
		{
			DestID = 101,
			SecType = 102,
			ShortName = 103,
			GiveupFirmID = 104,
			CMTAFirmID = 105,
			AlgoFlag = 106
		}

		public long? destID = null;
		public string secType = null;
		public string shortName = null;
		public string giveupFirmID = null;
		public string cmtaFirmID = null;
		public string algoFlag = null;
		public bool isAlgo = false;

		public class Collection
		{
			private List<KeyValuePair<int, DestinationMap>> _destinations = null;

			public DestinationMap FindOrAdd(int destinationKey)
			{
				int i = _destinations.FindIndex(n => n.Key == destinationKey);
				DestinationMap map;

				if (i < 0) {
					map = new DestinationMap();
					_destinations.Add(new KeyValuePair<int, DestinationMap>(destinationKey, map));
				} else {
					map = _destinations[i].Value;
				}
				return map;
			}

			public Enumerator GetEnumerator()
			{
				return new Enumerator(_destinations);
			}

			public class Enumerator : IEnumerator<DestinationMap>
			{
				private List<KeyValuePair<int, DestinationMap>> _values;
				private int _index = -1;

				internal Enumerator(List<KeyValuePair<int, DestinationMap>> destinations)
				{
					_values = destinations;
				}

				public DestinationMap Current => _values == null ? null : _values[_index].Value;

				object IEnumerator.Current => _values == null ? null : _values[_index].Value;

				public void Dispose()
				{
				}

				public bool MoveNext()
				{
					if (_values != null) {
						if (++_index < _values.Count)
							return true;
						_values = null;
					}
					return false;
				}

				public void Reset()
				{
				}
			}
		}
	}
}