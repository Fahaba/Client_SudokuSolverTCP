import org.apache.commons.io.IOUtils;

import java.io.IOException;

public class TaskStreamCopyHelper extends Thread{

    Process p;
    TaskStreamCopyHelper(String path, String initString, String boxName) throws IOException {
        p = new ProcessBuilder(path, boxName, initString, "127.0.0.1", "1337").start();
    }

    public void run() {
        while (true)
        {
            try {
                sleep(100);
                IOUtils.copy(p.getInputStream(), System.out);
            } catch (InterruptedException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
