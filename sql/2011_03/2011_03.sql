-- phpMyAdmin SQL Dump
-- version 3.5.4
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Feb 05, 2013 at 10:05 AM
-- Server version: 5.5.29
-- PHP Version: 5.4.11

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `2011_03`
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
(162, 1, 1, 640, 0),
(160, 1, 2, 622, 0),
(154, 1, 3, 613, 0),
(152, 1, 4, 639, 0),
(146, 2, 1, 633, 0),
(144, 2, 2, 632, 0),
(138, 2, 3, 624, 0),
(194, 3, 1, 644, 0),
(192, 3, 2, 631, 0),
(186, 3, 3, 630, 0),
(184, 3, 4, 618, 0),
(178, 4, 1, 630, 0),
(176, 4, 2, 630, 0),
(170, 4, 3, 628, 0),
(158, 5, 1, 673, 0),
(156, 5, 2, 629, 0),
(150, 5, 3, 614, 0),
(148, 5, 4, 633, 0),
(142, 6, 1, 632, 0),
(140, 6, 2, 620, 0),
(134, 6, 3, 615, 0),
(190, 7, 1, 649, 0),
(188, 7, 2, 633, 0),
(182, 7, 3, 618, 0),
(180, 7, 4, 630, 0),
(174, 8, 1, 629, 0),
(172, 8, 2, 615, 0),
(166, 8, 3, 617, 0),
(675, 9, 1, 590, 0),
(673, 9, 2, 596, 0),
(671, 9, 3, 592, 0),
(669, 9, 4, 587, 0),
(674, 9, 5, 579, 0),
(672, 9, 6, 579, 0),
(670, 9, 7, 590, 0),
(667, 10, 1, 589, 0),
(665, 10, 2, 585, 0),
(663, 10, 3, 579, 0),
(668, 10, 4, 577, 0),
(666, 10, 5, 572, 0),
(664, 10, 6, 590, 0),
(659, 11, 1, 590, 0),
(657, 11, 2, 584, 0),
(655, 11, 3, 587, 0),
(653, 11, 4, 580, 0),
(658, 11, 5, 580, 0),
(656, 11, 6, 572, 0),
(654, 11, 7, 590, 0),
(651, 12, 1, 590, 0),
(649, 12, 2, 590, 0),
(647, 12, 3, 586, 0),
(652, 12, 4, 579, 0),
(650, 12, 5, 577, 0),
(648, 12, 6, 582, 0),
(707, 13, 1, 600, 0),
(705, 13, 2, 589, 0),
(703, 13, 3, 604, 0),
(701, 13, 4, 611, 0),
(706, 13, 5, 593, 0),
(704, 13, 6, 578, 0),
(702, 13, 7, 600, 0),
(699, 14, 1, 595, 0),
(697, 14, 2, 595, 0),
(695, 14, 3, 596, 0),
(700, 14, 4, 602, 0),
(698, 14, 5, 582, 0),
(696, 14, 6, 568, 0),
(691, 15, 1, 600, 0),
(689, 15, 2, 589, 0),
(687, 15, 3, 598, 0),
(685, 15, 4, 607, 0),
(690, 15, 5, 598, 0),
(688, 15, 6, 578, 0),
(686, 15, 7, 600, 0),
(683, 16, 1, 600, 0),
(681, 16, 2, 600, 0),
(679, 16, 3, 600, 0),
(684, 16, 4, 608, 0),
(682, 16, 5, 593, 0),
(680, 16, 6, 579, 0),
(762, 17, 1, 0, 0),
(760, 17, 2, 0, 0),
(758, 17, 3, 0, 0),
(748, 17, 4, 0, 0),
(746, 17, 5, 0, 0),
(744, 17, 6, 0, 0),
(770, 18, 1, 0, 0),
(768, 18, 2, 0, 0),
(766, 18, 3, 0, 0),
(764, 18, 4, 0, 0),
(754, 18, 5, 0, 0),
(752, 18, 6, 0, 0),
(750, 18, 7, 0, 0),
(763, 19, 1, 0, 0),
(761, 19, 2, 0, 0),
(759, 19, 3, 0, 0),
(749, 19, 4, 0, 0),
(747, 19, 5, 0, 0),
(745, 19, 6, 0, 0),
(771, 20, 1, 0, 0),
(769, 20, 2, 0, 0),
(767, 20, 3, 0, 0),
(765, 20, 4, 0, 0),
(755, 20, 5, 0, 0),
(753, 20, 6, 0, 0),
(751, 20, 7, 0, 0);

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
(9, 10, 'dch_10y', 7, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(10, 10, 'dch_10y^', 6, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(11, 10, 'dch_11y', 7, 5.2000, 0.0000, 4.2000, 2.1000, 1),
(12, 10, 'dch_11y^', 6, 7.8000, 0.0000, 4.2000, 2.1000, 1),
(13, 11, 'dch_10x', 7, 0.0000, 0.0000, 4.2000, 2.1000, 1),
(14, 11, 'dch_10x^', 6, 2.6000, 0.0000, 4.2000, 2.1000, 1),
(15, 11, 'dch_11x', 7, 5.2000, 0.0000, 4.2000, 2.1000, 1),
(16, 11, 'dch_11x^', 6, 7.8000, 0.0000, 4.2000, 2.1000, 1),
(17, 0, 'dch_10v^', 6, 0.0000, 0.0000, 4.2000, 2.1000, -1),
(18, 0, 'dch_10v', 7, 2.6000, 0.0000, 4.2000, 2.1000, -1),
(19, 0, 'dch_10u^', 6, 0.0000, 0.0000, 4.2000, 2.1000, -1),
(20, 0, 'dch_10u', 7, 2.6000, 0.0000, 4.2000, 2.1000, -1);

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
