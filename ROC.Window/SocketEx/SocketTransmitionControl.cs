using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace SocketEx
{
	#region - API -

	public static class API
	{
		[DllImport("kernel32.dll")]
		public static extern void Sleep(uint dwMilliseconds);

		[DllImport("kernel32.dll")]
		public static extern uint SleepEx(uint dwMilliseconds, bool bAlertable);
	}
	
	#endregion

	// [prefix length of(P)][P=prefix length of(data)][data...]
	public static class SocketTransmitionControl
	{
		public static byte[] AddPrefix(byte[] data)
		{
			byte[] bytArrDataLength = ByteArrayOperations.ConvertLongToByteArray((long)data.Length);
			byte[] bytArrDataLengthPrefix = new byte[1] { (byte)bytArrDataLength.Length };
			
			byte[] newData = new byte[bytArrDataLengthPrefix.Length + bytArrDataLength.Length + data.Length];
			
			bytArrDataLengthPrefix.CopyTo(newData, 0);
			bytArrDataLength.CopyTo(newData, bytArrDataLengthPrefix.Length);
			data.CopyTo(newData, bytArrDataLengthPrefix.Length + bytArrDataLength.Length);
			
			return newData;
		}

		public static bool IsIntegratedDataWaiting(byte[] data)
		{
			bool bIntegratedDataWaiting = false;
			if (data.Length > 1)
			{
				byte bytDataLengthPrefix = data[0];
				if (data.Length > bytDataLengthPrefix)
				{
					byte[] bytArrDataLength = ByteArrayOperations.CopyFromByteArray(data, 1, bytDataLengthPrefix);
					long lDataLength = ByteArrayOperations.ConvertByteArrayToLong(bytArrDataLength);
					if (lDataLength <= data.Length - (bytArrDataLength.Length + 1))
					{
						bIntegratedDataWaiting = true;
					}
				}
			}
			return bIntegratedDataWaiting;
		}

		public static byte[] ExtractIntegratedDataFromBuffer(ref byte[] data)
		{
			byte[] integratedData = new byte[0];
			if (IsIntegratedDataWaiting(data))
			{
				byte bytDataLengthPrefix = data[0];
				byte[] bytArrDataLength = ByteArrayOperations.CopyFromByteArray(data, 1, bytDataLengthPrefix);
				long lDataLength = ByteArrayOperations.ConvertByteArrayToLong(bytArrDataLength);
				
				integratedData = ByteArrayOperations.CopyFromByteArray(data, bytArrDataLength.Length + 1, lDataLength);
				data = ByteArrayOperations.RemoveFromByteArray(data, 0, 1 + bytArrDataLength.Length + lDataLength);
			}
			return integratedData;
		}
	}

	internal static class ByteArrayOperations
	{
		public static byte[] AddByteArrays(byte[] destinationArray, byte[] inputArray)
		{
			if (destinationArray == null)
			{
				destinationArray = new byte[0];
			}
			if (inputArray == null)
			{
				return destinationArray;
			}

			byte[] byteArrayResult = new byte[destinationArray.Length + inputArray.Length];
			destinationArray.CopyTo(byteArrayResult, 0);
			inputArray.CopyTo(byteArrayResult, destinationArray.Length);

			return byteArrayResult;
		}

		public static byte[] CopyFromByteArray(byte[] sourceArray, long startIndex, long count)
		{
			byte[] byteArrayResult = new byte[count];
			Array.Copy(sourceArray, startIndex, byteArrayResult, 0, count);
			return byteArrayResult;
		}

		public static byte[] RemoveFromByteArray(byte[] sourceArray, long startIndex, long count)
		{
			byte[] byteArrayResult = new byte[sourceArray.Length - count];
			Array.Copy(sourceArray, count, byteArrayResult, 0, byteArrayResult.Length);
			return byteArrayResult;
		}

		public static byte[] ConvertLongToByteArray(long number)
		{
			return BitConverter.GetBytes(number);
		}

		public static long ConvertByteArrayToLong(byte[] data)
		{
			return BitConverter.ToInt64(data, 0);
		}
	}
}
