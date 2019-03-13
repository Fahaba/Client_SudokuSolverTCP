package src.unused;
import org.apache.camel.CamelContext;
import org.apache.camel.impl.DefaultCamelContext;

public class FeedThreadManager extends Thread {

    private String boxName;
    private String URI;
    private CamelContext context;
    private CamelContext restSpringBox;

    public FeedThreadManager(String URI, String BoxName) {
        this.boxName = BoxName;
        this.URI = URI;
        context = new DefaultCamelContext();
        restSpringBox = new DefaultCamelContext();

        try {
            MainRoute mainroute = new MainRoute("a");
            context.addRoutes(mainroute);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void run() {

        while(true) {
            try {
                context.start();
                sleep(5000);
            } catch (Exception e) {
                e.printStackTrace();
            }

            // start camel context
        }
    }
}
