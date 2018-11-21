using System;
using System.Collections.Generic;
using BenchmarkDotNet.Attributes;
using BenchmarkDotNet.Running;
using Util;

namespace Benchmarking
{
    [MemoryDiagnoser, CoreJob]
    public class ExtractClaimsBenchmarks
    {
        private string Data;
        private byte[] byteData;

        [Params(10, 1000, 10_000, 100_000)]
        public int NumClaims { get; set; }

        [GlobalSetup]
        public void Setup()
        {
            Data = DataGeneration.GenerateStringClaims(NumClaims);
            byteData = DataGeneration.GenerateByteArrayClaims(NumClaims);
        }

        [Benchmark]
        public List<Guid> ExtractClaims() => ClaimParsing.ExtractClaims(Data);

        [Benchmark(Baseline = true)]
        public List<Guid> ExtractClaims_Bytes() => ClaimParsing.ExtractClaims(byteData);
    }

    public class Program
    {
        public static void Main()
        {
            var summary = BenchmarkRunner.Run<ExtractClaimsBenchmarks>();
        }
    }
}
