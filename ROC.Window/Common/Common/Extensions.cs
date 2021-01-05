using System.Collections.Generic;

namespace Common
{
	public static class Extensions
	{
		// This extension lets us use:
		//     foreach ((TKey key, TValue value) in myDictionary) {
		//         ...
		//     }
		// instead of
		//     foreach (KeyValuePair<TKey, TValue> entry in myDictionary) {
		//         TKey key = entry.Key;
		//         TValue value = entry.Value;
		//         ...
		//      }
		public static void Deconstruct<TKey, TValue>(this KeyValuePair<TKey, TValue> item, out TKey key, out TValue value)
		{
			key = item.Key;
			value = item.Value;
		}

		// Encapsulate the ContainsKey() + Add() for Dictionary.
		public static bool TryAdd<TKey, TValue>(this Dictionary<TKey, TValue> collection, TKey key, TValue value)
		{
			if ((collection == null) || collection.ContainsKey(key))
				return false;
			collection.Add(key, value);
			return true;
		}

		// Apply the given action to all values in a dictionary.  Replaces all of the iteration over Dictionary.Values
		// I found in the code.
		public static void WithValues<TKey, TValue>(this Dictionary<TKey, TValue> collection, System.Action<TValue> action)
		{
			if ((collection != null) && (action != null)) {
				Dictionary<TKey, TValue>.Enumerator i = collection.GetEnumerator();
				while (i.MoveNext())
					action(i.Current.Value);
			}
		}
	}
}
