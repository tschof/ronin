using System;
using System.Reflection;
using System.Collections;
using System.Diagnostics;

namespace BenchMarkEx
{
	[AttributeUsage(AttributeTargets.Method)]
	public class BenchmarkAttribute : Attribute
	{
	}

	public class Benchmark
	{
		public Benchmark(string[] args)
		{
			// Save all the benchmark classes from doing a nullity test
			if (args == null)
			{
				args = new string[0];
			}

			// We're only ever interested in public static methods. This variable
			// just makes it easier to read the code...
			BindingFlags publicStatic = BindingFlags.Public | BindingFlags.Static;

			foreach (Type type in Assembly.GetCallingAssembly().GetTypes())
			{
				// Find an Init method taking string[], if any
				MethodInfo initMethod = type.GetMethod("Init", publicStatic, null, new Type[] { typeof(string[]) }, null);

				// Find a parameterless Reset method, if any
				MethodInfo resetMethod = type.GetMethod("Reset", publicStatic, null, new Type[0], null);

				// Find a parameterless Check method, if any
				MethodInfo checkMethod = type.GetMethod("Check", publicStatic, null, new Type[0], null);

				// Find all parameterless methods with the [Benchmark] attribute
				ArrayList benchmarkMethods = new ArrayList();
				foreach (MethodInfo method in type.GetMethods(publicStatic))
				{
					ParameterInfo[] parameters = method.GetParameters();
					if (parameters != null && parameters.Length != 0)
					{
						continue;
					}

					if (method.GetCustomAttributes(typeof(BenchmarkAttribute), false).Length != 0)
					{
						benchmarkMethods.Add(method);
					}
				}

				// Ignore types with no appropriate methods to benchmark
				if (benchmarkMethods.Count == 0)
				{
					continue;
				}

				Debug.Print("Benchmarking type {0}", type.Name);

				// If we've got an Init method, call it once
				try
				{
					if (initMethod != null)
					{
						initMethod.Invoke(null, new object[] { args });
					}
				}
				catch (TargetInvocationException e)
				{
					Exception inner = e.InnerException;
					string message = (inner == null ? null : inner.Message);
					if (message == null)
					{
						message = "(No message)";
					}

					Debug.Print("Init failed ({0})", message);
					continue; // Next type
				}

				foreach (MethodInfo method in benchmarkMethods)
				{
					try
					{
						// Reset (if appropriate)
						if (resetMethod != null)
							resetMethod.Invoke(null, null);

						// Give the test as good a chance as possible
						// of avoiding garbage collection
						GC.Collect();
						GC.WaitForPendingFinalizers();
						GC.Collect();

						// Now run the test itself
						DateTime start = DateTime.Now;
						method.Invoke(null, null);
						DateTime end = DateTime.Now;

						// Check the results (if appropriate)
						// Note that this doesn't affect the timing
						if (checkMethod != null)
							checkMethod.Invoke(null, null);

						// If everything's worked, report the time taken, 
						// nicely lined up (assuming no very long method names!)
						Debug.Print("  {0,-20} {1}", method.Name, end - start);
					}
					catch (TargetInvocationException e)
					{
						Exception inner = e.InnerException;
						string message = (inner == null ? null : inner.Message);
						if (message == null)
						{
							message = "(No message)";
						}
						Debug.Print("  {0}: Failed ({1})", method.Name, message);
					}
				}
			}
		}
	}
}
