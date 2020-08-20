using System;
using System.Collections;
using TposWrapper;

namespace Ntest
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Ntest
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			string und = "IBM";
			ArrayList result = new ArrayList();
            string u = "RONX";
            string p = "RONX";
            int port = 2917;
            string s = "10.104.35.24";
			TposProxy proxy = new TposProxy(u, p, s, port);
			proxy.Connect();
			proxy.GetOptionChain(und, result);
			Console.WriteLine(result.Count);
			for (int i=0; i< result.Count; i++)
			{
				OptionDesc od = (OptionDesc)result[i];
				Console.WriteLine(i + ": " + od.opraSymbol + ", " + od.strike.ToString());
			}
			// return ret;
			//
			// TODO: Add code to start application here
			//
		}
	}
}
