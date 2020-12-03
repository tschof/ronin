using System;
using System.Collections.Generic;
using System.Text;

namespace LBMEx
{
	public class LBM : LBMBase
	{
		public LBM(
				string middleware,
				int maxRetries,
				uint idelTimeBetweenRetries,
				bool enableMamaLog,
				string mamaLogFilePath,
				string transportName,
				string dictionarySourceSymbolNamespace,
				int throttleDefault,
				int throttleInitial,
				int throttleRecap,
				int changeNotificationRate,
				bool useChangeNotificationRate,
				bool useWaitForSymbol,
				int mamaMsgQueueLimitPerSymbol,
				string sourceRange,
				string symbolRange,
				int bookDepthLimit,
				string stockSources,
				string futureSources,
				string optionSources,
				bool setRecoverGaps,
				int rawQueueLimit,
				bool logDroppedMessages)
			: base(
				middleware,
				maxRetries,
				idelTimeBetweenRetries,
				enableMamaLog,
				mamaLogFilePath,
				transportName,
				dictionarySourceSymbolNamespace,
				throttleDefault,
				throttleInitial,
				throttleRecap,
				changeNotificationRate,
				useChangeNotificationRate,
				useWaitForSymbol,
				mamaMsgQueueLimitPerSymbol,
				sourceRange,
				symbolRange,
				bookDepthLimit,
				stockSources,
				futureSources,
				optionSources,
				setRecoverGaps,
				rawQueueLimit,
				logDroppedMessages)
		{
		}
	}
}
