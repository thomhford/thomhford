import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.math.BigInteger;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.security.PrivateKey;
import java.security.cert.Certificate;
import java.util.Arrays;

public class Server {
    static ServerSocket serverSocket;
    public static void main(String[] args) {
        // TCP Connection
        try {
            serverSocket = new ServerSocket(12345);
            System.out.println("Server initialized: waiting for connections...\n");
        }
        catch (Exception client) {
            System.out.println("Connection to Client failed... " + client);
            System.exit(1);
        }

        while (true) { // continuous while loop is required to listen for clients and accept any connection that might be made
            try {
                // Accept client connection, will be used to receive and send data
                Socket clientSocket = serverSocket.accept();
                System.out.println("Connected to client...");
                // Create input and output streams for connection to the client socket
                ObjectOutputStream outStream = new ObjectOutputStream(clientSocket.getOutputStream());
                ObjectInputStream inStream = new ObjectInputStream(clientSocket.getInputStream());

                // Server: Certificate, DiffieHellman public key, Signed DiffieHellman public key
                // Certificate
                String serverCertificateFileName = "Certificates/CASignedServerCertificate.pem";
                Certificate serverCertificate = CertificateReader.readInCertificate(serverCertificateFileName);
                // Public Key
                BigInteger serverSecret = SecretGenerator.generateNewSecret();
                BigInteger serverPublicKey = KeyGeneration.createPublicKey(serverSecret);
                // Signed public key
                String serverKeyPath = "Certificates/serverPrivateKey.der";
                PrivateKey serverPrivateKey = KeyGeneration.readInPrivateKey(serverKeyPath);
                byte[] signedServerPublicKey = KeyGeneration.signPublicKey(serverPrivateKey, serverPublicKey);

                // Read in Client Nonce
                byte[] clientNonce = (byte[]) inStream.readObject();

                System.out.println("Read in client nonce, writing authentication");

                // Send certificate, DH public key, signed DH public key
                outStream.writeObject(serverCertificate);
                outStream.writeObject(serverPublicKey);
                outStream.writeObject(signedServerPublicKey);
                outStream.flush();

                // Read in client certificate, public key and signed public key
                Certificate clientCertificate = (Certificate) inStream.readObject();
                BigInteger clientPublicKey = (BigInteger) inStream.readObject();
                byte[] signedClientPublicKey = (byte[]) inStream.readObject();

                // Verify the client's digital signature and certificate
                if (CertificateReader.verifyCertificate(clientCertificate)) {
                    System.out.println("Validated client certificate!");
                }
                else{
                    System.err.println("Client's Certificate is incorrect");
                    clientSocket.close();
                }
                if (KeyGeneration.verifySignature(clientCertificate, clientPublicKey, signedClientPublicKey)) {
                    System.out.println("Validated client signature!");
                }
                else {
                    System.err.println("Client's digital signature is incorrect");
                    clientSocket.close();
                }

                // Compute the shared secret here using DH
                BigInteger serverSharedSecret = KeyGeneration.getDiffieHellmanSharedSecret(clientPublicKey, serverSecret);

                // Calculation of the session key, MAC, and IV for CBC
                SymmetricKeys serverSymmetricKeys = new SymmetricKeys(clientNonce, serverSharedSecret);

                // MAC(all handshake messages so far, Server's MAC key)
                MessageHistory serverHistory = new MessageHistory(clientNonce, serverCertificate, serverPublicKey,
                        signedServerPublicKey, clientCertificate, clientPublicKey, signedClientPublicKey, null);
                byte[] serverMac = serverHistory.HMAC(serverHistory.historyToByteArray(),
                        serverSymmetricKeys.serverMAC.getEncoded());
                outStream.writeObject(serverMac);
                outStream.flush();

                // Read in client MAC
                byte[] clientMac = (byte[]) inStream.readObject();

                // Verify clientMac
                MessageHistory clientHistoryVerify = new MessageHistory(clientNonce, serverCertificate, serverPublicKey,
                        signedServerPublicKey, clientCertificate, clientPublicKey, signedClientPublicKey, serverMac);
                byte[] clientMacVerify = clientHistoryVerify.HMAC(clientHistoryVerify.historyToByteArray(),
                        serverSymmetricKeys.clientMAC.getEncoded());
                if (Arrays.equals(clientMac, clientMacVerify)) {
                    System.out.println("Verified Client MAC history!");
                }
                else {
                    System.err.println("Client MAC history does not match");
                    clientSocket.close();
                }

                System.out.println("Handshake Complete!");

                // Send Message to client
                String serverMessage = "Hello Client! (FirstMessage)";
                byte[] encryptedServerMessage = Cipher.encrypt(serverMessage.getBytes(StandardCharsets.UTF_8),
                        serverSymmetricKeys.serverMAC, serverSymmetricKeys.serverEncrypt, serverSymmetricKeys.serverIV);
                outStream.writeObject(encryptedServerMessage);
                outStream.flush();

                // Receive message from client
                byte[] encryptedClientMessage = (byte[]) inStream.readObject();
                try {
                    byte[] decryptedClientMessage = Cipher.decrypt(encryptedClientMessage,
                            serverSymmetricKeys.clientMAC, serverSymmetricKeys.clientEncrypt,
                            serverSymmetricKeys.clientIV);
                    String s1 = new String(decryptedClientMessage, StandardCharsets.UTF_8);
                    System.out.println("Message from Client: " + s1);
                } catch (Cipher.IncorrectHMACException e) {
                    System.err.println(e + "Closing connection to client...");
                    outStream.close();
                    inStream.close();
                    clientSocket.close();
                    continue;
                }

                // Send Message to client
                String serverSecondMessage = "Goodbye client! (SecondMessage)";
                byte[] encryptedServerSecondMessage = Cipher.encrypt(serverSecondMessage.getBytes(StandardCharsets.UTF_8),
                        serverSymmetricKeys.serverMAC, serverSymmetricKeys.serverEncrypt, serverSymmetricKeys.serverIV);
                outStream.writeObject(encryptedServerSecondMessage);
                outStream.flush();

                // Receive message from client
                byte[] encryptedClientSecondMessage = (byte[]) inStream.readObject();
                try {
                    byte[] decryptedClientSecondMessage = Cipher.decrypt(encryptedClientSecondMessage,
                            serverSymmetricKeys.clientMAC, serverSymmetricKeys.clientEncrypt,
                            serverSymmetricKeys.clientIV);
                    String s2 = new String(decryptedClientSecondMessage, StandardCharsets.UTF_8);
                    System.out.println("Message from client: " + s2);
                }catch (Cipher.IncorrectHMACException e) {
                    System.err.println(e + "Closing Connection to client...");
                    outStream.close();
                    inStream.close();
                    clientSocket.close();
                    continue;
                }

                System.out.println('\n');

            } catch (Exception client) {
                System.out.println("Connection to Client failed..." + client);
                // continue;
            }
        }
    }
}
