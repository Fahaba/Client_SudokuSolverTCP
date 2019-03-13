<?php
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

define("DB_HOST", "localhost");
define("DB_USER", "root");
define("DB_PASSWORD", "");
define("DB_NAME","FeedDB");

class Database extends MySQLi {
    private static $instance = null ;
    public function __construct($host, $user, $password, $database){
        parent::__construct($host, $user, $password, $database);
    }

    public static function getInstance(){
        if (self::$instance == null){
            self::$instance = new self(DB_HOST, DB_USER, DB_PASSWORD, DB_NAME);
            mysqli_set_charset(self::$instance, "UTF8");
        }
        return self::$instance ;
    }
}