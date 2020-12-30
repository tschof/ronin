using System;
using System.Diagnostics;

namespace DateTimeEx
{
	public class DateTimeHP
	{
		/// Creates a new instance of DateTimePrecise.
		/// A large value of synchronizePeriodSeconds may cause arithmetic overthrow
		/// exceptions to be thrown. A small value may cause the time to be unstable.
		/// A good value is 10.
		/// synchronizePeriodSeconds = The number of seconds after which the
		/// DateTimePrecise will synchronize itself with the system clock.

		public DateTimeHP()
		{
			Initialize(10);
		}
		public DateTimeHP(long synchronizePeriodSeconds)
		{
			Initialize(synchronizePeriodSeconds);
		}

		private void Initialize(long synchronizePeriodSeconds)
		{
			Stopwatch = Stopwatch.StartNew();
			this.Stopwatch.Start();

			DateTime t = DateTime.UtcNow;
			_immutable = new DateTimePreciseSafeImmutable(t, t, Stopwatch.ElapsedTicks, Stopwatch.Frequency);

			_synchronizePeriodSeconds = synchronizePeriodSeconds;
			_synchronizePeriodStopwatchTicks = synchronizePeriodSeconds * Stopwatch.Frequency;
			_synchronizePeriodClockTicks = synchronizePeriodSeconds * _clockTickFrequency;
		}

		/// Returns the current date and time, just like DateTime.UtcNow.
		public DateTime UtcNow
		{
			get
			{
				try
				{
					long s = this.Stopwatch.ElapsedTicks;
					DateTimePreciseSafeImmutable immutable = _immutable;

					if (s < immutable._s_observed + _synchronizePeriodStopwatchTicks)
					{
						return immutable._t_base.AddTicks((s - immutable._s_observed) * _clockTickFrequency / immutable._stopWatchFrequency);
					}
					else
					{
						DateTime t = DateTime.UtcNow;
						DateTime t_base_new = immutable._t_base.AddTicks((s - immutable._s_observed) * _clockTickFrequency / immutable._stopWatchFrequency);
						try
						{
							checked
							{
								_immutable = new DateTimePreciseSafeImmutable(
									t,
									t_base_new,
									s,
									((s - immutable._s_observed) * _clockTickFrequency * 2) /
									(t.Ticks - immutable._t_observed.Ticks + t.Ticks + t.Ticks - t_base_new.Ticks - immutable._t_observed.Ticks));
							}
						}
						catch
						{
							_immutable = new DateTimePreciseSafeImmutable(t, t, this.Stopwatch.ElapsedTicks, Stopwatch.Frequency);
							t_base_new = DateTime.UtcNow;
						}
						
						//long freq = t.Ticks - immutable._t_observed.Ticks + t.Ticks + t.Ticks - t_base_new.Ticks - immutable._t_observed.Ticks;
						//if (freq > 0)
						//{
						//    _immutable = new DateTimePreciseSafeImmutable(t, t_base_new, s, ((s - immutable._s_observed) * _clockTickFrequency * 2) / freq);
						//} 

						return t_base_new;
					}
				}
				catch
				{
					return DateTime.UtcNow;
				}
			}
		}

		/// Returns the current date and time, just like DateTime.Now.
		public DateTime Now
		{
			get
			{
				try
				{
					return this.UtcNow.ToLocalTime();
				}
				catch
				{
				}

				return DateTime.Now;
			}
		}

		/// The internal System.Diagnostics.Stopwatch used by this instance.
		public Stopwatch Stopwatch;

		private long _synchronizePeriodStopwatchTicks;
		private long _synchronizePeriodSeconds;
		private long _synchronizePeriodClockTicks;
		private const long _clockTickFrequency = 10000000;
		private DateTimePreciseSafeImmutable _immutable;
	}

	internal sealed class DateTimePreciseSafeImmutable
	{
		internal DateTimePreciseSafeImmutable(DateTime t_observed, DateTime t_base,
			 long s_observed, long stopWatchFrequency)
		{
			_t_observed = t_observed;
			_t_base = t_base;
			_s_observed = s_observed;
			_stopWatchFrequency = stopWatchFrequency;
		}
		internal readonly DateTime _t_observed;
		internal readonly DateTime _t_base;
		internal readonly long _s_observed;
		internal readonly long _stopWatchFrequency;
	}
}
