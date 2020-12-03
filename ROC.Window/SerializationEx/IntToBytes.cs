using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace SerializationEx
{
	[StructLayout(LayoutKind.Explicit)]
	public struct IntToBytes
	{
		public IntToBytes(Int32 _value) { b0 = b1 = b2 = b3 = 0; i32 = _value; }
		public IntToBytes(byte _b0, byte _b1, byte _b2, byte _b3)
		{
			i32 = 0;
			b0 = _b0;
			b1 = _b1;
			b2 = _b2;
			b3 = _b3;
		}
		[FieldOffset(0)]
		public Int32 i32;
		[FieldOffset(0)]
		public byte b0;
		[FieldOffset(1)]
		public byte b1;
		[FieldOffset(2)]
		public byte b2;
		[FieldOffset(3)]
		public byte b3;
	}
}
