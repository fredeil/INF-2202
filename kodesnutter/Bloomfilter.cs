namespace BloomFilter
{
	using System;
	using System.Collections;

	/// <summary>
	/// Bloom filter.
	/// </summary>
	/// <typeparam name="T">Item type </typeparam>
	public class Filter<T>
	{
		private readonly int _hashFunctionCount;
		private readonly BitArray _hashBits;
		private readonly HashFunction _getHashSecondary;

        /// <summary>
		/// A function that can be used to hash input.
		/// </summary>
		/// <param name="input">The values to be hashed.</param>
		/// <returns>The resulting hash code.</returns>
		public delegate int HashFunction(T input);

		/// <summary>
		/// Creates a new Bloom filter.
		/// </summary>
		/// <param name="capacity">The anticipated number of items to be added to the filter. 
        /// More than this number of items can be added, but the error rate will exceed what is expected.</param>
		/// <param name="hashFunction">The function to hash the input values.</param>
		/// <param name="m">The number of elements in the BitArray.</param>
		/// <param name="k">The number of hash functions to use.</param>
		public Filter(int capacity, HashFunction hashFunction, int m, int k)
		{
			// set the secondary hash function
			if (hashFunction == null)
			{
				if (typeof(T) == typeof(string))
				{
					this._getHashSecondary = HashString;
				}
				else if (typeof(T) == typeof(int))
				{
					this._getHashSecondary = HashInt32;
				}
				else
				{
					throw new ArgumentException("hashFunction", "Please provide a hash function for your type T, when T is not a string or int.");
				}
			}
			else
			{
				this._getHashSecondary = hashFunction;
			}

			this._hashFunctionCount = k;
			this._hashBits = new BitArray(m);
		}

		/// <summary>
		/// Adds a new item to the filter. It cannot be removed.
		/// </summary>
		/// <param name="item">The item.</param>
		public void Add(T item)
		{
			// start flipping bits for each hash of item
			int primaryHash = item.GetHashCode();
			int secondaryHash = this._getHashSecondary(item);
			for (int i = 0; i < this._hashFunctionCount; i++)
			{
				int hash = this.ComputeHash(primaryHash, secondaryHash, i);
				this._hashBits[hash] = true;
			}
		}

		/// <summary>
		/// Checks for the existance of the item in the filter for a given probability.
		/// </summary>
		/// <param name="item"> The item. </param>
		/// <returns> The <see cref="bool"/>. </returns>
		public bool Contains(T item)
		{
			int primaryHash = item.GetHashCode();
			int secondaryHash = this._getHashSecondary(item);
			for (int i = 0; i < this._hashFunctionCount; i++)
			{
				int hash = this.ComputeHash(primaryHash, secondaryHash, i);
				if (this._hashBits[hash] == false)
				{
					return false;
				}
			}

			return true;
		}

		/// <summary>
		/// Hashes a 32-bit signed int using Thomas Wang's method v3.1 (http://www.concentric.net/~Ttwang/tech/inthash.htm).
		/// Runtime is suggested to be 11 cycles. 
		/// </summary>
		/// <param name="input">The integer to hash.</param>
		/// <returns>The hashed result.</returns>
		private static int HashInt32(T input)
		{
			uint? x = input as uint?;
			unchecked
			{
				x = ~x + (x << 15); // x = (x << 15) - x- 1, as (~x) + y is equivalent to y - x - 1 in two's complement representation
				x = x ^ (x >> 12);
				x = x + (x << 2);
				x = x ^ (x >> 4);
				x = x * 2057; // x = (x + (x << 3)) + (x<< 11);
				x = x ^ (x >> 16);
				return (int)x;
			}
		}

		/// <summary>
		/// Hashes a string using Bob Jenkin's "One At A Time" method from Dr. Dobbs (http://burtleburtle.net/bob/hash/doobs.html).
		/// Runtime is suggested to be 9x+9, where x = input.Length. 
		/// </summary>
		/// <param name="input">The string to hash.</param>
		/// <returns>The hashed result.</returns>
		private static int HashString(T input)
		{
			string s = input as string;
			int hash = 0;

			for (int i = 0; i < s.Length; i++)
			{
				hash += s[i];
				hash += (hash << 10);
				hash ^= (hash >> 6);
			}

			hash += (hash << 3);
			hash ^= (hash >> 11);
			hash += (hash << 15);
			return hash;
		}

		/// <summary>
		/// Performs Dillinger and Manolios double hashing. 
		/// </summary>
		/// <param name="primaryHash"> The primary hash. </param>
		/// <param name="secondaryHash"> The secondary hash. </param>
		/// <param name="i"> The i. </param>
		/// <returns> The <see cref="int"/>. </returns>
		private int ComputeHash(int primaryHash, int secondaryHash, int i)
		{
			int resultingHash = (primaryHash + (i * secondaryHash)) % this._hashBits.Count;
			return Math.Abs((int)resultingHash);
		}
	}
}