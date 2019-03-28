import org.apache.camel.Exchange;
import org.apache.camel.Message;
import org.apache.camel.ProducerTemplate;
import org.apache.camel.builder.RouteBuilder;
import org.apache.commons.io.IOUtils;
import org.json.*;

import java.util.Iterator;

public class RestRoute extends RouteBuilder{

        @Override
        public void configure() throws Exception {
            System.out.println("Hi.");

            ProducerTemplate template = this.getContext().createProducerTemplate(0);

            Exchange ex = template
                    .request("http://127.0.0.1:4242/api/initialize", null);
            Message msg = ex.getOut();
            JSONObject json = new JSONObject(msg.getBody(String.class));

            String boxName =  ((String) json.get("boxname")).split("/")[1].toUpperCase();
            String mqtt_ip =  (String)json.get("mqtt_ip");
            int mqtt_port =  Integer.parseInt((String)json.get("mqtt_port"));
            String init = (String) json.get("init");
            JSONArray arr = new JSONArray(init);
            System.err.println(boxName);

            int initializer[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
            for (int i = 0; i < arr.length(); i++) {
                if (!arr.isNull(i)) {
                    JSONObject obj = arr.getJSONObject(i);
                    Iterator<String> keys = obj.keys();
                    // only one key
                    String firstKey = keys.next();
                    int first = Character.getNumericValue((firstKey.charAt(0)));
                    int second = Character.getNumericValue((firstKey.charAt(1)));
                    initializer[first * 3 + second] = (Integer) obj.get((firstKey));
                }
            }
            String initString = "";
            for(int i = 0; i < initializer.length; i++)
            {
                initString += initializer[i] + " ";
            }
            System.err.println(initString);
//            System.err.println(arr.get(0));/
//            String bodyString = msg.getBody(String.class);
//            System.err.println(boxName);

            // start box with parameters from exchange out
            String boxPath = "E:\\UniProjekte\\Client_SudokuTCP\\Debug\\SudokuTCP.exe";
            TaskStreamCopyHelper helper = new TaskStreamCopyHelper(boxPath, initString, boxName);
            helper.start();
            //Process p = new ProcessBuilder("C:\\Users\\fahas\\Desktop\\Client_SudokuSolverTCP\\Client_SudokuTCP\\Debug\\SudokuTCP.exe", boxName, initString, "127.0.0.1", "1337").start();

            System.out.println("TEST");

            template.request("http://127.0.0.1:4242/api/ready?box=sudoku/"+boxName.toLowerCase(), null);

            // start mqtt route
            FeedParser.mqtt_ip = mqtt_ip;
            FeedParser.mqtt_port = mqtt_port;
            FeedParser.boxName = boxName;

//            FeedParser.mqtt_ip = "127.0.0.1";
//            FeedParser.mqtt_port = 1338;
//            FeedParser.boxName = boxName;

//            System.err.println("Starting RssToMqtt Thread");
//            RssThread rssT = new RssThread(mqtt_ip, mqtt_port, boxName);
//            rssT.start();

            MQTTThread mqttT = new MQTTThread();
            mqttT.start();

            System.err.println("done");

        }
}
