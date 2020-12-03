using System;
using System.Reflection;
using System.Threading;

namespace ThreadEx
{
	public class PoolEx
	{
		private class TargetInfo
		{
			internal TargetInfo(Delegate d, object[] args)
			{
				Target = d;
				Args = args;
			}

			internal readonly Delegate Target;
			internal readonly object[] Args;
		}

		private static WaitCallback dynamicInvokeShim = new WaitCallback(DynamicInvokeShim);

		public static void FireAndForget(Delegate d)
		{
			FireAndForget(d, null);
		}
		public static void FireAndForget(Delegate d, params object[] args)
		{
			ThreadPool.QueueUserWorkItem(dynamicInvokeShim, new TargetInfo(d, args));
		}

		private static void DynamicInvokeShim(object o)
		{
			TargetInfo ti = (TargetInfo)o;
			ti.Target.DynamicInvoke(ti.Args);
		}
	}
}
