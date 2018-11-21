using System;
using System.Collections.Generic;

namespace Util
{
    public static class ClaimParsing
    {
        public static List<Guid> ExtractClaims(string data)
        {
            ReadOnlySpan<char> utf8Text = data;

            // By setting the capacity we can remove the overhead
            // of reallocating new arrays when the list grows
            var guids = new List<Guid>(data.Length / 36);

            for (int i = 0; i < data.Length; i += 36)
            {
                // Slicing causes no overhad in allocation,
                // and is much faster than Substring/Linq etc
                guids.Add(Guid.Parse(utf8Text.Slice(i, 36)));
            }

            return guids;
        }

        public static List<Guid> ExtractClaims(byte[] data)
        {
            ReadOnlySpan<byte> utf8Text = data;

            // By setting the capacity we can remove the overhead
            // of reallocating new arrays when the list grows
            var guids = new List<Guid>(data.Length / 16);

            for (int i = 0; i < data.Length; i += 16)
            {
                guids.Add(new Guid(utf8Text.Slice(i, 16)));
            }

            return guids;
        }
    }
}
