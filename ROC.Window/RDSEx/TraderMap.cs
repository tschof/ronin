using System.Collections.Generic;

namespace RDSEx
{
	public class TraderMap
	{
		private Dictionary<string, AccountMap> _csAccounts;
		public Dictionary<string, AccountMap> CSAccounts {
			get {
				if (_csAccounts == null) {
					_csAccounts = new Dictionary<string, AccountMap>();
				}
				return _csAccounts;
			}
			set {
				_csAccounts = value;
			}
		}

		private Dictionary<string, AccountMap> _optAccounts;
		public Dictionary<string, AccountMap> OPTAccounts {
			get {
				if (_optAccounts == null) {
					_optAccounts = new Dictionary<string, AccountMap>();
				}
				return _optAccounts;
			}
			set {
				_optAccounts = value;
			}
		}

		private Dictionary<string, AccountMap> _futAccounts;
		public Dictionary<string, AccountMap> FUTAccounts {
			get {
				if (_futAccounts == null) {
					_futAccounts = new Dictionary<string, AccountMap>();
				}
				return _futAccounts;
			}
			set {
				_futAccounts = value;
			}
		}

		public string tradeFor = null;
		public string localAcAcrn = null;
	}
}