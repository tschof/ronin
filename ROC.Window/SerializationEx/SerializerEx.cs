using System;
using System.IO;
using System.Collections.Generic;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace SerializationEx
{
	internal enum TypeEnum : byte
	{
		nullType = 0,
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

	public class SerializationWriter
	{
		private const int STREAM_SIZE = 10240;

		private readonly BinaryWriter _writer;

		public SerializationWriter()
		{
			_writer = new BinaryWriter(new MemoryStream(STREAM_SIZE));
		}

		// Writes a string to the buffer.  
		// Overrides the base implementation so it can cope with nulls
		public void Write(string str)
		{
			if (str == null)
			{
				_writer.Write((byte)TypeEnum.nullType);
			}
			else
			{
				_writer.Write((byte)TypeEnum.stringType);
				_writer.Write(str);
			}
		}

		// Writes a byte array to the buffer.  
		// Overrides the base implementation to send the length of the array which is needed when it is retrieved.
		public void Write(byte[] b)
		{
			if (b == null) {
				_writer.Write((byte)TypeEnum.nullType);
			} else {
				int len = b.Length;
				_writer.Write(len);
				if (len > 0) _writer.Write(b);
			}
		}

		// Writes a char array to the buffer.  
		// Overrides the base implementation to sends the length of the array which is needed when it is read.
		public void Write(char[] c)
		{
			if (c == null) {
				_writer.Write((byte)TypeEnum.nullType);
			} else {
				int len = c.Length;
				_writer.Write(len);
				if (len > 0) _writer.Write(c);
			}
		}

		// Writes a generic ICollection (such as an IList<T>) to the buffer.
		public void Write<T>(ICollection<T> c)
		{
			if (c == null) {
				_writer.Write(-1);
			} else {
				_writer.Write(c.Count);
				foreach (T item in c) WriteObject(item);
			}
		}

		// Writes a generic IDictionary to the buffer.
		public void Write<T, U>(IDictionary<T, U> d)
		{
			if (d == null) {
				_writer.Write(-1);
			} else {
				_writer.Write(d.Count);
				foreach (KeyValuePair<T, U> kvp in d) {
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
			if (obj == null) {
				_writer.Write((byte)TypeEnum.nullType);
				return;
			}

			switch (obj) {
				case bool boolval: _writer.Write((byte)TypeEnum.boolType); _writer.Write(boolval); break;
				case byte byteval: _writer.Write((byte)TypeEnum.byteType); _writer.Write(byteval); break;
				case UInt16 uint16val: _writer.Write((byte)TypeEnum.uint16Type); _writer.Write(uint16val); break;
				case UInt32 uint32val: _writer.Write((byte)TypeEnum.uint32Type); _writer.Write(uint32val); break;
				case UInt64 uint64val: _writer.Write((byte)TypeEnum.uint64Type); _writer.Write(uint64val); break;
				case SByte sbyteval: _writer.Write((byte)TypeEnum.sbyteType); _writer.Write(sbyteval); break;
				case Int16 int16val: _writer.Write((byte)TypeEnum.int16Type); _writer.Write(int16val); break;
				case Int32 int32val: _writer.Write((byte)TypeEnum.int32Type); _writer.Write(int32val); break;
				case Int64 int64val: _writer.Write((byte)TypeEnum.int64Type); _writer.Write(int64val); break;
				case char charval: _writer.Write((byte)TypeEnum.charType); _writer.Write(charval); break;
				case string stringval: _writer.Write((byte)TypeEnum.stringType); _writer.Write(stringval); break;
				case Single singleval: _writer.Write((byte)TypeEnum.singleType); _writer.Write(singleval); break;
				case double doubleval: _writer.Write((byte)TypeEnum.doubleType); _writer.Write(doubleval); break;
				case decimal decimalval: _writer.Write((byte)TypeEnum.decimalType); _writer.Write(decimalval); break;
				case DateTime dateval: _writer.Write((byte)TypeEnum.dateTimeType); _writer.Write(dateval.Ticks); break;
				case byte[] bytearray: _writer.Write((byte)TypeEnum.byteArrayType); _writer.Write(bytearray); break;
				case char[] chararray: _writer.Write((byte)TypeEnum.charArrayType); _writer.Write(chararray); break;
				default:
					_writer.Write((byte)TypeEnum.otherType);
					new BinaryFormatter().Serialize(_writer.BaseStream, obj);
					break;
			}
		}

		public void Write(int ival)
		{
			_writer.Write(ival);
		}

		public void Write(double dval)
		{
			_writer.Write(dval);
		}

		public void Write(long lval)
		{
			_writer.Write(lval);
		}

		// Adds the SerializationWriter buffer to the SerializationInfo at the end of GetObjectData()
		public void AddToInfo(SerializationInfo info)
		{
			byte[] b = ((MemoryStream)_writer.BaseStream).ToArray();
			info.AddValue("X", b, typeof(byte[]));
		}
	}

	public class SerializationReader
	{
		private BinaryReader _reader;

		public SerializationReader(SerializationInfo info)
		{
			byte[] byteArray = (byte[])info.GetValue("X", typeof(byte[]));
			_reader = new BinaryReader(new MemoryStream(byteArray));
		}

		// Reads a string from the buffer.  
		// Overrides the base implementation so it can cope with nulls.
		public string ReadString()
		{
			TypeEnum t = (TypeEnum)_reader.ReadByte();
			if (t == TypeEnum.stringType) return _reader.ReadString();
			return null;
		}

		// Reads a byte array from the buffer, handling nulls and the array length. 
		public byte[] ReadByteArray()
		{
			int len = _reader.ReadInt32();
			if (len > 0) return _reader.ReadBytes(len);
			if (len < 0) return null;
			return new byte[0];
		}

		// Reads a char array from the buffer, handling nulls and the array length.
		public char[] ReadCharArray()
		{
			int len = _reader.ReadInt32();
			if (len > 0) return _reader.ReadChars(len);
			if (len < 0) return null;
			return new char[0];
		}

		// Reads a DateTime from the buffer.
		public DateTime ReadDateTime() { 
			return new DateTime(_reader.ReadInt64()); 
		}

		// Reads a generic list from the buffer.
		public IList<T> ReadList<T>()
		{
			int count = _reader.ReadInt32();
			if (count < 0) return null;
			IList<T> list = new List<T>();
			for (int i = 0; i < count; i++) list.Add((T)ReadObject());
			return list;
		}

		// Reads a generic Dictionary from the buffer. 
		public IDictionary<T, U> ReadDictionary<T, U>()
		{
			int count = _reader.ReadInt32();
			if (count < 0) return null;
			IDictionary<T, U> d = new Dictionary<T, U>();
			for (int i = 0; i < count; i++) d[(T)ReadObject()] = (U)ReadObject();
			return d;
		}

		public void Read(out int ival)
		{
			ival = _reader.ReadInt32();
		}

		public void Read(out double dval)
		{
			dval = _reader.ReadDouble();
		}

		public void Read(out long lval)
		{
			lval = _reader.ReadInt64();
		}

		// Reads an object which was added to the buffer by WriteObject.
		private object ReadObject()
		{
			TypeEnum t = (TypeEnum)_reader.ReadByte();
			switch (t)
			{
				case TypeEnum.boolType: return _reader.ReadBoolean();
				case TypeEnum.byteType: return _reader.ReadByte();
				case TypeEnum.uint16Type: return _reader.ReadUInt16();
				case TypeEnum.uint32Type: return _reader.ReadUInt32();
				case TypeEnum.uint64Type: return _reader.ReadUInt64();
				case TypeEnum.sbyteType: return _reader.ReadSByte();
				case TypeEnum.int16Type: return _reader.ReadInt16();
				case TypeEnum.int32Type: return _reader.ReadInt32();
				case TypeEnum.int64Type: return _reader.ReadInt64();
				case TypeEnum.charType: return _reader.ReadChar();
				case TypeEnum.stringType: return _reader.ReadString();
				case TypeEnum.singleType: return _reader.ReadSingle();
				case TypeEnum.doubleType: return _reader.ReadDouble();
				case TypeEnum.decimalType: return _reader.ReadDecimal();
				case TypeEnum.dateTimeType: return new DateTime(_reader.ReadInt64());
				case TypeEnum.byteArrayType: return ReadByteArray();
				case TypeEnum.charArrayType: return ReadCharArray();
				case TypeEnum.otherType: return new BinaryFormatter().Deserialize(_reader.BaseStream);
				default: return null;
			}
		}
	} 
}
