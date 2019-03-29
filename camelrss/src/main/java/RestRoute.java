import org.apache.camel.Exchange;
import org.apache.camel.Message;
import org.apache.camel.ProducerTemplate;
import org.apache.camel.builder.RouteBuilder;
import org.json.*;

import java.util.Iterator;

public class RestRoute extends RouteBuilder{

        @Override
        public void configure() throws Exception {

            ProducerTemplate template = this.getContext().createProducerTemplate(0);

            Exchange ex = template
                    .request(FeedParser.SpringURI+"/api/initialize", null);
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

            // start box with parameters from exchange out
            String boxPath = FeedParser.exePath;
            TaskStreamCopyHelper helper = new TaskStreamCopyHelper(boxPath, initString, boxName);
            helper.start();

            template.request(FeedParser.SpringURI+"/api/ready?box=sudoku/"+boxName.toLowerCase(), null);

            // start mqtt route
            FeedParser.mqtt_ip = mqtt_ip;
            FeedParser.mqtt_port = mqtt_port;
            FeedParser.boxName = boxName;

            MQTTThread mqttT = new MQTTThread();
            mqttT.start();
        }
}
