import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.math.BigInteger;
import java.util.Arrays;

public class SymmetricKeys {
    byte[] prk; // pseudo-random key, used to extract "randomness"
    SecretKeySpec serverEncrypt; // "write key"
    SecretKeySpec clientEncrypt; // "write key"
    SecretKeySpec serverMAC; // Message authentication code
    SecretKeySpec clientMAC; // Message authentication code
    IvParameterSpec serverIV; // Initialization vector
    IvParameterSpec clientIV; // Initialization vector

    /**
     * Constructor for symmetric keys used by both client and server. Will be created and used on both the client
     * and server sides to authenticate and encrypt / decrypt messages. Uses a key derivation function to create
     * SecretKeySpec. These SecretKeySpec must be marked as using to "AES" algorithm to allow for use in encryption
     * and decryption
     * @param clientNonce used to create a secret key spec used to create the symmetric keys
     * @param DHSharedSecret will be the same for both client and server so convenient to use to create keys
     */
    SymmetricKeys(byte[] clientNonce, BigInteger DHSharedSecret)throws Exception{
        SecretKeySpec secretKeySpec = new SecretKeySpec(clientNonce,"RawBytes");

        prk = Cipher.hmacEncoder(secretKeySpec, DHSharedSecret.toByteArray()); // "Randomness Extractor"
        serverEncrypt = new SecretKeySpec(hKDFExpand(prk,"server Encrypt"), "AES");
        clientEncrypt = new SecretKeySpec(hKDFExpand(serverEncrypt.getEncoded(), "client Encrypt"), "AES");
        serverMAC = new SecretKeySpec(hKDFExpand(clientEncrypt.getEncoded(),"server MAC"), "AES");
        clientMAC = new SecretKeySpec(hKDFExpand(serverMAC.getEncoded(),"client MAC"), "AES");
        serverIV = new IvParameterSpec(hKDFExpand(clientMAC.getEncoded(),"server IV"));
        clientIV = new IvParameterSpec(hKDFExpand(serverIV.getIV(),"client IV"));
    }

    /**
     * Key Derivation Function
     * @param key initially starts with a secret key created using the Diffie-Hellman shared secret then creates a
     *            new key using the previously created key and a "tag"
     * @param tag this is a string that differentiates each key. This tag could be whatever, but it is convenient to
     *            have the "tag" match what key is being created. "tag" needs to be consistent between server and client
     *            so symmetricKeys can be recreated on both sides
     * @return the first 16 bytes of the 32 byte HMAC
     */
    private byte[] hKDFExpand(byte[] key, String tag) throws Exception {
        SecretKeySpec secretKeySpec = new SecretKeySpec(key,"RawBytes");
        byte[] tagConcatenate = stringToBytesPlusOne(tag);
        byte[] hmacOut = Cipher.hmacEncoder(secretKeySpec, tagConcatenate);
        return Arrays.copyOf(hmacOut,16); // Only need first 16 bytes
    }

    /**
     * Converts strings to byte[]
     * Plus one is needed to ensure that a key value is never empty allowing for the next key to be created
     */
    private byte[] stringToBytesPlusOne(String tag){
        byte[] tagBytes = tag.getBytes();
        byte[] plusOne = new byte[]{1};
        byte[] ret = new byte[tagBytes.length + 1];
        System.arraycopy(tagBytes,0,ret,0,tagBytes.length);
        System.arraycopy(plusOne,0,ret,tagBytes.length,1);
        return ret;
    }

    /**
     * For checking that symmetric keys match. Cannot be done in practice so not useful for TSLlite.
     * Only useful in testing
     */
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (!(o instanceof SymmetricKeys that)) return false;
        return Arrays.equals(prk, that.prk)
            && Arrays.equals(serverEncrypt.getEncoded(), that.serverEncrypt.getEncoded())
            && Arrays.equals(clientEncrypt.getEncoded(), that.clientEncrypt.getEncoded())
            && Arrays.equals(serverMAC.getEncoded(), that.serverMAC.getEncoded())
            && Arrays.equals(clientMAC.getEncoded(), that.clientMAC.getEncoded())
            && Arrays.equals(serverIV.getIV(), that.serverIV.getIV())
            && Arrays.equals(clientIV.getIV(), that.clientIV.getIV());
    }

}
