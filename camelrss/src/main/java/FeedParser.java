import com.sun.org.apache.xerces.internal.parsers.DOMParser;
import org.apache.camel.*;
import org.apache.camel.builder.RouteBuilder;
import org.apache.camel.impl.DefaultCamelContext;
import org.apache.camel.impl.DefaultExchange;
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
    public static String SpringURI;
    public static String RssURI;
    public static String exePath;

    public static String processToRss(String messageBody){

        JSONObject json = null;
        try {
            json = new JSONObject(messageBody);

            // catch result message
            if ((json.has("result")))
            {
                System.err.println("RESULT CALLED");
                return null;
            }

            // catch start message
            if ((json.has("SUDOKU")))
            {
                // start box and return
                System.err.println("Starting RssToMqtt Thread");
                RssThread rssT = new RssThread(mqtt_ip, mqtt_port, boxName);
                rssT.start();
                return null;
            }

            if (!json.has("r_row") || !json.has("r_column")
            || !json.has("value") || !json.has("box"))
                return null;

            int row = json.getInt("r_row");
            int col = json.getInt("r_column");
            int val = json.getInt("value");
            String box = json.getString("box").split("/")[1].toUpperCase();
            String header = box + "%2C" + row + "%2C" + col + "%2C" + val;
            return header;
        } catch (JSONException e) {
            e.printStackTrace();
        }

        // should not happen
        return null;
    }

    public static String processValues(String messageBody) throws IOException, SAXException, ParserConfigurationException, JSONException {

        DOMParser parser = new DOMParser();

        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        DocumentBuilder builder = factory.newDocumentBuilder();
        InputSource is = new InputSource(new StringReader(messageBody));

        parser.parse(is);
        parser.element("item", null);

        Document doc = parser.getDocument();
        JSONObject json = new JSONObject();
        NodeList list = doc.getElementsByTagName("item");
        for (int i = 0; i < list.getLength(); i++)
        {
            String itemValue = list.item(i).getTextContent();
            // send to mqtt server
            int messageIndex = Integer.parseInt(itemValue.split("\n")[1].split(":")[1]);
            if (messageIndex > latestMessage)
            {
                // send new value
                latestMessage = messageIndex;
                System.out.println(latestMessage);

                String[] resultTest = itemValue.split("\n")[3].split(":");
                if (resultTest[0].trim().equals("RESULT"))
                {
                    System.err.println(resultTest[0]);
                    String resRaw = resultTest[1];
                    String[] resSplitted = resultTest[1].split(",");

                    if (!resSplitted[0].equalsIgnoreCase(boxName))
                        return "";

                    String finishedBox = "sudoku/" + resSplitted[0];
                    String result = resSplitted[1];

                    String formatResult = "[";
                    for (int j = 0; j < 9; j++)
                    {
                        formatResult += result.charAt(j) + ",";
                    }
                    formatResult = formatResult.substring(0, formatResult.length()-1) + "]";

                    json.put("result", formatResult);
                    json.put("box", finishedBox);

                    System.err.println(json.toString());
                    final String jsonString = json.toString();
                    try {
                        new DefaultCamelContext().addRoutes(new RouteBuilder() {
                            public void configure() throws Exception {
                                ProducerTemplate template = this.getContext().createProducerTemplate(0);
                                Exchange ex = new DefaultExchange(this.getContext());
                                ex.getIn().setBody(jsonString);

                                template.send(SpringURI+"/api/result", ex);
                            }
                        });
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    return null;
                    //System.exit(1);
                }

                String[] msgarr = itemValue.split("\n")[3].split(",");

                json.put("box", "sudoku/" + msgarr[0].toLowerCase().trim());
                json.put("r_column", msgarr[2]);
                json.put("r_row", msgarr[1]);
                json.put("value", msgarr[3]);

                return json.toString();
            }
        }
        // no new messages
        return null;
    }

}
