import com.sun.org.apache.xerces.internal.parsers.DOMParser;
import org.apache.camel.CamelContext;
import org.apache.camel.impl.DefaultCamelContext;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.io.IOException;
import java.io.StringReader;

public class FeedParser {

    public static int latestMessage = 0;
    public static String mqtt_ip;
    public static int mqtt_port;
    public static String boxName;

    public static String processToRss(String messageBody){

        JSONObject json = null;
        try {
            json = new JSONObject(messageBody);

            if ((json.has("SUDOKU")))
            {
                // start box and return
                System.err.println("Starting RssToMqtt Thread");
                RssThread rssT = new RssThread(mqtt_ip, mqtt_port, boxName);
                rssT.start();
                return null;
            }

            if (!json.has("r_row") || !json.has("r_column")
            || !json.has("value"))
                return null;

            int row = json.getInt("r_row");
            int col = json.getInt("r_column");
            int val = json.getInt("value");

            String header = boxName + "%2C" + row + "%2C" + col + "%2C" + val;
            return header;
        } catch (JSONException e) {
            e.printStackTrace();
        }

        return "";
    }

    public static String processValues(String messageBody) throws IOException, SAXException, ParserConfigurationException, JSONException {
        //JOptionPane.showMessageDialog(null, messageBody);
        //System.out.println("rftphojmeroihgmer0ihmer0ihmerhmer0hmiermhiermh");
        //System.out.println(messageBody);
        DOMParser parser = new DOMParser();

        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        InputSource is = new InputSource(new StringReader(messageBody));

        parser.parse(is);
        parser.element("item", null);

        Document doc = parser.getDocument();
        //JSONArray arr = new JSONArray();
        JSONObject json = new JSONObject();
        NodeList list = doc.getElementsByTagName("item");
        for (int i = 0; i < list.getLength(); i++)
        {
            String itemValue = list.item(i).getTextContent();
            //System.out.println(itemValue.split("\n")[1]);
            //System.out.println(itemValue.split("\n")[3]);
            // send to mqtt server
            int messageIndex = Integer.parseInt(itemValue.split("\n")[1].split(":")[1]);
            if (messageIndex > latestMessage)
            {
                // send new value
                latestMessage = messageIndex;
                System.out.println(latestMessage);

                String[] msgarr = itemValue.split("\n")[3].split(",");

                json.put("box", "sudoku/" + msgarr[0].toLowerCase().trim());
                json.put("r_column", msgarr[2]);
                json.put("r_row", msgarr[1]);
                json.put("value", msgarr[3]);

                return json.toString();
                //arr.put(json);
                // send this
                //CamelContext context = new DefaultCamelContext();

            }
        }
        // no new messages
        return null;
    }

}
