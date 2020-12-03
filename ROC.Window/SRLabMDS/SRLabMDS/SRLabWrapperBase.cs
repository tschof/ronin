using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace SRLabTest
{
	public class SRLabWrapperBase
	{
		#region - API Calls -

		public delegate void CallBackChanged(ManagedTypedMap data);

		[DllImport("MarketData.dll")]
		public unsafe static extern void ReturnDelta(CallBackChanged delta);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Initialize(string configFileLocation, string configFile);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Start();

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Stop();

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Subscribe(string symbol, string source, bool reload);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr SubscribeL2(string symbol, string source, bool reload);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr SubscribeBook(string symbol, string source, bool reload);

		[DllImport("APISRLab.dll")]
		private unsafe static extern IntPtr Unsubscribe(string symbol);

		#endregion

		public SRLabWrapperBase()
		{
		}
	}
}
