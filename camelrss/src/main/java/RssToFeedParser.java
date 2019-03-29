
import org.apache.camel.builder.RouteBuilder;

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
        from("rss:"+FeedParser.RssURI+"/rss.xml?&splitEntries=false&consumer.delay=10000")
                .setHeader("json", body())
                .loopDoWhile(simple("${body} != null"))
                .setBody(method(FeedParser.class, "processValues(${header.json})"))
                    .to("mqtt:bar?host=tcp://"
                            + mqtt_ip
                            +":"
                            + mqtt_port
                            + "&publishTopicName=TEST/sudoku/"
                            + boxName.toLowerCase())
                .end();
    }
}
