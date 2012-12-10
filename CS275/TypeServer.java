import java.io.*;
import java.net.*;
import java.util.*;

/* This program simply types out each line received from a client program. */
/* it can be activated as                                                  */
/*    java TypeServer <any server port number between 5001 and 2**16 - 1>  */
/*    e.g., java TypeServer 12345                                          */
 

class TypeServer {
  public static void main(String[] args) {
    if (args.length != 1) {     // server port # required as argument
       System.out.println("Usage: java TypeServer <port#>");
       return;
    }
    ServerSocket serverSocket = null;
    try {                      // open server socket
      serverSocket = new ServerSocket(Integer.parseInt(args[0]));
    } catch (Exception e) {
      System.out.println("server: cannot open port: " + args[0] + ", " + e);
      System.exit(1);
    }
    System.out.println("\nserver: socket opened on port: " + args[0]);
    Socket newSocket = null;
    try {
      newSocket = serverSocket.accept();  // connect to client
    } catch (IOException e) {
      System.out.println("server: accept failed: " + e);
      System.exit(1);
    }
    System.out.println("server: connected from client");
    try {
      BufferedReader inputStream = new BufferedReader(
      new InputStreamReader(newSocket.getInputStream()));
      for ( ; ; ) {  // loop forever
        String inputLine = inputStream.readLine();  // get data from client
        if (inputLine != null)           // print line received from client
	  System.out.println("from client: " + inputLine);      
        else break;
      }
      inputStream.close();
      newSocket.close();
      serverSocket.close();
    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}
