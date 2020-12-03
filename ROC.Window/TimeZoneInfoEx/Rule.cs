/*
 * ZoneInfo .NET API
 * Developed by Mark Rodrigues
 * Published under the Microsoft Public License (Ms-PL)
 * For the latest version and to log issues please go to:
 * http://www.codeplex.com/zoneinfo
 */
using System;
using System.Diagnostics;

namespace TimeZoneInfoEx
{
	/// <summary>
	/// A class representing the rules which can be associated
	/// with zones.
	/// </summary>
	public class Rule
	{
		#region Constructor

		/// <summary>
		/// Creates a rule from a rule entry
		/// </summary>
		/// <param name="line">The line from a timezone file</param>
		/// <remarks>
		/// The format of the file is:
		/// Rule	NAME	FROM	TO	TYPE	IN	ON	AT	SAVE	LETTER/S
		/// </remarks>
		public Rule(string line)
		{
			// Parse the string
			string[] arr = line.Split('\t', ' ');

			// Assert 
			Debug.Assert(arr[0] == "Rule", "String array must start with a \"Rule\" value");
			Debug.Assert(arr.Length >= 10, "Invalid string array - At least 10 values expected", arr.Length.ToString());


			Name = arr[1];

			// To From year can be "min"
			if (arr[2] == "min")
			{
				From = DateTime.MinValue.Year;
			}
			else
			{
				From = Convert.ToInt32(arr[2]);
			}

			To = ConvertToDateYear(arr[3], From);
			Type = arr[4];
			In = Database.ConvertToMonth(arr[5]);

			// For some reason this is hack is needed where
			// array 6 can be empty
			int arrayOffset = 0;
			if (arr[6].Length == 0)
			{
				arrayOffset++;
			}
			On = arr[6 + arrayOffset];

			// Antother hack
			if (arr[7 + arrayOffset].Length == 0)
			{
				arrayOffset++;
			}
			At = Database.ConvertToTimeSpan(arr[7 + arrayOffset]);
			Save = Database.ConvertToTimeSpan(arr[8 + arrayOffset]);
			Letters = arr[9 + arrayOffset];
		}

		#endregion // Constructor

		#region Public Methods

		/// <summary>
		/// Tries to get the rule cutover for the current rule
		/// </summary>
		/// <param name="year">The year to try and get
		/// the cutover date for</param>
		/// <param name="dt">If a date does exist for the current
		/// rule and year then it is returned as an out parameter</param>
		/// <returns>False if the rule has no date in the range,
		/// True otherwise</returns>
		public bool TryGetCutoverDateTime(int year, out DateTime dt)
		{
			// Get out if the date is not in the range
			if (year < From || year > To)
			{
				dt = DateTime.MinValue;
				return false;
			}

			dt = new DateTime(year, In, Database.ConvertToDateTime(On, year, In).Day,   // YMD
							  At.Hours, At.Minutes, At.Seconds,                         // HMS
							  DateTimeKind.Local);

			return true;
		}

		#endregion // Public Methods

		#region Public Members

		/// <summary>
		/// The name of the given rule
		/// </summary>
		public String Name
		{
			get
			{
				return _name;
			}
			set
			{
				_name = value;
			}
		}
		private string _name;

		/// <summary>
		/// The year in which the rule starts
		/// </summary>
		public int From
		{
			get
			{
				return _from;
			}
			set
			{
				_from = value;
			}
		}
		private int _from;

		/// <summary>
		/// The year in which the rule ends
		/// </summary>
		public int To
		{
			get
			{
				return _to;
			}
			set
			{
				_to = value;
			}
		}
		private int _to;

		/// <summary>
		/// The type value for the given rule.
		/// This will often be "-"
		/// </summary>
		public string Type
		{
			get
			{
				return _type;
			}
			set
			{
				_type = value;
			}
		}
		private string _type;

		/// <summary>
		/// The month in which the rule starts
		/// </summary>
		public int In
		{
			get
			{
				return _in;
			}
			set
			{
				_in = value;
			}
		}
		private int _in;

		/// <summary>
		/// The day in which the rule starts
		/// </summary>
		public string On
		{
			get
			{
				return _on;
			}
			set
			{
				_on = value;
			}
		}
		private string _on;

		// The time is which the rule starts
		public TimeSpan At
		{
			get
			{
				return _at;
			}
			set
			{
				_at = value;
			}
		}
		private TimeSpan _at;

		/// <summary>
		/// The amount to add to a given date if covered by this rule
		/// </summary>
		public TimeSpan Save
		{
			get
			{
				return _save;
			}
			set
			{
				_save = value;
			}
		}
		private TimeSpan _save;

		/// <summary>
		/// Any letters to apply to the datetime code
		/// when the rule is in effect
		/// </summary>
		public string Letters
		{
			get
			{
				return _letters;
			}
			set
			{
				_letters = value;
			}
		}
		private string _letters;

		#endregion // Public Methods

		#region Private Static Methods

		private static int ConvertToDateYear(string year, int onlyYear)
		{
			int val;

			if (!Int32.TryParse(year, out val))
			{
				switch (year)
				{
					case "max":
						val = DateTime.MaxValue.Year;
						break;
					case "only":
						val = onlyYear;
						break;
					default:
						throw new ArgumentException("The value is invalid in for a TimeZone Rule", year);
				}
			}

			return val;
		}

		#endregion // Private Static Methods
	}
}
