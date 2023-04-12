import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.Signature;
import java.security.cert.Certificate;
import java.security.spec.PKCS8EncodedKeySpec;

public class KeyGeneration {
    /**
     *  2048 bit Diffie-Hellman hexadecimal prime value is:
     */
    private static final String hexN =
            "FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD1" +
            "29024E088A67CC74020BBEA63B139B22514A08798E3404DD" +
            "EF9519B3CD3A431B302B0A6DF25F14374FE1356D6D51C245" +
            "E485B576625E7EC6F44C42E9A637ED6B0BFF5CB6F406B7ED" +
            "EE386BFB5A899FA5AE9F24117C4B1FE649286651ECE45B3D" +
            "C2007CB8A163BF0598DA48361C55D39A69163FA8FD24CF5F" +
            "83655D23DCA3AD961C62F356208552BB9ED529077096966D" +
            "670C354E4ABC9804F1746C08CA18217C32905E462E36CE3B" +
            "E39E772C180E86039B2783A2EC07A28FB5C55DF06F4C52C9" +
            "DE2BCBF6955817183995497CEA956AE515D2261898FA0510" +
            "15728E5A8AACAA68FFFFFFFFFFFFFFFF";

    private static final BigInteger N = new BigInteger(hexN,16);

    /**
     *  2048 bit Diffie-Hellman generator is 2
     */
    private static final BigInteger g = new BigInteger("2");

    /**
     * Diffie-Hellman public key
     */
    public static BigInteger createPublicKey(BigInteger secretValue) { return g.modPow(secretValue,N); }

    public static BigInteger getDiffieHellmanSharedSecret(BigInteger theirPublicKey, BigInteger myPrivateSecret){
        return theirPublicKey.modPow(myPrivateSecret, N);
    }


    public static PrivateKey readInPrivateKey(String filename) throws Exception {
        byte[] keyBytes = Files.readAllBytes(Paths.get(filename));
        PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(keyBytes);
        KeyFactory keyFactory = KeyFactory.getInstance("RSA");
        return keyFactory.generatePrivate(keySpec);
    }

    public static byte[] signPublicKey(PrivateKey privateKey, BigInteger publicKey) throws Exception{
        Signature serverSignature = Signature.getInstance("SHA256WithRSA");
        serverSignature.initSign(privateKey);
        serverSignature.update(publicKey.toByteArray());
        return serverSignature.sign();
    }

    public static boolean verifySignature(Certificate certificate, BigInteger publicKey, byte[] signedPublicKey) throws Exception {
        Signature verifySignature = Signature.getInstance("SHA256WithRSA");
        verifySignature.initVerify(certificate.getPublicKey());
        verifySignature.update(publicKey.toByteArray());
        return verifySignature.verify(signedPublicKey);
    }

}