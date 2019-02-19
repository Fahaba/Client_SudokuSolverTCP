import org.apache.camel.CamelContext;
import org.apache.camel.ConsumerTemplate;
import org.apache.camel.impl.DefaultCamelContext;
import org.apache.camel.util.jndi.JndiContext;

public class Main {

    public static void main (String[] argv) throws Exception{
        CamelContext context = new DefaultCamelContext();
        context.addRoutes(new MainRoute());
        while(true) {
            context.start();
            ConsumerTemplate con = context.createConsumerTemplate();
            System.out.println(con.receiveBody("seda:end", String.class) + "\n\n");
        }}
}
