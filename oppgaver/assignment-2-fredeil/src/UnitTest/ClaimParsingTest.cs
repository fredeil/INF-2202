using System.Linq;
using Util;
using Xunit;

namespace UtilTest
{
    public class ClaimParsingTest
    {
        [Theory]
        [InlineData(1)]
        [InlineData(10)]
        [InlineData(100)]
        public void ExtractClaimsString_Returns_Right_Amount_Of_Guids(int numclaims)
        {
            var guids = DataGeneration.GenerateStringClaims(numclaims);

            var expected = numclaims;
            var actual = ClaimParsing.ExtractClaims(guids).Count();

            Assert.Equal(expected, actual);
        }

        [Theory]
        [InlineData(1)]
        [InlineData(10)]
        [InlineData(100)]
        public void ExtractClaimsBytes_Returns_Right_Amount_Of_Guids(int numclaims)
        {
            var guids = DataGeneration.GenerateByteArrayClaims(numclaims);

            var expected = numclaims;
            var actual = ClaimParsing.ExtractClaims(guids).Count();

            Assert.Equal(expected, actual);
        }
    }
}
