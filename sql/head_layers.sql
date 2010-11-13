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
