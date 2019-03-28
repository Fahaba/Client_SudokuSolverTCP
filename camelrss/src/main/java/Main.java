import org.apache.camel.CamelContext;
import org.apache.camel.impl.DefaultCamelContext;

public class Main {

    public static void main(String[] argv) throws Exception {

        String boxName = "";
        String uri = "rss:http://localhost/rss.xml?&splitEntries=true&consumer.delay=3000";
        new SpringBoxThread(uri, boxName);

    }
}
