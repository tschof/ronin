/*
 * ZoneInfo .NET API
 * Developed by Mark Rodrigues
 * Published under the Microsoft Public License (Ms-PL)
 * For the latest version and to log issues please go to:
 * http://www.codeplex.com/zoneinfo
 */
using System;
using System.Collections.Generic;
using System.Text;

namespace TimeZoneInfoEx
{
	/// <summary>
	/// Represents an association between zones
	/// and rules and the date ranges in which this
	/// association applies
	/// </summary>
	public class ZoneRule
	{
		#region Public Members

		/// <summary>
		/// The name of the zone
		/// </summary>
		public string ZoneName
		{
			get
			{
				return _zoneName;
			}
			set
			{
				_zoneName = value;
			}
		}
		private string _zoneName;

		/// <summary>
		/// The standard GMT offset when this rule applies
		/// </summary>
		public TimeSpan GmtOffset
		{
			get
			{
				return _gmtOffset;
			}
			set
			{
				_gmtOffset = value;
			}
		}
		private TimeSpan _gmtOffset;

		/// <summary>
		/// The name of the rule which applies for this
		/// zone
		/// </summary>
		public string RuleName
		{
			get
			{
				return _ruleName;
			}
			set
			{
				_ruleName = value;
			}
		}
		private string _ruleName;

		/// <summary>
		/// The format string when this rule is used
		/// in this zone
		/// </summary>
		public string Format
		{
			get
			{
				return _format;
			}
			set
			{
				_format = value;
			}
		}
		private string _format;

		/// <summary>
		/// The date until which this rule applies
		/// for this zone
		/// </summary>
		/// <remarks>
		/// The datetime return is of type DateTimeKind.Local
		/// </remarks>
		public DateTime Until
		{
			get
			{
				return _until;
			}
			set
			{
				_until = value;
			}
		}
		private DateTime _until;

		#endregion // Public Members
	}
}
