using System;
using System.IO;
using System.Reflection;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace SerializationEx
{
	internal enum ObjType : byte
	{
		nullType,
		boolType,
		byteType,
		uint16Type,
		uint32Type,
		uint64Type,
		sbyteType,
		int16Type,
		int32Type,
		int64Type,
		charType,
		stringType,
		singleType,
		doubleType,
		decimalType,
		dateTimeType,
		byteArrayType,
		charArrayType,
		otherType
	}

	public class SerializationWriter : BinaryWriter
	{
		private SerializationWriter(Stream s) : base(s) { }

		// Static method to initialise the writer with a suitable MemoryStream.
		public static SerializationWriter GetWriter()
		{
			MemoryStream ms = new MemoryStream(10240);
			return new SerializationWriter(ms);
		}

		// Writes a string to the buffer.  
		// Overrides the base implementation so it can cope with nulls
		public override void Write(string str)
		{
			if (str == null)
			{
				Write((byte)ObjType.nullType);
			}
			else
			{
				Write((byte)ObjType.stringType);
				base.Write(str);
			}
		}

		// Writes a byte array to the buffer.  
		// Overrides the base implementation to send the length of the array which is needed when it is retrieved.
		public override void Write(byte[] b)
		{
			if (b == null)
			{
				Write(-1);
			}
			else
			{
				int len = b.Length;
				Write(len);
				if (len > 0) base.Write(b);
			}
		}

		// Writes a char array to the buffer.  
		// Overrides the base implementation to sends the length of the array which is needed when it is read.
		public override void Write(char[] c)
		{
			if (c == null)
			{
				Write(-1);
			}
			else
			{
				int len = c.Length;
				Write(len);
				if (len > 0) base.Write(c);
			}
		}

		// Writes a DateTime to the buffer. 
		public void Write(DateTime dt) { Write(dt.Ticks); }

		// Writes a generic ICollection (such as an IList<T>) to the buffer.
		public void Write<T>(ICollection<T> c)
		{
			if (c == null)
			{
				Write(-1);
			}
			else
			{
				Write(c.Count);
				foreach (T item in c) WriteObject(item);
			}
		}

		// Writes a generic IDictionary to the buffer.
		public void Write<T, U>(IDictionary<T, U> d)
		{
			if (d == null)
			{
				Write(-1);
			}
			else
			{
				Write(d.Count);
				foreach (KeyValuePair<T, U> kvp in d)
				{
					WriteObject(kvp.Key);
					WriteObject(kvp.Value);
				}
			}
		}

		// Writes an arbitrary object to the buffer.  
		// Useful where we have something of type "object" and don't know how to treat it.  
		// This works out the best method to use to write to the buffer.
		public void WriteObject(object obj)
		{
			if (obj == null)
			{
				Write((byte)ObjType.nullType);
			}
			else
			{

				switch (obj.GetType().Name)
				{

					case "Boolean": Write((byte)ObjType.boolType);
						Write((bool)obj);
						break;

					case "Byte": Write((byte)ObjType.byteType);
						Write((byte)obj);
						break;

					case "UInt16": Write((byte)ObjType.uint16Type);
						Write((ushort)obj);
						break;

					case "UInt32": Write((byte)ObjType.uint32Type);
						Write((uint)obj);
						break;

					case "UInt64": Write((byte)ObjType.uint64Type);
						Write((ulong)obj);
						break;

					case "SByte": Write((byte)ObjType.sbyteType);
						Write((sbyte)obj);
						break;

					case "Int16": Write((byte)ObjType.int16Type);
						Write((short)obj);
						break;

					case "Int32": Write((byte)ObjType.int32Type);
						Write((int)obj);
						break;

					case "Int64": Write((byte)ObjType.int64Type);
						Write((long)obj);
						break;

					case "Char": Write((byte)ObjType.charType);
						base.Write((char)obj);
						break;

					case "String": Write((byte)ObjType.stringType);
						base.Write((string)obj);
						break;

					case "Single": Write((byte)ObjType.singleType);
						Write((float)obj);
						break;

					case "Double": Write((byte)ObjType.doubleType);
						Write((double)obj);
						break;

					case "Decimal": Write((byte)ObjType.decimalType);
						Write((decimal)obj);
						break;

					case "DateTime": Write((byte)ObjType.dateTimeType);
						Write((DateTime)obj);
						break;

					case "Byte[]": Write((byte)ObjType.byteArrayType);
						base.Write((byte[])obj);
						break;

					case "Char[]": Write((byte)ObjType.charArrayType);
						base.Write((char[])obj);
						break;

					default: Write((byte)ObjType.otherType);
						new BinaryFormatter().Serialize(BaseStream, obj);
						break;

				} // switch

			} // if obj==null

		}

		// Adds the SerializationWriter buffer to the SerializationInfo at the end of GetObjectData()
		public void AddToInfo(SerializationInfo info)
		{
			byte[] b = ((MemoryStream)BaseStream).ToArray();
			info.AddValue("X", b, typeof(byte[]));
		}
	}

	public class SerializationReader : BinaryReader
	{
		private SerializationReader(Stream s) : base(s) { }

		// Static method to take a SerializationInfo object 
		// (an input to an ISerializable constructor)
		// and produce a SerializationReader from which serialized objects can be read.
		public static SerializationReader GetReader(SerializationInfo info)
		{
			byte[] byteArray = (byte[])info.GetValue("X", typeof(byte[]));
			MemoryStream ms = new MemoryStream(byteArray);
			return new SerializationReader(ms);
		}

		// Reads a string from the buffer.  
		// Overrides the base implementation so it can cope with nulls.
		public override string ReadString()
		{
			ObjType t = (ObjType)ReadByte();
			if (t == ObjType.stringType) return base.ReadString();
			return null;
		}

		// Reads a byte array from the buffer, handling nulls and the array length. 
		public byte[] ReadByteArray()
		{
			int len = ReadInt32();
			if (len > 0) return ReadBytes(len);
			if (len < 0) return null;
			return new byte[0];
		}

		// Reads a char array from the buffer, handling nulls and the array length.
		public char[] ReadCharArray()
		{
			int len = ReadInt32();
			if (len > 0) return ReadChars(len);
			if (len < 0) return null;
			return new char[0];
		}

		// Reads a DateTime from the buffer.
		public DateTime ReadDateTime() { return new DateTime(ReadInt64()); }

		// Reads a generic list from the buffer.
		public IList<T> ReadList<T>()
		{
			int count = ReadInt32();
			if (count < 0) return null;
			IList<T> d = new List<T>();
			for (int i = 0; i < count; i++) d.Add((T)ReadObject());
			return d;
		}

		// Reads a generic Dictionary from the buffer. 
		public IDictionary<T, U> ReadDictionary<T, U>()
		{
			int count = ReadInt32();
			if (count < 0) return null;
			IDictionary<T, U> d = new Dictionary<T, U>();
			for (int i = 0; i < count; i++) d[(T)ReadObject()] = (U)ReadObject();
			return d;
		}

		// Reads an object which was added to the buffer by WriteObject.
		public object ReadObject()
		{
			ObjType t = (ObjType)ReadByte();
			switch (t)
			{
				case ObjType.boolType: return ReadBoolean();
				case ObjType.byteType: return ReadByte();
				case ObjType.uint16Type: return ReadUInt16();
				case ObjType.uint32Type: return ReadUInt32();
				case ObjType.uint64Type: return ReadUInt64();
				case ObjType.sbyteType: return ReadSByte();
				case ObjType.int16Type: return ReadInt16();
				case ObjType.int32Type: return ReadInt32();
				case ObjType.int64Type: return ReadInt64();
				case ObjType.charType: return ReadChar();
				case ObjType.stringType: return base.ReadString();
				case ObjType.singleType: return ReadSingle();
				case ObjType.doubleType: return ReadDouble();
				case ObjType.decimalType: return ReadDecimal();
				case ObjType.dateTimeType: return ReadDateTime();
				case ObjType.byteArrayType: return ReadByteArray();
				case ObjType.charArrayType: return ReadCharArray();
				case ObjType.otherType: return new BinaryFormatter().Deserialize(BaseStream);
				default: return null;
			}
		}
	} 
}
