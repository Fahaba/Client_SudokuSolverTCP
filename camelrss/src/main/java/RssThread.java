import org.apache.camel.CamelContext;
import org.apache.camel.impl.DefaultCamelContext;

public class RssThread extends Thread {

    private CamelContext context;

    public RssThread(String mqtt_ip, int mqtt_port, String boxName) {

        context = new DefaultCamelContext();
        try {
            context.addRoutes(new RssToFeedParser(mqtt_ip, mqtt_port, boxName));
            context.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void run() {

    }
}
