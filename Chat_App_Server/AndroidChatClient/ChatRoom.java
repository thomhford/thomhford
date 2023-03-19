package com.example.androidchatclient;

import android.content.Intent;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.util.ArrayList;

public class ChatRoom extends AppCompatActivity {

    public static ArrayList<String> messages_ = new ArrayList<>();

    public static ArrayAdapter<String> returnMessageAdapter_;
    public static ListView lv_;

    private EditText userMessage_;
    private String roomName_;
    private String userName_;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Button sendButton, clearButton, leaveButton;

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_chat_room);

        sendButton = findViewById(R.id.sendButton);
        clearButton = findViewById(R.id.clearButton);
        leaveButton = findViewById(R.id.leaveButton);

        userMessage_ = findViewById(R.id.textArea);

        sendButton.setOnClickListener(v -> {
            String userMessageText = userMessage_.getText().toString();

            LogInPage.ws_.sendText( userName_ + " " + userMessageText);

            userMessage_.setText("");

        });
        clearButton.setOnClickListener(v -> userMessage_.setText(""));
        leaveButton.setOnClickListener(v -> {
            LogInPage.ws_.sendText("leave " + userName_ + " " + roomName_);
            Intent i = new Intent(getApplicationContext(), LogInPage.class);
            startActivity(i);
        });

        TextView textview = findViewById(R.id.roomNameTitle);
        Intent textIntent = getIntent();
        roomName_ = textIntent.getStringExtra("roomName");
        userName_ = textIntent.getStringExtra("userName");
        textview.setText(roomName_);

        lv_ = findViewById(R.id.chatArea);
        returnMessageAdapter_ = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, messages_);
        lv_.setAdapter(returnMessageAdapter_);
    }
}