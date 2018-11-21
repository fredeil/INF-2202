using System;
using Util;
using Xunit;

namespace UtilTest
{
    public class DataGenerationTest
    {
        [Theory]
        [InlineData(1)]
        [InlineData(10)]
        [InlineData(100)]
        public void GenerateStringClaims_Generates_Right_Amount_Of_Guids(int numclaims)
        {
            var guids = DataGeneration.GenerateStringClaims(numclaims);

            // With hypens one guid should be 36 chars
            var expected = numclaims * 36;
            var actual = guids.Length;

            Assert.NotNull(guids);
            Assert.Equal(expected, actual);
        }

        [Fact]
        public void GenerateStringClaims_Generates_Throws_Right_Exception()
        {
            Assert.Throws<ArgumentNullException>(()
                => DataGeneration.GenerateStringClaims(0));
        }

        [Theory]
        [InlineData(1)]
        [InlineData(10)]
        [InlineData(100)]
        public void GenerateByteArrayClaims_Generates_Right_Amount_Of_Guids(int numclaims)
        {
            var guids = DataGeneration.GenerateByteArrayClaims(numclaims);

            // lets just do it he microsoft way
            // https://github.com/dotnet/corefx/blob/master/src/Common/src/CoreLib/System/Guid.cs#L50
            var actual = guids.Length;
            var expected = numclaims * 16;

            Assert.NotNull(guids);
            Assert.Equal(expected, actual);
        }

        [Fact]
        public void GenerateByteArrayClaims_Generates_Throws_Right_Exception()
        {
            Assert.Throws<ArgumentNullException>(()
                => DataGeneration.GenerateByteArrayClaims(0));
        }
    }
}
