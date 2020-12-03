using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public static class HelperSecurityInfo
	{
		public static void UpdatePositions()
		{
			frmPositions[] ws = new frmPositions[0];
			if (ws.Length != GLOBAL.HWindows.PositionWindows.Count)
			{
				lock (GLOBAL.HWindows.PositionWindows)
				{
					ws = new frmPositions[GLOBAL.HWindows.PositionWindows.Count];
					GLOBAL.HWindows.PositionWindows.Values.CopyTo(ws, 0);
				}
			}

			foreach (frmPositions w in ws)
			{
				if (!w.IsProcessing)
				{
					w.UpdateSecurityInfo();
				}
			}
		}
	}
}
