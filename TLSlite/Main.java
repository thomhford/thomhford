import java.math.BigInteger;
import java.nio.charset.StandardCharsets;
import java.security.PrivateKey;
import java.security.SecureRandom;
import java.security.cert.Certificate;
import java.util.Arrays;

public class Main {

    public static void main(String[] args) throws Exception {

    // Client: Nonce (32 bytes from a SecureRandom object)
        byte[] clientNonce = new byte[32];
        SecureRandom secureRandom = new SecureRandom();
        secureRandom.nextBytes(clientNonce);


    // Server: Server Certificate, DiffieHellman public key, Signed DiffieHellman public key
        // Certificate
        String serverCertificateFileName = "Certificates/CASignedServerCertificate.pem";
        Certificate serverCertificate = CertificateReader.readInCertificate(serverCertificateFileName);

        // Public Key
        BigInteger serverSecret = SecretGenerator.generateNewSecret();
        BigInteger serverPublicKey = KeyGeneration.createPublicKey(serverSecret);

        // Signed public key
        String serverKeyPath = "Certificates/serverPrivateKey.der";
        PrivateKey serverPrivateKey = KeyGeneration.readInPrivateKey(serverKeyPath);
        byte[] signedServerPublicKey = KeyGeneration.signPublicKey(serverPrivateKey,serverPublicKey);


    // Client: Client Certificate, DiffieHellman public key, Signed DiffieHellman public key
        // Certificate
        String clientCertificateFileName = "Certificates/CASignedClientCertificate.pem";
        Certificate clientCertificate = CertificateReader.readInCertificate(clientCertificateFileName);

        // Public Key
        BigInteger clientSecret = SecretGenerator.generateNewSecret();
        BigInteger clientPublicKey = KeyGeneration.createPublicKey(clientSecret);

        // Signed public key
        String clientKeyPath = "Certificates/clientPrivateKey.der";
        PrivateKey clientPrivateKey = KeyGeneration.readInPrivateKey(clientKeyPath);
        byte[] signedClientPublicKey = KeyGeneration.signPublicKey(clientPrivateKey,clientPublicKey);


    // Client: Verify the server's digital signature and certificate
        if (!CertificateReader.verifyCertificate(serverCertificate)){
            System.err.println("Server's Certificate is incorrect");
            return;
        }
        if (!KeyGeneration.verifySignature(serverCertificate, serverPublicKey, signedServerPublicKey)){
            System.err.println("Server's digital signature is incorrect");
            return;
        }

    // Server: Verify the client's digital signature and certificate
        if (!CertificateReader.verifyCertificate(clientCertificate)){
            System.err.println("Client's Certificate is incorrect");
            return;
        }
        if (!KeyGeneration.verifySignature(clientCertificate, clientPublicKey, signedClientPublicKey)) {
            System.err.println("Client's digital signature is incorrect");
            return;
        }

    // Client and server compute the shared secret here using DH
        BigInteger clientSharedSecret = KeyGeneration.getDiffieHellmanSharedSecret(serverPublicKey,clientSecret);
        BigInteger serverSharedSecret = KeyGeneration.getDiffieHellmanSharedSecret(clientPublicKey,serverSecret);

    // Verify that everything is correct so far
        if(!clientSharedSecret.equals(serverSharedSecret)){
            System.err.println("Diffie Hellman shared secret not equal");
            return;
        }

    //Server calculation of the session key, MAC, and IV for CBC
        SymmetricKeys serverSymmetricKeys = new SymmetricKeys(clientNonce,serverSharedSecret);

    // Client calculation of the session key, MAC, and IV for CBC
        SymmetricKeys clientSymmetricKeys = new SymmetricKeys(clientNonce,clientSharedSecret);


    // Verify that everything is correct so far
        if(!clientSymmetricKeys.equals(serverSymmetricKeys)){
            System.err.println("Secret key generation not equal");
            return;
        }
    // Server: MAC(all handshake messages so far, Server's MAC key)
        MessageHistory serverHistory = new MessageHistory(clientNonce, serverCertificate, serverPublicKey,
                signedServerPublicKey, clientCertificate, clientPublicKey, signedClientPublicKey, null);
        byte[] serverMac = serverHistory.HMAC(serverHistory.historyToByteArray(),
                serverSymmetricKeys.serverMAC.getEncoded());

    // Client: Verify serverMAC
        MessageHistory serverHistoryVerify = new MessageHistory(clientNonce, serverCertificate, serverPublicKey,
                signedServerPublicKey, clientCertificate, clientPublicKey, signedClientPublicKey, null);
        byte[] serverMacVerify = serverHistoryVerify.HMAC(serverHistoryVerify.historyToByteArray(),
                clientSymmetricKeys.serverMAC.getEncoded());

        if (!Arrays.equals(serverMac, serverMacVerify)){
            System.err.println("Server MAC history does not match");
            return;
        }

    // Client: MAC(all handshake messages so far including the previous step, Client's MAC key).
        MessageHistory clientHistoryPlusServer = new MessageHistory(clientNonce, serverCertificate, serverPublicKey,
                signedServerPublicKey, clientCertificate, clientPublicKey, signedClientPublicKey, serverMac);
        byte[] clientMac = serverHistoryVerify.HMAC(clientHistoryPlusServer.historyToByteArray(),
                clientSymmetricKeys.clientMAC.getEncoded());

    // Server: Verify clientMac
        MessageHistory clientHistoryVerify = new MessageHistory(clientNonce, serverCertificate, serverPublicKey,
                signedServerPublicKey, clientCertificate, clientPublicKey, signedClientPublicKey, serverMac);
        byte[] clientMacVerify = clientHistoryVerify.HMAC(clientHistoryVerify.historyToByteArray(),
                serverSymmetricKeys.clientMAC.getEncoded());

        if (!Arrays.equals(clientMac, clientMacVerify)){
            System.err.println("Client MAC history does not match");
            return;
        }

    // Client: Send a message to server
        String clientMessage = "Hello server!";
        byte[] encryptedClientMessage = Cipher.encrypt(clientMessage.getBytes(StandardCharsets.UTF_8),
                clientSymmetricKeys.clientMAC, clientSymmetricKeys.clientEncrypt, clientSymmetricKeys.clientIV);
        String encryptedClientString = new String(encryptedClientMessage, StandardCharsets.UTF_8);
        System.out.println("Encrypted Message: " + encryptedClientString);

    // Server: Receive message from client
        byte[] decryptedClientMessage = Cipher.decrypt(encryptedClientMessage, serverSymmetricKeys.clientMAC,
                serverSymmetricKeys.clientEncrypt, serverSymmetricKeys.clientIV);
        String s = new String(decryptedClientMessage, StandardCharsets.UTF_8);

        if (!s.equals(clientMessage)){
            System.err.println("Encrypt to Decrypt does not match");
            return;
        }
        System.out.println("Decrypted Message: " + s);

    // Server: Send Message to client
        String serverMessage = "Hello client!";
        byte[] encryptedServerMessage = Cipher.encrypt(serverMessage.getBytes(StandardCharsets.UTF_8),
                serverSymmetricKeys.serverMAC, serverSymmetricKeys.serverEncrypt, serverSymmetricKeys.serverIV);
        String encryptedServerString = new String(encryptedServerMessage, StandardCharsets.UTF_8);
        System.out.println("Encrypted Message: " + encryptedServerString);

    // Client: Receive message from server
        byte[] decryptedServerMessage = Cipher.decrypt(encryptedServerMessage, clientSymmetricKeys.serverMAC,
                clientSymmetricKeys.serverEncrypt, clientSymmetricKeys.serverIV);
        String c = new String(decryptedServerMessage, StandardCharsets.UTF_8);

        if (!c.equals(serverMessage)){
            System.err.println("Encrypt to Decrypt does not match");
            return;
        }
        System.out.println("Decrypted Message: " + c);
        return;
    }
}