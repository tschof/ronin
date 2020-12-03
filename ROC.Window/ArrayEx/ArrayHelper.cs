using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

namespace ArrayEx
{
	public static class ArrayHelper
	{
		public static Array Resize(Array array, int newSize)
		{
			Type type = array.GetType();
			Array newArray = Array.CreateInstance(type.GetElementType(), newSize);
			if (array.Length > 0)
			{
				Array.Copy(array, 0, newArray, 0, array.Length);
			}
			return newArray;
		}

		public static Array Update(Array array, int index, object val)
		{
			if (val.GetType().Name == array.GetType().Name.Trim(new char[] { '[', ']' }))
			{
				if (array.Length < index + 1)
				{
					array = Resize(array, index + 1);
				}

				array.SetValue(val, index);
			}
			else
			{
				Debug.Assert(false, "Type mismatch!");
			}

			return array;
		}

		public static Array Insert(Array array, int index, int count)
		{
			Array.Copy(array, index, array, index + count, array.Length - (index + count));
			Array.Clear(array, index, count);
			return array;
		}

		public static Array Remove(Array array, int index, int count)
		{
			Array.Copy(array, index+count, array, index, array.Length - (index+count));
			Array.Clear(array, array.Length - count, count);
			return array;
		}

		public static Array Convert(Array array, Type type)
		{
			Array newArray = Array.CreateInstance(type, array.Length);
			Array.Copy(array, 0, newArray, 0, array.Length);
			return newArray;
		}

		public static Array GetRange(Array range, int start, int count)
		{
			Type type = range.GetType();
			Array newArray = Array.CreateInstance(type.GetElementType(), count);
			Array.Copy(range, start, newArray, 0, count);
			return newArray;
		}

		public static List<double> ConvertToList(Dictionary<double, long> input)
		{
			List<double> result = new List<double>();

			double[] inputs = new double[input.Count];
			input.Keys.CopyTo(inputs, 0);
			result.AddRange(inputs);

			return result;
		}
	}
}
