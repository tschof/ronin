namespace RDSEx
{
	public class AccountMap
	{
		private DestinationMap.Collection _destinations = null;

		public DestinationMap.Collection Destinations {
			get {
				if (_destinations == null)
					_destinations = new DestinationMap.Collection();
				return _destinations;
			}
		}

		public long? interfaceID = null;
		public string account = null;
		public string type = null;
		public string clearingAcID = null;
		public string localAcAcrn = null;
		public string firmAcr = null;
		public string omAcctType = null;
		public string capacity = null;
		public string clearingFirmID = null;
		public string occAcr = null;
		public string homeExchange = null;

		public string clearingAcIDShort {
			get {
				string value = clearingAcID;
				if (value == null) {
					return null;
				} else if (value.Length > 5) {
					return value.Substring(0, 5);
				} else {
					return value;
				}
			}
		}

		public void Update(RDSEx.WEB.AcctProfile profile)
		{
			interfaceID = profile.interfaceID;
			account = profile.account.ToUpper();
			type = profile.type.ToUpper();
			clearingAcID = profile.clearingAcID.ToUpper();
			localAcAcrn = profile.localAcAcrn.ToUpper();
			firmAcr = profile.firmAcr.ToUpper();
			omAcctType = profile.omAcctType.ToUpper();
			capacity = profile.capacity.ToUpper();
			interfaceID = profile.interfaceID;
			clearingFirmID = profile.clearingFirmID.ToUpper();
			occAcr = profile.occAcr.ToUpper();
			homeExchange = profile.homeExchange.ToUpper();
		}

		/*
		public enum FieldIds : int
		{
			InterfaceID = 11,
			Account = 12,
			Type = 13,
			ClearingAcID = 14,
			LocalAcAcrn = 15,
			FirmAcr = 16,
			OmAcctType = 17,
			Capacity = 18,
			ClearingFirmID = 19,
			OccAcr = 20,
			HomeExchange = 21
		}

		public void Set(FieldIds which, long value)
		{
			if (which == FieldIds.InterfaceID)
				interfaceID = value;
		}

		public void Set(FieldIds which, string value)
		{
			switch (which) {
				case FieldIds.Account: account = value; break;
				case FieldIds.Type: type = value; break;
				case FieldIds.ClearingAcID: clearingAcID = value; break;
				case FieldIds.LocalAcAcrn: localAcAcrn = value; break;
				case FieldIds.FirmAcr: firmAcr = value; break;
				case FieldIds.OmAcctType: omAcctType = value; break;
				case FieldIds.Capacity: capacity = value; break;
				case FieldIds.ClearingFirmID: clearingFirmID = value; break;
				case FieldIds.OccAcr: occAcr = value; break;
				case FieldIds.HomeExchange: homeExchange = value; break;
			}
		}
		*/
	}
}
