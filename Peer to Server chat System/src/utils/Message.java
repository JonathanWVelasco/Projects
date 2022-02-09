package utils;

import java.io.Serializable;

public class Message implements MessageTypes, Serializable {

    // initialize variables
    public String content;
    public NodeInfo node;

    // message constructor
    public Message(String content, NodeInfo node)
    {
        this.content = content;
        this.node = node;
    }

    //methods

    // get content
    public String getContent()
    {
        return this.content;
    }

    // set content
    public void setContent(String info)
    {
        this.content = info;
    }

    // get node
    public NodeInfo getNode()
    {
        return this.node;
    }

    // set node
    public void setNode(NodeInfo node)
    {
        this.node = node;
    }

}