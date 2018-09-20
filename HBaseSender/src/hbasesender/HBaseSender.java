package hbasesender;

import java.io.*;
import java.net.*;
import java.io.DataOutputStream;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.BitSet;
import java.util.HashMap;

public class HBaseSender {
    public static void main(String[] args) throws IOException {
        System.out.println("Sending start...");
        // arg[0] -> file
        // arg[1] -> ip
        // arg[2] -> port Integer.parseInt(args[2])
        String dataString = new String(Files.readAllBytes(Paths.get(args[0])), "UTF-8");
        
        try {
            System.out.println("Connecting to " + args[1] + ":" + args[2]);
            Socket clientSocket = new Socket(args[1], Integer.parseInt(args[2]));
            DataOutputStream output = new DataOutputStream(clientSocket.getOutputStream());

           // output.write(data, 0, data.length);
            output.writeUTF(dataString);
            output.flush();

            clientSocket.close();
        } catch(IOException ex)
        {
            ex.printStackTrace();
        }
        
        System.out.println("Sending finished...");
        System.exit(1); 
    }
}