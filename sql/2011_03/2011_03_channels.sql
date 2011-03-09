--
-- seance 2011_03
--

CREATE TABLE `channels` (
  `Nadc` smallint(5) unsigned NOT NULL default '0',
  `DetNumb` smallint(4) unsigned default NULL,
  `DetChan` smallint(4) unsigned default NULL,
  `T0` smallint(5) default NULL,
  `TMax` smallint(5) default NULL,
  PRIMARY KEY  (`Nadc`)
) ENGINE=MyISAM DEFAULT CHARSET=ascii;



INSERT INTO `channels` (`Nadc`, `DetNumb`, `DetChan`, `T0`, `TMax`) VALUES
(  162 /*   0*/,   1 /*    dch_1y*/,   1,    0,    0),
(  160 /*   1*/,   1 /*    dch_1y*/,   2,    0,    0),
(  154 /*   2*/,   1 /*    dch_1y*/,   3,    0,    0),
(  152 /*   3*/,   1 /*    dch_1y*/,   4,    0,    0),

(  146 /*   4*/,   2 /*   dch_1y^*/,   1,    0,    0),
(  144 /*   5*/,   2 /*   dch_1y^*/,   2,    0,    0),
(  138 /*   6*/,   2 /*   dch_1y^*/,   3,    0,    0),

(  194 /*   8*/,   3 /*    dch_2y*/,   1,    0,    0),
(  192 /*   9*/,   3 /*    dch_2y*/,   2,    0,    0),
(  186 /*  10*/,   3 /*    dch_2y*/,   3,    0,    0),
(  184 /*  11*/,   3 /*    dch_2y*/,   4,    0,    0),

(  178 /*  12*/,   4 /*   dch_2y^*/,   1,    0,    0),
(  176 /*  13*/,   4 /*   dch_2y^*/,   2,    0,    0),
(  170 /*  14*/,   4 /*   dch_2y^*/,   3,    0,    0),
--
-- next cham
--
(  158 /*  16*/,   5 /*    dch_1x*/,   1,    0,    0),
(  156 /*  17*/,   5 /*    dch_1x*/,   2,    0,    0),
(  150 /*  18*/,   5 /*    dch_1x*/,   3,    0,    0),
(  148 /*  19*/,   5 /*    dch_1x*/,   4,    0,    0),

(  142 /*  20*/,   6 /*   dch_1x^*/,   1,    0,    0),
(  140 /*  21*/,   6 /*   dch_1x^*/,   2,    0,    0),
(  134 /*  22*/,   6 /*   dch_1x^*/,   3,    0,    0),

(  190 /*  24*/,   7 /*    dch_2x*/,   1,    0,    0),
(  188 /*  25*/,   7 /*    dch_2x*/,   2,    0,    0),
(  182 /*  26*/,   7 /*    dch_2x*/,   3,    0,    0),
(  180 /*  27*/,   7 /*    dch_2x*/,   4,    0,    0),

(  174 /*  28*/,   8 /*   dch_2x^*/,   1,    0,    0),
(  172 /*  29*/,   8 /*   dch_2x^*/,   2,    0,    0),
(  166 /*  30*/,   8 /*   dch_2x^*/,   3,    0,    0),
--
-- next cham
--
(  675 /*  95*/,   9 /*   dch_10y*/,   1,    0,    0),
(  673 /*  93*/,   9 /*   dch_10y*/,   2,    0,    0),
(  671 /*  91*/,   9 /*   dch_10y*/,   3,    0,    0),
(  669 /*  89*/,   9 /*   dch_10y*/,   4,    0,    0),
(  674 /*  94*/,   9 /*   dch_10y*/,   5,    0,    0),
(  672 /*  92*/,   9 /*   dch_10y*/,   6,    0,    0),
(  670 /*  90*/,   9 /*   dch_10y*/,   7,    0,    0),

(  667 /*  87*/,  10 /*  dch_10y^*/,   1,    0,    0),
(  665 /*  85*/,  10 /*  dch_10y^*/,   2,    0,    0),
(  663 /*  83*/,  10 /*  dch_10y^*/,   3,    0,    0),
(  668 /*  88*/,  10 /*  dch_10y^*/,   4,    0,    0),
(  666 /*  86*/,  10 /*  dch_10y^*/,   5,    0,    0),
(  664 /*  84*/,  10 /*  dch_10y^*/,   6,    0,    0),

(  659 /*  79*/,  11 /*   dch_11y*/,   1,    0,    0),
(  657 /*  77*/,  11 /*   dch_11y*/,   2,    0,    0),
(  655 /*  75*/,  11 /*   dch_11y*/,   3,    0,    0),
(  653 /*  73*/,  11 /*   dch_11y*/,   4,    0,    0),
(  658 /*  78*/,  11 /*   dch_11y*/,   5,    0,    0),
(  656 /*  76*/,  11 /*   dch_11y*/,   6,    0,    0),
(  654 /*  74*/,  11 /*   dch_11y*/,   7,    0,    0),

(  651 /*  71*/,  12 /*  dch_11y^*/,   1,    0,    0),
(  649 /*  69*/,  12 /*  dch_11y^*/,   2,    0,    0),
(  647 /*  67*/,  12 /*  dch_11y^*/,   3,    0,    0),
(  652 /*  72*/,  12 /*  dch_11y^*/,   4,    0,    0),
(  650 /*  70*/,  12 /*  dch_11y^*/,   5,    0,    0),
(  648 /*  68*/,  12 /*  dch_11y^*/,   6,    0,    0),
--
-- next cham
--
(  707 /* 127*/,  13 /*   dch_10x*/,   1,    0,    0),
(  705 /* 125*/,  13 /*   dch_10x*/,   2,    0,    0),
(  703 /* 123*/,  13 /*   dch_10x*/,   3,    0,    0),
(  701 /* 121*/,  13 /*   dch_10x*/,   4,    0,    0),
(  706 /* 126*/,  13 /*   dch_10x*/,   5,    0,    0),
(  704 /* 124*/,  13 /*   dch_10x*/,   6,    0,    0),
(  702 /* 122*/,  13 /*   dch_10x*/,   7,    0,    0),

(  699 /* 119*/,  14 /*  dch_10x^*/,   1,    0,    0),
(  697 /* 117*/,  14 /*  dch_10x^*/,   2,    0,    0),
(  695 /* 115*/,  14 /*  dch_10x^*/,   3,    0,    0),
(  700 /* 120*/,  14 /*  dch_10x^*/,   4,    0,    0),
(  698 /* 118*/,  14 /*  dch_10x^*/,   5,    0,    0),
(  696 /* 116*/,  14 /*  dch_10x^*/,   6,    0,    0),

(  691 /* 111*/,  15 /*   dch_11x*/,   1,    0,    0),
(  689 /* 109*/,  15 /*   dch_11x*/,   2,    0,    0),
(  687 /* 107*/,  15 /*   dch_11x*/,   3,    0,    0),
(  685 /* 105*/,  15 /*   dch_11x*/,   4,    0,    0),
(  690 /* 110*/,  15 /*   dch_11x*/,   5,    0,    0),
(  688 /* 108*/,  15 /*   dch_11x*/,   6,    0,    0),
(  686 /* 106*/,  15 /*   dch_11x*/,   7,    0,    0),

(  683 /* 103*/,  16 /*  dch_11x^*/,   1,    0,    0),
(  681 /* 101*/,  16 /*  dch_11x^*/,   2,    0,    0),
(  679 /*  99*/,  16 /*  dch_11x^*/,   3,    0,    0),
(  684 /* 104*/,  16 /*  dch_11x^*/,   4,    0,    0),
(  682 /* 102*/,  16 /*  dch_11x^*/,   5,    0,    0),
(  680 /* 100*/,  16 /*  dch_11x^*/,   6,    0,    0),
--
-- next cham
--
(  762 /* 150*/,  17 /*  dch_10v^*/,   1,    0,    0),
(  760 /* 148*/,  17 /*  dch_10v^*/,   2,    0,    0),
(  758 /* 146*/,  17 /*  dch_10v^*/,   3,    0,    0),
(  748 /* 136*/,  17 /*  dch_10v^*/,   4,    0,    0),
(  746 /* 134*/,  17 /*  dch_10v^*/,   5,    0,    0),
(  744 /* 132*/,  17 /*  dch_10v^*/,   6,    0,    0),

(  770 /* 158*/,  18 /*   dch_10v*/,   1,    0,    0),
(  768 /* 156*/,  18 /*   dch_10v*/,   2,    0,    0),
(  766 /* 154*/,  18 /*   dch_10v*/,   3,    0,    0),
(  764 /* 152*/,  18 /*   dch_10v*/,   4,    0,    0),
(  754 /* 142*/,  18 /*   dch_10v*/,   5,    0,    0),
(  752 /* 140*/,  18 /*   dch_10v*/,   6,    0,    0),
(  750 /* 138*/,  18 /*   dch_10v*/,   7,    0,    0),

(  763 /* 151*/,  19 /*  dch_10u^*/,   1,    0,    0),
(  761 /* 149*/,  19 /*  dch_10u^*/,   2,    0,    0),
(  759 /* 147*/,  19 /*  dch_10u^*/,   3,    0,    0),
(  749 /* 137*/,  19 /*  dch_10u^*/,   4,    0,    0),
(  747 /* 135*/,  19 /*  dch_10u^*/,   5,    0,    0),
(  745 /* 133*/,  19 /*  dch_10u^*/,   6,    0,    0),

(  771 /* 159*/,  20 /*   dch_10u*/,   1,    0,    0),
(  769 /* 157*/,  20 /*   dch_10u*/,   2,    0,    0),
(  767 /* 155*/,  20 /*   dch_10u*/,   3,    0,    0),
(  765 /* 153*/,  20 /*   dch_10u*/,   4,    0,    0),
(  755 /* 143*/,  20 /*   dch_10u*/,   5,    0,    0),
(  753 /* 141*/,  20 /*   dch_10u*/,   6,    0,    0),
(  751 /* 139*/,  20 /*   dch_10u*/,   7,    0,    0);
