package src.unused;
import com.sun.org.apache.xerces.internal.parsers.DOMParser;
import org.apache.camel.builder.RouteBuilder;
import org.w3c.dom.*;
import org.xml.sax.*;

import javax.xml.parsers.*;
import java.io.IOException;
import java.io.StringReader;

public class MainRoute extends RouteBuilder{

    private String a;

    public MainRoute(String a){
        this.a = a;
    }

    public void configure() throws IOException {

        from("rss:http://localhost/rss.xml?&splitEntries=false&consumer.delay=10000")
                .to("bean:FeedParser?method=processValues(${body})");
    }


}
