--
-- seance 2007_03
--

CREATE TABLE `layers` (
  `Numb` smallint(4) unsigned NOT NULL default '0',
  `Tracker` smallint(4) unsigned default NULL,
  `Name` varchar(10) default NULL,
  `NChan` smallint(4) unsigned default NULL,
  `Z` float(8,4) default NULL,
  `Shift` float(8,4) default NULL,
  `Delta` float(8,4) default NULL,
  `Range` float(8,4) default NULL,
  `Direct` smallint(2) default NULL,
  PRIMARY KEY  (`Numb`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;



INSERT INTO `layers` (`Numb`, `Tracker`, `Name`, `NChan`, `Z`, `Shift`, `Delta`, `Range`, `Direct`) VALUES
(  1,  2,   'dch_1y',   4,   0.0000,   0.0000,   4.2000,   2.1000,  1),
(  2,  2,  'dch_1y^',   3,   2.6000,   0.0000,   4.2000,   2.1000,  1),
(  3,  2,   'dch_2y',   4,   5.2000,   0.0000,   4.2000,   2.1000,  1),
(  4,  2,  'dch_2y^',   3,   7.8000,   0.0000,   4.2000,   2.1000,  1),

(  5,  1,   'dch_1x',   4,   0.0000,   0.0000,   4.2000,   2.1000,  1),
(  6,  1,  'dch_1x^',   3,   2.6000,   0.0000,   4.2000,   2.1000,  1),
(  7,  1,   'dch_2x',   4,   5.2000,   0.0000,   4.2000,   2.1000,  1),
(  8,  1,  'dch_2x^',   3,   7.8000,   0.0000,   4.2000,   2.1000,  1),

(  9,  4,   'dch_3y',   4,   0.0000,   0.0000,   4.2000,   2.1000,  1),
( 10,  4,  'dch_3y^',   3,   2.6000,   0.0000,   4.2000,   2.1000,  1),
( 11,  3,   'dch_3x',   4,   0.0000,   0.0000,   4.2000,   2.1000,  1),
( 12,  3,  'dch_3x^',   3,   2.6000,   0.0000,   4.2000,   2.1000,  1),

( 13,  5,   'dch_4x',   4,   0.0000,   0.0000,   4.2000,   2.1000,  1),
( 14,  5,   'dch_5x',   4,   2.6000,   0.0000,   4.2000,   2.1000,  1),
( 15,  5,  'dch_4x^',   3,   5.2000,   0.0000,   4.2000,   2.1000,  1),
( 16,  5,  'dch_5x^',   3,   7.8000,   0.0000,   4.2000,   2.1000,  1),

( 17,  4,   'dch_6y',   4,  59.4000,   0.0000,   4.2000,   2.1000,  1),
( 18,  4,  'dch_6y^',   3,  62.0000,   0.0000,   4.2000,   2.1000,  1),
( 19,  3,   'dch_6x',   4,  59.4000,   0.0000,   4.2000,   2.1000,  1),
( 20,  3,  'dch_6x^',   3,  62.0000,   0.0000,   4.2000,   2.1000,  1),

( 21, 11,  'dch_10y',   7,   0.0000,   0.0000,   4.2000,   2.1000,  1),
( 22, 11, 'dch_10y^',   6,   2.6000,   0.0000,   4.2000,   2.1000,  1),
( 23, 11,  'dch_11y',   7,   5.2000,   0.0000,   4.2000,   2.1000,  1),
( 24, 11, 'dch_11y^',   6,   7.8000,   0.0000,   4.2000,   2.1000,  1),

( 25, 10,  'dch_10x',   7,   0.0000,   0.0000,   4.2000,   2.1000,  1),
( 26, 10, 'dch_10x^',   6,   2.6000,   0.0000,   4.2000,   2.1000,  1),
( 27, 10,  'dch_11x',   7,   5.2000,   0.0000,   4.2000,   2.1000,  1),
( 28, 10, 'dch_11x^',   6,   7.8000,   0.0000,   4.2000,   2.1000,  1);
