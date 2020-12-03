using System;
using System.Collections.Generic;
using System.Text;

namespace ROMEx
{
	public class ROMMessage
	{
		public string GetLogin(string username, string password)
		{
			CSV csv = new CSV();

			csv.SetAt(CSVFields.CSV_COMMAND, "L");
			csv.SetAt(CSVFields.CSV_TRADER, username);
			csv.SetAt(CSVFields.CSV_PASSWORD, password);
			csv.SetAt(CSVFields.CSV_CANCEL_BOOL, 0); //get all open orders

			csv.Encode();
			return csv.EncodeMessage;
		}
	}
}
