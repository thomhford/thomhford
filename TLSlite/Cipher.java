import javax.crypto.Mac;
import javax.crypto.SecretKey;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.io.ByteArrayOutputStream;
import java.util.Arrays;

public class Cipher {

    /**
     * Exception to be thrown if message is not correctly authenticated. Used in the decrypt method.
     */
    public static class IncorrectHMACException extends Exception {
        public IncorrectHMACException(String errorMessage) {
            super(errorMessage);
        }
    }

    /**
     * Used to encrypt messages for transmission
     * @param messageToEncrypt what will be encrypted.
     * @param myMACkey what will be used to include authentication in the message being sent.
     * @param myEncryptKey the key used to encrypt the message
     * @param myIV IV added during encryption to increase security of encryption
     * @return and encrypted byte array
     */
    public static byte[] encrypt(byte[] messageToEncrypt, SecretKey myMACkey, SecretKeySpec myEncryptKey,
                          IvParameterSpec myIV) throws Exception {
        // write message to byte buffer to help with HMAC concatenation
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream( );
        outputStream.write(messageToEncrypt);

        // MAC the message to be checked on the other side and concatenate
        byte[] hmacMessage = hmacEncoder(myMACkey, messageToEncrypt);
        outputStream.write(hmacMessage);

        // Encrypt the message
        javax.crypto.Cipher cipher = javax.crypto.Cipher.getInstance("AES/CBC/PKCS5Padding");
        cipher.init(javax.crypto.Cipher.ENCRYPT_MODE, myEncryptKey, myIV);

        return cipher.doFinal(outputStream.toByteArray());

    }

    /**
     * Used to decrypt messages from transmission
     * @param encryptedMessage what will be decrypted
     * @param theirMACkey what will be checked against to ensure authentication in the message being received.
     * @param theirEncryptKey key used to decrypt the encrypted message
     * @param theirIV what was used to increase security of message
     * @return a decrypted message
     * @throws IncorrectHMACException if the authentication does not match
     */
    public static byte[] decrypt(byte[] encryptedMessage, SecretKeySpec theirMACkey, SecretKeySpec theirEncryptKey,
                          IvParameterSpec theirIV) throws Exception {
        // Decrypt message
        javax.crypto.Cipher cipher = javax.crypto.Cipher.getInstance("AES/CBC/PKCS5Padding");
        cipher.init(javax.crypto.Cipher.DECRYPT_MODE, theirEncryptKey, theirIV);
        byte[] decryptedMessageWithHMAC = cipher.doFinal(encryptedMessage);

        // Removed appended HMAC
        byte[] hmacMessage = Arrays.copyOfRange(decryptedMessageWithHMAC, decryptedMessageWithHMAC.length - 32,
                decryptedMessageWithHMAC.length);
        byte[] decryptedMessageBytes = Arrays.copyOf(decryptedMessageWithHMAC,
                decryptedMessageWithHMAC.length - 32);

        // Check appended HMAC
        byte[] hmacMessageCheck = hmacEncoder(theirMACkey, decryptedMessageBytes);
        if (!Arrays.equals(hmacMessage, hmacMessageCheck)){
            throw new IncorrectHMACException("Cannot Authenticate Message!");
        }

        // Return decrypted message
        return decryptedMessageBytes;

    }

    /**
     * Helper method that computes HMAC of a string using the MACKey
     * @param MACkey What will be used to compute the HMAC
     * @param whatToHMAC What will be HMACed
     * @return the HMAC
     */
    public static byte[] hmacEncoder(SecretKey MACkey, byte[] whatToHMAC) throws Exception {
        Mac hmac = Mac.getInstance("HmacSHA256");
        hmac.init(MACkey);
        return hmac.doFinal(whatToHMAC);
    }
}
