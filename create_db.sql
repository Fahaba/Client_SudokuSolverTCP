-- --------------------------------------------------------
-- Host:                         127.0.0.1
-- Server Version:               10.1.22-MariaDB - mariadb.org binary distribution
-- Server Betriebssystem:        Win32
-- HeidiSQL Version:             9.4.0.5125
-- --------------------------------------------------------

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET NAMES utf8 */;
/*!50503 SET NAMES utf8mb4 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;


-- Exportiere Datenbank Struktur für feeddb
CREATE DATABASE IF NOT EXISTS `feeddb` /*!40100 DEFAULT CHARACTER SET latin1 */;
USE `feeddb`;

-- Exportiere Struktur von Tabelle feeddb.feedentries
CREATE TABLE IF NOT EXISTS `feedentries` (
  `item_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `item_link` varchar(50) DEFAULT NULL,
  `item_description` varchar(50) DEFAULT NULL,
  PRIMARY KEY (`item_id`)
) ENGINE=InnoDB AUTO_INCREMENT=1954 DEFAULT CHARSET=latin1;

-- Exportiere Struktur von Tabelle feeddb.feedversion
CREATE TABLE IF NOT EXISTS `feedversion` (
  `pubDate` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- Exportiere Daten aus Tabelle feeddb.feedversion: ~0 rows (ungefähr)
/*!40000 ALTER TABLE `feedversion` DISABLE KEYS */;
INSERT INTO `feedversion` (`pubDate`) VALUES
	('2019-03-13 09:57:01');
/*!40000 ALTER TABLE `feedversion` ENABLE KEYS */;

/*!40101 SET SQL_MODE=IFNULL(@OLD_SQL_MODE, '') */;
/*!40014 SET FOREIGN_KEY_CHECKS=IF(@OLD_FOREIGN_KEY_CHECKS IS NULL, 1, @OLD_FOREIGN_KEY_CHECKS) */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
