-- ALTER TABLE `creature` CHANGE `zoneId` `zoneId` SMALLINT(8) UNSIGNED DEFAULT '0' NOT NULL, CHANGE `areaId` `areaId` SMALLINT(8) UNSIGNED DEFAULT '0' NOT NULL;ALTER TABLE `creature` CHANGE `zoneId` `zoneId` INT(8) UNSIGNED DEFAULT '0' NOT NULL, CHANGE `areaId` `areaId` INT(8) UNSIGNED DEFAULT '0' NOT NULL;