import javax.crypto.Mac;
import javax.crypto.spec.SecretKeySpec;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.math.BigInteger;
import java.security.cert.Certificate;

public class MessageHistory {

    byte[] clientNonce;
    byte[] serverCertificate;
    byte[] serverDHPublicKey;
    byte[] signedServerPublicKey;
    byte[] clientCertificate;
    byte[] clientDHPublicKey;
    byte[] signedClientPublicKey;
    byte[] serverMAC;

    /**
     * Constructor for MessageHistory that sets values needed for authentication of all messages sent up to this point
     */
    public MessageHistory(byte[] clientNonce, Certificate serverCertificate, BigInteger serverDHPublicKey,
                          byte[] signedServerPublicKey, Certificate clientCertificate, BigInteger clientDHPublicKey,
                          byte[] signedClientPublicKey, byte[] serverMAC) throws Exception {
        this.clientNonce = clientNonce;
        this.serverCertificate = serverCertificate.getEncoded();
        this.serverDHPublicKey = serverDHPublicKey.toByteArray();
        this.signedServerPublicKey = signedServerPublicKey;
        this.clientCertificate = clientCertificate.getEncoded();
        this.clientDHPublicKey = clientDHPublicKey.toByteArray();
        this.signedClientPublicKey = signedClientPublicKey;
        this.serverMAC = serverMAC;
    }

    /**
     * Helper method to convert message history to a byte array
     * @return a byte array of all message history
     */
    public byte[] historyToByteArray() throws IOException {
        byte[] ret;
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream( );
        outputStream.write(clientNonce);
        outputStream.write(serverCertificate);
        outputStream.write(serverDHPublicKey);
        outputStream.write(signedServerPublicKey);
        outputStream.write(clientCertificate);
        outputStream.write(clientDHPublicKey);
        outputStream.write(signedClientPublicKey);
        if (serverMAC == null){
            ret = outputStream.toByteArray();
            return ret;
        }
        outputStream.write(serverMAC);
        ret = outputStream.toByteArray();
        return ret;
    }
    /**
     * Helper method that computes HMAC of a string using the MACKey
     * @param history What will be HMACed
     * @param MACKey What will be used to compute the HMAC
     * @return the HMAC
     */
    public byte[] HMAC(byte[] history, byte[] MACKey) throws Exception {
        Mac macHistory = Mac.getInstance("HmacSHA256");
        SecretKeySpec secretKeySpec = new SecretKeySpec(MACKey, "RawBytes");
        macHistory.init(secretKeySpec);
        return macHistory.doFinal(history);
    }

}