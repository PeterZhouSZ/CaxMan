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
public class IntegrityChecks 
{
    static String toolPath  = "/root/CaxMan/integrity_checks/build/integrity_checks";
    
    public
    int run (String in_filename, String out_filename) throws IOException, InterruptedException
    {
        //////////////////////////////////////////////////////////////////////////////////
            // Integrity checks
            String cmd = toolPath + " " + in_filename + " " + out_filename;
            
            System.out.print("[RUNNING] : " + cmd);
            
            Process p = Runtime.getRuntime().exec(cmd);

            p.waitFor();   // wait the integrity checks process to finish its task
            
            System.out.print("[COMPLETED] : " + cmd);
            
            return p.exitValue();
            
    }
    

}
