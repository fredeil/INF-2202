using System;
using System.Globalization;
using System.IO;
using System.Text;

namespace SystemWideFileAccessLog
{
    class Program
    {
        static void Main(string[] args)
        {
            int sizeInMb = 2;
            if (args.Length > 0)
                if (!int.TryParse(args[0], out sizeInMb) || sizeInMb < 1)
                    throw new ArgumentException(nameof(sizeInMb));

            Console.WriteLine($"---------\n\nGenerating {sizeInMb}Mb of data....\n\n---------");

            int lines = 1;
            long bytes = 0;
            int errors = 1;
            var random = new Random();
            var date = new DateTime(2018, 1, 1);
            var sizeInBytes = sizeInMb * 1024 * 1024;
            var datestring = DateTime.Now.ToString("MM-dd-yyyy-mm", CultureInfo.InvariantCulture);

            using (var outfile = File.Open("data_" + datestring + ".tsv", FileMode.Create))
            {
                while (bytes < sizeInBytes)
                {
                    int numEntriesForDate = random.Next(0, 500000);
                    for (int i = 0; i < numEntriesForDate; i++)
                    {
                        date = date.AddHours(1);
                        var logEntry = new SystemWideLogEntry(date);
                        if (logEntry.ToString().Contains("Error"))
                            errors++;

                        byte[] entryBytes = Encoding.UTF8.GetBytes(logEntry.ToString());

                        outfile.Write(entryBytes, 0, entryBytes.Length);
                        lines++;

                        bytes += entryBytes.Length;

                        if (i % 250 == 0)
                            Console.WriteLine($"[N={lines}] :: {logEntry.ToString()}");

                        if (bytes >= sizeInBytes)
                            break;
                    }
                }
            }

            Console.WriteLine($"---------\n\nFinished with {lines} lines and {errors} errors\n\n---------");
            Console.ReadKey();
        }
    }
}
