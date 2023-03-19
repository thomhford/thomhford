/*
*   Class where I store data about chat rooms so clients can communicate with each-other.
*   This is where I store a list of all existing rooms and a map of users and their DataOutputStreams
*   Also where I store map of users and messages for sending to later joining users
* */


import java.io.DataOutputStream;
import java.util.ArrayList;
import java.util.HashMap;

public class Room  {
    public static ArrayList<Room> rooms_ = new ArrayList<>();

    public String roomName_;
    public static HashMap<String, DataOutputStream> roomMap_ = new HashMap<>();
    public static HashMap<String, String> messageMap_ = new HashMap<>();


    public Room(DataOutputStream userOutput, String roomName) {

        roomMap_.put( roomName, userOutput );
        roomName_ = roomName;
        rooms_.add(this);

    }
    public String getRoomName_(){
        return this.roomName_;
    }

    public static HashMap<String,DataOutputStream> getRoomMap(){
        return roomMap_;
    }
}


