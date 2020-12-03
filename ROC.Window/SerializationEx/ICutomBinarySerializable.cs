using System;
using System.IO;

namespace SerializationEx
{
    public interface ICustomBinarySerializable
    {
        void WriteDataTo(BinaryWriter writer);
        void SetDataFrom(BinaryReader reader);
    }
}
