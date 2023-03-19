/*
*   Added timestamp to all WebSocket communication from the server to clients. This is done by concatenation of the
*   outgoing JSON strings with a generated timestamp string.
*
*   I have also started and hope to complete a way of storing WebSocket communications from rooms which I can then send
*   to connecting users of those rooms.
**/

/*
*   This is my main server class that contains my main function and calls all my other classes which connect and
*   communicate with clients. It is here where I create a server socket through which I am able to connect to client
*   sockets.
**/



import java.net.ServerSocket;
import java.net.Socket;


public class MyHttpServer {
    public static ServerSocket serverSocket; static {t // not sure why this needs two static statements...
        try {
            serverSocket = new ServerSocket(8080); // where we can connect client sockets!
        }
        catch (Exception server) {
            System.out.println("Server failed to start.");
            System.exit(404);
        }
    }
    public static void main(String[] args) {
        while (true) { // continuous while loop is required to listen for clients and accept any connection that might be made
            try {
                Socket clientSocket = serverSocket.accept();  // accept client connection, will be used to receive and send data
                Thread thread = new Thread( new HandleThreadConnections(clientSocket)); // create thread to handle requests and responses
//                {
//                    try {
//                        HTTPRequest request = new HTTPRequest();
//                        HTTPResponse response = new HTTPResponse();
//                        String resource = request.requestReader(clientSocket);
//                        response.responseWriter(resource, clientSocket);
//                        clientSocket.close();
//                    }catch (Exception e){
//                        throw new RuntimeException(e);
//                    }
//                });
                thread.start(); // Need to tell thread to start!
            } catch (Exception accept) {
                System.out.println("Server failed to accept request from client.");
            }
        }
    }
}
