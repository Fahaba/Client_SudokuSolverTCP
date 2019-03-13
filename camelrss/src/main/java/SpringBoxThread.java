import org.apache.camel.CamelContext;
import org.apache.camel.ConsumerTemplate;
import org.apache.camel.ProducerTemplate;
import org.apache.camel.impl.DefaultCamelContext;

public class SpringBoxThread extends Thread {

    private String boxName;
    private String URI;
    private CamelContext context;

    public SpringBoxThread(String URI, String BoxName) {
        this.boxName = BoxName;
        this.URI = URI;

        context = new DefaultCamelContext();

        try {
            context.addRoutes(new RestRoute());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void run() {

        while(true) {
            try {
                context.start();
                //ConsumerTemplate con = context.createConsumerTemplate();
//                System.out.println(con.receiveBody("seda:end", String.class) + "\n\n");
                sleep(5000);
            } catch (Exception e) {
                e.printStackTrace();
            }

        }
    }
}
