using System;
using System.Text;

namespace Util
{
    public static class DataGeneration
    {
        private static readonly Random random = new Random();
        private static readonly string[] regions = { "EUR", "CAN", "JPN", "US", "CHN", "GER", "APAC", "AUS" };

        public static string GetRandomRegion() => regions[random.Next(0, regions.Length - 1)];

        public static string GenerateStringClaims(int numClaims)
        {
            if (numClaims < 1)
                throw new ArgumentOutOfRangeException(nameof(numClaims));

            var sb = new StringBuilder(numClaims * 36);

            for (var i = 0; i < numClaims; i++)
                sb.Append(Guid.NewGuid().ToString());

            return sb.ToString();
        }

        public static byte[] GenerateByteArrayClaims(int numClaims)
        {
            if (numClaims < 1)
                throw new ArgumentOutOfRangeException(nameof(numClaims));

            var buffer = new byte[numClaims * 16];

            for (int i = 0; i < numClaims * 16; i += 16)
                Array.Copy(Guid.NewGuid().ToByteArray(), 0, buffer, i, 16);

            return buffer;
        }
    }
}
