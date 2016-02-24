-- Dudgeboat Salvage loot for quest Ship Salvage
DELETE FROM gameobject_loot_template where entry = 230879;
insert into `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `lootmode`, `groupid`, `mincountOrRef`, `maxcount`, `itemBonuses`) values
('230879','111907','-100','1','0','1','1','');

UPDATE gameobject_template SET data1 = 230879, flags = 0 WHERE entry in (230879, 230880, 230881);

-- Drudgeboat Salvage spawn
DELETE FROM gameobject WHERE id IN (230879, 230880, 230881);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7062','1','1','5656.31','4919.1','15.0634','5.73839','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7062','1','1','5742.41','4892.2','30.0262','3.64361','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7062','1','1','5742.94','4898.1','29.7908','0.0978507','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7062','1','1','5686.52','5029.7','31.7383','2.35632','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7257','1','1','5654.38','4933.54','14.4938','5.54686','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7257','1','1','5657.88','5023.83','28.8026','0.0978507','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7062','1','1','5736.98','4899.85','29.355','0.0600323','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7062','1','1','5655.77','4912.72','14.7241','1.22845','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7062','1','1','5723.81','4939.8','29.9174','3.64023','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7257','1','1','5662.62','5022.9','29.0532','2.52414','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7257','1','1','5663.23','4947.1','15.2013','5.67186','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7062','1','1','5730.66','4921.95','28.6963','0.194753','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230879','1116','6720','7062','1','1','5649.41','4926.8','15.1692','5.17388','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230879','1116','6720','7062','1','1','5735.31','4917.07','28.6798','2.92067','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230879','1116','6720','7257','1','1','5661.23','4955.27','13.7948','5.59297','0','0','0','0','120','255','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230879','1116','6720','7128','1','1','5852.26','5043.41','53.3798','5.43446','0','0','0.411739','-0.911302','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7128','1','1','5867.49','5054.64','48.6147','0.729129','0','0','0.356543','0.934279','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7128','1','1','5889.85','5024.66','34.9337','6.19001','0','0','0.0465722','-0.998915','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230879','1116','6720','7128','1','1','5873.61','4994.04','26.2117','2.95417','0','0','0.995612','0.0935741','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7128','1','1','5893.28','4990.24','26.8873','6.11853','0','0','0.0822324','-0.996613','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7128','1','1','5907.62','4958.48','26.6633','5.96931','0','0','0.156296','-0.98771','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230879','1116','6720','7128','1','1','5907.75','4939.98','26.211','5.43131','0','0','0.413177','-0.910651','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7128','1','1','5847.49','4967.93','26.2113','3.76233','0','0','0.95222','-0.305412','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230879','1116','6720','7062','1','1','5829.51','5175.79','85.2755','4.76763','0','0','0.68731','-0.726364','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7062','1','1','5842.03','5192.14','90.5812','6.15621','0','0','0.0634458','-0.997985','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230880','1116','6720','7062','1','1','5848.93','5245.97','93.499','2.20958','0','0','0.89337','0.449322','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7062','1','1','5868.88','5234.23','93.4689','5.93237','0','0','0.174508','-0.984656','300','0','1','0','0',NULL);
insert into `gameobject` (`id`, `map`, `zoneId`, `areaId`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`, `isActive`, `custom_flags`, `protect_anti_doublet`) values('230881','1116','6720','7062','1','1','5819.16','5208.95','89.2052','3.32328','0','0','0.995876','-0.0907198','300','0','1','0','0',NULL);


-- What we got only doable after Garrison Tier 2
UPDATE quest_template SET PrevquestId = 36567 WHERE id = 34824;

-- Build your baracks doable after completing What we Need
UPDATE quest_template SET PrevquestId = 34822 WHERE id = 34461;

-- The Ogron Lives doable after Garrison is available
UPDATE quest_template SET PrevQuestId = 34378, NextQuestId = 0 WHERE id = 34823;

-- Mission Probable doable after Winds of Change
UPDATE quest_template SET PrevQuestId = 34462 WHERE id = 34775;

-- Winds of Change doable after We need an army
UPDATE quest_template SET NextQuestId = 34775 WHERE id = 34462;
