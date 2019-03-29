import org.apache.camel.CamelContext;
import org.apache.camel.impl.DefaultCamelContext;

public class MQTTThread extends Thread {

    private String boxName;
    private String URI;
    private CamelContext context;
    private CamelContext restSpringBox;

    public MQTTThread() {

        context = new DefaultCamelContext();
        try {
            context.addRoutes(new MQTTtoRss());
            context.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void run() {

    }
}
