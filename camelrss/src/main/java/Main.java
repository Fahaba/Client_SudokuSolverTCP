import org.apache.camel.CamelContext;
import org.apache.camel.ConsumerTemplate;
import org.apache.camel.impl.DefaultCamelContext;
import org.apache.camel.util.jndi.JndiContext;

public class Main {

    public static void main(String[] argv) throws Exception {
        CamelContext context = new DefaultCamelContext();
        CamelContext restSpringBox = new DefaultCamelContext();
        //context.addRoutes(new MainRoute());
        ///restSpringBox.addRoutes(new RestRoute());


        String boxName = "";
        // pass exe in argv
        //Process process = new ProcessBuilder("E:\\UniProjekte\\Client_SudokuTCP\\Debug\\SudokuTCP.exe", "BOX_A1", "./fieldCFG1.conf", "127.0.0.1", "1337").start();
        //thread
        String uri = "rss:http://localhost/rss.xml?&splitEntries=true&consumer.delay=10000";
        //FeedThreadManager manager = new FeedThreadManager(uri, boxName);
        //manager.start();

        SpringBoxThread spring = new SpringBoxThread(uri, boxName);
        //spring.start();

//        CamelContext mqttContext = new DefaultCamelContext();
//        System.err.println("ekgnbwpegnweüognweüpognwoeügnüwe");
//        mqttContext.addRoutes(new MQTTtoRss());
//        mqttContext.start();
        // thread
//        while(true) {
//            context.start();
//            ConsumerTemplate con = context.createConsumerTemplate();
//            System.out.println(con.receiveBody("seda:end", String.class) + "\n\n");
//        }

    }
}
