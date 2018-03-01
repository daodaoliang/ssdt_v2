-- ----------------------------
-- Database
-- ----------------------------
DROP DATABASE IF EXISTS `$$DatabaseName$$`;
CREATE DATABASE `$$DatabaseName$$`;

-- ----------------------------
-- Table structure for address
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`address`;
CREATE TABLE `$$DatabaseName$$`.`address` (
  `address_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` int(10) unsigned NOT NULL,
  `ap_name` varchar(50) NOT NULL,
  `ip` varchar(15) NOT NULL,
  `ip_subnet` varchar(15) NOT NULL,
  `ip_gateway` varchar(15) NOT NULL,
  `osi_nsap` varchar(50) NOT NULL,
  `osi_tsel` varchar(50) NOT NULL,
  `osi_ssel` varchar(50) NOT NULL,
  `osi_psel` varchar(50) NOT NULL,
  `osi_ap_title` varchar(50) NOT NULL,
  `osi_ap_invoke` varchar(50) NOT NULL,
  `osi_ae_qualifier` varchar(50) NOT NULL,
  `osi_ae_invoke` varchar(50) NOT NULL,
  PRIMARY KEY (`address_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for bay
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`bay`;
CREATE TABLE `$$DatabaseName$$`.`bay` (
  `bay_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `number` varchar(50) NOT NULL DEFAULT '',
  `vlevel` int(11) NOT NULL DEFAULT '-1',
  PRIMARY KEY (`bay_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for board
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`board`;
CREATE TABLE `$$DatabaseName$$`.`board` (
  `board_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` int(11) unsigned NOT NULL,
  `position` varchar(50) NOT NULL DEFAULT '',
  `description` varchar(100) NOT NULL DEFAULT '',
  `manufacture` varchar(50) NOT NULL DEFAULT '',
  `type` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`board_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cable
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`cable`;
CREATE TABLE `$$DatabaseName$$`.`cable` (
  `cable_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `cubicle1_id` int(10) unsigned NOT NULL,
  `cubicle2_id` int(10) unsigned NOT NULL,
  `name` varchar(50) NOT NULL,
  `cable_type` tinyint(4) NOT NULL,
  PRIMARY KEY (`cable_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cubicle
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`cubicle`;
CREATE TABLE `$$DatabaseName$$`.`cubicle` (
  `cubicle_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `room_id` int(10) unsigned NOT NULL DEFAULT '0',
  `name` varchar(50) NOT NULL,
  `size` varchar(50) NOT NULL DEFAULT '',
  `manufacture` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`cubicle_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for cubicle_connection
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`cubicle_connection`;
CREATE TABLE `$$DatabaseName$$`.`cubicle_connection` (
  `connection_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `cubicle1_id` int(10) unsigned NOT NULL,
  `use_odf1` tinyint(4) unsigned NOT NULL,
  `passcubicle1_id` int(10) unsigned NOT NULL,
  `cubicle2_id` int(10) unsigned NOT NULL,
  `use_odf2` tinyint(4) unsigned NOT NULL,
  `passcubicle2_id` int(10) unsigned NOT NULL,
  PRIMARY KEY (`connection_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for device
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`device`;
CREATE TABLE `$$DatabaseName$$`.`device` (
  `device_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `cubicle_id` int(10) unsigned NOT NULL DEFAULT '0',
  `cubicle_pos` varchar(50) NOT NULL DEFAULT '',
  `cubicle_num` varchar(50) NOT NULL DEFAULT '',
  `bay_id` int(10) unsigned NOT NULL,
  `device_type` tinyint(4) NOT NULL,
  `name` varchar(50) NOT NULL,
  `description` varchar(100) NOT NULL DEFAULT '',
  `manufacture` varchar(50) NOT NULL DEFAULT '',
  `type` varchar(50) NOT NULL DEFAULT '',
  `config_version` varchar(50) NOT NULL DEFAULT '',
  `vlevel` int(11) NOT NULL DEFAULT '0',
  `icd_content` longblob,
  `lock` bit(1) NOT NULL DEFAULT b'0',
  PRIMARY KEY (`device_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for dwg
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`dwg`;
CREATE TABLE `$$DatabaseName$$`.`dwg` (
  `dwg_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `volume_id` int(10) unsigned NOT NULL,
  `type` tinyint(4) NOT NULL,
  `name` varchar(50) NOT NULL,
  `description` varchar(100) NOT NULL,
  `file` longblob,
  PRIMARY KEY (`dwg_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for fiber
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`fiber`;
CREATE TABLE `$$DatabaseName$$`.`fiber` (
  `fiber_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `cable_id` int(10) unsigned NOT NULL,
  `port1_id` int(10) unsigned NOT NULL,
  `port2_id` int(10) unsigned NOT NULL,
  `index` int(10) unsigned NOT NULL DEFAULT '0',
  `fiber_color` tinyint(50) NOT NULL DEFAULT '0',
  `pipe_color` tinyint(50) NOT NULL DEFAULT '0',
  `reserve` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`fiber_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for gse
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`gse`;
CREATE TABLE `$$DatabaseName$$`.`gse` (
  `gse_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` int(10) unsigned NOT NULL,
  `ap_name` varchar(50) NOT NULL,
  `ld_inst` varchar(50) NOT NULL DEFAULT '',
  `cb_name` varchar(50) NOT NULL,
  `mac_address` varchar(17) NOT NULL,
  `vlan_id` varchar(50) NOT NULL,
  `vlan_priority` varchar(50) NOT NULL,
  `appid` varchar(50) NOT NULL,
  `mintime` varchar(50) NOT NULL,
  `maxtime` varchar(50) NOT NULL,
  `station` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`gse_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for infoset
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`infoset`;
CREATE TABLE `$$DatabaseName$$`.`infoset` (
  `infoset_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `description` varchar(100) NOT NULL DEFAULT '',
  `type` tinyint(4) NOT NULL,
  `group` int(10) unsigned NOT NULL,
  `txied_id` int(10) unsigned NOT NULL DEFAULT '0',
  `txiedport_id` int(10) unsigned NOT NULL DEFAULT '0',
  `rxied_id` int(10) unsigned NOT NULL DEFAULT '0',
  `rxiedport_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch1_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch1_txport_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch1_rxport_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch2_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch2_txport_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch2_rxport_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch3_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch3_txport_id` int(10) unsigned NOT NULL DEFAULT '0',
  `switch3_rxport_id` int(10) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`infoset_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for port
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`port`;
CREATE TABLE `$$DatabaseName$$`.`port` (
  `port_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `board_id` int(11) unsigned NOT NULL,
  `name` varchar(50) NOT NULL,
  `group` int(10) unsigned NOT NULL,
  `type` tinyint(4) NOT NULL,
  `direction` tinyint(4) NOT NULL,
  `fiber_plug` tinyint(4) NOT NULL,
  `fiber_mode` tinyint(4) NOT NULL,
  `cascade` tinyint(4) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`port_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for room
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`room`;
CREATE TABLE `$$DatabaseName$$`.`room` (
  `room_id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  PRIMARY KEY (`room_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for scl_template
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`scl_template`;
CREATE TABLE `$$DatabaseName$$`.`scl_template` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `template_blob` longblob COMMENT 'scl模板',
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for smv
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`smv`;
CREATE TABLE `$$DatabaseName$$`.`smv` (
  `smv_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` int(10) unsigned NOT NULL,
  `ap_name` varchar(50) NOT NULL,
  `ld_inst` varchar(50) NOT NULL DEFAULT '',
  `cb_name` varchar(50) NOT NULL,
  `mac_address` varchar(17) NOT NULL,
  `vlan_id` varchar(50) NOT NULL,
  `vlan_priority` varchar(50) NOT NULL,
  `appid` varchar(50) NOT NULL,
  PRIMARY KEY (`smv_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for strap
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`strap`;
CREATE TABLE `$$DatabaseName$$`.`strap` (
  `strap_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` int(10) unsigned NOT NULL,
  `strap_no` int(10) unsigned NOT NULL,
  `description` varchar(100) NOT NULL DEFAULT '',
  `ld_inst` varchar(50) NOT NULL DEFAULT '',
  `ln_prefix` varchar(50) NOT NULL DEFAULT '',
  `ln_class` varchar(50) NOT NULL DEFAULT '',
  `ln_inst` varchar(50) NOT NULL DEFAULT '',
  `do_name` varchar(50) NOT NULL DEFAULT '',
  `da_name` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`strap_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for volume
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`volume`;
CREATE TABLE `$$DatabaseName$$`.`volume` (
  `volume_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(50) NOT NULL,
  `description` varchar(100) NOT NULL DEFAULT '',
  PRIMARY KEY (`volume_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for vterminal
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`vterminal`;
CREATE TABLE `$$DatabaseName$$`.`vterminal` (
  `vterminal_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` int(10) unsigned NOT NULL,
  `type` tinyint(4) NOT NULL,
  `direction` tinyint(4) NOT NULL,
  `vterminal_no` int(10) unsigned NOT NULL,
  `ied_desc` varchar(100) NOT NULL DEFAULT '',
  `pro_desc` varchar(100) NOT NULL DEFAULT '',
  `ld_inst` varchar(50) NOT NULL DEFAULT '',
  `ln_prefix` varchar(50) NOT NULL DEFAULT '',
  `ln_class` varchar(50) NOT NULL DEFAULT '',
  `ln_inst` varchar(50) NOT NULL DEFAULT '',
  `do_name` varchar(50) NOT NULL DEFAULT '',
  `da_name` varchar(50) NOT NULL DEFAULT '',
  PRIMARY KEY (`vterminal_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- ----------------------------
-- Table structure for vterminal_connection
-- ----------------------------
DROP TABLE IF EXISTS `$$DatabaseName$$`.`vterminal_connection`;
CREATE TABLE `$$DatabaseName$$`.`vterminal_connection` (
  `connection_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `txvterminal_id` int(10) unsigned NOT NULL,
  `txstrap_id` int(10) unsigned NOT NULL DEFAULT '0',
  `rxvterminal_id` int(10) unsigned NOT NULL,
  `rxstrap_id` int(10) unsigned NOT NULL DEFAULT '0',
  `straight` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`connection_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
