/*
*   This class implements the Runnable portion of my threads. This is where all of my connections are handled.
*
**/


import java.net.Socket;

public class HandleThreadConnections implements Runnable{

    private final Socket clientSocket; // Where I store the client socket to receive and send data

    HandleThreadConnections(Socket clientSocket_){
        clientSocket = clientSocket_;
    }

    @Override
    public void run() {
        try {
            HTTPRequest request = new HTTPRequest();
            HTTPResponse response = new HTTPResponse();
            String resource = request.httpRequestReader(clientSocket);
            response.httpResponseWriter(resource, clientSocket, request); // [resource] class that will give
            // me the file being requested [clientSocket] socket of client [request] calling the request class to handle WSRequest boolean
            clientSocket.close();
        }catch (Exception e){
            System.out.println("User has disconnected from server!");
        }
    }
}