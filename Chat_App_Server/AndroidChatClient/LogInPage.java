package com.example.androidchatclient;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.EditText;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import com.neovisionaries.ws.client.WebSocket;
import com.neovisionaries.ws.client.WebSocketFactory;

import java.io.IOException;

public class LogInPage extends AppCompatActivity {

    Button proceedButton, clearButton;

    EditText userName, roomName;

    boolean isAllFieldsFilled = false;

    public static WebSocket ws_;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login_page);

        try {
            ws_ = new WebSocketFactory().createSocket("ws://10.0.2.2:8080/endpoint", 1000 );
        }
        catch( IOException e ) {
            Log.e( "Tf:LoginPage","WS error" );
        }
        ws_.addListener( new MyWebSocket() );
        ws_.connectAsynchronously();

        proceedButton = findViewById(R.id.sendButton);
        clearButton = findViewById(R.id.clearButton);

        userName = findViewById(R.id.userName);
        roomName = findViewById(R.id.roomName);

        proceedButton.setOnClickListener(v -> {
            String roomNameTxt = roomName.getText().toString();
            String userNameTxt = userName.getText().toString();

            isAllFieldsFilled = CheckAllFields();

            if (isAllFieldsFilled) {
                ws_.sendText("join " + userNameTxt + " " + roomNameTxt);
                Intent i = new Intent(getApplicationContext(), ChatRoom.class);
                Bundle e = new Bundle();
                e.putString("roomName", roomNameTxt);
                e.putString("userName", userNameTxt);
                i.putExtras(e);
                startActivity(i);
            }
        });
        clearButton.setOnClickListener(v -> {
            userName.setText("");
            roomName.setText("");
        });

    }
    private boolean CheckAllFields() {
        if (userName.length() == 0) {
            userName.setError("This field is required");
            return false;
        }
        if (roomName.length() == 0) {
            roomName.setError("This field is required");
            return false;
        }
        if (!ws_.isOpen()){
            AlertDialog.Builder builder = new AlertDialog.Builder(LogInPage.this);
            builder.setMessage("Chat Server Is Not Open!");
            builder.setTitle("Alert !");
            builder.setCancelable(false);
            builder.setPositiveButton("Close App", (dialog, which) -> finish());
            builder.setNegativeButton("Retry", (dialog, which) -> dialog.cancel());
            AlertDialog alertDialog = builder.create();
            alertDialog.show();
            return false;
        }
        return true;
    }
}