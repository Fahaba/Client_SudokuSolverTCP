import com.sun.org.apache.xerces.internal.parsers.DOMParser;
import org.apache.abdera.model.Feed;
import org.apache.camel.builder.RouteBuilder;
import org.w3c.dom.*;
import org.xml.sax.*;

import javax.xml.parsers.*;
import java.io.IOException;
import java.io.StringReader;

public class RssToFeedParser extends RouteBuilder{

    private static String mqtt_ip;
    private static int mqtt_port;
    private static String boxName;


    public RssToFeedParser(String ip, int port, String boxName) {
        mqtt_ip = ip;
        mqtt_port = port;
        this.boxName = boxName;
    }

    @Override
    public void configure() throws Exception {
        System.out.println("Hi.");
        from("rss:http://localhost/rss.xml?&splitEntries=false&consumer.delay=10000")
                .setHeader("json", body())
                .loopDoWhile(simple("${body} != null"))

                .setBody(method(FeedParser.class, "processValues(${header.json})"))
                //.log("${header.json}")
                    .to("mqtt:bar?host=tcp://"
                            + FeedParser.mqtt_ip
                            +":"
                            + FeedParser.mqtt_port
                            + "&publishTopicName=sudoku/"
                            + FeedParser.boxName.toLowerCase())

                .end();
    }
}
