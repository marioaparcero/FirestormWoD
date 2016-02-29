-- LEVEL 1

	INSERT INTO gameobject_template (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `data24`, `data25`, `data26`, `data27`, `data28`, `data29`, `data30`, `data31`, `data32`, `unkInt32`, `BuildVerified`) VALUES(237027, 45, 15585, "Trading Post Work Order", "", "", "", 1, 0, 0, 0, 0, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19702) ON DUPLICATE KEY UPDATE `entry` = VALUES(`entry`), `type` = VALUES(`type`), `displayId` = VALUES(`displayId`), `name` = VALUES(`name`), `IconName` = VALUES(`IconName`), `castBarCaption` = VALUES(`castBarCaption`), `unk1` = VALUES(`unk1`), `size` = VALUES(`size`), `questItem1` = VALUES(`questItem1`), `questItem2` = VALUES(`questItem2`), `questItem3` = VALUES(`questItem3`), `questItem4` = VALUES(`questItem4`), `questItem5` = VALUES(`questItem5`), `questItem6` = VALUES(`questItem6`), `data0` = VALUES(`data0`), `data1` = VALUES(`data1`), `data2` = VALUES(`data2`), `data3` = VALUES(`data3`), `data4` = VALUES(`data4`), `data5` = VALUES(`data5`), `data6` = VALUES(`data6`), `data7` = VALUES(`data7`), `data8` = VALUES(`data8`), `data9` = VALUES(`data9`), `data10` = VALUES(`data10`), `data11` = VALUES(`data11`), `data12` = VALUES(`data12`), `data13` = VALUES(`data13`), `data14` = VALUES(`data14`), `data15` = VALUES(`data15`), `data16` = VALUES(`data16`), `data17` = VALUES(`data17`), `data18` = VALUES(`data18`), `data19` = VALUES(`data19`), `data20` = VALUES(`data20`), `data21` = VALUES(`data21`), `data22` = VALUES(`data22`), `data23` = VALUES(`data23`), `data24` = VALUES(`data24`), `data25` = VALUES(`data25`), `data26` = VALUES(`data26`), `data27` = VALUES(`data27`), `data28` = VALUES(`data28`), `data29` = VALUES(`data29`), `data30` = VALUES(`data30`), `data31` = VALUES(`data31`), `data32` = VALUES(`data32`), `unkInt32` = VALUES(`unkInt32`), `BuildVerified` = VALUES(`BuildVerified`);

	UPDATE creature_template SET ScriptName = "npc_TraderJoseph_Garr", npcFlag=3, npcFlag2=32, minlevel=100, maxlevel=100, `exp`=5 WHERE entry=87212;
	REPLACE INTO creature_queststarter VALUE (87212, 37088);
	REPLACE INTO creature_questender VALUE (87212, 37088);

	REPLACE INTO spell_script_names VALUE (174569, "spell_GarrisonRouseTrader");

	-- Krixel Pinchwhistle <Trader>
	UPDATE `creature_template` SET `npcflag`=3, minlevel=100, maxlevel=100, `exp`=5, faction = 35 WHERE `entry`=87200;
	DELETE FROM `npc_vendor` WHERE `entry` IN (87200, 87212) AND `type` = 1;

	DELETE FROM garrison_plot_content WHERE plot_type_or_building=-111 AND faction_index=1;
	insert into `garrison_plot_content` (`plot_type_or_building`, `faction_index`, `creature_or_gob`, `x`, `y`, `z`, `o`) values('-111','1','87212','6.83673','3.43797','1.09345','5.66093');
	insert into `garrison_plot_content` (`plot_type_or_building`, `faction_index`, `creature_or_gob`, `x`, `y`, `z`, `o`) values('-111','1','-237027','18.2104','-7.43243','0.000198','5.54704');

-- LEVEL 2

-- LEVEL 3