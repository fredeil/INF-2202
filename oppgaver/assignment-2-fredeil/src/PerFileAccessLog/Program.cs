using System;
using System.IO;
using System.Text;

namespace PerFileAccessLog
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 2)
                throw new ArgumentException();

            if (!int.TryParse(args[0], out int sizeInMb) || sizeInMb < 1)
                throw new ArgumentNullException(nameof(sizeInMb));

            string outputFile = args[1];

            long bytes = 0;
            var random = new Random();
            var date = new DateTime(2018, 1, 1);
            int sizeInBytes = sizeInMb * 1024 * 1024;

            using (FileStream outfile = File.Open(outputFile, FileMode.CreateNew))
            {
                while (bytes < sizeInBytes)
                {
                    int numEntriesForDate = random.Next(0, 500000);
                    for (int i = 0; i < numEntriesForDate; i++)
                    {
                        date = date.AddHours(1);
                        var logEntry = new PerFileLogEntry(date);
                        byte[] entryBytes = Encoding.UTF8.GetBytes(logEntry.ToString());
                        outfile.Write(entryBytes, 0, entryBytes.Length);

                        bytes += entryBytes.Length;
                        if (bytes >= sizeInBytes)
                            break;
                    }
                }
            }
        }
    }
}
