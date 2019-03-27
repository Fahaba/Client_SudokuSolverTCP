import org.apache.abdera.model.Feed;
import org.apache.camel.Processor;
import org.apache.camel.builder.RouteBuilder;

public class ReadyRoute extends RouteBuilder {

    public void configure() throws Exception {


        System.err.println("route rss to mock and http");

        //Processor proc = new ProcessorToRss();

        from("mqtt:bar?host=tcp://"
                + FeedParser.mqtt_ip
                + ":"
                + FeedParser.mqtt_port
                + "&subscribeTopicNames=TEST/sudoku/+")
                .setHeader("val", method(FeedParser.class, "processToRss(${body})"))
                .recipientList(simple("http://127.0.0.1:1234/HandleAddFeed.php?message=${header.val}"));
    }
}