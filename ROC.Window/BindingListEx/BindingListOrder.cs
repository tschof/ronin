using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

namespace BindingListEx
{
	public class OrderBindingList<T> : BindingListBase<T>
	{
		//public override void Save(string filename)
		//{
		//    BinaryFormatter formatter = new BinaryFormatter();
		//    using (FileStream stream = new FileStream(filename, FileMode.Create))
		//    {
		//        // Serialize data list items
		//        formatter.Serialize(stream, (List<T>)base.Items);
		//    }
		//}

		//public override void Load(string filename)
		//{
		//    this.ClearItems();

		//    if (File.Exists(filename))
		//    {
		//        BinaryFormatter formatter = new BinaryFormatter();
		//        using (FileStream stream = new FileStream(filename, FileMode.Open))
		//        {
		//            // Deserialize data list items
		//            ((List<T>)base.Items).AddRange((IEnumerable<T>)formatter.Deserialize(stream));
		//        }
		//    }

		//    // Let bound controls know they should refresh their views
		//    base.OnListChanged(new ListChangedEventArgs(ListChangedType.Reset, -1));
		//}

		#region - Not Used -

		//public void Save(string filename)
		//{
		//    BinaryFormatter formatter = new BinaryFormatter();
		//    using (FileStream stream = new FileStream(filename, FileMode.Create))
		//    {
		//        // Serialize data list items
		//        formatter.Serialize(stream, (List<T>)this.Items);
		//    }
		//}

		//public void Load(string filename)
		//{
		//    this.ClearItems();

		//    if (File.Exists(filename))
		//    {
		//        BinaryFormatter formatter = new BinaryFormatter();
		//        using (FileStream stream = new FileStream(filename, FileMode.Open))
		//        {
		//            // Deserialize data list items
		//            ((List<T>)this.Items).AddRange((IEnumerable<T>)formatter.Deserialize(stream));
		//        }
		//    }

		//    // Let bound controls know they should refresh their views
		//    this.OnListChanged(new ListChangedEventArgs(ListChangedType.Reset, -1));
		//}

		#endregion

		public override string Export()
		{
			throw new Exception("The method or operation is not implemented.");
		}

		public override void Import(string s)
		{
			throw new Exception("The method or operation is not implemented.");
		}
	}
}
