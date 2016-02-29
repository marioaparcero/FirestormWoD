////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2014-2015 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#include "GarrisonGO.hpp"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ScriptedGossip.h"
#include "GameObjectAI.h"
#include "Spell.h"
#include "GarrisonMgr.hpp"
#include "Sites/GarrisonSiteBase.hpp"

namespace MS { namespace Garrison 
{
    /// Constructor
    go_garrison_cache::go_garrison_cache()
        : GameObjectScript("go_garrison_cache")
    { 

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a player opens a gossip dialog with the GameObject.
    /// @p_Player     : Source player instance
    /// @p_GameObject : Target GameObject instance
    bool go_garrison_cache::OnGossipHello(Player * p_Player, GameObject * p_GameObject)
    {
        if (p_Player->GetGarrison())
        {
            p_Player->GetGarrison()->RewardGarrisonCache();

            /// Alliance
            if (p_Player->HasQuest(Quests::QUEST_KEEPING_IT_TOGETHER) && p_GameObject)
                p_Player->QuestObjectiveSatisfy(41327, 1, QUEST_OBJECTIVE_TYPE_CRITERIA_TREE, p_GameObject->GetGUID());

            /// Horde
            if (p_Player->HasQuest(Quests::QUEST_WHAT_WE_GOT) && p_GameObject)
            {
                p_Player->QuestObjectiveSatisfy(41325, 2, QUEST_OBJECTIVE_TYPE_CRITERIA_TREE, p_GameObject->GetGUID());

                Creature * l_Creature = p_Player->FindNearestCreature(NPCs::NPC_LADY_SENA, 15);

                if (l_Creature)
                    l_Creature->AI()->Talk(0);
            }
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_outhouse::go_garrison_outhouse()
        : GameObjectScript("go_garrison_outhouse")
    { 

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a player opens a gossip dialog with the GameObject.
    /// @p_Player     : Source player instance
    /// @p_GameObject : Target GameObject instance
    bool go_garrison_outhouse::OnGossipHello(Player * p_Player, GameObject * p_GameObject)
    {
        p_Player->CastSpell(p_Player, MS::Garrison::Spells::SPELL_RELIEVED);

        if (!p_Player->GetAchievementMgr().HasAchieved(Achievements::ACHIEVEMENT_STAYING_REGULAR))
            p_Player->GetAchievementMgr().CompletedAchievement(sAchievementStore.LookupEntry(Achievements::ACHIEVEMENT_STAYING_REGULAR), nullptr);

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_shipment_container::go_garrison_shipment_container()
        : GameObjectScript("go_garrison_shipment_container")
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a GameObjectAI object is needed for the GameObject.
    /// @p_GameObject : GameObject instance
    GameObjectAI* go_garrison_shipment_container::GetAI(GameObject * p_GameObject) const
    {
        return new go_garrison_shipment_containerAI(p_GameObject);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_shipment_container::go_garrison_shipment_containerAI::go_garrison_shipment_containerAI(GameObject * p_GameObject)
        : GameObjectAI(p_GameObject)
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a player opens a gossip dialog with the GameObject.
    /// @p_Player     : Source player instance
    bool go_garrison_shipment_container::go_garrison_shipment_containerAI::GossipHello(Player * p_Player)
    {
        if (!p_Player || !p_Player->GetGarrison())
            return false;

        Garrison::Manager* l_Garrison               = p_Player->GetGarrison();
        std::vector<GarrisonWorkOrder> l_WorkOrders = l_Garrison->GetWorkOrders();

        uint32 l_ThisGobPlotInstanceID = l_Garrison->GetGameObjectPlotInstanceID(go->GetGUID());

        if (!l_ThisGobPlotInstanceID)
            return false;

        std::map<uint32, bool> l_ToastStatus;
        for (uint32 l_I = 0; l_I < l_WorkOrders.size(); ++l_I)
        {
            if (l_WorkOrders[l_I].PlotInstanceID != l_ThisGobPlotInstanceID)
                continue;

            if (l_WorkOrders[l_I].CompleteTime > time(nullptr))
                continue;

            CharShipmentEntry const* l_ShipmentEntry = sCharShipmentStore.LookupEntry(l_WorkOrders[l_I].ShipmentID);

            if (!l_ShipmentEntry)
                continue;

            ShipmentCurrency l_ShipmentCurrency;
            uint32 l_RewardID = l_ShipmentEntry->ResultItemID;
            std::map<uint32, uint32> l_RewardItems;

            l_RewardItems.insert(std::make_pair(l_RewardID, 0));

            using namespace ShipmentContainer;

            switch (l_ShipmentEntry->ShipmentContainerID)
            {
                /// Standard Handling
                case ShipmentTest:
                case ShipmentUnk1:
                case ShipmentFishingHut:
                case ShipmentAlchemyLab:
                case ShipmentTailoring:
                case ShipmentBlacksmitthing:
                case ShipmentLeathorworking:
                case ShipmentJewelCrafting:
                case ShipmentEnchanting:
                case ShipmentEngineering:
                case ShipmentInscription:
                case ShipmentAlchemyUnk:
                case ShipmentBlacksmitthingUnk:
                case ShipmentEnchantingUnk:
                case ShipmentEngineeringUnk:
                case ShipmentInscriptionUnk:
                case ShipmentJewelCraftingUnk:
                case ShipmentLeathorworkingUnk:
                case ShipmentTailoringUnk:
                case ShipmentConquerorsTribute:
                case ShipmentOverchargedDemolisher:
                case ShipmentOverchargedSiegeEngine:
                case ShipmentArmory:
                case ShipmentArmoryUnk:
                case ShipmentShipDestroyer:
                case ShipmentShipSubmarine:
                case ShipmentShipBattleship:
                case ShipmentShipTransport:
                case ShipmentShipCarrier:
                case ShipmentShipTransportUnk:
                case ShipmentShipSubmarineUnk:
                case ShipmentShipBattleshipUnk:
                case ShipmentShipDestroyerUnk:
                case ShipmentShipDestroyerUnk2:
                case ShipmentShipDestroyerUnk3:
                case ShipmentShipDestroyerUnk4:
                case ShipmentShipDestroyerUnk5:
                    break;
                    /// Custom Handling
                case ShipmentLumberMill:
                case ShipmentTradingPost:
                case ShipmentTradingPostUnk:
                case ShipmentMineUnk:
                case ShipmentMine:
                    l_ShipmentCurrency.CurrencyID = CurrencyTypes::CURRENCY_TYPE_APEXIS_CRYSTAL;
                    l_ShipmentCurrency.CurrencyAmount = urand(0, 5);
                    break;
                case ShipmentBarn:
                {
                    l_RewardItems.clear();
                    l_ShipmentCurrency.CurrencyID = CurrencyTypes::CURRENCY_TYPE_APEXIS_CRYSTAL;
                    l_ShipmentCurrency.CurrencyAmount = urand(0, 5);

                    switch (l_ShipmentEntry->ID)
                    {
                        case Barn::ShipmentIDS::ShipmentPowerfulFurredBeast:
                            l_RewardItems.insert(std::make_pair(118472, urand(0, 3)));
                            l_RewardItems.insert(std::make_pair(111557, urand(12, 15)));
                            break;
                        case Barn::ShipmentIDS::ShipmentFurredBeast:
                            l_RewardItems.insert(std::make_pair(111557, urand(5, 8)));
                            break;
                        case Barn::ShipmentIDS::ShipmentPowerfulLeatheredBeast:
                            l_RewardItems.insert(std::make_pair(118472, urand(0, 3)));
                            l_RewardItems.insert(std::make_pair(110609, urand(12, 15)));
                            break;
                        case Barn::ShipmentIDS::ShipmentLeatheredBeast:
                            l_RewardItems.insert(std::make_pair(110609, urand(5, 8)));
                            break;
                        case Barn::ShipmentIDS::ShipmentPowerfulMeatyBeast:
                            l_RewardItems.insert(std::make_pair(118472, urand(0, 3)));
                            l_RewardItems.insert(std::make_pair(118576, urand(12, 15)));
                            break;
                        case Barn::ShipmentIDS::ShipmentMeatyBeast:
                            l_RewardItems.insert(std::make_pair(118576, urand(5, 8)));
                            break;
                        default:
                            break;
                    }
                    break;
                }
                case ShipmentHerbGarden:
                case ShipmentHerbGardenUnk:
                    l_ShipmentCurrency.CurrencyID = CurrencyTypes::CURRENCY_TYPE_APEXIS_CRYSTAL;
                    l_ShipmentCurrency.CurrencyAmount = urand(0, 5);
                    l_RewardItems.insert(std::make_pair(g_HerbEntries[urand(0, 5)], 8));
                    break;
                case ShipmentMageTower:
                case ShipmentMageTowerUnk:
                    l_ShipmentCurrency.CurrencyID = CurrencyTypes::CURRENCY_TYPE_APEXIS_CRYSTAL;
                    l_ShipmentCurrency.CurrencyAmount = urand(100, 300);
                    l_RewardItems.insert(std::make_pair(122514, 1));
                    break;
                default:
                break;
            }

            /// Adding items
            bool l_CanGetItems = true;
            uint32 l_NoSpaceForCount = 0;
            uint8 l_Itr = 0;

            for (auto l_RewardItem : l_RewardItems)
            {
                if (!l_RewardItem.second)
                    continue;

                uint32 l_ItemCount = l_RewardItem.second * l_Garrison->CalculateAssignedFollowerShipmentBonus(l_ThisGobPlotInstanceID);

                /// check space and find places
                ItemPosCountVec l_Destination;
                InventoryResult l_Message = p_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, l_RewardItem.first, l_ItemCount, &l_NoSpaceForCount);

                if (l_Message != EQUIP_ERR_OK)
                {
                    p_Player->SendEquipError(l_Message, nullptr, nullptr, l_RewardItem.first);
                    l_CanGetItems = false;
                    break;
                }
            }

            if (l_CanGetItems)
            {
                for (auto l_RewardItem : l_RewardItems)
                {
                    if (!l_RewardItem.second)
                        continue;

                    uint32 l_ItemCount = l_RewardItem.second * l_Garrison->CalculateAssignedFollowerShipmentBonus(l_ThisGobPlotInstanceID);

                    /// check space and find places
                    ItemPosCountVec l_Destination;
                    InventoryResult l_Message = p_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, l_RewardItem.first, l_ItemCount, &l_NoSpaceForCount);

                    if (l_Message == EQUIP_ERR_OK)
                    {
                        if (Item* l_Item = p_Player->StoreNewItem(l_Destination, l_RewardItem.first, true, Item::GenerateItemRandomPropertyId(l_RewardItem.first)))
                            sScriptMgr->OnPlayerItemLooted(p_Player, l_Item);

                        if (l_ToastStatus[l_RewardItem.first] == false)
                        {
                            p_Player->SendDisplayToast(l_RewardItem.first, l_ItemCount, DISPLAY_TOAST_METHOD_LOOT, TOAST_TYPE_NEW_ITEM, false, false);
                            l_ToastStatus[l_RewardItem.first] = true;
                        }
                    }
                    else
                        p_Player->SendEquipError(l_Message, nullptr, nullptr, l_RewardItem.first);
                }

                if (l_ShipmentCurrency.CurrencyID && l_ShipmentCurrency.CurrencyAmount)
                    p_Player->ModifyCurrency(l_ShipmentCurrency.CurrencyID, l_ShipmentCurrency.CurrencyAmount);

                l_Garrison->DeleteWorkOrder(l_WorkOrders[l_I].DatabaseID);
            }
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_herb::go_garrison_herb()
        : GameObjectScript("go_garrison_herb")
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a player opens a gossip dialog with the GameObject.
    /// @p_Player     : Source player instance
    /// @p_GameObject : Target GameObject instance
    bool go_garrison_herb::OnGossipHello(Player* p_Player, GameObject* p_GameObject)
    {
        Sites::GarrisonSiteBase* l_GarrisonSite = (Sites::GarrisonSiteBase*)p_GameObject->GetInstanceScript();

        if (!l_GarrisonSite)
            return true;

        Player* l_Owner = l_GarrisonSite->GetOwner();

        if (l_Owner != p_Player)
            return true;

        float l_BaseX = p_GameObject->GetPositionX();
        float l_BaseY = p_GameObject->GetPositionY();

        uint64 l_PackedValue = 0;
        l_PackedValue |= ((uint64)*(uint32*)(&l_BaseX)) << 0;
        l_PackedValue |= ((uint64)*(uint32*)(&l_BaseY)) << 32;

        std::vector<uint64> l_Creatures = l_Owner->GetGarrison()->GetBuildingCreaturesByBuildingType(BuildingType::Farm);

        for (uint64 l_CreatureGUID : l_Creatures)
        {
            Creature* l_Creature = HashMapHolder<Creature>::Find(l_CreatureGUID);

            if (l_Creature && l_Creature->AI())
                l_Creature->AI()->SetGUID(l_PackedValue, CreatureAIDataIDs::GatheredPos);
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_deposit::go_garrison_deposit()
        : GameObjectScript("go_garrison_deposit")
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a player opens a gossip dialog with the GameObject.
    /// @p_Player     : Source player instance
    /// @p_GameObject : Target GameObject instance
    bool go_garrison_deposit::OnGossipHello(Player* p_Player, GameObject* p_GameObject)
    {
        Sites::GarrisonSiteBase* l_GarrisonSite = (Sites::GarrisonSiteBase*)p_GameObject->GetInstanceScript();

        if (!l_GarrisonSite)
            return true;

        Player* l_Owner = l_GarrisonSite->GetOwner();

        if (l_Owner != p_Player)
            return true;

        float l_BaseX = p_GameObject->GetPositionX();
        float l_BaseY = p_GameObject->GetPositionY();

        uint64 l_PackedValue = 0;
        l_PackedValue |= ((uint64)*(uint32*)(&l_BaseX)) << 0;
        l_PackedValue |= ((uint64)*(uint32*)(&l_BaseY)) << 32;

        std::vector<uint64> l_Creatures = l_Owner->GetGarrison()->GetBuildingCreaturesByBuildingType(BuildingType::Mine);

        for (uint64 l_CreatureGUID : l_Creatures)
        {
            Creature* l_Creature = HashMapHolder<Creature>::Find(l_CreatureGUID);

            if (l_Creature && l_Creature->AI())
                l_Creature->AI()->SetGUID(l_PackedValue, CreatureAIDataIDs::GatheredPos);
        }

        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    /// 234186 - Iron Trap                                                 ///
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    gob_IronTrap_Garrison::gob_IronTrap_Garrison()
        : GameObjectScript("npc_IronTrap_Garr")
    {
    }

    /// Constructor
    /// @p_Gob : AI Owner
    gob_IronTrap_Garrison::gob_IronTrap_GarrisonAI::gob_IronTrap_GarrisonAI(GameObject* p_Gob)
        : GameObjectAI(p_Gob)
    {
        m_UpdateTimer = 0;
    }

    void gob_IronTrap_Garrison::gob_IronTrap_GarrisonAI::Reset()
    {
        Sites::GarrisonSiteBase* l_GarrisonSite = (Sites::GarrisonSiteBase*)go->GetInstanceScript();

        if (l_GarrisonSite == nullptr)
            return;

        Player* l_Owner = l_GarrisonSite->GetOwner();

        if (l_Owner == nullptr)
            return;

        switch (l_Owner->GetTeamId())
        {
            case TEAM_ALLIANCE:
                if (l_Owner->IsQuestRewarded(MS::Garrison::Quests::Alliance_BreakingIntoTheTrapGame))
                    go->SetDisplayId(MS::Garrison::DisplayIDs::GobIronTrapDisplay);
                else
                {
                    go->SetDisplayId(MS::Garrison::DisplayIDs::InvisibleDisplay);
                    m_UpdateTimer = 1500;
                }
                break;
            case TEAM_HORDE:
                if (l_Owner->IsQuestRewarded(MS::Garrison::Quests::Horde_BreakingIntoTheTrapGame))
                    go->SetDisplayId(MS::Garrison::DisplayIDs::GobIronTrapDisplay);
                else
                {
                    go->SetDisplayId(MS::Garrison::DisplayIDs::InvisibleDisplay);
                    m_UpdateTimer = 1500;
                }
                break;
            default:
                break;
        }
    }

    void gob_IronTrap_Garrison::gob_IronTrap_GarrisonAI::UpdateAI(uint32 const p_Diff)
    {
        if (m_UpdateTimer)
        {
            if (m_UpdateTimer <= p_Diff)
            {
                Sites::GarrisonSiteBase* l_GarrisonSite = (Sites::GarrisonSiteBase*)go->GetInstanceScript();

                if (l_GarrisonSite == nullptr)
                    return;

                Player* l_Owner = l_GarrisonSite->GetOwner();

                if (l_Owner == nullptr)
                    return;

                switch (l_Owner->GetTeamId())
                {
                    case TEAM_ALLIANCE:
                        if (l_Owner->IsQuestRewarded(MS::Garrison::Quests::Alliance_BreakingIntoTheTrapGame))
                        {
                            go->SetDisplayId(MS::Garrison::DisplayIDs::GobIronTrapDisplay);
                            m_UpdateTimer = 0;
                        }
                        else
                            m_UpdateTimer = 1500;
                        break;
                    case TEAM_HORDE:
                        if (l_Owner->IsQuestRewarded(MS::Garrison::Quests::Horde_BreakingIntoTheTrapGame))
                        {
                            go->SetDisplayId(MS::Garrison::DisplayIDs::GobIronTrapDisplay);
                            m_UpdateTimer = 0;
                        }
                        else
                            m_UpdateTimer = 1500;
                        break;
                    default:
                        break;
                }

                m_UpdateTimer = 1500;
            }
            else m_UpdateTimer -= p_Diff;
        }
    }

    GameObjectAI* gob_IronTrap_Garrison::GetAI(GameObject* p_Gob) const
    {
        return new gob_IronTrap_GarrisonAI(p_Gob);
    }

    //////////////////////////////////////////////////////////////////////////
    /// 233604 - Small Timber                                              ///
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_timber::go_garrison_timber()
        : GameObjectScript("go_garrison_timber")
    {
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a GameObjectAI object is needed for the GameObject.
    /// @p_GameObject : GameObject instance
    GameObjectAI* go_garrison_timber::GetAI(GameObject* p_GameObject) const
    {
        return new go_garrison_timberAI(p_GameObject);
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_timber::go_garrison_timberAI::go_garrison_timberAI(GameObject* p_GameObject)
        : GameObjectAI(p_GameObject)
    {
        /// Small Timbers
        m_TimberDisplayIDs.insert(std::make_pair(234193, 19771)); ///< Nagrand
        m_TimberDisplayIDs.insert(std::make_pair(234197, 19776)); ///< Nagrand
        m_TimberDisplayIDs.insert(std::make_pair(233604, 14683)); ///< Gorgrond
        m_TimberDisplayIDs.insert(std::make_pair(234080, 18958)); ///< Gorgrond
        m_TimberDisplayIDs.insert(std::make_pair(234122, 19585)); ///< Spires of Arak
        m_TimberDisplayIDs.insert(std::make_pair(234126, 19592)); ///< Spires of Arak
        m_TimberDisplayIDs.insert(std::make_pair(234109, 19473)); ///< Shadowmoon Valley
        m_TimberDisplayIDs.insert(std::make_pair(234110, 19580)); ///< Shadowmoon Valley
        m_TimberDisplayIDs.insert(std::make_pair(233922, 19864)); ///< Frostfire Ridge
        m_TimberDisplayIDs.insert(std::make_pair(234097, 19575)); ///< Talador

        /// Medium Timbers

        m_TimberDisplayIDs.insert(std::make_pair(234000, 19188)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234022, 19474)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234098, 19576)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234119, 19474)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234123, 19586)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234127, 19593)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234194, 19772)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234196, 19812)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234198, 19777)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(233634, 14682)); ///< Unk yet

        /// Large Timbers

        m_TimberDisplayIDs.insert(std::make_pair(233635, 19485)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234007, 19865)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234099, 19577)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234120, 19475)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234124, 19587)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234128, 19594)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234195, 19773)); ///< Unk yet
        m_TimberDisplayIDs.insert(std::make_pair(234199, 19778)); ///< Unk yet
    }

    bool go_garrison_timber::OnGossipHello(Player* p_Player, GameObject* p_GameObject)
    {
        if (!p_GameObject->AI())
            return false;

        using namespace LumberMillData;

        if (std::find(g_SmallTimber.begin(), g_SmallTimber.end(), p_GameObject->GetEntry()) != g_SmallTimber.end())
            p_GameObject->AI()->SetData64(eDatas::ChopCount, urand(4, 6));
        else if (std::find(g_MediumTimber.begin(), g_MediumTimber.end(), p_GameObject->GetEntry()) != g_MediumTimber.end())
            p_GameObject->AI()->SetData64(eDatas::ChopCount, urand(8, 10));
        else if (std::find(g_LargeTimber.begin(), g_LargeTimber.end(), p_GameObject->GetEntry()) != g_LargeTimber.end())
            p_GameObject->AI()->SetData64(eDatas::ChopCount, urand(16, 25));

        p_GameObject->AI()->SetData64(eDatas::AnimTimer, 5000);
        p_GameObject->AI()->SetData64(eDatas::PlayerGuid, p_Player->GetGUID());
        p_GameObject->AI()->DoAction(eDatas::ActionGossip);
        p_GameObject->SetFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);

        return true;
    }

    void go_garrison_timber::go_garrison_timberAI::DoAction(const int32 p_Action)
    {
        if (p_Action == eDatas::ActionGossip)
        {
            if (Player* l_Player = sObjectAccessor->FindPlayer(m_PlayerGuid))
            {
                if (l_Player->GetTeamId() == TEAM_HORDE)
                    go->SummonCreature(NPCs::NpcHordePeon, *l_Player);
                else if (l_Player->GetTeamId() == TEAM_ALLIANCE)
                    go->SummonCreature(NPCs::NpcAllianceLumberjack, *l_Player);

                l_Player->AddItem(Items::ItemTimber, m_ChopCount);
            }
        }
    }

    void go_garrison_timber::go_garrison_timberAI::SetData64(uint32 p_Id, uint64 p_Value)
    {
        switch (p_Id)
        {
            case eDatas::AnimTimer:
                m_AnimTimer = p_Value;
                break;
            case eDatas::ChopCount:
                m_ChopCount = p_Value;
                break;
            case eDatas::PlayerGuid:
                m_PlayerGuid = p_Value;
                break;
            default:
                break;
        }
    }

    void go_garrison_timber::go_garrison_timberAI::UpdateAI(uint32 p_Diff)
    {
        if (m_AnimTimer)
        {
            if (m_AnimTimer <= p_Diff)
            {
                if (Player* l_Player = sObjectAccessor->FindPlayer(m_PlayerGuid))
                {
                    if (!l_Player->GetSession())
                        return;

                    if (l_Player->HasQuest(Quests::Horde_EasingIntoLumberjacking) || l_Player->HasQuest(Quests::Alliance_EasingIntoLumberjacking))
                    {
                        /// Adding items
                        uint32 l_NoSpaceForCount = 0;

                        /// check space and find places
                        ItemPosCountVec l_Destination;
                        InventoryResult l_Message = l_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, Items::ItemTimberSample, m_ChopCount, &l_NoSpaceForCount);

                        if (l_Message == EQUIP_ERR_OK)
                            l_Player->StoreNewItem(l_Destination, Items::ItemTimberSample, true, Item::GenerateItemRandomPropertyId(Items::ItemTimberSample));
                        else
                            l_Player->SendEquipError(l_Message, nullptr, nullptr, Items::ItemTimberSample);
                    }
                    else
                    {
                        /// Adding items
                        uint32 l_NoSpaceForCount = 0;

                        /// check space and find places
                        ItemPosCountVec l_Destination;
                        InventoryResult l_Message = l_Player->CanStoreNewItem(NULL_BAG, NULL_SLOT, l_Destination, Items::ItemTimber, m_ChopCount, &l_NoSpaceForCount);

                        if (l_Message == EQUIP_ERR_OK)
                            l_Player->StoreNewItem(l_Destination, Items::ItemTimber, true, Item::GenerateItemRandomPropertyId(Items::ItemTimber));
                        else
                            l_Player->SendEquipError(l_Message, nullptr, nullptr, Items::ItemTimber);
                    }

                    l_Player->SendItemBonusDebug(m_ChopCount, l_Player->GetSession()->GetTrinityString(TrinityStrings::GarrisonChop), l_Player);

                    go->CastSpell(l_Player, Spells::SpellSummonStump);
                    go->SetDisplayId(9145); ///< Invisible Display

                    if (Creature* l_Creature = go->FindNearestCreature(l_Player->GetTeamId() == TEAM_HORDE ? NPCs::NpcHordePeon : NPCs::NpcAllianceLumberjack, 10.0f))
                    {
                        if (l_Creature->AI())
                            l_Creature->AI()->Talk(0);

                        l_Creature->DespawnOrUnsummon();
                    }
                }

                m_AnimTimer   = 0;
                m_RefillTimer = 450 * IN_MILLISECONDS; ///< You can get chop from the tree like every 7 minutes
            }
            else
                m_AnimTimer -= p_Diff;
        }
        else if (m_RefillTimer)
        {
            if (m_RefillTimer <= p_Diff)
            {
                if (GameObject* l_Gob = go->FindNearestGameObject(GameObjects::GobStump, 1.0f))
                    l_Gob->Delete();

                if (m_TimberDisplayIDs.find(go->GetEntry()) != m_TimberDisplayIDs.end())
                {
                    go->SetDisplayId(m_TimberDisplayIDs[go->GetEntry()]);
                    go->RemoveFlag(GAMEOBJECT_FIELD_FLAGS, GO_FLAG_NOT_SELECTABLE);
                }

                m_RefillTimer  = 0;
                m_RespawnTimer = 1500;
            }
            else
                m_RefillTimer -= p_Diff;
        }
        else if (m_RespawnTimer)
        {
            if (m_RespawnTimer <= p_Diff)
            {
                if (m_TimberDisplayIDs.find(go->GetEntry()) != m_TimberDisplayIDs.end())
                    go->SetDisplayId(m_TimberDisplayIDs[go->GetEntry()]);

                m_RespawnTimer = 0;
            }
            else
                m_RefillTimer -= p_Diff;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// 236911, 236774, 236906, 236910, 236912, 236765                     ///
    /// Deactivated Portals from Mage Tower                                ///
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_deactivated_mage_portal::go_garrison_deactivated_mage_portal()
        : GameObjectScript("go_garrison_deactivated_mage_portal")
    {
    }

    bool go_garrison_deactivated_mage_portal::OnGameObjectSpellCasterUse(const GameObject* p_GameObject, Player* p_User) const
    {
        uint8 l_BuildingLevel = 0;

        if (p_User->GetGarrison())
        {
            std::vector<GarrisonBuilding> l_Buildings = p_User->GetGarrison()->GetBuildings();

            for (GarrisonBuilding l_Building : l_Buildings)
            {
                switch (l_Building.BuildingID)
                {
                    case Buildings::MageTower_SpiritLodge_Level1:
                        l_BuildingLevel = 1;
                        break;
                    case Buildings::MageTower_SpiritLodge_Level2:
                        l_BuildingLevel = 2;
                        break;
                    case Buildings::MageTower_SpiritLodge_Level3:
                        l_BuildingLevel = 3;
                        break;
                    default:
                        break;
                }
            }
        }

        if (!l_BuildingLevel)
        {
            if (p_User->GetSession())
                ChatHandler(p_User).PSendSysMessage(TrinityStrings::GarrisonNoMageTower);

            return false;
        }

        std::vector<uint32> const l_QuestList =
        {
            GarrisonPortals::PortalsQuests::QuestFrostfireRidge,
            GarrisonPortals::PortalsQuests::QuestGorgrond,
            GarrisonPortals::PortalsQuests::QuestNagrand,
            GarrisonPortals::PortalsQuests::QuestShadowmoon,
            GarrisonPortals::PortalsQuests::QuestSpiresOfArak,
            GarrisonPortals::PortalsQuests::QuestTalador
        };

        uint8 l_Itr = 0;

        for (uint32 l_QuestID : l_QuestList)
        {
            if (!p_User->IsQuestRewarded(l_QuestID))
            {
                if (l_Itr >= l_BuildingLevel)
                {
                    if (p_User->GetSession())
                        ChatHandler(p_User).PSendSysMessage(TrinityStrings::GarrisonTooMuchPortals);

                    return false;
                }
            }
            else
                l_Itr++;
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    /// 237335, 237132                                                     ///
    //////////////////////////////////////////////////////////////////////////

    /// Constructor
    go_garrison_essence_font::go_garrison_essence_font()
        : GameObjectScript("go_garrison_essence_font")
    {

    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    /// Called when a player opens a gossip dialog with the GameObject.
    /// @p_Player     : Source player instance
    /// @p_GameObject : Target GameObject instance
    bool go_garrison_essence_font::OnGossipHello(Player* p_Player, GameObject* p_GameObject)
    {
        if (p_Player)
        {
            p_Player->CastSpell(p_Player->ToUnit(), 161736, true);
            p_Player->CastSpell(p_Player->ToUnit(), 161735, true);
        }

        return true;
    }

}   ///< namespace Garrison
}   ///< namespace MS

void AddSC_Garrison_GO()
{
    new MS::Garrison::go_garrison_deactivated_mage_portal;
    new MS::Garrison::go_garrison_cache;
    new MS::Garrison::go_garrison_outhouse;
    new MS::Garrison::go_garrison_shipment_container;
    new MS::Garrison::go_garrison_herb;
    new MS::Garrison::go_garrison_deposit;
    new MS::Garrison::gob_IronTrap_Garrison;
    new MS::Garrison::go_garrison_timber;
    new MS::Garrison::go_garrison_essence_font;
}