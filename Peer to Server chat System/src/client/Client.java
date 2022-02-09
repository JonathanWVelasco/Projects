package client;

import java.io.*;
import java.net.Socket;
import java.net.ServerSocket;
import java.util.Properties;
import java.util.Scanner;

import utils.Message;
import utils.PropertyHandler;
import utils.NodeInfo;
import utils.MessageTypes;

import java.util.concurrent.atomic.AtomicBoolean;


public class Client  implements MessageTypes{

    // initialize variables
    private Socket socket;
    private ObjectInputStream objectReader;
    private ObjectOutputStream objectWriter;

    // reusable Message for use in chat functionality
    private static Message msg;

    // message shell used to send myInfo over socket
    private Message myInfo;

    // the ip, port, name
    private NodeInfo myNode;
    private static String serverIP = "0.0.0.0";
    private static int serverPort = 0;
    private String username;
    private ServerSocket localSocketServer;
    private final AtomicBoolean running = new AtomicBoolean(false);


    // creates a node with client information
    public Client(String username){

        this.username = username;
        // create a blank message to be populated later
        this.msg  = new Message(null, null);

        System.out.println("Client " + this.username + " created");
        // generates a socket, sends client info to server, terminates socket
        sendClientInfo();
        String messageResult = "";
        while(true)
        {
            messageResult = sendMessage();
        }



    }
    // sends the server myInfo (which is all the client information) and launches generateLocalServer()
    public void sendClientInfo()
    {
        try
        {
            generateSocket();
            this.myNode = new NodeInfo(this.socket.getLocalAddress().toString().replace("/", ""),
                    this.socket.getLocalPort(),
                    this.username,
                    JOIN);
            System.out.println("---------------------------");
            System.out.println("Client Info");
            System.out.println( "Port: " + this.socket.getLocalPort());
            System.out.println( "IP Address: " +this.socket.getLocalAddress().toString().replace("/", ""));
            System.out.println("---------------------------");
            this.myInfo = new Message(username, this.myNode);
            this.msg.setNode(this.myNode);
            objectWriter.writeObject(myInfo);
            objectWriter.flush();

            // creates the local server with port and ip information the actual server knows
            generateLocalServer();
            generateListener();
            //sendMessage();
            terminateSocket();
        }
        catch(IOException e)
        {
            closeEverything(socket, objectReader, objectWriter);
        }
    }
    public void generateLocalServer()
    {
        try
        {
            this.localSocketServer = new ServerSocket(this.socket.getLocalPort(),
                    100, this.socket.getLocalAddress());
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
    public void terminateLocalServer()
    {
        try
        {
            this.localSocketServer.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void generateListener()
    {
        new Thread(new Runnable() {
            @Override
            public void run() {
                //System.out.println("GenerateLister Activated");
                running.set(true);
                while (running.get()) {
                    if (!localSocketServer.isClosed()) {
                        try {
                            Socket socket = localSocketServer.accept();
                            //System.out.println("SERVER SPEAKING TO ME");
                            listenForMessage(socket);
                        } catch (IOException e) {
                            // When one LEAVES then JOINS again this catch gets activated. I don't know how to do it
                            //without an empty catch, it just means this instance of generateListener stopped, but
                            // another instance is up and running correctly
                        }
                    }
                }
            }
        }).start();
    }
    public void terminateListener()
    {
        this.running.set(false);
    }


    public void listenForMessage(Socket localServerSocket) {

        Message objectFromChat;
        if (localServerSocket.isConnected()) {
            ObjectInputStream localObjectReader = null;
            try {
                localObjectReader = new ObjectInputStream(localServerSocket.getInputStream());
                objectFromChat = (Message) localObjectReader.readObject();
                // print out content from object
                System.out.println(objectFromChat.getContent());
                localObjectReader.close();
                localServerSocket.close();
            } catch (ClassNotFoundException e) {
                closeEverything(localServerSocket, localObjectReader, objectWriter);
            } catch (IOException e) {
                System.out.println("ERROR MESSAGE: " + e.getMessage());
                closeEverything(localServerSocket, localObjectReader, objectWriter);
                e.printStackTrace();
            }
        }
    }

    public String sendMessage()
    {
        //System.out.print("sendMessage: ");
        String message = null;
        try
        {
            generateSocket();
            Scanner sc = new Scanner(System.in);
            message = sc.nextLine();

            // assign object message to string
            msg.setContent(message);
            if (message.equals("JOIN"))
            {
                terminateListener();
                terminateLocalServer();
                sendClientInfo();
            }
            else if (message.equals("LEAVE"))
            {
                msg.getNode().setType(LEAVE);

            }
            else if ( message.equals("SHUTDOWN") && msg.getNode().getType() == LEAVE  )
            {
                System.exit(0);
            }
            else
            {
                msg.getNode().setType(NOTE);
            }


            objectWriter.writeObject(msg);

            objectWriter.flush();
            terminateSocket();
        }
        catch(IOException e)
        {
            closeEverything(socket, objectReader, objectWriter);
        }
        return message;
    }
    // generates the socket itself and the object read/writers
    public void generateSocket()
    {
        try
        {

            this.socket = new Socket(serverIP, serverPort);

            this.objectWriter = new ObjectOutputStream((socket.getOutputStream()));

            this.objectReader = new ObjectInputStream(socket.getInputStream());
        }
        catch(IOException e)
        {
            System.out.println(e.getCause());
            closeEverything(socket, objectReader, objectWriter);
        }


    }
    public void terminateSocket()
    {
        closeEverything(this.socket, this.objectReader, this.objectWriter);
    }



    public void closeEverything(Socket socket, ObjectInputStream bufferedReader, ObjectOutputStream bufferedWriter){
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

    public static void main(String[] args)  {

        //initialize methods/variables
        String propertiesFile = null;
        Scanner sc = new Scanner(System.in);

        // Read properties file
        propertiesFile = "src/client/Server.properties";
        //
        // get the properties of the server
        //
        Properties properties = null;
        // open properties
        try
        {
            properties = new PropertyHandler(propertiesFile);
        }
        catch (IOException ex)
        {
            System.out.println(ex.getMessage());
            System.exit(1);
        }
        // get server IP
        try
        {
            serverIP = properties.getProperty("SERVER_IP");
        }
        catch (Exception ex)
        {
            System.exit(1);
        }

        // get server port
        try
        {
            serverPort = Integer.parseInt(properties.getProperty("SERVER_PORT"));
        }
        catch (NumberFormatException ex)
        {
            System.exit(1);
        }
        // CHAT SYSTEM

        System.out.println("Type JOIN to join chat or SHUTDOWN to disconnect: ");

        // chat loop
        while(true)
        {
            // record chat
            String MessageType = sc.nextLine();

            // if user JOINs chat
            if( MessageType.equals("JOIN") )
            {
                // request user name
                System.out.println("Type username: ");

                String username = sc.nextLine();

                // connect to server
                Client client = new Client(username);

                break;
            }
            //SHUTDOWN chat
            else if ( MessageType.equals("SHUTDOWN") )
            {
                System.exit(0);
            }
            // tell user to JOIN chat
            else
            {
                System.out.println("Type JOIN to join chat or SHUTDOWN to disconnect: ");
            }
        }



    }




}