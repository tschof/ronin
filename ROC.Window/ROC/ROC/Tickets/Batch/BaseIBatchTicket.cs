using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public interface BaseIBatchTicket
	{
		#region - From Event -
		
		void AutoResetShow();
		
		#endregion

		#region - Build Order -

		void BuildOrder(bool selectedOnly);

		#endregion

		#region - Export & Import -

		string ExportGrid();
		void ImportGrid(string s);

		#endregion

		#region - Row Menu Support -

		void ClearDataRow();
		
		#endregion
	}
}
