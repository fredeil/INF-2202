using System.IO;

namespace Util
{
    public static class BinaryReaderExtensions
    {
        private const int BufferSize = 8192;
        public static byte[] ReadAllBytes(this BinaryReader reader)
        {
            using (var memoryStream = new MemoryStream())
            {
                byte[] buffer = new byte[BufferSize];
                int count;
                while ((count = reader.Read(buffer, 0, buffer.Length)) != 0)
                {
                    memoryStream.Write(buffer, 0, count);
                }

                return memoryStream.ToArray();
            }
        }
    }
}
