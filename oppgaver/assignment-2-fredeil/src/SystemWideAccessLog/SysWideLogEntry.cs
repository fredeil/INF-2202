using System;
using System.Globalization;
using Bogus;
using Util;

namespace SystemWideFileAccessLog
{
    public class SystemWideLogEntry
    {
        readonly string DateTimeOfAccess;
        readonly string Region;
        readonly string TenantName;
        readonly string UserName;
        readonly string FileName;
        readonly FileAccessStatus Status;
        readonly string Exception;

        readonly Faker faker = new Faker();
        readonly string formatString = "{0}\t{1}\t{2}\t{3}\t{4}\t{5}\t{6}" + Environment.NewLine;

        public SystemWideLogEntry(DateTime date, FileAccessStatus status)
        {
            DateTimeOfAccess = date.Date.ToString("dd.MM.yyyy", CultureInfo.InvariantCulture);
            Region = DataGeneration.GetRandomRegion();
            TenantName = faker.Person.FirstName;
            UserName = faker.Person.UserName;
            FileName = faker.System.FileName("tsv");
            Status = status;

            if (status == FileAccessStatus.Error)
                Exception = faker.System.Exception().GetType().ToString();
        }

        public SystemWideLogEntry(DateTime date) : this(date, randFileAccess)
        {

        }

        public override string ToString()
            => string.Format(formatString, DateTimeOfAccess.Split(" ")[0], Region, TenantName, UserName, FileName, Status, Exception);

        private static FileAccessStatus randFileAccess => new Faker().PickRandom<FileAccessStatus>();
    }

    public enum FileAccessStatus
    {
        OK,
        Error
    }
}
