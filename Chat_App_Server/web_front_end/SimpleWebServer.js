"use strict";

console.log( "Hello" );

let wsOpen = false;

let users = document.getElementById("peoplepara");
let msgArea = document.getElementById("chatpara");

function handleConnectCB() {
    console.log("Server Connected")
    wsOpen = true;
}
let leftDiv = document.getElementById('left');
function handleMessageFromWSCB( event ) {
    let message = event.data;
    let obj = JSON.parse( message );
    if (obj.type === "message"){
        msgArea.innerHTML += ("<br>" + obj.user + ": " + obj.message);
    }

    else if (obj.type === "join"){
        let para = document.createElement("p");
        para.setAttribute('id', obj.user);
        para.innerHTML += ("<br>" + obj.user);
        leftDiv.appendChild(para);
        msgArea.innerHTML += ("<br>" + obj.user + "  has entered the chat room: " + obj.room);
    }

    else if (obj.type === "leave"){
        let leaveUser = document.getElementById(obj.user);
        leaveUser.remove();
        // users.innerHTML -= ("<br>" + obj.user)
        msgArea.innerHTML += ("<br>" + obj.user + "  has left the chat room: " + obj.room);
    }

}
function handleCloseCB() {
    console.log("Server Disconnected")
    //wsOpen = false;
    msgArea.innerHTML += ("<br>" + "You have disconnected from the chat room");
    ws.send("leave " + username_.value + " " + roomname_.value);
}
function handleErrorCB() {
    alert( "Server Error" );
}

// Function to handle ENTER or the Send Button
function handleConnectToServer( event ) {
    console.log("entered function");

    if( event.keyCode === 13 ) {
        console.log("entered ConnectToServer if");

        if (username_.value === null){
            alert("Please enter a username!");
            username_.select();
            event.preventDefault();
            return;
        }

        for (let i = 0; i < roomname_.value.length; i++){
            if( roomname_.value.charAt(i) < 'a' && roomname_.value.charAt(i) > 'z' ) {
                alert("Please make sure room name is all lowercase");
                roomname_.value = "Enter a room name with all lowercase";
                roomname_.select();
                event.preventDefault();
                return;
            }
        }
        // Make WebSocket request to get reply...
        if ( wsOpen ){
            console.log("wsOpen send")
            ws.send("join " + username_.value + " " + roomname_.value);
            console.log(username_.value + " " + roomname_.value);
        }
        else {
            messageArea.value = "Chat Server is not open...";
        }
    }
}
function handleDisconnectFromServer( event ){
    console.log("entered disconnectFromServer")
    if( event.type === "click"){
        handleCloseCB();
    }
}
//let message = msgArea.value;
function handleSendMessage( event ){
    console.log("entered sendMessage function");

    if( event.keyCode === 13 ) {
        console.log("entered sendMessage if");
        if ( wsOpen && messageArea.value != null ) {
            console.log("entered wsOpen if");
            // the web sever is expecting a string of the form "# <space> #"
            ws.send(username_.value + " " + messageArea.value);
        }
    }
}

function handleCloseBrowser( event ){
    ws.send("leave " + username_.value + " " + roomname_.value);
}

let username_ = document.getElementById( "userName" );
let roomname_ = document.getElementById( "roomName" );

let messageArea = document.getElementById( "msgArea" );
let button = document.getElementById( "leave" );

username_.addEventListener( "keypress", handleConnectToServer );
roomname_.addEventListener( "keypress", handleConnectToServer );
messageArea.addEventListener( "keypress", handleSendMessage );

button.addEventListener( "click", handleDisconnectFromServer );



// Create the WebSocket...
let ws = new WebSocket("ws://localhost:8080");

ws.onopen = handleConnectCB;
ws.onclose = handleCloseCB;

ws.onerror = handleErrorCB;
ws.onmessage = handleMessageFromWSCB;
window.onbeforeunload = handleCloseCB;

