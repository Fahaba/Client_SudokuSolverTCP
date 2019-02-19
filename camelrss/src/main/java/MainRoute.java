import com.sun.syndication.feed.synd.SyndEntry;
import com.sun.syndication.feed.synd.SyndFeed;
import org.apache.camel.Body;
import org.apache.camel.builder.RouteBuilder;
import org.apache.camel.component.mock.MockEndpoint;
import org.apache.camel.util.jndi.JndiContext;
import org.apache.camel.test.junit4.CamelTestSupport;
import org.junit.Test;

import javax.naming.Context;
public class MainRoute extends RouteBuilder{
    @Override
    public void configure() throws Exception {
        System.out.println("Hi.");
        from("rss:http://localhost/rss.xml?&splitEntries=true&consumer.delay=10000")
                .log("HAJHAHA")
                .to("seda:end");
    }
}
