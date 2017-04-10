/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.tools;

import java.io.File;
import java.io.IOException;

/**
 *
 * @author daniela
 */
public class CavitiesDetection 
{
    static String toolPath  = "/root/CaxMan/thinwalls_and_cavities/build/thinwalls_and_cavities";
    
    public
    int run (String in_filename, String out_filename) throws IOException, InterruptedException
    {
        //////////////////////////////////////////////////////////////////////////////////
        // Cavities Detection
        String cmd = toolPath + " --cavities --input " + in_filename + " --output " + out_filename;

        System.out.print("[RUNNING] : " + cmd);

        Process p = Runtime.getRuntime().exec(cmd);

        p.waitFor();   // wait the integrity cjhecks process to finish its task

        System.out.print("[COMPLETED] : " + cmd);

        File output = new File(out_filename);

        if (!output.getAbsoluteFile().exists()) 
            throw new IOException("Error in generating output " + out_filename);

        return p.exitValue();
    }
    
}
