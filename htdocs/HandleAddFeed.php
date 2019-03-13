<?php
/**
 * Created by PhpStorm.
 * User: fahas
 * Date: 24.02.2019
 * Time: 23:34
 */
include_once(dirname(__FILE__).'/DBconnector.php');

if (isset($_GET['message']))
{
    $conn = Database::getInstance();

    $query1 = "UPDATE feedVersion SET pubDate=now();";
    $query2 = "INSERT INTO feedEntries(item_link,item_description) VALUES (\"localhost\",\"".$_GET['message']."\")";
    $conn->query($query1);
    if ($conn->query($query2) === FALSE)
    {
        echo "ERROR: " .$conn->error;
    }
    BuildNewFeedVersion();
}

function BuildNewFeedVersion()
{
    $conn = Database::getInstance();
    $pubDateQuery = "SELECT * FROM feedVersion";
    $res = $conn->query($pubDateQuery);
    $pubDate = $res->fetch_row();

    $newRssFeed = "";
    $newRssFeed .= "<rss version='2.0'>\r\n";
    $newRssFeed .= "<channel>\n";
    $newRssFeed .= "<title>Sodoku-Feed</title>\n";
    $newRssFeed .= "<link>http://localhost/</link>\n";
    $newRssFeed .= "<description>Distributed Sudoku Solver Feed</description>\n";
    $newRssFeed .= "<language>en-us</language>\n";
    $newRssFeed .= "<copyright>Copyright (C) 2019 Fabian Hasselbach</copyright>\n";
    $newRssFeed .= "<pubDate>".date("D, d M Y H:i:s O", strtotime($pubDate[0]))."</pubDate>\n";

    $query = "SELECT * FROM feedentries";
    $result = $conn->query($query);

    while ($row = $result->fetch_array(MYSQLI_BOTH))
    {
        $newRssFeed .= "<item>\n";
        $newRssFeed .= "<title>Message:".$row[0]."</title>\n";
        $newRssFeed .= "<link>http://".$row[1]."/</link>\n";
        $newRssFeed .= "<description>".$row[2]."</description>\n";
        $newRssFeed .= "</item>\n";
    }

    $newRssFeed .= "</channel>\n";
    $newRssFeed .= "</rss>\n";

    file_put_contents(dirname(__FILE__).'/rss.xml', $newRssFeed);
    echo $newRssFeed;
}