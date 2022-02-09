package utils;

import java.io.Serializable;

public class NodeInfo implements Serializable {

    public String IP;
    public int port;
    public String username;
    public int type;

    public NodeInfo(String IP, int port, String username, int type)
    {
        this.IP = IP;
        this.port = port;
        this.username = username;
        this.type = type;

    }


    // node methods

    // get Ip
    public String getIP()
    {
        return IP;
    }

    // set IP
    public void setIP(String ip)
    {
        this.IP = ip;
    }

    // get port
    public int getPort()
    {
        return port;
    }

    //set port
    public void setPort(int port)
    {
        this.port = port;
    }

    // get username
    public String getUsername()
    {
        return username;
    }

    // set username
    public void setUsername(String username)
    {
        this.username = username;
    }

    // get type
    public int getType()
    {
        return type;
    }

    // set type
    public void setType(int type)
    {
        this.type = type;
    }

    // check if equal to object
    @Override
    public boolean equals(Object o)
    {
        try{
            NodeInfo obj = (NodeInfo) o;
            if (( obj.getIP().equals(this.getIP())) && ( obj.getPort() == this.getPort()))
            {
                return true;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }
}