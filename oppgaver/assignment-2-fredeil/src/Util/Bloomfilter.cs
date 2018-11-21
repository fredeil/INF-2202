using System;

namespace Util
{
    public class Bloomfilter<T> : IBloomfilter<T>
    {
        private readonly byte[] hashBits;

        public Bloomfilter(int capacity)
        {
            if (capacity < 1)
                throw new ArgumentException(nameof(capacity));

            hashBits = new byte[capacity];
        }

        public void Add(T guid)
        {
            int hash = Hash(guid.ToString()) & 0x7FFFFFFF;
            byte bit = (byte)(1 << (hash & 7));
            hashBits[hash % hashBits.Length] |= bit;
        }

        public bool Contains(T guid)
        {
            int hash = Hash(guid.ToString()) & 0x7FFFFFFF;
            byte bit = (byte)(1 << (hash & 7));
            return (hashBits[hash % hashBits.Length] & bit) != 0;
        }


        // totally stole this online and modified it
        /// <summary>
        /// Hashes a string using Bob Jenkin's "One At A Time" method
        /// from Dr. Dobbs (http://burtleburtle.net/bob/hash/doobs.html).
        /// Runtime around to be 9x+9, where x = input.Length.
        /// </summary>
        private static int Hash(string input)
        {
            int hash = 0;

            for (int i = 0; i < input.Length; i++)
            {
                hash += input[i];
                hash += (hash << 10);
                hash ^= (hash >> 6);
            }

            hash += (hash << 3);
            hash ^= (hash >> 11);
            hash += (hash << 15);
            return hash;
        }
    }
}
