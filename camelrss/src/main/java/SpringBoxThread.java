import org.apache.camel.CamelContext;
import org.apache.camel.impl.DefaultCamelContext;

public class SpringBoxThread extends Thread {

    private CamelContext context;

    public SpringBoxThread() {

        context = new DefaultCamelContext();

        try {
            context.addRoutes(new RestRoute());
            context.start();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void run() {

    }
}
