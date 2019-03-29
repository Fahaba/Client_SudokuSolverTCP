import org.apache.abdera.model.Feed;
import org.apache.camel.Processor;
import org.apache.camel.builder.RouteBuilder;

public class MQTTtoRss extends RouteBuilder {

    public void configure() throws Exception {

        from("mqtt:bar?host=tcp://"
            + FeedParser.mqtt_ip
            + ":"
            + FeedParser.mqtt_port
            + "&subscribeTopicNames=TEST/sudoku/+")
                .setHeader("val", method(FeedParser.class, "processToRss(${body})"))
                .recipientList(simple(FeedParser.RssURI+"/HandleAddFeed.php?message=${header.val}"));
    }
}
