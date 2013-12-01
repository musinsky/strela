-- phpMyAdmin SQL Dump
-- version 3.5.4
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Feb 05, 2013 at 10:04 AM
-- Server version: 5.5.29
-- PHP Version: 5.4.11

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `2009_12`
--

-- --------------------------------------------------------

--
-- Table structure for table `channels`
--

CREATE TABLE IF NOT EXISTS `channels` (
  `Nadc` smallint(5) unsigned NOT NULL DEFAULT '0',
  `DetNumb` smallint(4) unsigned DEFAULT NULL,
  `DetChan` smallint(4) unsigned DEFAULT NULL,
  `T0` smallint(5) DEFAULT NULL,
  `TMax` smallint(5) DEFAULT NULL,
  PRIMARY KEY (`Nadc`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

--
-- Dumping data for table `channels`
--

INSERT INTO `channels` (`Nadc`, `DetNumb`, `DetChan`, `T0`, `TMax`) VALUES
(162, 1, 1, 922, 0),
(160, 1, 2, 876, 0),
(154, 1, 3, 868, 0),
(152, 1, 4, 882, 0),
(146, 2, 1, 833, 0),
(144, 2, 2, 890, 0),
(138, 2, 3, 882, 0),
(194, 3, 1, 952, 0),
(192, 3, 2, 889, 0),
(186, 3, 3, 889, 0),
(184, 3, 4, 856, 0),
(178, 4, 1, 936, 0),
(176, 4, 2, 889, 0),
(170, 4, 3, 885, 0),
(158, 5, 1, 914, 0),
(156, 5, 2, 885, 0),
(150, 5, 3, 884, 0),
(148, 5, 4, 924, 0),
(142, 6, 1, 884, 0),
(140, 6, 2, 884, 0),
(134, 6, 3, 876, 0),
(190, 7, 1, 894, 0),
(188, 7, 2, 890, 0),
(182, 7, 3, 883, 0),
(180, 7, 4, 888, 0),
(174, 8, 1, 877, 0),
(172, 8, 2, 877, 0),
(166, 8, 3, 880, 0),
(1187, 9, 1, 1067, 0),
(1185, 9, 2, 970, 0),
(1183, 9, 3, 967, 0),
(1181, 9, 4, 1073, 0),
(1179, 10, 1, 960, 0),
(1177, 10, 2, 966, 0),
(1175, 10, 3, 976, 0),
(1171, 11, 1, 1151, 0),
(1169, 11, 2, 985, 0),
(1167, 11, 3, 974, 0),
(1165, 11, 4, 1083, 0),
(1163, 12, 1, 983, 0),
(1161, 12, 2, 972, 0),
(1159, 12, 3, 972, 0),
(1186, 13, 1, 0, 0),
(1184, 13, 2, 0, 0),
(1182, 13, 3, 0, 0),
(1180, 13, 4, 0, 0),
(1178, 14, 1, 0, 0),
(1176, 14, 2, 0, 0),
(1174, 14, 3, 0, 0),
(1170, 15, 1, 0, 0),
(1168, 15, 2, 0, 0),
(1166, 15, 3, 0, 0),
(1164, 15, 4, 0, 0),
(1162, 16, 1, 0, 0),
(1160, 16, 2, 0, 0),
(1158, 16, 3, 0, 0),
(1219, 17, 1, 1075, 0),
(1217, 17, 2, 947, 0),
(1215, 17, 3, 944, 0),
(1213, 17, 4, 1022, 0),
(1211, 18, 1, 1067, 0),
(1209, 18, 2, 943, 0),
(1207, 18, 3, 940, 0),
(1205, 18, 4, 1041, 0),
(1203, 19, 1, 941, 0),
(1201, 19, 2, 942, 0),
(1199, 19, 3, 937, 0),
(1197, 20, 1, 948, 0),
(1195, 20, 2, 942, 0),
(1193, 20, 3, 943, 0),
(1218, 21, 1, 1050, 0),
(1216, 21, 2, 958, 0),
(1214, 21, 3, 948, 0),
(1212, 21, 4, 1021, 0),
(1210, 22, 1, 953, 0),
(1208, 22, 2, 989, 0),
(1206, 22, 3, 993, 0),
(1202, 23, 1, 1068, 0),
(1200, 23, 2, 1004, 0),
(1198, 23, 3, 1013, 0),
(1196, 23, 4, 1055, 0),
(1194, 24, 1, 963, 0),
(1192, 24, 2, 962, 0),
(1190, 24, 3, 958, 0),
(675, 25, 1, 1084, 0),
(673, 25, 2, 920, 0),
(671, 25, 3, 918, 0),
(669, 25, 4, 916, 0),
(674, 25, 5, 912, 0),
(672, 25, 6, 930, 0),
(670, 25, 7, 1043, 0),
(667, 26, 1, 924, 0),
(665, 26, 2, 910, 0),
(663, 26, 3, 909, 0),
(668, 26, 4, 907, 0),
(666, 26, 5, 903, 0),
(664, 26, 6, 921, 0),
(659, 27, 1, 1057, 0),
(657, 27, 2, 910, 0),
(655, 27, 3, 916, 0),
(653, 27, 4, 910, 0),
(658, 27, 5, 908, 0),
(656, 27, 6, 901, 0),
(654, 27, 7, 1113, 0),
(651, 28, 1, 924, 0),
(649, 28, 2, 916, 0),
(647, 28, 3, 917, 0),
(652, 28, 4, 908, 0),
(650, 28, 5, 910, 0),
(648, 28, 6, 982, 0),
(707, 29, 1, 1056, 0),
(705, 29, 2, 925, 0),
(703, 29, 3, 928, 0),
(701, 29, 4, 926, 0),
(706, 29, 5, 919, 0),
(704, 29, 6, 922, 0),
(702, 29, 7, 1018, 0),
(699, 30, 1, 927, 0),
(697, 30, 2, 928, 0),
(695, 30, 3, 916, 0),
(700, 30, 4, 917, 0),
(698, 30, 5, 909, 0),
(696, 30, 6, 877, 0),
(691, 31, 1, 1026, 0),
(689, 31, 2, 925, 0),
(687, 31, 3, 920, 0),
(685, 31, 4, 923, 0),
(690, 31, 5, 940, 0),
(688, 31, 6, 929, 0),
(686, 31, 7, 1035, 0),
(683, 32, 1, 929, 0),
(681, 32, 2, 928, 0),
(679, 32, 3, 925, 0),
(684, 32, 4, 915, 0),
(682, 32, 5, 939, 0),
(680, 32, 6, 943, 0),
(762, 33, 1, 0, 0),
(760, 33, 2, 0, 0),
(758, 33, 3, 0, 0),
(748, 33, 4, 0, 0),
(746, 33, 5, 0, 0),
(744, 33, 6, 0, 0),
(770, 34, 1, 0, 0),
(768, 34, 2, 0, 0),
(766, 34, 3, 0, 0),
(764, 34, 4, 0, 0),
(754, 34, 5, 0, 0),
(752, 34, 6, 0, 0),
(750, 34, 7, 0, 0),
(763, 35, 1, 0, 0),
(761, 35, 2, 0, 0),
(759, 35, 3, 0, 0),
(749, 35, 4, 0, 0),
(747, 35, 5, 0, 0),
(745, 35, 6, 0, 0),
(771, 36, 1, 0, 0),
(769, 36, 2, 0, 0),
(767, 36, 3, 0, 0),
(765, 36, 4, 0, 0),
(755, 36, 5, 0, 0),
(753, 36, 6, 0, 0),
(751, 36, 7, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `layers`
--

CREATE TABLE IF NOT EXISTS `layers` (
  `Numb` smallint(4) unsigned NOT NULL DEFAULT '0',
  `Tracker` smallint(4) unsigned DEFAULT NULL,
  `Name` varchar(10) DEFAULT NULL,
  `NChan` smallint(4) unsigned DEFAULT NULL,
  `Z` float(8,4) DEFAULT NULL,
  `Shift` float(8,4) DEFAULT NULL,
  `Delta` float(8,4) DEFAULT NULL,
  `Range` float(8,4) DEFAULT NULL,
  `Direct` smallint(2) DEFAULT NULL,
  PRIMARY KEY (`Numb`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;

--
-- Dumping data for table `layers`
--

INSERT INTO `layers` (`Numb`, `Tracker`, `Name`, `NChan`, `Z`, `Shift`, `Delta`, `Range`, `Direct`) VALUES
(1, 1, 'dch_1y', 4, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(2, 1, 'dch_1y^', 3, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(3, 1, 'dch_2y', 4, 5.2000, 0.0000, 4.2000, 2.1000, 1),
(4, 1, 'dch_2y^', 3, 7.8000, 0.0000, 4.2000, 2.1000, 1),
(5, 2, 'dch_1x', 4, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(6, 2, 'dch_1x^', 3, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(7, 2, 'dch_2x', 4, 5.2000, 0.0000, 4.2000, 2.1000, 1),
(8, 2, 'dch_2x^', 3, 7.8000, 0.0000, 4.2000, 2.1000, 1),
(9, 3, 'dch_3y', 4, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(10, 3, 'dch_3y^', 3, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(11, 4, 'dch_3x', 4, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(12, 4, 'dch_3x^', 3, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(13, 0, 'dch_1v', 4, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(14, 0, 'dch_1v^', 3, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(15, 0, 'dch_1u', 4, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(16, 0, 'dch_1u^', 3, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(17, 5, 'dch_4x', 4, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(18, 5, 'dch_5x', 4, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(19, 5, 'dch_4x^', 3, 5.2000, 0.0000, 4.2000, 2.1000, 1),
(20, 5, 'dch_5x^', 3, 7.8000, 0.0000, 4.2000, 2.1000, 1),
(21, 3, 'dch_6y', 4, 59.4000, 0.0000, 4.2000, 2.1000, 1),
(22, 3, 'dch_6y^', 3, 62.0000, 0.0000, 4.2000, 2.1000, 1),
(23, 4, 'dch_6x', 4, 59.4000, 0.0000, 4.2000, 2.1000, 1),
(24, 4, 'dch_6x^', 3, 62.0000, 0.0000, 4.2000, 2.1000, 1),
(25, 10, 'dch_10y', 7, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(26, 10, 'dch_10y^', 6, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(27, 10, 'dch_11y', 7, 5.2000, 0.0000, 4.2000, 2.1000, 1),
(28, 10, 'dch_11y^', 6, 7.8000, 0.0000, 4.2000, 2.1000, 1),
(29, 11, 'dch_10x', 7, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(30, 11, 'dch_10x^', 6, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(31, 11, 'dch_11x', 7, 5.2000, 0.0000, 4.2000, 2.1000, 1),
(32, 11, 'dch_11x^', 6, 7.8000, 0.0000, 4.2000, 2.1000, 1),
(33, 0, 'dch_10v^', 6, 0.0000, 0.0000, 4.2000, 2.1000, -1),
(34, 0, 'dch_10v', 7, 2.6000, 0.0000, 4.2000, 2.1000, -1),
(35, 0, 'dch_10u^', 6, 0.0000, 0.0000, 4.2000, 2.1000, -1),
(36, 0, 'dch_10u', 7, 2.6000, 0.0000, 4.2000, 2.1000, -1);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;