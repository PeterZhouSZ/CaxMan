///*/*
// * To change this license header, choose License Headers in Project Properties.
// * To change this template file, choose Tools | Templates
// * and open the template in the editor.
// */
//package it.imati.cnr.tools.demo;
//
//import java.io.File;
//import java.io.IOException;
//import java.text.DateFormat;
//import java.text.SimpleDateFormat;
//import java.util.Date;
//import java.util.logging.Level;
//import java.util.logging.Logger;
//import javax.jws.WebService;
//import javax.jws.WebMethod;
//import javax.jws.WebParam;
//import org.w3c.dom.Document;
//import javax.xml.parsers.DocumentBuilder;
//import javax.xml.parsers.DocumentBuilderFactory;
//import javax.xml.parsers.ParserConfigurationException;
//import javax.xml.ws.Holder;
//import org.w3c.dom.NodeList;
//import org.xml.sax.SAXException;
//
///**
// *
// * @author daniela
// */
//@WebService(serviceName = "GetOrientationParametersFromFile")
//public class GetOrientationParametersFromFile
//{
//
//    private final String namespace = "http://demo.tools.cnr.imati.it/";
//
//    /**
//     * This is a sample web service operation
//     * @param serviceID
//     * @param sessionToken
//     * @param param_file_in
//     * @param wq
//     * @param wt
//     * @param ws
//     * @param threshold
//     * @param ndirs
//     */
//    @WebMethod(operationName = "GetOrientationParametersFromFileMethod")
//    public void get_orientation_parameters(@WebParam(name            = "serviceID",
//                      targetNamespace = namespace,
//                      mode            = WebParam.Mode.IN)  String serviceID,
//
//            @WebParam(name            = "sessionToken",
//                      targetNamespace = namespace,
//                      mode            = WebParam.Mode.IN)  String sessionToken,
//
//            @WebParam(name            = "param_file_in",
//                      targetNamespace = namespace,
//                      mode            = WebParam.Mode.IN)  String param_file_in,
//
//             @WebParam(name            = "wq",
//                      targetNamespace = namespace,
//                      mode            = WebParam.Mode.OUT) Holder<Double> wq,
//
//              @WebParam(name            = "wt",
//                      targetNamespace = namespace,
//                      mode            = WebParam.Mode.OUT)  Holder<Double> wt,
//
//               @WebParam(name            = "ws",
//                      targetNamespace = namespace,
//                      mode            = WebParam.Mode.OUT)  Holder<Double> ws,
//
//                @WebParam(name            = "threshold",
//                      targetNamespace = namespace,
//                      mode            = WebParam.Mode.OUT)  Holder<Double> threshold ,
//
//                 @WebParam(name            = "ndirs",
//                      targetNamespace = namespace,
//                      mode            = WebParam.Mode.OUT)  Holder<Integer> ndirs)
//    {
//
//        DateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmss");
//        String sdate = dateFormat.format(new Date());
//
//        String pathGSSTools         = "/root/infrastructureClients/gssClients/gssPythonClients/";
//        String downloadedFilename   = "/root/CAxManIO/dowloaded_" + sdate + ".xml";
//
//        String cmdDownload = "python " + pathGSSTools + "download_gss.py " + param_file_in + " " + downloadedFilename + " " + sessionToken;
//
//        try
//        {
//            //##########################################################################################################
//            // Download File
//            System.out.print("[RUNNING] : " + cmdDownload);
//
//            Process p1 = Runtime.getRuntime().exec(cmdDownload);
//
//            p1.waitFor();   // wait the download process to finish its task
//
//            System.out.print("[COMPLETED] : " + cmdDownload);
//
//            // Check if the input has been downloaded
//            File input = new File(downloadedFilename);
//            if (!input.getAbsoluteFile().exists()) throw new IOException("Error in downloading " + param_file_in);
//
//            //##########################################################################################################
//            // Parse file to get parameters
//
//            File inputFile = new File(downloadedFilename);
//            DocumentBuilderFactory dbFactory = DocumentBuilderFactory.newInstance();
//            DocumentBuilder dBuilder = dbFactory.newDocumentBuilder();
//
//            Document doc;
//            doc = dBuilder.parse(inputFile);
//
//            doc.getDocumentElement().normalize();
//            System.out.println("Root element :" + doc.getDocumentElement().getNodeName());
//
//            NodeList nList_ws = doc.getElementsByTagName("ws");
//            NodeList nList_wq = doc.getElementsByTagName("wq");
//            NodeList nList_wt = doc.getElementsByTagName("wt");
//
//            NodeList nList_threshold = doc.getElementsByTagName("threshold");
//            NodeList nList_ndirs = doc.getElementsByTagName("ndirs");
//
//            ws.value = Double.parseDouble(nList_ws.item(0).getTextContent());
//            wt.value = Double.parseDouble(nList_wt.item(0).getTextContent());
//            wq.value = Double.parseDouble(nList_wq.item(0).getTextContent());
//
//            threshold.value = Double.parseDouble(nList_threshold.item(0).getTextContent());
//            ndirs.value = Integer.parseInt(nList_ndirs.item(0).getTextContent());
//
//        }
//        catch (ParserConfigurationException | SAXException | IOException e)
//        {
//
//        } catch (InterruptedException ex) {
//            Logger.getLogger(GetOrientationParametersFromFile.class.getName()).log(Level.SEVERE, null, ex);
//        }
//
//    }
//}*/