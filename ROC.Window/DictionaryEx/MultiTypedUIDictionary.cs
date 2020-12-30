using System;
using System.Drawing;
using System.Runtime.Serialization;

using SerializationEx;

namespace DictionaryEx
{
	[Serializable]
	public class MultiTypedUIDictionary : MultiTypedDictionary, ISerializable
	{
		private GenericDictionary<Color> _colors = new GenericDictionary<Color>();
		private GenericDictionary<Font> _fonts = new GenericDictionary<Font>();

		public MultiTypedUIDictionary() : base()
		{
		}

		public bool TryGet(int key, out Color value)
		{
			return _colors.TryGet(key, out value);
		}

		public bool TryGet(int key, out Font value)
		{
			return _fonts.TryGet(key, out value);
		}

		public bool Set(int key, Color value)
		{
			return _colors.Set(key, value, true);
		}

		public bool Set(int key, Font value)
		{
			return _fonts.Set(key, value, true);
		}

		public new void Clear()
		{
			base.Clear();
			_colors.Clear();
			_fonts.Clear();
		}

		#region ISerializable Members

		// Serialize the object.
		public new void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context);

			SerializationWriter writer = new SerializationWriter();
			_colors.Serialize(writer);
			_fonts.Serialize(writer);
			writer.AddToInfo(info);
		}

		// Deserialization constructor.
		public MultiTypedUIDictionary(SerializationInfo info, StreamingContext ctxt) : base(info, ctxt)
		{
			SerializationReader reader = new SerializationReader(info);
			_colors = new GenericDictionary<Color>(reader);
			_fonts = new GenericDictionary<Font>(reader);
		}

		#endregion
	}
}
