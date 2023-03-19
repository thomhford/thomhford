/*
*   This class reads client HTTP requests and returns a string of what is being requested.
*   Also has the functionality of creating a web-socket-key for use in WebSocket connections which is also returned
*
* */

import java.io.*;
import java.net.Socket;
import java.util.HashMap;

public class HTTPRequest {

    public boolean WSConnection = false;
    public String httpRequestReader(Socket clientSocket) {

        InputStreamReader clientRequest;
        try {
            clientRequest = new InputStreamReader(clientSocket.getInputStream());
        } catch (IOException e) {
            System.out.println("Could not read client request.");
            return "404";
        }
        BufferedReader reader = new BufferedReader(clientRequest);
        StringBuilder request = new StringBuilder();

        String line;
        try {
            line = reader.readLine();//read in HTTP Request
        } catch (IOException e) {
            System.out.println("Could not read client request.");
            return "404";
        }

        while (!line.isBlank()) {
            request.append(line).append("\r\n"); //request comes in long single line unless we add carriage returns
            try {
                line = reader.readLine();
            } catch (IOException e) {
                System.out.println("Could not read client request.");
                return "404";
            }
        }
        //print request to see what is being requested
        //System.out.println(request);
        String firstLine = request.toString().split("\r\n")[0];
        String resource = firstLine.split(" ")[1]; //split the line to access what is being requested
        //print resource to confirm we are getting correct request
        //System.out.println(resource);

        HashMap<String, String> header = new HashMap<>();

        //System.out.println(request);
        String[] map = request.toString().split(System.lineSeparator());
        //String connectionKey = map[2].split(" ")[1];
        //System.out.println(connectionKey);
        //System.out.println(map[0]);
        for (String requestLine : map){
            String[] hash = requestLine.split(": ");
            if( hash[0].equals( "Connection" )){
                hash[1] = hash[1].substring(0, hash[1].length() - 1);
                header.put("Connection", hash[1]);
            }
            if( hash[0].equals( "Sec-WebSocket-Key" )){
                hash[1] = hash[1].substring(0, hash[1].length() - 1);
                header.put( "Sec-WebSocket-Key", hash[1]);
            }
        }
        //System.out.println(header.get("Connection"));
        //System.out.println(header.get("Sec-WebSocket-Key"));

        String concatenate = header.get("Sec-WebSocket-Key") + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"; //concatenate with magic key!
//        if (header.containsKey("Connection")){
//            System.out.println("contains key");
//        }
        //System.out.println( header.get("Connection") + "Test" );
//        System.out.println(header.get("Connection"));
//        if (header.get("Connection").equals("Upgrade")){
//            System.out.println("Test for Upgrade");
//        }
        if ( header.get("Connection").equals( "Upgrade" )){ // if equals upgrade we will switch to Web Socket
            WSConnection = true; // need boolean allow entrance to WS loop in HTTPResponse
            return concatenate; // return websocket key concatenated with magic key for use in HTTPResponse
        }
        //System.out.println(concatenate);
        if (resource.equals("/")) { // request initially asks for '/' but we want to send .html
            resource = "/webChatClient.html";
        }
        return resource; // return clients request to HTTPResponse for handling
    }
}
