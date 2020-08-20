using System;
using System.Threading;

public class ThreadHelper
{
	class TargetInfo
	{
		public TargetInfo(Delegate d, object[] args)
		{
			Target = d;
			Args = args;
		}

		public readonly Delegate Target;
		public readonly object[] Args;
	}

	private static WaitCallback dynamicInvokeShim = new WaitCallback(DynamicInvokeShim);

	public static void FireAndForget(Delegate d, params object[] args)
	{
		ThreadPool.QueueUserWorkItem(dynamicInvokeShim, new TargetInfo(d, args));
	}

	public static void DynamicInvokeShim(object o)
	{
		TargetInfo ti = (TargetInfo)o;
		ti.Target.DynamicInvoke(ti.Args);
	}
}
