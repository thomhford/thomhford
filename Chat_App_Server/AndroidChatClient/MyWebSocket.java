package com.example.androidchatclient;

import android.util.Log;

import com.google.gson.Gson;
import com.neovisionaries.ws.client.WebSocket;
import com.neovisionaries.ws.client.WebSocketAdapter;

import java.util.Map;
import java.util.Objects;

public class MyWebSocket extends WebSocketAdapter {

    @Override
    public void onTextMessage(WebSocket websocket, String text) throws Exception {
        Log.e("Tf:MyWebSocket", ("MessageIn: " + text ));
        //JSONObject serverMessage = new JSONObject(text);
        Map<String, String> serverMessage = new Gson().fromJson( text, Map.class );
        String user = serverMessage.get("user");
        Log.e("Tf:MyWebSocket", ("User: " + user));
        String message = serverMessage.get("message");
        Log.e("Tf:MyWebSocket", ("Message: " + message));
        String room = serverMessage.get("room");
        Log.e("Tf:MyWebSocket", ("Room: " + room));

        if (Objects.equals(serverMessage.get("type"), "message")) {
            String messageMessage = (user + ": " + message);
            Log.e("Tf:MyWebSocket", ("MessageOut: " + messageMessage));
            ChatRoom.messages_.add(messageMessage);

        }
        else if ((Objects.equals(serverMessage.get("type"), "join"))) {
            String joinMessage = (user + " has joined " + room);
            Log.e("Tf:MyWebSocket", ("JoinOut: " + joinMessage));
            ChatRoom.messages_.add(joinMessage);

        }
        else if (Objects.equals(serverMessage.get("type"), "leave")) {
            String leaveMessage = (user + " has left " + room);
            Log.e("Tf:MyWebSocket", ("LeaveOut: " + leaveMessage));
            ChatRoom.messages_.add(leaveMessage);
        }
        ChatRoom.lv_.post(() -> {
            ChatRoom.returnMessageAdapter_.notifyDataSetChanged();
            ChatRoom.lv_.smoothScrollToPosition(ChatRoom.returnMessageAdapter_.getCount()
            );
        });

    }
}
