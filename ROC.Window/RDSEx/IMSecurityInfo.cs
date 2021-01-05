using DictionaryEx;
using System.Collections.Generic;

namespace RDSEx
{
	public class IMSecurityInfo : BaseSecurityInfo
	{
		private readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		internal void Set<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		public override string MDSource {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.dataSourceInfo, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override string MDSymbol {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.genericMDSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override double ContractSize {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.contractSize, out double value)) {
					return value;
				} else {
					return 1;
				}
			}
		}

		public override double TickSize {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.tickSize, out double value)) {
					return value;
				} else {
					return 0.01;
				}
			}
		}

		public override string SecType {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.securityType, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public override string LongName {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.longName, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public string Underlying {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.baseSymbol, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		public string Expiration {
			get {
				if (_data.TryGet(SecurityFieldIDs.Security.expirationDate, out string value)) {
					return value;
				} else {
					return "";
				}
			}
		}

		private Dictionary<string, IMOptionInfo> _optionChain;
		public Dictionary<string, IMOptionInfo> OptionChain {
			get {
				if (_optionChain == null) {
					_optionChain = new Dictionary<string, IMOptionInfo>();
				}
				return _optionChain;
			}
			set {
				_optionChain = value;
			}
		}

		private Dictionary<string, IMSSFutureInfo> _ssfutureChain;
		public Dictionary<string, IMSSFutureInfo> SSFutureChain {
			get {
				if (_ssfutureChain == null) {
					_ssfutureChain = new Dictionary<string, IMSSFutureInfo>();
				}
				return _ssfutureChain;
			}
			set {
				_ssfutureChain = value;
			}
		}
	}
}
