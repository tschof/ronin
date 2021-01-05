namespace RDSEx
{
	public abstract class BaseSecurityInfo
	{
		public abstract string MDSource {
			get;
		}

		public abstract string MDSymbol {
			get;
		}

		public abstract double ContractSize {
			get;
		}

		public abstract double TickSize {
			get;
		}

		public abstract string SecType {
			get;
		}

		public abstract string LongName {
			get;
		}
	}
}
