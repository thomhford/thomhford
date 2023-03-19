import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;

public class DNSServer {

    /**
     * Size limits
     *
     * Various objects and parameters in the DNS have size limits.  They are
     * listed below.  Some could be easily changed, others are more
     * fundamental.
     *
     * labels          63 octets or less
     *
     * names           255 octets or less
     *
     * TTL             positive values of a signed 32 bit number.
     *
     * UDP messages    512 octets or less
     */

    public static DatagramSocket dgSocket; static{
        try{
            dgSocket = new DatagramSocket(8053);
        } catch (SocketException e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Server that listens for DNS requests from clients and serves DNS responses corresponding to those questions.
     */
    public static void main(String[] args) throws IOException {
        int packetLength = 512;
        int DNSPort = 53;
        byte[] dataPacket = new byte[512];
        java.net.InetAddress clientAddress = null;
        int clientPort = 0;
        while(true){ // Listens for any incoming packets continually
            // Receive packet and parse message
            System.out.println("Cache size: " + DNSCache.dnsCache.size());
            DatagramPacket packetIN = new DatagramPacket(dataPacket,packetLength);
            dgSocket.receive(packetIN);
            byte[] dataIN = packetIN.getData();
            DNSMessage message = DNSMessage.decodeMessage(dataIN);
            // Check if message is a query(only contains question) or a response(contains question and answer)
            if (DNSMessage.isQuery(message)) {
                clientAddress = packetIN.getAddress(); // Store client address so we can respond
                clientPort = packetIN.getPort(); // Store client port so we can respond
                System.out.println("Query = " + message);
                // For every question in query, either send a response to client, or forward request to Google
                for (DNSQuestion question : message.getQuestions_()) {
                    if (DNSCache.checkHashMap(question) != null) {
                        // Send response, including answer from cache, to client
                        byte[] dataOUT = message.toBytes(question);
                        System.out.println("Writing to client...");
                        System.out.println("Response = " + message );
                        DatagramPacket packetOUT = new DatagramPacket(dataOUT, dataOUT.length, clientAddress, clientPort);
                        dgSocket.send(packetOUT);
                    } else {
                        // Send request to Google
                        InetAddress addrGoogle = InetAddress.getByName("8.8.8.8");
                        DatagramPacket packetOUT = new DatagramPacket(dataIN, dataIN.length, addrGoogle, DNSPort);
                        System.out.println("Writing to Google...");
                        dgSocket.send(packetOUT);
                    }
                }
            }
            // Responses will always be from Google replying with answers to add to cache and message to send to client
            else{
                System.out.println("Reading from Google...");
                System.out.println("Response = " + message );
                System.out.println("Writing to client...");
                // For every question in response from Google, store answer
                for (DNSQuestion question : message.getQuestions_()) {
                    // If there are no answers for a question, do not add to cache!!!
                    if (message.getHeader_().getANCOUNT_() == 0){
                        DNSCache.storeHashMap(question, null);
                    }
                    else {
                        DNSCache.storeHashMap(question, message.getAnswer());
                        System.out.println("Cache size: " + DNSCache.dnsCache.size());
                    }
                }
                // Send full response from Google to client. No need to send from cache yet!!!
                DatagramPacket packetOUT = new DatagramPacket(dataIN,dataIN.length, clientAddress, clientPort);
                dgSocket.send(packetOUT);
            }
        }
    }
}
