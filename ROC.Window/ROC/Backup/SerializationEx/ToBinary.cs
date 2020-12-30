using System;
using System.IO;
using System.IO.Compression;
using System.Security.Cryptography;
using System.Runtime.Serialization.Formatters.Binary;
using System.Runtime.Serialization.Formatters.Soap;

using System.Diagnostics;

namespace SerializationEx
{
	public enum SerializationTypes
	{
		Normal,
		Compress,
		Encypt,
		EncyptThenCompress,
		CompressThenEncypt,
	}

	public class ToBinary
	{
		public ToBinary()
		{
		}

		#region - Serialization Functions -

		public byte[] Serialize(object obj)
		{
			return Serialize(obj, SerializationTypes.Normal, null, null);
		}
		public byte[] Serialize(object obj, SerializationTypes type)
		{
			return Serialize(obj, type, null, null);
		}
		public byte[] Serialize(object obj, SerializationTypes type, byte[] key, byte[] iv)
		{
			byte[] buffer = new byte[0];
			MemoryStream memStream = new MemoryStream();

			switch (type)
			{
				case SerializationTypes.Compress:
					buffer = Compress(obj);
					break;
				case SerializationTypes.CompressThenEncypt:
					buffer = Encrypt(Compress(obj), key, iv);
					break;
				case SerializationTypes.Encypt:
					buffer = Encrypt(obj, key, iv);
					break;
				case SerializationTypes.EncyptThenCompress:
					buffer = Compress(Encrypt(obj, key, iv));
					break;
				case SerializationTypes.Normal:
				default:
					new BinaryFormatter().Serialize(memStream, obj);
					buffer = memStream.ToArray();
					break;
			}

			return buffer;
		}

		public object Deserialize(byte[] input)
		{
			return Deserialize(input, SerializationTypes.Normal, null, null);
		}
		public object Deserialize(byte[] input, SerializationTypes type)
		{
			return Deserialize(input, type, null, null);
		}
		public object Deserialize(byte[] input, SerializationTypes type, byte[] key, byte[] iv)
		{
			BinaryFormatter binFormatter = new BinaryFormatter();
			MemoryStream memStream = new MemoryStream();

			switch (type)
			{
				case SerializationTypes.Compress:
					memStream = new MemoryStream(Decompress(input));
					break;
				case SerializationTypes.CompressThenEncypt:
					memStream = new MemoryStream(Decompress(Decrypt(input, key, iv)));
					break;
				case SerializationTypes.Encypt:
					memStream = new MemoryStream(Decrypt(input, key, iv));
					break;
				case SerializationTypes.EncyptThenCompress:
					memStream  = new MemoryStream(Decrypt(Decompress(input), key, iv));
					break;
				case SerializationTypes.Normal:
				default:
					memStream = new MemoryStream(input);
					break;
			}

			memStream.Position = 0;
			try
			{
				return new BinaryFormatter().Deserialize(memStream);
			}
			catch (Exception ex)
			{
				Debug.Assert(false, ex.Message, ex.StackTrace);
				return null;
			}
		}

		#endregion

		#region - Compression Functions -

		private byte[] Compress(object obj)
		{
			BinaryFormatter binFormatter = new BinaryFormatter();
			MemoryStream memStream = new MemoryStream();
			using (GZipStream zipStream = new GZipStream(memStream, CompressionMode.Compress))
			{
				binFormatter.Serialize(zipStream, obj);
			}

			return memStream.ToArray();
		}

		private byte[] Compress(byte[] input)
		{
			MemoryStream memStream = new MemoryStream();
			using (GZipStream zipStream = new GZipStream(memStream, CompressionMode.Compress))
			{
				zipStream.Write(input, 0, input.Length);
			}

			return memStream.ToArray();
		}
		
		private byte[] Decompress(byte[] input)
		{
			byte[] buffer;
			int totalBytes = 0;

			#region - Get Uncompressed Size -

			MemoryStream memStream = new MemoryStream(input);
			using (GZipStream gzipStream = new GZipStream(memStream, CompressionMode.Decompress))
			{
				totalBytes = GetOrigionalSize(gzipStream);
			}

			#endregion

			#region - Uncompress -

			buffer = new byte[totalBytes];
			memStream = new MemoryStream(input);
			using (GZipStream gzipStream = new GZipStream(memStream, CompressionMode.Decompress))
			{
				gzipStream.Read(buffer, 0, totalBytes);
			}

			#endregion

			return buffer;
		}

		#endregion

		#region - Encryption Functions -

		private byte[] Encrypt(object obj, byte[] key, byte[] iv)
		{
			BinaryFormatter binFormatter = new BinaryFormatter();
			MemoryStream memStream = new MemoryStream();

			Rijndael rij = Rijndael.Create();
			using (CryptoStream cyptStream = new CryptoStream(memStream, rij.CreateEncryptor(key, iv), CryptoStreamMode.Write))
			{
				binFormatter.Serialize(cyptStream, obj);
				cyptStream.FlushFinalBlock();
			}

			return memStream.ToArray();
		}
		
		private byte[] Encrypt(byte[] input, byte[] key, byte[] iv)
		{
			MemoryStream memStream = new MemoryStream();

			Rijndael rij = Rijndael.Create();
			using (CryptoStream cyptStream = new CryptoStream(memStream, rij.CreateEncryptor(key, iv), CryptoStreamMode.Write))
			{
				cyptStream.Write(input, 0, input.Length);
			}

			input = memStream.ToArray();
			return input;
		}

		private byte[] Decrypt(byte[] input, byte[] key, byte[] iv)
		{
			byte[] buffer;
			int totalBytes = 0;

			Rijndael rij = Rijndael.Create();

			#region - Get Unencypted Size -

			MemoryStream memStream = new MemoryStream(input);
			using (CryptoStream cyptStream = new CryptoStream(memStream, rij.CreateDecryptor(key, iv), CryptoStreamMode.Read))
			{
				totalBytes = GetOrigionalSize(cyptStream);
			}

			#endregion

			#region - Decypt -

			buffer = new byte[totalBytes];
			memStream = new MemoryStream(input);
			using (CryptoStream cyptStream = new CryptoStream(memStream, rij.CreateDecryptor(key, iv), CryptoStreamMode.Read))
			{
				cyptStream.Read(buffer, 0, totalBytes);
			}

			#endregion

			return buffer;
		}

		#endregion

		#region - Helper Functions -

		private int GetOrigionalSize(Stream stream)
		{
			return GetOrigionalSize(stream, 1000);
		}
		private int GetOrigionalSize(Stream stream, int size)
		{
			int totalBytes = 0;

			byte[] smallBuffer = new byte[size];
			int bytesRead = stream.Read(smallBuffer, 0, size);
			while (bytesRead > 0)
			{
				totalBytes += bytesRead;
				bytesRead = stream.Read(smallBuffer, 0, size);
			}

			return totalBytes;
		}

		#endregion
	}
}
