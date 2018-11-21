using System;
using System.Collections.Generic;
using System.Globalization;
using Util;

namespace PerFileAccessLog
{
    public class PerFileLogEntry
    {
        private readonly DateTime Date;
        private readonly string Region;
        private readonly string TenantName;
        private readonly string FileName;
        private readonly string ModifyingUsers;
        private readonly DateTime FirstAccess;
        private readonly DateTime LastAccess;
        private static readonly Bogus.DataSets.Company FakeCompanies = new Bogus.DataSets.Company();
        private static readonly Bogus.DataSets.System FakeSystem = new Bogus.DataSets.System();
        private static readonly Bogus.DataSets.Internet FakeInternet = new Bogus.DataSets.Internet();
        private readonly string formatString = "{0:s}\t{1}\t{2}\t{3}\t{4}\t{5:s}\t{6:s}" + Environment.NewLine;

        public PerFileLogEntry(DateTime date)
        {
            var random = new Random();
            Date = date.Date;
            Region = DataGeneration.GetRandomRegion();
            TenantName = FakeCompanies.CompanyName();
            FileName = FakeSystem.FileName();
            ModifyingUsers = GenerateUsersString();
            FirstAccess = Date.AddHours(random.Next(0, 6))
                              .AddMinutes(random.Next(0, 59))
                              .AddSeconds(random.Next(0, 59));
            LastAccess = Date.AddHours(random.Next(11, 23))
                             .AddMinutes(random.Next(0, 59))
                             .AddSeconds(random.Next(0, 59));
        }

        public override string ToString()
        {
            return string.Format(formatString, Date.ToString("dd.MM.yyyy", CultureInfo.InvariantCulture).Split(" ")[0], Region, TenantName, FileName,
                ModifyingUsers, FirstAccess.ToString("dd.MM.yyyy", CultureInfo.InvariantCulture).Split(" ")[0], LastAccess.ToString("dd.MM.yyyy", CultureInfo.InvariantCulture).Split(" ")[0]);
        }

        private static string GenerateUsersString()
        {
            var random = new Random();
            int numUsers = random.Next(1, 25);
            return string.Join(",", UserEmails(numUsers));
        }

        private static IEnumerable<string> UserEmails(int numUsers)
        {
            for (int i = 0; i < numUsers; i++)
                yield return FakeInternet.Email();
        }
    }
}
