using System;
using System.Collections.Generic;
using SerializationEx;

namespace BindingListEx
{
	public class BindingListWatch<T> : BindingListBase<T>
	{
		public override string Export()
		{
			ToBinary b = new ToBinary();
			byte[] bytes = b.Serialize((List<T>)base.Items, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public override void Import(string s)
		{
			ToBinary b = new ToBinary();
			byte[] bytes = System.Convert.FromBase64String(s);
			List<T> items = (List<T>)b.Deserialize(bytes, SerializationTypes.Normal);

			this.Clear();
			for (int i = 0; i < items.Count; i++)
			{
				this.Add(items[i]);
			}
		}
	}
}
