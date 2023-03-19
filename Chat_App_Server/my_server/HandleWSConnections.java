/*
*   This class handles WebSocket connections. This is where I read messages from the WebSocket, parse those
*           messages and reply to the client.
* */


import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.nio.charset.StandardCharsets;
import java.util.Arrays;


public class HandleWSConnections {

    HandleWSConnections(Socket clientSocket, StringBuilder WSMessageType_, StringBuilder WSMessageUser_,
                        StringBuilder WSMessage_, StringBuilder WSRoomName_) throws IOException {
        handleWSWriter(readWSMessage(getWSMessage(clientSocket),clientSocket, WSMessageType_, WSMessageUser_,
                WSMessage_, WSRoomName_));
    }

    // Method to read in from WebSocket client. Takes in client socket to get DataInputStream
    private String getWSMessage(Socket clientSocket){
        //byte[] mask;
        try {
            DataInputStream input = new DataInputStream( clientSocket.getInputStream());

            byte[] data = input.readNBytes(2);

            if (data[0] == (byte) 0x88){
                return null;
            }
            if (data[0] == (byte) 0x80){
                return null;
            }
            System.out.println(Arrays.toString(data));

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

    //Method to parse WebSocket message. Takes in message from getWSMessage as well as StringBuilders which store
    //      variable used to create responses to client. This is also where I create rooms
    private String readWSMessage(String ascii_msg, Socket clientSocket,
                                 StringBuilder WSMessageType_, StringBuilder WSMessageUser_,
                                 StringBuilder WSMessage_, StringBuilder WSRoomName_) throws IOException {

        if (ascii_msg == null){
            return null;
        }
        else {
            String[] pieces = ascii_msg.split(" ");  // Split message received to be read
            if (pieces[0].equals("join")) {
                // I create a new DataOutputStream for joining users
                // leaving users or users sending messages should already have DataOutputStreams
                DataOutputStream userOutput = new DataOutputStream(clientSocket.getOutputStream());
                WSMessageType_.append("join");  // Because WSMessageType_ type is StringBuilder, values must be appended
                WSMessageUser_.append(pieces[1]);
                WSRoomName_.append(pieces[2]);
                System.out.println("entered readWSMessage");
                if (Room.rooms_.size() == 0) { // If no rooms exist make a room for user
                    //Room newRoom = new Room(userOutput, WSRoomName_.toString(), WSMessageUser_.toString());
                    getRoom(userOutput, WSRoomName_);
                    System.out.println("should be creating a new room...");
                } else {
                    for (String room : Room.getRoomMap().keySet()) { //Check if a new room needs to be make or to add to existing room
                        System.out.println("entered rooms_ for each loop");
                        if (room.equals(WSRoomName_.toString())) {
                            Room.getRoomMap().put(WSMessageUser_.toString(), userOutput); // Store user and their DataOutputStream
                            System.out.println("should be adding user to existing room...");
                        } else {
                            System.out.println("should be creating a new room...");
                            //Room newRoom = new Room(userOutput, WSRoomName_.toString(), WSMessageUser_.toString());
                            getRoom(userOutput, WSRoomName_);
                        }
                    }
                }
                return JSONCreator.createJSONJoinLeave(WSMessageType_.toString(),
                        WSMessageUser_.toString(), WSRoomName_.toString());
            } else if (pieces[0].equals(WSMessageUser_.toString())) {
                WSMessage_ = new StringBuilder(pieces[1]);
                for (int i = 2; i < pieces.length; i++) {
                    WSMessage_.append(" ").append(pieces[i]);
                }
                Room.messageMap_.put(WSMessageUser_.toString(), WSMessage_.toString()); // To be used to store messages from rooms to send to later joining users
                return JSONCreator.createJSONMessage(WSMessageUser_.toString(),
                        WSRoomName_.toString(), WSMessage_.toString());
            } else if (pieces[0].equals("leave")) {
                removeUser(WSMessageUser_);
                WSMessageType_ = new StringBuilder("leave");
                return JSONCreator.createJSONJoinLeave(WSMessageType_.toString(),
                        WSMessageUser_.toString(), WSRoomName_.toString());
            } else {
                return null;
            }
        }
    }
    private synchronized void getRoom(DataOutputStream userOut, StringBuilder roomName){
        new Room(userOut, roomName.toString());
    }
    private synchronized void removeUser(StringBuilder leavingUser){
        for (String user : Room.getRoomMap().keySet()){
            if (user.equals(leavingUser.toString())){
                Room.getRoomMap().remove(user);
            }
        }
    }

    // Method to write responses to clients. Takes in JSON strings created by readWSMessage and JSONCreator.
    private synchronized void handleWSWriter(String json) throws IOException {
        System.out.println("entered handleWSWriter");
        if (json != null) {
            for (DataOutputStream userOutput : Room.getRoomMap().values()) {
                //DataOutputStream userOutput = new DataOutputStream(clientSocket.getOutputStream());
                // Send the ascii_msg as a WebSocket msg...
                // 1st byte is fin res res res opcode
                // 1000 0001
                userOutput.writeByte(0x81);
                // 2nd byte is: mask bit-length of msg
                // 0 _______
                int length = json.length();
                if (length > 125 && length < 65535) {
                    userOutput.writeByte(126);
                    userOutput.writeShort(length);
                } else if (length > 65535) {
                    userOutput.writeByte(127);
                    userOutput.writeLong(length);
                } else {
                    userOutput.writeByte(length);
                }
                userOutput.write(json.getBytes());

                userOutput.flush();
            }
        }
        else {
            System.out.println("JSON is null!");
        }
    }
}