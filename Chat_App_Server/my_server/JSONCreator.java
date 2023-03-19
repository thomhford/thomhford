/*
*   Class where I create JSON strings to return to be sent to clients
* */


import java.text.SimpleDateFormat;

public class JSONCreator {
    public JSONCreator(){}

    public static String createJSONMessage(String user, String room, String message){
        String timeStamp = new SimpleDateFormat("MM/dd : HH:mm:ss").format(new java.util.Date());
        message += (" @: " + timeStamp);
        System.out.println("entered createJSONMessage");
        String json = "{ \"type\": \"message\", \"user\": \"" + user + "\", \"room\": \"" +
                room + "\", \"message\": \"" + message + "\" }";
        System.out.println(json);
        return json;
    }
    public static String createJSONJoinLeave(String type, String user, String room) {
//        String timeStamp = new SimpleDateFormat("MM/dd : HH:mm:ss").format(new java.util.Date());
//        room += (" @: " + timeStamp);
        System.out.println("entered createJSONJoinLeave");
        String json = "{ \"type\": \""+ type +"\", \"user\": \"" + user + "\", \"room\": \"" +
                room + "\"}";
        System.out.println(json);
        return json;

    }
}
