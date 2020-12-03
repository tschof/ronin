using System;
using System.IO;
using System.Runtime.Serialization;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace SerializationEx
{
	public class CustomBinaryFormatter : IFormatter
	{
		private SerializationBinder m_Binder;
		private StreamingContext m_StreamingContext;
		private ISurrogateSelector m_SurrogateSelector;
		private readonly MemoryStream m_WriteStream;
		private MemoryStream m_IndexWriteStream;
		private readonly MemoryStream m_ReadStream;
		private readonly BinaryWriter m_Writer;
		private readonly BinaryReader m_Reader;
		private readonly Dictionary<Type, int> m_ByType = new Dictionary<Type, int>();
		private readonly Dictionary<int, Type> m_ById = new Dictionary<int, Type>();
		private readonly byte[] m_LengthBuffer = new byte[4];
		private readonly byte[] m_CopyBuffer;
		private Stream indexStream = null;
		private int count = 0;

		public CustomBinaryFormatter(Stream indexStream)
		{
			m_CopyBuffer = new byte[20000];
			m_WriteStream = new MemoryStream(10000);
			m_ReadStream = new MemoryStream(10000);
			m_Writer = new BinaryWriter(m_WriteStream);
			m_Reader = new BinaryReader(m_ReadStream);
			m_IndexWriteStream = new MemoryStream(1000 * 4);
			this.indexStream = indexStream;
			if (indexStream != null)
			{
				bool indexReady;
				if (indexStream.Length >= 4)
					if (indexStream.Read(m_LengthBuffer, 0, 4) == 4)
					{
						IntToBytes length = new IntToBytes(m_LengthBuffer[0], m_LengthBuffer[1], m_LengthBuffer[2], m_LengthBuffer[3]);
						count = length.i32;
						indexReady = true;
					}
					else
						indexReady = false;
				else
					indexReady = false;
				if (!indexReady)
				{
					IntToBytes length = new IntToBytes(0);
					indexStream.Position = 0;
					indexStream.WriteByte(length.b0);
					indexStream.WriteByte(length.b1);
					indexStream.WriteByte(length.b2);
					indexStream.WriteByte(length.b3);
				}
				indexStream.Seek(indexStream.Length, SeekOrigin.Begin);
			}
		}

		public void Close()
		{
			if (indexStream != null && count % 1000 != 0)
			{
				byte[] buffer = m_IndexWriteStream.ToArray();
				indexStream.Write(buffer, 0, buffer.Length);
			}
		}

		public void Register<T>(int _TypeId) where T : ICustomBinarySerializable
		{
			m_ById.Add(_TypeId, typeof(T));
			m_ByType.Add(typeof(T), _TypeId);
		}

		public void MoveTo(Stream serializationStream, int Index)
		{
			if (indexStream != null && serializationStream != null)
			{
				if (Index >= 0 && Index * 4 <= (indexStream.Length - 4))
				{
					long pos = indexStream.Position;
					indexStream.Position = 4 + Index * 4;
					if (indexStream.Read(m_LengthBuffer, 0, 4) == 4)
					{
						IntToBytes length = new IntToBytes(m_LengthBuffer[0], m_LengthBuffer[1], m_LengthBuffer[2], m_LengthBuffer[3]);
						serializationStream.Seek(length.i32, SeekOrigin.Begin);
					}
					indexStream.Position = pos;
				}
			}
		}

		public object Deserialize(Stream serializationStream)
		{
			if (serializationStream.Read(m_LengthBuffer, 0, 4) != 4)
				throw new SerializationException("Could not read length from the stream.");
			IntToBytes length = new IntToBytes(m_LengthBuffer[0], m_LengthBuffer[1], m_LengthBuffer[2], m_LengthBuffer[3]);
			//TODO make this support partial reads from stream
			if (serializationStream.Read(m_CopyBuffer, 0, length.i32) != length.i32)
				throw new SerializationException("Could not read " + length + " bytes from the stream.");
			m_ReadStream.Seek(0L, SeekOrigin.Begin);
			m_ReadStream.Write(m_CopyBuffer, 0, length.i32);
			m_ReadStream.Seek(0L, SeekOrigin.Begin);
			int typeid = m_Reader.ReadInt32();
			Type t;
			if (!m_ById.TryGetValue(typeid, out t))
				throw new SerializationException("TypeId " + typeid + " is not a registerred type id");
			object obj = FormatterServices.GetUninitializedObject(t);
			ICustomBinarySerializable deserialize = (ICustomBinarySerializable)obj;
			deserialize.SetDataFrom(m_Reader);
			if (m_ReadStream.Position != length.i32)
				throw new SerializationException("object of type " + t + " did not read its entire buffer during deserialization. This is most likely an inbalance between the writes and the reads of the object.");
			return deserialize;
		}

		public void Serialize(Stream serializationStream, object graph)
		{
			int key;
			if (!m_ByType.TryGetValue(graph.GetType(), out key))
				throw new SerializationException(graph.GetType() + " has not been registered with the serializer");
			ICustomBinarySerializable c = (ICustomBinarySerializable)graph; //this will always work due to generic constraint on the Register
			m_WriteStream.Seek(0L, SeekOrigin.Begin);
			m_Writer.Write((int)key);
			c.WriteDataTo(m_Writer);
			IntToBytes length;
			if (indexStream != null)
			{
				count++;
				if (count % 1000 == 0)
				{
					byte[] buffer = m_IndexWriteStream.ToArray();
					indexStream.Write(buffer, 0, buffer.Length);
					m_IndexWriteStream = new MemoryStream(1000 * 4);
				}
				length = new IntToBytes((int)serializationStream.Position);
				m_IndexWriteStream.WriteByte(length.b0);
				m_IndexWriteStream.WriteByte(length.b1);
				m_IndexWriteStream.WriteByte(length.b2);
				m_IndexWriteStream.WriteByte(length.b3);
			}
			length = new IntToBytes((int)m_WriteStream.Position);
			serializationStream.WriteByte(length.b0);
			serializationStream.WriteByte(length.b1);
			serializationStream.WriteByte(length.b2);
			serializationStream.WriteByte(length.b3);
			serializationStream.Write(m_WriteStream.GetBuffer(), 0, (int)m_WriteStream.Position);
		}

		public ISurrogateSelector SurrogateSelector
		{
			get { return m_SurrogateSelector; }
			set { m_SurrogateSelector = value; }
		}

		public SerializationBinder Binder
		{
			get { return m_Binder; }
			set { m_Binder = value; }
		}

		public StreamingContext Context
		{
			get { return m_StreamingContext; }
			set { m_StreamingContext = value; }
		}
	}
}
