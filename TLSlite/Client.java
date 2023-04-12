import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.math.BigInteger;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.security.PrivateKey;
import java.security.SecureRandom;
import java.security.cert.Certificate;
import java.util.Arrays;

public class Client {

    public static void main(String[] args) {

        try{
            // create a socket and connect it to the server
            Socket clientSocket = new Socket("localhost", 12345);
            System.out.println("Connected to server...");

            // create input and output streams for the client socket
            ObjectOutputStream outStream = new ObjectOutputStream(clientSocket.getOutputStream());
            ObjectInputStream inStream = new ObjectInputStream(clientSocket.getInputStream()) ;

            // Client: Nonce (32 bytes from a SecureRandom object)
            byte[] clientNonce = new byte[32];
            SecureRandom secureRandom = new SecureRandom();
            secureRandom.nextBytes(clientNonce);

            // Client: Certificate, DiffieHellman public key, Signed DiffieHellman public key
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

            // Write nonce to server
            System.out.println("Writing Nonce to Server...");
            outStream.writeObject(clientNonce);
            outStream.flush();

            // Read in server certificate, public key and signed public key
            Certificate serverCertificate = (Certificate) inStream.readObject();
            BigInteger serverPublicKey = (BigInteger) inStream.readObject();
            byte[] signedServerPublicKey = (byte[]) inStream.readObject();

            // Verify the server's digital signature and certificate
            if (CertificateReader.verifyCertificate(serverCertificate)) {
                System.out.println("Server's Certificate Validated!");
            }
            else{
                System.err.println("Server's Certificate is incorrect...");
                clientSocket.close();
            }
            if (KeyGeneration.verifySignature(serverCertificate, serverPublicKey, signedServerPublicKey)) {
                System.out.println("Server's Signature Validated!");
            }
            else{
                System.err.println("Server's digital signature is incorrect");
                clientSocket.close();
            }

            // Send certificate, DH public key, signed DH public key
            outStream.writeObject(clientCertificate);
            outStream.writeObject(clientPublicKey);
            outStream.writeObject(signedClientPublicKey);
            outStream.flush();

            // Compute the shared secret here using DH
            BigInteger clientSharedSecret = KeyGeneration.getDiffieHellmanSharedSecret(serverPublicKey,clientSecret);

            // Calculation of the session key, MAC, and IV for CBC
            SymmetricKeys clientSymmetricKeys = new SymmetricKeys(clientNonce,clientSharedSecret);

            byte[] serverMac = (byte[])inStream.readObject();

            // Client: Verify serverMAC
            MessageHistory serverHistoryVerify = new MessageHistory(clientNonce, serverCertificate, serverPublicKey,
                    signedServerPublicKey, clientCertificate, clientPublicKey, signedClientPublicKey, null);
            byte[] serverMacVerify = serverHistoryVerify.HMAC(serverHistoryVerify.historyToByteArray(),
                    clientSymmetricKeys.serverMAC.getEncoded());
            if (Arrays.equals(serverMac, serverMacVerify)) {
                System.out.println("Server MAC History Verified!");
            }
            else{
                System.err.println("Server MAC history does not match");
                clientSocket.close();
            }

            // MAC(all handshake messages so far including the previous step, Client's MAC key).
            MessageHistory clientHistoryPlusServer = new MessageHistory(clientNonce, serverCertificate, serverPublicKey,
                    signedServerPublicKey, clientCertificate, clientPublicKey, signedClientPublicKey, serverMac);
            byte[] clientMac = serverHistoryVerify.HMAC(clientHistoryPlusServer.historyToByteArray(),
                    clientSymmetricKeys.clientMAC.getEncoded());
            outStream.writeObject(clientMac);
            outStream.flush();

            System.out.println("Handshake Complete!");

            // Receive message from server
            byte[] encryptedServerMessage = (byte[])inStream.readObject();
            try {
                byte[] decryptedServerMessage = Cipher.decrypt(encryptedServerMessage, clientSymmetricKeys.serverMAC,
                        clientSymmetricKeys.serverEncrypt, clientSymmetricKeys.serverIV);
                String c1 = new String(decryptedServerMessage, StandardCharsets.UTF_8);
                System.out.println("Message from server: " + c1);
            }catch (Cipher.IncorrectHMACException e) {
                System.err.println(e + "Closing Connection to server...");
                outStream.close();
                inStream.close();
                clientSocket.close();
                return;
            }

            // Client: Send a message to server
            String clientMessage = "Hello server! (First Message)";
            byte[] encryptedClientMessage = Cipher.encrypt(clientMessage.getBytes(StandardCharsets.UTF_8),
                    clientSymmetricKeys.clientMAC, clientSymmetricKeys.clientEncrypt, clientSymmetricKeys.clientIV);
            outStream.writeObject(encryptedClientMessage);
            outStream.flush();

            // Receive message from server
            byte[] encryptedServerSecondMessage = (byte[])inStream.readObject();
            try {
                byte[] decryptedServerSecondMessage = Cipher.decrypt(encryptedServerSecondMessage,
                        clientSymmetricKeys.serverMAC, clientSymmetricKeys.serverEncrypt, clientSymmetricKeys.serverIV);
                String c2 = new String(decryptedServerSecondMessage, StandardCharsets.UTF_8);
                System.out.println("Message from server: " + c2);
            }catch (Cipher.IncorrectHMACException e) {
                System.err.println(e + "Closing Connection to server...");
                outStream.close();
                inStream.close();
                clientSocket.close();
                return;
            }

            // Client: Send a message to server
            String clientSecondMessage = "Goodbye server! (Second Message)";
            byte[] encryptedClientSecondMessage = Cipher.encrypt(clientSecondMessage.getBytes(StandardCharsets.UTF_8),
                    clientSymmetricKeys.clientMAC, clientSymmetricKeys.clientEncrypt, clientSymmetricKeys.clientIV);
            outStream.writeObject(encryptedClientSecondMessage);
            outStream.flush();

            // Finished connection to server. Close streams and sockets
            outStream.close();
            inStream.close();
            clientSocket.close();
        }
        catch (Exception server) {
            System.out.println("Connection to Server failed..." + server);
            System.exit(1);
        }
    }
}
