CREATE DATABASE IF NOT EXISTS `vital_project` DEFAULT CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci;
USE `vital_project`;
DROP TABLE IF EXISTS `VitalData`;
CREATE TABLE IF NOT EXISTS `VitalData` (
  `id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `temp` varchar(30) NOT NULL,
  `heart_rate` varchar(30) NOT NULL,
  `reading_time` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=116 DEFAULT CHARSET=utf8mb4;
