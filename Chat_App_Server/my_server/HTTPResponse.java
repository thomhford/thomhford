/*
*   This class takes in client requests as strings from HTTPRequest and replies to clients with
*   requested files if available
*
*   This class also handles WebSocket requests and connections. If the client does request a WebSocket connection and
*   key is sent from HTTPRequest a WebSocket connection is made.
*
* */


import java.io.*;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Base64;

public class HTTPResponse {
    public void httpResponseWriter(String resource, Socket clientSocket,
                                   HTTPRequest httpRequest) throws NoSuchAlgorithmException {

        if (httpRequest.WSConnection) {
            System.out.println("Hello from WSConnection");
            try {
                OutputStream clientOutput = clientSocket.getOutputStream();
                clientOutput.write(("http/1.1 101 Switching Protocols\r\n" + "Upgrade: websocket\r\n" +
                        "Connection: Upgrade\r\n" + "Sec-WebSocket-Accept: " +
                        Base64.getEncoder().encodeToString(MessageDigest.getInstance(
                                "SHA-1").digest(resource.getBytes(
                                StandardCharsets.UTF_8))) + ("\r\n")).getBytes());

                clientOutput.write(("\r\n").getBytes());
                clientOutput.flush();
                // clientOutput.close(); // with this active, we never enter the WSConnection

                // This block of StringBuilders is to ensure that data persists throughout usages
                // Could not get my variables to keep values when passed around
                //      when strings so stored them as StringBuilder
                StringBuilder WSMessageType_ = new StringBuilder();
                StringBuilder WSMessageUser_ = new StringBuilder();
                StringBuilder WSMessage_ = new StringBuilder();
                StringBuilder WSRoomName_ = new StringBuilder();
                while (true){
                    // Created an object to handle all of my WebSocket connections. Pass to this object
                    //      all my stored StingBuilder variables
                    HandleWSConnections newConnection = new HandleWSConnections(clientSocket,
                            WSMessageType_, WSMessageUser_, WSMessage_, WSRoomName_);
                }
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
        else{
            try { // Send to client whatever file they are requesting over HTTP
                FileInputStream fileOut = new FileInputStream("resources/" + resource);
                OutputStream clientOutput = clientSocket.getOutputStream();
                clientOutput.write(("http/1.1 200 Success!\r\n").getBytes());
                clientOutput.write(("\r\n").getBytes());
                clientOutput.write(fileOut.readAllBytes());
                clientOutput.flush();
                clientOutput.close();

            } catch (Exception fileNotFound) {
                try { // If file does not exist or failure in sending file, send 404 page
                    FileInputStream error = new FileInputStream("resources/404.html");
                    OutputStream clientOutput = clientSocket.getOutputStream();
                    clientOutput.write(("http/1.1 404 File not found!\r\n").getBytes());
                    clientOutput.write(("\r\n").getBytes());
                    clientOutput.write((error).readAllBytes());
                    clientOutput.flush();
                    clientOutput.close();
                } catch (IOException e) {
                    System.out.println("404 Cannot write response to client.");
                }
            }
        }
    }


    /*
    public String getWSMessage(Socket clientSocket){
        //byte[] mask;
        try {
            DataInputStream input = new DataInputStream( clientSocket.getInputStream());

            byte[] data = input.readNBytes(2);

            // parse / handle 1st byte...
            // fin and opcode

            // parse (handle) 2nd byte...
            // mask length

            boolean isMasked = (data[1] & 0x80) > 0; // 1000 0000
            int length = data[1] & 0x7F; // 0111 1111

            //if ( isMasked ){
            // read the mask... 4 bytes
            byte[] mask = input.readNBytes(4);
            //}

            byte[] payload = input.readNBytes(length);
            // Fix the payload... unmask it

            for(int i = 0; i < payload.length; i++){
                payload[i] = (byte) (payload[i] ^ mask[i % 4]);
            }
            String decodedText = new String(payload, StandardCharsets.UTF_8);
            System.out.println(decodedText);
            return decodedText;


        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }
    private void handleWSWriter(String ascii_msg, Socket clientSocket) throws IOException {
        String json = "{ \"type\": \"message\", \"user\": \"" + "user" + "\", \"room\": \"" +
                "room" + "\", \"message\": \"" + ascii_msg + "\" }";

        DataOutputStream output = new DataOutputStream(clientSocket.getOutputStream());
        // Send the ascii_msg as a WebSocket msg...
        // 1st byte is fin res res res opcode
        // 1000 0001
        output.writeByte( 0x81 );
        // 2nd byte is: mask bit-length of msg
        // 0 _______
        int length = json.length();
        if ( length > 125 && length < 65535 ){
            output.writeByte(126);
            output.writeShort(length);
        }
        else if ( length > 65535 ){
            output.writeByte(127);
            output.writeLong(length);
        }
        else {
            output.writeByte(length);
        }
        output.writeBytes( json );

        output.flush();
    }
*/
}
