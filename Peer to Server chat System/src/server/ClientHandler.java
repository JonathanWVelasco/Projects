package server;

import utils.Message;

import java.io.*;
import java.net.Socket;
import java.util.ArrayList;
import utils.MessageTypes;
import utils.NodeInfo;

import javax.swing.*;

public class ClientHandler implements Runnable, MessageTypes{

    // initialize variables

    // list of clients
    public static ArrayList<NodeInfo> clientArray = new ArrayList<>();

    // socket/objectStream
    public Socket socket;
    public ObjectInputStream objectReader;
    public ObjectOutputStream bufferedWriter;
    public String clientUsername;

    // client handler constructor
    public ClientHandler(Socket socket) {
        try
        {
            this.socket = socket;
            this.objectReader = new ObjectInputStream(this.socket.getInputStream());
            this.bufferedWriter = new ObjectOutputStream(this.socket.getOutputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    @Override
    public void run()
    {
        Message objectFromChat;
        try
        {
            objectFromChat = (Message) this.objectReader.readObject();
            int messageType = objectFromChat.getNode().getType();
            if ( messageType == JOIN)
            {
                // assign client username for array from incoming object
                this.clientUsername = objectFromChat.getNode().getUsername();

                //add client to array list
                clientArray.add(objectFromChat.getNode());

                broadcastMessage("SERVER: " + clientUsername +" has entered the chat");
                //System.out.println("SERVER: " + clientUsername + " has entered the Chat");
            }
            else if (messageType == LEAVE)
            {
                clientArray.remove(objectFromChat.getNode());
                broadcastMessage("SERVER: " + objectFromChat.getNode().getUsername() + " has left the chat");
            }
            else
            {
                if (clientArray.contains(objectFromChat.getNode()))
                {
                    broadcastMessage(objectFromChat.getNode().getUsername() + ": " + objectFromChat.getContent());
                }
                else
                {
                    System.out.println("ERROR");
                }
            }
            closeEverything(this.socket, this.objectReader,this.bufferedWriter);
        } catch (IOException e) {

            //e.printStackTrace();
            System.out.println("User disconnected.");
        } catch (ClassNotFoundException e) {
            e.printStackTrace();

        }
    }


    // broadcast message to all users
    public void broadcastMessage(String message){
        //System.out.println("Im trying to broadcast now");
        Message msg;
        for(NodeInfo client : clientArray){
            try{
                // create a socket to the client
                Socket outgoingSocket = new Socket(client.getIP(), client.getPort());

                //connection established message send
                System.out.println("---------------------------");
                System.out.println("Socket Created");

                //ObjectInputStream localObjectReader = new ObjectInputStream(outgoingSocket.getInputStream());
                ObjectOutputStream localObjectWriter = new ObjectOutputStream(outgoingSocket.getOutputStream());


                // create a message object to send
                msg = new Message(message, null);

                localObjectWriter.writeObject(msg);
                localObjectWriter.flush();

                System.out.println("Action from IP: " + client.getIP() +" Port: " + client.getPort());

                // socket closed
                System.out.println("Socket Closed");
                System.out.println("---------------------------");

            }catch (IOException e){
                closeEverything(socket, objectReader,bufferedWriter);
            }
        }
    }

    // remove client from client list
    public void removeClient(){
        clientArray.remove(this);

    }

    // method to close sockets
    public void closeEverything(Socket socket, ObjectInputStream bufferedReader, ObjectOutputStream bufferedWriter){
        removeClient();
        try{
            if(bufferedReader != null){
                bufferedReader.close();
            }
            if(bufferedWriter != null){
                bufferedReader.close();
            }
            if(socket!=null){
                socket.close();
            }
        }catch(IOException e){
            e.printStackTrace();
        }
    }


}