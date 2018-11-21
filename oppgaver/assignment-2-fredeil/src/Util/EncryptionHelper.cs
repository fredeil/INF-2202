using System;
using System.IO;
using System.Security.Cryptography;

namespace Util
{
    public static class EncryptionHelper
    {
        public const int KeySize = 256;
        public const int BlockSize = 128;
        private const CipherMode CipherMode = System.Security.Cryptography.CipherMode.CBC;
        private const PaddingMode PaddingMode = System.Security.Cryptography.PaddingMode.PKCS7;

        public static byte[] Encrypt(byte[] bytesToEncrypt, byte[] secret)
        {
            if (bytesToEncrypt == null)
                throw new ArgumentNullException(nameof(bytesToEncrypt));

            if (secret == null)
                throw new ArgumentNullException(nameof(secret));

            using (AesCryptoServiceProvider aes = CreateAes(secret: secret, iv: null))
            using (ICryptoTransform encryptor = aes.CreateEncryptor(aes.Key, aes.IV))
            using (var memoryStream = new MemoryStream())
            {
                using (var cryptoStream = new CryptoStream(memoryStream, encryptor, CryptoStreamMode.Write))
                    cryptoStream.Write(bytesToEncrypt, 0, bytesToEncrypt.Length);

                byte[] encryptedBytes = memoryStream.ToArray();
                byte[] encryptedContent = new byte[encryptedBytes.Length + aes.IV.Length];
                Array.Copy(aes.IV, 0, encryptedContent, 0, aes.IV.Length);
                Array.Copy(encryptedBytes, 0, encryptedContent, aes.IV.Length, encryptedBytes.Length);

                return encryptedContent;
            }
        }

        public static byte[] Decrypt(byte[] encryptedContent, byte[] secret)
        {
            if (encryptedContent == null)
                throw new ArgumentNullException(nameof(encryptedContent));

            if (secret == null)
                throw new ArgumentNullException(nameof(secret));

            byte[] iv = new byte[BlockSize / 8];
            Array.Copy(encryptedContent, 0, iv, 0, iv.Length);

            using (AesCryptoServiceProvider aes = CreateAes(secret, iv))
            using (ICryptoTransform decryptor = aes.CreateDecryptor(aes.Key, aes.IV))
            using (var decryptMemoryStream = new MemoryStream(encryptedContent, aes.IV.Length, encryptedContent.Length - aes.IV.Length))
            using (var cryptoStream = new CryptoStream(decryptMemoryStream, decryptor, CryptoStreamMode.Read))
            using (var binaryReader = new BinaryReader(cryptoStream))
            {
                return binaryReader.ReadAllBytes();
            }
        }

        private static AesCryptoServiceProvider CreateAes(byte[] secret, byte[] iv)
        {
            var aes = new AesCryptoServiceProvider
            {
                KeySize = KeySize,
                BlockSize = BlockSize,
                Mode = CipherMode,
                Padding = PaddingMode,
            };

            if (iv == null)
                aes.GenerateIV();
            else
                aes.IV = iv;

            using (var sha256 = SHA256.Create())
                aes.Key = sha256.ComputeHash(secret, 0, secret.Length);

            return aes;
        }
    }
}
