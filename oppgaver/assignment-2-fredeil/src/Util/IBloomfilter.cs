using System;

namespace Util
{
    /// <summary>
    /// The interface of a bloom filter.
    /// You need to make an implementation of this interface in exercise 3.
    /// </summary>
    public interface IBloomfilter<T>
    {
        /// <summary>
        /// Adds a item to the bloom filter.
        /// </summary>
        /// <param name="item">The item to be added.</param>
        void Add(T item);

        /// <summary>
        /// Returns whether or not an item is contained in the bloom filter.
        /// </summary>
        /// <param name="item">The item to be checked for.</param>
        /// <returns>True if item is present, false if not.</returns>
        bool Contains(T guid);
    }
}
