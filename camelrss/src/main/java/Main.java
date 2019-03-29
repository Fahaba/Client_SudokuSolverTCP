import org.apache.camel.CamelContext;
import org.apache.camel.impl.DefaultCamelContext;

public class Main {

    public static void main(String[] argv) throws Exception {

        if (argv.length < 3){
            System.err.println("usage 'SpringBoxURI:Port' 'RssURI:Port' '.exePathToBox'");
            System.exit(0);
        }
        System.out.println("Arguments are:\n" +argv[0] + " " + argv[1] + " " + argv[2]);

        FeedParser.SpringURI = argv[0];
        FeedParser.RssURI = argv[1];
        FeedParser.exePath = argv[2];

        new SpringBoxThread();

    }
}
