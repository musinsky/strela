CREATE TABLE `channels` (
  `Nadc` smallint(5) unsigned NOT NULL default '0',
  `DetNumb` smallint(4) unsigned default NULL,
  `DetChan` smallint(4) unsigned default NULL,
  `T0` smallint(5) default NULL,
  `TMax` smallint(5) default NULL,
  PRIMARY KEY  (`Nadc`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;
