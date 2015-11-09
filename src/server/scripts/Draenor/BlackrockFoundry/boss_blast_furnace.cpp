////////////////////////////////////////////////////////////////////////////////
///
///  MILLENIUM-STUDIO
///  Copyright 2015 Millenium-studio SARL
///  All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

# include "blackrock_foundry.hpp"

Position const g_EncounterAddSpawns[2][3] =
{
    /// Left side
    {
        127.299f, 3496.977f, 246.085f, 0.7516f,
        127.599f, 3504.188f, 246.085f, 0.0054f,
        130.231f, 3515.183f, 245.085f, 0.0368f
    },
    /// Right side
    {
        265.278f, 3515.901f, 246.551f, 2.2870f,
        265.739f, 3520.793f, 246.551f, 3.0881f,
        265.634f, 3527.234f, 246.551f, 3.2569f
    }
};

Position const g_BellowsOperatorSpawns[2] =
{
    272.4757f, 3591.769f, 246.4359f, 3.071f,
    123.9306f, 3592.554f, 246.4359f, 0.106f
};

Position const g_PrimalElementalistsSpawns[eFoundryDatas::MaxPrimalElementalists] =
{
    195.6962f, 3423.204f, 267.0585f, 1.581610f,
    202.3524f, 3422.990f, 267.0098f, 1.581610f,
    242.0573f, 3459.246f, 266.6673f, 3.206762f,
    149.9497f, 3461.144f, 267.3971f, 0.000000f
};

Position const g_PrimalElementalistsMoves[eFoundryDatas::MaxPrimalElementalists] =
{
    181.700f, 3542.40f, 217.517f, 0.0f,
    181.884f, 3519.69f, 217.381f, 0.0f,
    211.403f, 3516.13f, 217.492f, 0.0f,
    217.771f, 3546.35f, 217.408f, 0.0f
};

/// Heart of the Mountain - 76806
class boss_heart_of_the_mountain : public CreatureScript
{
    public:
        boss_heart_of_the_mountain() : CreatureScript("boss_heart_of_the_mountain") { }

        enum eTalks
        {
            Phase3Freedom,
            HeatLevelRising,
            WarnBlast,
            TalkMelt,
            Slay,
            Wipe,
            Death,
            BlastWarning,
            BombWarning,
            VolatileFireWarning,
            MeltWarning
        };

        enum eSpells
        {
            /// Misc
            HeartOfTheFurnace   = 155288,
            BlastFurnaceBonus   = 177531,
            BustLoose           = 160823,
            BustLooseSecond     = 160833,
            BlastWaveCosmetic   = 177388,
            Containment         = 155265,
            ContainedDNT        = 177434,
            /// Encounter
            Blast               = 155209,
            SlagPoolAreatrigger = 155738,
            Tempered            = 155240,
            Heat                = 155242,
            Melt                = 155225
        };

        enum eEvents
        {
            EventBerserker = 1,
            EventWarnBlast,
            EventBlast,
            EventSlagElemental,
            EventFirecaller,
            EventSecurityGuard,
            EventHeat,
            EventMelt
        };

        enum eActions
        {
            ActionBlastIncreased        = 2,
            ActionFreeFury              = 2,
            ActionElementalistKilled    = 3,
            ActionSwitchToPhase2        = 3,
            ActionFuryDead              = 4
        };

        enum eVisuals
        {
            OneShotBattleRoar   = 53,
            EmoteContained      = 64
        };

        enum eCreatures
        {
            PrimalElementalist  = 76815,
            SecurityGuardFight  = 76812,
            Firecaller          = 76821,
            SlagElemental       = 78463
        };

        enum eDatas
        {
            ElementalistMoveIndex,
            ElementalistKilled
        };

        struct boss_heart_of_the_mountainAI : public BossAI
        {
            boss_heart_of_the_mountainAI(Creature* p_Creature) : BossAI(p_Creature, eFoundryDatas::DataBlastFurnace)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            EventMap m_Events;

            bool m_Enabled;

            uint8 m_ElementalistMoveIndex;
            uint8 m_ElementalistKilled;

            void Reset() override
            {
                m_Events.Reset();

                _Reset();

                me->RemoveAllAreasTrigger();

                me->RemoveAura(eSpells::HeartOfTheFurnace);
                me->RemoveAura(eFoundrySpells::Berserker);
                me->RemoveAura(eSpells::ContainedDNT);

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);

                me->SetPower(Powers::POWER_ALTERNATE_POWER, 0);

                me->SetReactState(ReactStates::REACT_PASSIVE);
                me->AddUnitState(UnitState::UNIT_STATE_STUNNED);

                m_Enabled = false;

                m_ElementalistMoveIndex = 0;
                m_ElementalistKilled = 0;

                if (m_Instance != nullptr)
                {
                    if (GameObject* l_Gate = GameObject::GetGameObject(*me, m_Instance->GetData64(eFoundryGameObjects::FurnaceGate)))
                    {
                        l_Gate->SetGoState(GOState::GO_STATE_READY);
                        l_Gate->RemoveFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_IN_USE);
                    }

                    if (Creature* l_Foreman = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::ForemanFeldspar)))
                    {
                        if (l_Foreman->IsAIEnabled && !l_Foreman->isAlive())
                        {
                            l_Foreman->Respawn();
                            l_Foreman->AI()->EnterEvadeMode();
                        }
                    }
                }
            }

            void KilledUnit(Unit* p_Who) override
            {
                if (p_Who->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(eTalks::Slay);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                _EnterCombat();

                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 1);

                me->CastSpell(me, eSpells::HeartOfTheFurnace, true);

                m_Events.ScheduleEvent(eEvents::EventBerserker, 780 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventWarnBlast, GetBlastTimer() - 3 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventBlast, GetBlastTimer());
            }

            void JustReachedHome() override
            {
                if (m_Enabled)
                    Talk(eTalks::Wipe);

                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void EnterEvadeMode() override
            {
                if (me->HasReactState(ReactStates::REACT_AGGRESSIVE))
                    Talk(eTalks::Wipe);

                if (m_Instance != nullptr)
                {
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Heat);
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Tempered);
                }

                me->ClearUnitState(UnitState::UNIT_STATE_STUNNED);

                CreatureAI::EnterEvadeMode();

                me->StopMoving();
                me->ClearUnitState(UnitState::UNIT_STATE_EVADE);

                me->NearTeleportTo(me->GetHomePosition());

                JustReachedHome();
            }

            void JustDied(Unit* p_Killer) override
            {
                me->RemoveAllAreasTrigger();

                summons.DespawnAll();

                Talk(eTalks::Death);

                _JustDied();

                if (m_Instance != nullptr)
                {
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Heat);
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Tempered);

                    CastSpellToPlayers(me->GetMap(), me, eSpells::BlastFurnaceBonus, true);

                    if (Creature* l_Blackhand = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BlackhandCosmetic)))
                    {
                        if (l_Blackhand->IsAIEnabled)
                            l_Blackhand->AI()->DoAction(eActions::ActionFuryDead);
                    }
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::BustLoose:
                    {
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Position const l_TeleportPos = { 197.707f, 3529.08f, 217.234f, me->GetOrientation() };

                            me->CastSpell(l_TeleportPos, eSpells::BustLooseSecond, true);
                            me->NearTeleportTo(l_TeleportPos);

                            if (m_Instance != nullptr)
                            {
                                if (GameObject* l_Gate = GameObject::GetGameObject(*me, m_Instance->GetData64(eFoundryGameObjects::FurnaceGate)))
                                {
                                    l_Gate->SetGoState(GOState::GO_STATE_ACTIVE);
                                    l_Gate->SetFlag(EGameObjectFields::GAMEOBJECT_FIELD_FLAGS, GameObjectFlags::GO_FLAG_IN_USE);
                                }

                                if (Creature* l_Foreman = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::ForemanFeldspar)))
                                {
                                    if (l_Foreman->IsAIEnabled)
                                        l_Foreman->AI()->DoAction(eActions::ActionSwitchToPhase2);
                                }
                            }
                        });

                        AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->RemoveAura(eSpells::BustLoose);
                            me->SetAnimTier(0);
                            me->SetDisableGravity(false);
                            me->SetPlayerHoverAnim(false);
                            me->HandleEmoteCommand(eVisuals::OneShotBattleRoar);

                            for (uint8 l_I = 0; l_I < eFoundryDatas::MaxPrimalElementalists; ++l_I)
                                me->SummonCreature(eCreatures::PrimalElementalist, g_PrimalElementalistsSpawns[l_I]);

                            me->CastSpell(me, eSpells::SlagPoolAreatrigger, true);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHit(Unit* p_Attacker, SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::Containment && !me->HasAura(eSpells::ContainedDNT))
                {
                    /// Primal Elementalist
                    if (p_Attacker != nullptr)
                        p_Attacker->CastSpell(me, eSpells::ContainedDNT, true);

                    me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, eVisuals::EmoteContained);
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::Melt)
                    Talk(eTalks::MeltWarning, p_Target->GetGUID(), TextRange::TEXT_RANGE_NORMAL);
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionFreeFury:
                    {
                        me->CastSpell(me, eSpells::BustLoose, false);

                        if (!IsLFR())
                            m_Events.ScheduleEvent(eEvents::EventSlagElemental, 13 * TimeConstants::IN_MILLISECONDS);

                        m_Events.ScheduleEvent(eEvents::EventFirecaller, 76 * TimeConstants::IN_MILLISECONDS);
                        m_Events.ScheduleEvent(eEvents::EventSecurityGuard, 71 * TimeConstants::IN_MILLISECONDS + 500);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SetPower(Powers p_Power, int32 p_Value) override
            {
                if (p_Power != Powers::POWER_ALTERNATE_POWER || m_Instance == nullptr)
                    return;

                int32 l_OldPower = me->GetPower(p_Power);

                if ((l_OldPower < 75 && p_Value >= 75) ||
                    (l_OldPower < 50 && p_Value >= 50) ||
                    (l_OldPower < 25 && p_Value >= 25))
                {
                    if (Creature* l_Foreman = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::ForemanFeldspar)))
                    {
                        if (l_Foreman->IsAIEnabled)
                            l_Foreman->AI()->DoAction(eActions::ActionBlastIncreased);
                    }
                }
            }

            uint32 GetData(uint32 p_ID) override
            {
                switch (p_ID)
                {
                    case eDatas::ElementalistMoveIndex:
                        return (uint32)m_ElementalistMoveIndex;
                    case eDatas::ElementalistKilled:
                        return (uint32)m_ElementalistKilled;
                    default:
                        break;
                }

                return 0;
            }

            void SetData(uint32 p_ID, uint32 p_Value) override
            {
                switch (p_ID)
                {
                    case eDatas::ElementalistMoveIndex:
                    {
                        ++m_ElementalistMoveIndex;
                        break;
                    }
                    case eDatas::ElementalistKilled:
                    {
                        ++m_ElementalistKilled;

                        if (m_ElementalistKilled == eFoundryDatas::MaxPrimalElementalists)
                        {
                            me->RemoveAura(eSpells::ContainedDNT);
                            me->SetUInt32Value(EUnitFields::UNIT_FIELD_EMOTE_STATE, 0);

                            m_Events.CancelEvent(eEvents::EventFirecaller);
                            m_Events.CancelEvent(eEvents::EventSecurityGuard);
                            m_Events.CancelEvent(eEvents::EventSlagElemental);

                            AddTimedDelayedOperation(4 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                                me->ClearUnitState(UnitState::UNIT_STATE_STUNNED);

                                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS2, eUnitFlags2::UNIT_FLAG2_SELECTION_DISABLED);

                                Talk(eTalks::Phase3Freedom);

                                m_Events.ScheduleEvent(eEvents::EventHeat, 6 * TimeConstants::IN_MILLISECONDS);
                                m_Events.ScheduleEvent(eEvents::EventMelt, 11 * TimeConstants::IN_MILLISECONDS);
                            });
                        }

                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Blackhand = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BlackhandCosmetic)))
                            {
                                if (l_Blackhand->IsAIEnabled)
                                    l_Blackhand->AI()->DoAction(eActions::ActionElementalistKilled);
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventBerserker:
                    {
                        me->CastSpell(me, eFoundrySpells::Berserker, true);
                        break;
                    }
                    case eEvents::EventWarnBlast:
                    {
                        Talk(eTalks::BlastWarning);
                        m_Events.ScheduleEvent(eEvents::EventWarnBlast, GetBlastTimer());
                        break;
                    }
                    case eEvents::EventBlast:
                    {
                        if (me->HasReactState(ReactStates::REACT_AGGRESSIVE) && urand(0, 1))
                            Talk(eTalks::WarnBlast);

                        me->CastSpell(me, eSpells::Blast, true);
                        m_Events.ScheduleEvent(eEvents::EventBlast, GetBlastTimer());
                        break;
                    }
                    case eEvents::EventSlagElemental:
                    {
                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                            {
                                for (uint8 l_I = 0; l_I < 2; ++l_I)
                                {
                                    if (Creature* l_Elemental = me->SummonCreature(eCreatures::SlagElemental, g_EncounterAddSpawns[l_I][urand(0, 2)]))
                                    {
                                        float l_O = l_Elemental->GetAngle(l_Fury);
                                        float l_X = l_Elemental->GetPositionX() + 20.0f * cos(l_O);
                                        float l_Y = l_Elemental->GetPositionY() + 20.0f * sin(l_O);

                                        l_Elemental->GetMotionMaster()->MoveJump(l_X, l_Y, me->GetPositionZ(), 10.0f, 30.0f);
                                    }
                                }
                            }
                        }

                        m_Events.ScheduleEvent(eEvents::EventSlagElemental, 55 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventFirecaller:
                    {
                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                            {
                                for (uint8 l_I = 0; l_I < 2; ++l_I)
                                {
                                    if (Creature* l_Firecaller = me->SummonCreature(eCreatures::Firecaller, g_EncounterAddSpawns[l_I][urand(0, 2)]))
                                    {
                                        float l_O = l_Firecaller->GetAngle(l_Fury);
                                        float l_X = l_Firecaller->GetPositionX() + 20.0f * cos(l_O);
                                        float l_Y = l_Firecaller->GetPositionY() + 20.0f * sin(l_O);

                                        l_Firecaller->GetMotionMaster()->MoveJump(l_X, l_Y, me->GetPositionZ(), 10.0f, 30.0f);
                                    }
                                }
                            }
                        }

                        m_Events.ScheduleEvent(eEvents::EventFirecaller, 45 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventSecurityGuard:
                    {
                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                            {
                                for (uint8 l_I = 0; l_I < 2; ++l_I)
                                {
                                    if (Creature* l_Guard = me->SummonCreature(eCreatures::SecurityGuardFight, g_EncounterAddSpawns[l_I][urand(0, 2)]))
                                    {
                                        float l_O = l_Guard->GetAngle(l_Fury);
                                        float l_X = l_Guard->GetPositionX() + 20.0f * cos(l_O);
                                        float l_Y = l_Guard->GetPositionY() + 20.0f * sin(l_O);

                                        l_Guard->GetMotionMaster()->MoveJump(l_X, l_Y, me->GetPositionZ(), 10.0f, 30.0f);
                                    }
                                }
                            }
                        }

                        m_Events.ScheduleEvent(eEvents::EventSecurityGuard, 40 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventHeat:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                        {
                            /// Tempered will increase its efficacy when Heat is initially applied to you.
                            if (!l_Target->HasAura(eSpells::Heat))
                                me->CastSpell(l_Target, eSpells::Tempered, true);

                            me->CastSpell(l_Target, eSpells::Heat, true);

                            /// Tempered will increase its efficacy when exposed to greater Heat levels than your current Tempered level.
                            if (AuraPtr l_Heat = l_Target->GetAura(eSpells::Heat))
                            {
                                if (AuraPtr l_Tempered = l_Target->GetAura(eSpells::Tempered))
                                {
                                    if (l_Heat->GetStackAmount() > l_Tempered->GetStackAmount())
                                        me->CastSpell(l_Target, eSpells::Tempered, true);
                                }
                            }
                        }

                        Talk(eTalks::HeatLevelRising);

                        m_Events.ScheduleEvent(eEvents::EventHeat, 11 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventMelt:
                    {
                        Talk(eTalks::TalkMelt);
                        me->CastSpell(me, eSpells::Melt, true);
                        m_Events.ScheduleEvent(eEvents::EventMelt, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            uint32 GetBlastTimer() const
            {
                /// The time until the next cast of Blast is indicated by The Heart of the Mountain's Energy bar, with Blast being cast whenever it reaches maximum Energy.
                /// In addition to this, The Heart of the Mountain also has a Heat resource, which determines exactly how quickly its Energy regenerates.
                /// The longer The Heart of the Mountain spends inside the Blast Furnace (in other words, the longer your raid spends in Phase One), the higher its Heat will be.
                uint32 l_Time = 0;
                int32 l_Power = me->GetPower(Powers::POWER_ALTERNATE_POWER);

                if (l_Power >= 75)
                    l_Time = 5 * TimeConstants::IN_MILLISECONDS;
                else if (l_Power >= 50)
                    l_Time = 10 * TimeConstants::IN_MILLISECONDS;
                else if (l_Power >= 25)
                    l_Time = 15 * TimeConstants::IN_MILLISECONDS;
                else
                    l_Time = 25 * TimeConstants::IN_MILLISECONDS;

                return l_Time;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_heart_of_the_mountainAI(p_Creature);
        }
};

/// Foreman Feldspar - 76809
class boss_foreman_feldspar : public CreatureScript
{
    public:
        boss_foreman_feldspar() : CreatureScript("boss_foreman_feldspar") { }

        enum eTalks
        {
            Aggro,
            BellowsOperators,
            FirstHeatDestroyed,
            Ability,
            Slay,
            Wipe,
            Death,
            BlastIncreased,
            RuptureWarning
        };

        enum eCreatures
        {
            /// Before fight
            SecurityGuard           = 88818,
            FurnaceEngineer         = 88820,
            BellowsOperator         = 88821,
            HeatRegulator           = 76808,
            /// During fight
            FurnaceEngineerFight    = 76810,
            BellowsOperatorFight    = 76811,
            SecurityGuardFight      = 76812
        };

        enum eActions
        {
            ActionActivateBellows,
            ActionRegulatorDestroyed,
            ActionBlastIncreased,
            ActionSwitchToPhase2
        };

        enum eEvents
        {
            EventBerserker = 1,
            EventPyroclasm,
            EventRupture
        };

        enum eCosmeticEvents
        {
            EventFurnaceEngineer = 1,
            EventSecurityGuard,
            EventBellowsOperator
        };

        enum eSpells
        {
            /// Misc
            Bomb            = 155192,
            BombOverrider   = 174716,
            /// Fight
            Pyroclasm       = 156937,
            Rupture         = 156934,
            RuptureDoT      = 156932,
            HotBlooded      = 158247
        };

        struct boss_foreman_feldsparAI : public BossAI
        {
            boss_foreman_feldsparAI(Creature* p_Creature) : BossAI(p_Creature, eFoundryDatas::DataForemanFeldspar)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            EventMap m_Events;
            EventMap m_CosmeticEvents;

            bool m_RegulatorDestroyed;

            void Reset() override
            {
                m_Events.Reset();
                m_CosmeticEvents.Reset();

                _Reset();

                me->RemoveAllAreasTrigger();

                me->RemoveAura(eFoundrySpells::Berserker);
                me->RemoveAura(eSpells::HotBlooded);

                m_RegulatorDestroyed = false;

                if (m_Instance != nullptr)
                {
                    if (Creature* l_Blackhand = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BlackhandCosmetic)))
                    {
                        if (l_Blackhand->IsAIEnabled)
                            l_Blackhand->AI()->Reset();
                    }
                }
            }

            void KilledUnit(Unit* p_Who) override
            {
                if (p_Who->GetTypeId() != TYPEID_PLAYER)
                    return;

                Talk(eTalks::Slay);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                me->CastSpell(me, eSpells::HotBlooded, true);

                Talk(eTalks::Aggro);

                std::list<Creature*> l_BellowsOperators;
                me->GetCreatureListWithEntryInGrid(l_BellowsOperators, eCreatures::BellowsOperator, 150.0f);

                for (Creature* l_Operator : l_BellowsOperators)
                {
                    if (l_Operator->IsAIEnabled)
                        l_Operator->AI()->DoAction(eActions::ActionActivateBellows);
                }

                if (m_Instance != nullptr)
                {
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);

                    CallAddsInCombat(p_Attacker);

                    if (Creature* l_HeartOfTheMountain = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                    {
                        if (l_HeartOfTheMountain->IsAIEnabled)
                            l_HeartOfTheMountain->SetInCombatWithZone();
                    }
                }

                m_Events.ScheduleEvent(eEvents::EventBerserker, 780 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventPyroclasm, 18 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventRupture, 26 * TimeConstants::IN_MILLISECONDS);

                m_CosmeticEvents.ScheduleEvent(eCosmeticEvents::EventFurnaceEngineer, GetSummoningTimer());
                m_CosmeticEvents.ScheduleEvent(eCosmeticEvents::EventSecurityGuard, GetSummoningTimer());
                m_CosmeticEvents.ScheduleEvent(eCosmeticEvents::EventBellowsOperator, GetSummoningTimer());
            }

            void JustDied(Unit* p_Killer) override
            {
                Talk(eTalks::Death);

                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustReachedHome() override
            {
                Talk(eTalks::Wipe);

                if (m_Instance != nullptr)
                {
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::RuptureDoT);
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::Bomb);
                    m_Instance->DoRemoveAurasDueToSpellOnPlayers(eSpells::BombOverrider);
                }

                summons.DespawnAll();

                ResetFirstAdds();
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionRegulatorDestroyed:
                    {
                        if (!m_RegulatorDestroyed)
                        {
                            Talk(eTalks::FirstHeatDestroyed);
                            m_RegulatorDestroyed = true;
                        }

                        break;
                    }
                    case eActions::ActionBlastIncreased:
                    {
                        Talk(eTalks::BlastIncreased);
                        break;
                    }
                    case eActions::ActionSwitchToPhase2:
                    {
                        m_CosmeticEvents.CancelEvent(eCosmeticEvents::EventFurnaceEngineer);
                        m_CosmeticEvents.CancelEvent(eCosmeticEvents::EventSecurityGuard);
                        m_CosmeticEvents.CancelEvent(eCosmeticEvents::EventBellowsOperator);
                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::Rupture:
                    {
                        Talk(eTalks::RuptureWarning, p_Target->GetGUID(), TextRange::TEXT_RANGE_NORMAL);
                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                m_CosmeticEvents.Update(p_Diff);

                switch (m_CosmeticEvents.ExecuteEvent())
                {
                    case eCosmeticEvents::EventFurnaceEngineer:
                    {
                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                            {
                                for (uint8 l_I = 0; l_I < 2; ++l_I)
                                {
                                    if (Creature* l_Engineer = me->SummonCreature(eCreatures::FurnaceEngineerFight, g_EncounterAddSpawns[l_I][urand(0, 2)]))
                                    {
                                        float l_O = l_Engineer->GetAngle(l_Fury);
                                        float l_X = l_Engineer->GetPositionX() + 20.0f * cos(l_O);
                                        float l_Y = l_Engineer->GetPositionY() + 20.0f * sin(l_O);

                                        l_Engineer->GetMotionMaster()->MoveJump(l_X, l_Y, me->GetPositionZ(), 10.0f, 30.0f);
                                    }
                                }
                            }
                        }

                        m_CosmeticEvents.ScheduleEvent(eCosmeticEvents::EventFurnaceEngineer, GetSummoningTimer());
                        break;
                    }
                    case eCosmeticEvents::EventSecurityGuard:
                    {
                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                            {
                                for (uint8 l_I = 0; l_I < 2; ++l_I)
                                {
                                    if (Creature* l_Guard = me->SummonCreature(eCreatures::SecurityGuardFight, g_EncounterAddSpawns[l_I][urand(0, 2)]))
                                    {
                                        float l_O = l_Guard->GetAngle(l_Fury);
                                        float l_X = l_Guard->GetPositionX() + 20.0f * cos(l_O);
                                        float l_Y = l_Guard->GetPositionY() + 20.0f * sin(l_O);

                                        l_Guard->GetMotionMaster()->MoveJump(l_X, l_Y, me->GetPositionZ(), 10.0f, 30.0f);
                                    }
                                }
                            }
                        }

                        m_CosmeticEvents.ScheduleEvent(eCosmeticEvents::EventSecurityGuard, GetSummoningTimer());
                        break;
                    }
                    case eCosmeticEvents::EventBellowsOperator:
                    {
                        Talk(eTalks::BellowsOperators);

                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                            {
                                for (uint8 l_I = 0; l_I < 2; ++l_I)
                                {
                                    if (Creature* l_Operator = me->SummonCreature(eCreatures::BellowsOperatorFight, g_BellowsOperatorSpawns[l_I]))
                                    {
                                        float l_O = l_Operator->GetAngle(l_Fury);
                                        float l_X = l_Operator->GetPositionX() + 12.0f * cos(l_O);
                                        float l_Y = l_Operator->GetPositionY() + 12.0f * sin(l_O);

                                        l_Operator->GetMotionMaster()->MoveJump(l_X, l_Y, me->GetPositionZ(), 10.0f, 30.0f);
                                    }
                                }
                            }
                        }

                        m_CosmeticEvents.ScheduleEvent(eCosmeticEvents::EventBellowsOperator, GetSummoningTimer());
                        break;
                    }
                    default:
                        break;
                }

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventBerserker:
                    {
                        me->CastSpell(me, eFoundrySpells::Berserker, true);
                        break;
                    }
                    case eEvents::EventPyroclasm:
                    {
                        Talk(eTalks::Ability);
                        me->CastSpell(me, eSpells::Pyroclasm, false);
                        m_Events.ScheduleEvent(eEvents::EventPyroclasm, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRupture:
                    {
                        me->CastSpell(me, eSpells::Rupture, false);
                        m_Events.ScheduleEvent(eEvents::EventRupture, 25 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }

            void ResetFirstAdds()
            {
                std::list<Creature*> l_CreatureList;

                uint32 const l_Entries[4] = { eCreatures::SecurityGuard, eCreatures::FurnaceEngineer, eCreatures::BellowsOperator, eCreatures::HeatRegulator };

                for (uint8 l_I = 0; l_I < 4; ++l_I)
                {
                    l_CreatureList.clear();

                    me->GetCreatureListWithEntryInGrid(l_CreatureList, l_Entries[l_I], 150.0f);

                    for (Creature* l_Iter : l_CreatureList)
                    {
                        if (l_Iter->isAlive() && !l_Iter->IsInEvadeMode() && l_Iter->IsAIEnabled)
                            l_Iter->AI()->EnterEvadeMode();
                        else if (!l_Iter->isAlive())
                        {
                            l_Iter->Respawn();
                            l_Iter->GetMotionMaster()->MoveTargetedHome();
                        }
                    }
                }
            }

            void CallAddsInCombat(Unit* p_Attacker)
            {
                std::list<Creature*> l_CreatureList;

                uint32 const l_Entries[4] = { eCreatures::SecurityGuard, eCreatures::FurnaceEngineer, eCreatures::BellowsOperator, eCreatures::HeatRegulator };

                for (uint8 l_I = 0; l_I < 4; ++l_I)
                {
                    l_CreatureList.clear();

                    me->GetCreatureListWithEntryInGrid(l_CreatureList, l_Entries[l_I], 150.0f);

                    for (Creature* l_Iter : l_CreatureList)
                    {
                        if (l_Iter->IsAIEnabled)
                        {
                            if (l_I < 2)
                                l_Iter->AI()->AttackStart(p_Attacker);
                            else
                                l_Iter->SetInCombatWithZone();
                        }
                    }
                }
            }

            uint32 GetSummoningTimer() const
            {
                uint32 l_Timer = 0;

                if (IsLFR())
                    l_Timer = 65 * TimeConstants::IN_MILLISECONDS;
                else if (IsHeroic())
                    l_Timer = 55 * TimeConstants::IN_MILLISECONDS;
                else if (IsMythic())
                    l_Timer = 40 * TimeConstants::IN_MILLISECONDS;
                else
                    l_Timer = 60 * TimeConstants::IN_MILLISECONDS;

                return l_Timer;
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new boss_foreman_feldsparAI(p_Creature);
        }
};

/// Blackhand (Cosmetic) - 76831
class npc_foundry_blackhand_cosmetic : public CreatureScript
{
    public:
        npc_foundry_blackhand_cosmetic() : CreatureScript("npc_foundry_blackhand_cosmetic") { }

        enum eTalks
        {
            Intro,
            FirstHeatDestroyedBefore,
            FirstHeatDestroyedAfter,
            HeartExposedBefore,
            HeartExposedAfter,
            Phase2,
            ThreeElementalists,
            TwoElementalists,
            OneElementalist,
            Phase3Freedom1,
            Phase3Freedom2,
            HeartDeath
        };

        enum eActions
        {
            ActionIntro,
            ActionRegulatorDestroyed,
            ActionFreeFury,
            ActionElementalistKilled,
            ActionFuryDead
        };

        struct npc_foundry_blackhand_cosmeticAI : public ScriptedAI
        {
            npc_foundry_blackhand_cosmeticAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            bool m_FirstHeatDestroyed;

            uint8 m_ElementalistKilled;

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_NPC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);

                m_FirstHeatDestroyed = false;

                m_ElementalistKilled = 0;
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionIntro:
                    {
                        Talk(eTalks::Intro);
                        break;
                    }
                    case eActions::ActionRegulatorDestroyed:
                    {
                        if (!m_FirstHeatDestroyed)
                        {
                            m_FirstHeatDestroyed = true;

                            Talk(eTalks::FirstHeatDestroyedBefore);

                            AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(eTalks::FirstHeatDestroyedAfter);
                            });
                        }
                        else
                        {
                            if (m_Instance != nullptr)
                            {
                                if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                                {
                                    if (l_Fury->IsAIEnabled)
                                        l_Fury->AI()->DoAction(eActions::ActionFreeFury);
                                }
                            }

                            Talk(eTalks::HeartExposedBefore);
                            Talk(eTalks::HeartExposedAfter);
                        }

                        break;
                    }
                    case eActions::ActionElementalistKilled:
                    {
                        ++m_ElementalistKilled;
                        Talk(eTalks::Phase2 + m_ElementalistKilled);

                        if (m_ElementalistKilled == eFoundryDatas::MaxPrimalElementalists)
                        {
                            AddTimedDelayedOperation(11 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(eTalks::Phase3Freedom2);
                            });
                        }

                        break;
                    }
                    case eActions::ActionFuryDead:
                    {
                        AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Talk(eTalks::HeartDeath);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_blackhand_cosmeticAI(p_Creature);
        }
};

/// Primal Elementalist - 76815
class npc_foundry_primal_elementalist : public CreatureScript
{
    public:
        npc_foundry_primal_elementalist() : CreatureScript("npc_foundry_primal_elementalist") { }

        enum eTalks
        {
            HeartExposed1,
            HeartExposed2,
            HeartExposed3,
            Phase3Freedom
        };

        enum eMoves
        {
            MoveFirst = 1,
            MoveJump
        };

        enum eDatas
        {
            ElementalistMoveIndex,
            ElementalistKilled
        };

        enum eSpells
        {
            /// Cosmetic
            Containment         = 155265,
            DamageShield        = 155176,
            ShieldsDown         = 158345,
            ReactiveEarthShield = 155173
        };

        enum eEvent
        {
            EventReactiveEerthShield = 1
        };

        struct npc_foundry_primal_elementalistAI : public ScriptedAI
        {
            npc_foundry_primal_elementalistAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_CanTalk(false)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            EventMap m_Events;

            bool m_CanTalk;

            void Reset() override
            {
                me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                AddTimedDelayedOperation(100, [this]() -> void
                {
                    Position const l_MovePos = { 199.236f, 3467.01f, 266.733f, me->GetOrientation() };

                    me->GetMotionMaster()->MovePoint(eMoves::MoveFirst, l_MovePos);
                });

                if (!IsLFR())
                    me->CastSpell(me, eSpells::DamageShield, true);

                m_Events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 3);
            }

            void JustDied(Unit* p_Killer) override
            {
                if (m_Instance != nullptr)
                {
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                    m_Instance->SetData(eFoundryDatas::PrimalElementalistTime, (uint32)time(nullptr));

                    if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                    {
                        if (l_Fury->IsAIEnabled)
                        {
                            l_Fury->AI()->SetData(eDatas::ElementalistKilled, 0);

                            uint32 l_Count = l_Fury->AI()->GetData(eDatas::ElementalistKilled);
                            if (l_Count == eFoundryDatas::MaxPrimalElementalists)
                                Talk(eTalks::Phase3Freedom);
                        }
                    }
                }
            }

            void EnterEvadeMode() override
            {
                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE && p_Type != MovementGeneratorType::EFFECT_MOTION_TYPE)
                    return;

                switch (p_ID)
                {
                    case eMoves::MoveFirst:
                    {
                        if (m_Instance != nullptr)
                        {
                            if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                            {
                                uint8 l_MoveI = 0;

                                if (l_Fury->IsAIEnabled)
                                {
                                    l_MoveI = l_Fury->AI()->GetData(eDatas::ElementalistMoveIndex);
                                    l_Fury->AI()->SetData(eDatas::ElementalistMoveIndex, 0);

                                    if (l_MoveI == (eFoundryDatas::MaxPrimalElementalists - 1))
                                        m_CanTalk = true;
                                }

                                AddTimedDelayedOperation(100, [this, l_MoveI]() -> void
                                {
                                    me->GetMotionMaster()->MoveJump(g_PrimalElementalistsMoves[l_MoveI], 20.0f, 19.2911f, eMoves::MoveJump);
                                });
                            }
                        }

                        break;
                    }
                    case eMoves::MoveJump:
                    {
                        AddTimedDelayedOperation(100, [this]() -> void
                        {
                            if (m_Instance != nullptr)
                            {
                                if (Creature* l_Fury = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                                    me->SetFacingTo(me->GetAngle(l_Fury));
                            }
                        });

                        AddTimedDelayedOperation(200, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::Containment, false);

                            if (m_CanTalk)
                                Talk(eTalks::HeartExposed1);
                        });

                        if (m_CanTalk)
                        {
                            AddTimedDelayedOperation(6 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(eTalks::HeartExposed2);
                            });

                            AddTimedDelayedOperation(8 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                            {
                                Talk(eTalks::HeartExposed3);
                            });
                        }

                        break;
                    }
                    default:
                        break;
                }
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::ShieldsDown)
                    m_Events.ScheduleEvent(eEvent::EventReactiveEerthShield, urand(1 * TimeConstants::IN_MILLISECONDS, 15 * TimeConstants::IN_MILLISECONDS));
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                m_Events.Update(p_Diff);

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventReactiveEerthShield:
                    {
                        me->CastSpell(me, eSpells::ReactiveEarthShield, true);
                        break;
                    }
                    default:
                        break;
                }
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_primal_elementalistAI(p_Creature);
        }
};

/// Bellows Operator - 88821
/// Bellows Operator (Fight) - 76811
class npc_foundry_bellows_operator : public CreatureScript
{
    public:
        npc_foundry_bellows_operator() : CreatureScript("npc_foundry_bellows_operator") { }

        enum eSpells
        {
            Loading = 155181
        };

        enum eActions
        {
            ActionActivateBellows,
            ActionPhase2
        };

        enum eCosmeticEvent
        {
            EventActivateBellows = 1
        };

        enum eCreatures
        {
            OperatorForFight    = 76811,
            Bellows             = 78477
        };

        struct npc_foundry_bellows_operatorAI : public ScriptedAI
        {
            npc_foundry_bellows_operatorAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_SwitchStatePct(50) { }

            EventMap m_CosmeticEvent;

            int32 m_SwitchStatePct;

            void Reset() override
            {
                m_CosmeticEvent.Reset();

                me->RemoveAura(eSpells::Loading);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                AddTimedDelayedOperation(5 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    if (Creature* l_Bellows = me->FindNearestCreature(eCreatures::Bellows, 50.0f))
                        me->EnterVehicle(l_Bellows);

                    if (me->GetEntry() == eCreatures::OperatorForFight)
                        m_CosmeticEvent.ScheduleEvent(eCosmeticEvent::EventActivateBellows, 1 * TimeConstants::IN_MILLISECONDS);
                });
            }

            void DoAction(int32 const p_Action) override
            {
                switch (p_Action)
                {
                    case eActions::ActionActivateBellows:
                    {
                        m_CosmeticEvent.ScheduleEvent(eCosmeticEvent::EventActivateBellows, 1 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eActions::ActionPhase2:
                    {
                        me->RemoveAura(eSpells::Loading);
                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        if (Player* l_Target = me->SelectNearestPlayerNotGM(50.0f))
                            AttackStart(l_Target);

                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (me->GetReactState() == ReactStates::REACT_AGGRESSIVE)
                    return;

                /// In Mythic Difficulty, the Bellows Operators will stop operating the bellows of the furnace and engage players in melee combat after reaching 50% health remaining.
                if (me->HealthBelowPctDamaged(m_SwitchStatePct, p_Damage))
                {
                    me->SetReactState(ReactStates::REACT_AGGRESSIVE);
                    AttackStart(p_Attacker);
                }
            }

            void EnterEvadeMode() override
            {
                me->RemoveAllAuras();

                me->InterruptNonMeleeSpells(true);

                /// Just in case, to prevent the fail Return to Home
                me->ClearUnitState(UnitState::UNIT_STATE_ROOT);
                me->ClearUnitState(UnitState::UNIT_STATE_DISTRACTED);
                me->ClearUnitState(UnitState::UNIT_STATE_STUNNED);

                CreatureAI::EnterEvadeMode();
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                m_CosmeticEvent.Update(p_Diff);

                switch (m_CosmeticEvent.ExecuteEvent())
                {
                    case eCosmeticEvent::EventActivateBellows:
                    {
                        AddTimedDelayedOperation(1 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            me->CastSpell(me, eSpells::Loading, false);
                            me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                        });

                        break;
                    }
                    default:
                        break;
                }

                if (!UpdateVictim())
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_bellows_operatorAI(p_Creature);
        }
};

/// Heat Regulator - 76808
class npc_foundry_heat_regulator : public CreatureScript
{
    public:
        npc_foundry_heat_regulator() : CreatureScript("npc_foundry_heat_regulator") { }

        enum eAction
        {
            RegulatorDestroyed = 1
        };

        enum eGameObjects
        {
            LeftHeatRegulator   = 237305,
            RightHeatRegulator  = 237304
        };

        struct npc_foundry_heat_regulatorAI : public ScriptedAI
        {
            npc_foundry_heat_regulatorAI(Creature* p_Creature) : ScriptedAI(p_Creature)
            {
                m_Instance = p_Creature->GetInstanceScript();
            }

            InstanceScript* m_Instance;

            void Reset() override
            {
                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->AddUnitState(UnitState::UNIT_STATE_STUNNED);
                me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                GameObject* l_Regulator = me->FindNearestGameObject(eGameObjects::LeftHeatRegulator, 10.0f);
                if (l_Regulator == nullptr)
                    l_Regulator = me->FindNearestGameObject(eGameObjects::RightHeatRegulator, 10.0f);

                if (l_Regulator != nullptr)
                    l_Regulator->SetGoState(GOState::GO_STATE_ACTIVE);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_ENGAGE, me, 2);

                /// Must be done at next update
                AddTimedDelayedOperation(100, [this]() -> void
                {
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC);
                });
            }

            void EnterEvadeMode() override
            {
                me->ClearUnitState(UnitState::UNIT_STATE_STUNNED);
                me->ClearUnitState(UnitState::UNIT_STATE_ROOT);

                CreatureAI::EnterEvadeMode();

                me->StopMoving();
                me->ClearUnitState(UnitState::UNIT_STATE_EVADE);

                JustReachedHome();
            }

            void JustReachedHome() override
            {
                if (m_Instance != nullptr)
                    m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);
            }

            void JustDied(Unit* p_Killer) override
            {
                if (m_Instance == nullptr)
                    return;

                m_Instance->SendEncounterUnit(EncounterFrameType::ENCOUNTER_FRAME_DISENGAGE, me);

                if (Creature* l_Foreman = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::ForemanFeldspar)))
                {
                    if (l_Foreman->IsAIEnabled)
                        l_Foreman->AI()->DoAction(eAction::RegulatorDestroyed);
                }

                if (Creature* l_Blackhand = Creature::GetCreature(*me, m_Instance->GetData64(eFoundryCreatures::BlackhandCosmetic)))
                {
                    if (l_Blackhand->IsAIEnabled)
                        l_Blackhand->AI()->DoAction(eAction::RegulatorDestroyed);
                }

                GameObject* l_Regulator = me->FindNearestGameObject(eGameObjects::LeftHeatRegulator, 10.0f);
                if (l_Regulator == nullptr)
                    l_Regulator = me->FindNearestGameObject(eGameObjects::RightHeatRegulator, 10.0f);

                if (l_Regulator != nullptr)
                    l_Regulator->SetGoState(GOState::GO_STATE_READY);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_heat_regulatorAI(p_Creature);
        }
};

/// Security Guard - 88818
/// Security Guard (Fight) - 76812
class npc_foundry_security_guard : public CreatureScript
{
    public:
        npc_foundry_security_guard() : CreatureScript("npc_foundry_security_guard") { }

        enum eSpells
        {
            DefenseAura     = 160379,   ///< Only for LFR
            DefenseMissile  = 177773
        };

        enum eEvent
        {
            EventDefense = 1
        };

        enum eCreature
        {
            GuardForFight = 76812
        };

        struct npc_foundry_security_guardAI : public ScriptedAI
        {
            npc_foundry_security_guardAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();

                me->RemoveAllAreasTrigger();

                me->RemoveAura(eSpells::DefenseAura);

                me->SetCanDualWield(false);

                if (me->GetEntry() == eCreature::GuardForFight)
                {
                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Player* l_Target = me->SelectNearestPlayerNotGM(50.0f))
                            AttackStart(l_Target);
                    });
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                me->RemoveAllAreasTrigger();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_Events.ScheduleEvent(eEvent::EventDefense, 5 * TimeConstants::IN_MILLISECONDS);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventDefense:
                    {
                        if (IsLFR())
                            me->CastSpell(me, eSpells::DefenseAura, false);
                        else
                            me->CastSpell(*me, eSpells::DefenseMissile, false);

                        m_Events.ScheduleEvent(eEvent::EventDefense, 40 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_security_guardAI(p_Creature);
        }
};

/// Furnace Engineer - 88820
/// Furnace Engineer (Fight) - 76810
class npc_foundry_furnace_engineer : public CreatureScript
{
    public:
        npc_foundry_furnace_engineer() : CreatureScript("npc_foundry_furnace_engineer") { }

        enum eSpells
        {
            Electrocution   = 155201,
            Bomb            = 155192,
            DropLitBombs    = 174726,
            Repair          = 155179
        };

        enum eEvents
        {
            EventElectrocution = 1,
            EventBomb,
            EventRepair
        };

        enum eTalk
        {
            BombWarning = 8
        };

        enum eCreatures
        {
            HeatRegulator       = 76808,
            EngineerForFight    = 76810
        };

        enum eMove
        {
            MoveRegulator = 1
        };

        struct npc_foundry_furnace_engineerAI : public ScriptedAI
        {
            npc_foundry_furnace_engineerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            bool m_IsInRepair;

            void Reset() override
            {
                m_Events.Reset();

                m_IsInRepair = false;

                if (me->GetEntry() == eCreatures::EngineerForFight)
                {
                    AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                    {
                        if (Player* l_Target = me->SelectNearestPlayerNotGM(50.0f))
                            AttackStart(l_Target);
                    });
                }
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_Events.ScheduleEvent(eEvents::EventElectrocution, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventBomb, 10 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventRepair, 15 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::Bomb)
                {
                    if (InstanceScript* l_InstanceScript = me->GetInstanceScript())
                    {
                        if (Creature* l_Creature = Creature::GetCreature(*me, l_InstanceScript->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                        {
                            if (l_Creature->IsAIEnabled)
                                l_Creature->AI()->Talk(eTalk::BombWarning, p_Target->GetGUID(), TextRange::TEXT_RANGE_NORMAL);
                        }
                    }
                }
            }

            void JustDied(Unit* p_Killer) override
            {
                me->CastSpell(me, eSpells::DropLitBombs, true);
            }

            void MovementInform(uint32 p_Type, uint32 p_ID) override
            {
                if (p_Type != MovementGeneratorType::POINT_MOTION_TYPE)
                    return;

                if (p_ID == eMove::MoveRegulator)
                    me->CastSpell(me, eSpells::Repair, false);
            }

            void OnSpellFinished(SpellInfo const* p_SpellInfo) override
            {
                if (p_SpellInfo->Id == eSpells::Repair)
                {
                    m_IsInRepair = false;

                    if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_TOPAGGRO))
                        AttackStart(l_Target);
                }
            }

            void OnTaunt(Unit* p_Taunter) override
            {
                if (!m_IsInRepair)
                    return;

                me->InterruptNonMeleeSpells(true);
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim() || m_IsInRepair)
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventElectrocution:
                    {
                        if (Unit* l_Target = SelectTarget(SelectAggroTarget::SELECT_TARGET_RANDOM))
                            me->CastSpell(l_Target, eSpells::Electrocution, false);

                        m_Events.ScheduleEvent(eEvents::EventElectrocution, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventBomb:
                    {
                        me->CastSpell(me, eSpells::Bomb, false);
                        m_Events.ScheduleEvent(eEvents::EventBomb, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventRepair:
                    {
                        Position const l_LeftPos = { 147.85f, 3572.02f, 218.251f, 0.507f };
                        Position const l_RightPos = { 247.14f, 3572.02f, 218.246f, 2.794f };

                        if (Unit* l_Regulator = me->FindNearestCreature(eCreatures::HeatRegulator, 100.0f))
                        {
                            if (l_Regulator->GetDistance(l_LeftPos) < l_Regulator->GetDistance(l_RightPos))
                                me->GetMotionMaster()->MovePoint(eMove::MoveRegulator, l_LeftPos);
                            else
                                me->GetMotionMaster()->MovePoint(eMove::MoveRegulator, l_RightPos);

                            m_IsInRepair = true;
                        }

                        m_Events.ScheduleEvent(eEvents::EventRepair, 20 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_furnace_engineerAI(p_Creature);
        }
};

/// Cluster of Lit Bombs - 87339
class npc_foundry_cluster_of_lit_bombs : public CreatureScript
{
    public:
        npc_foundry_cluster_of_lit_bombs() : CreatureScript("npc_foundry_cluster_of_lit_bombs") { }

        enum eSpells
        {
            ClusterOfLitBombs   = 174731,
            BombAoEDespawn      = 155187,
            BombOverrider       = 174716
        };

        struct npc_foundry_cluster_of_lit_bombsAI : public ScriptedAI
        {
            npc_foundry_cluster_of_lit_bombsAI(Creature* p_Creature) : ScriptedAI(p_Creature), m_MustExplode(true) { }

            bool m_MustExplode;

            void Reset() override
            {
                me->CastSpell(me, eSpells::ClusterOfLitBombs, true);

                me->SetReactState(ReactStates::REACT_PASSIVE);

                me->SetFlag(EUnitFields::UNIT_FIELD_NPC_FLAGS, NPCFlags::UNIT_NPC_FLAG_SPELLCLICK);

                me->SetUInt32Value(EUnitFields::UNIT_FIELD_INTERACT_SPELL_ID, eSpells::BombOverrider);
            }

            void OnSpellClick(Unit* p_Clicker) override
            {
                if (p_Clicker->HasAura(eSpells::BombOverrider))
                    return;

                p_Clicker->CastSpell(p_Clicker, eSpells::BombOverrider, true);

                if (AuraPtr l_Aura = me->GetAura(eSpells::ClusterOfLitBombs))
                    l_Aura->DropCharge();

                if (!me->HasAura(eSpells::ClusterOfLitBombs))
                {
                    m_MustExplode = false;
                    me->DespawnOrUnsummon();
                }
            }

            void JustDespawned() override
            {
                if (m_MustExplode)
                    me->CastSpell(me, eSpells::BombAoEDespawn, true);
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_cluster_of_lit_bombsAI(p_Creature);
        }
};

/// Slag Elemental - 78463
class npc_foundry_slag_elemental : public CreatureScript
{
    public:
        npc_foundry_slag_elemental() : CreatureScript("npc_foundry_slag_elemental") { }

        enum eSpells
        {
            Burn            = 155200,
            Fixate          = 155196,
            SlagBomb        = 176133,
            DamageShield    = 155176,
            Reanimate       = 155213,
            DropTarget      = 101438
        };

        enum eCreature
        {
            PrimalElementalist = 76815
        };

        enum eEvent
        {
            EventBurn = 1
        };

        struct npc_foundry_slag_elementalAI : public ScriptedAI
        {
            npc_foundry_slag_elementalAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            uint64 m_Target;

            void Reset() override
            {
                m_Events.Reset();

                m_Target = 0;

                me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS2, eUnitFlags2::UNIT_FLAG2_REGENERATE_POWER);
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                AddTimedDelayedOperation(2 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                {
                    me->CastSpell(me, eSpells::Fixate, true);

                    me->SetPower(Powers::POWER_ENERGY, 0);
                    me->SetMaxPower(Powers::POWER_ENERGY, 100);

                    m_Events.ScheduleEvent(eEvent::EventBurn, 5 * TimeConstants::IN_MILLISECONDS);
                });
            }

            void SetPower(Powers p_Power, int32 p_Value) override
            {
                if (p_Power != Powers::POWER_ENERGY)
                    return;

                if (p_Value >= 100)
                {
                    AddTimedDelayedOperation(50, [this]() -> void
                    {
                        me->CastSpell(me, eSpells::Reanimate, false);
                    });
                }
            }

            void RegeneratePower(Powers p_Power, int32& p_Value) override
            {
                if (p_Power != Powers::POWER_ENERGY)
                    return;

                p_Value = 0;
            }

            void OnSpellCasted(SpellInfo const* p_SpellInfo) override
            {
                switch (p_SpellInfo->Id)
                {
                    case eSpells::SlagBomb:
                    {
                        std::list<Creature*> l_Elementalists;
                        me->GetCreatureListWithEntryInGrid(l_Elementalists, eCreature::PrimalElementalist, 8.0f);

                        for (Creature* l_Creature : l_Elementalists)
                        {
                            if (l_Creature->HasAura(eSpells::DamageShield))
                                l_Creature->RemoveAura(eSpells::DamageShield);
                        }

                        break;
                    }
                    case eSpells::Reanimate:
                    {
                        me->RemoveAura(eSpells::SlagBomb);

                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE);
                        me->RemoveFlag(EUnitFields::UNIT_FIELD_FLAGS2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                        me->SetReactState(ReactStates::REACT_AGGRESSIVE);

                        AddTimedDelayedOperation(3 * TimeConstants::IN_MILLISECONDS, [this]() -> void
                        {
                            Reset();

                            me->CastSpell(me, eSpells::Fixate, true);
                        });

                        break;
                    }
                    default:
                        break;
                }
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                switch (p_SpellInfo->Id)
                {
                    case eSpells::Fixate:
                    {
                        m_Target = p_Target->GetGUID();
                        break;
                    }
                    default:
                        break;
                }
            }

            void DamageTaken(Unit* p_Attacker, uint32& p_Damage, SpellInfo const* p_SpellInfo) override
            {
                if (p_Damage >= me->GetHealth())
                {
                    p_Damage = 0;

                    m_Target = 0;

                    me->RemoveAllAuras();

                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS, eUnitFlags::UNIT_FLAG_IMMUNE_TO_PC | eUnitFlags::UNIT_FLAG_NON_ATTACKABLE | eUnitFlags::UNIT_FLAG_DISABLE_MOVE);
                    me->SetFlag(EUnitFields::UNIT_FIELD_FLAGS2, eUnitFlags2::UNIT_FLAG2_DISABLE_TURN);

                    me->SetReactState(ReactStates::REACT_PASSIVE);
                    me->AddUnitState(UnitState::UNIT_STATE_ROOT);

                    me->SetHealth(1);

                    me->CastSpell(me, eSpells::DropTarget, true);
                    me->CastSpell(me, eSpells::SlagBomb, false);
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                UpdateOperations(p_Diff);

                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING) || me->GetReactState() == ReactStates::REACT_PASSIVE)
                    return;

                if (Player* l_Target = Player::GetPlayer(*me, m_Target))
                {
                    if (!l_Target->isAlive())
                    {
                        m_Target = 0;
                        me->CastSpell(me, eSpells::Fixate, true);
                        return;
                    }

                    if (!me->IsWithinMeleeRange(l_Target))
                        me->GetMotionMaster()->MovePoint(0, *l_Target);
                }

                switch (m_Events.ExecuteEvent())
                {
                    case eEvent::EventBurn:
                    {
                        if (Player* l_Target = Player::GetPlayer(*me, m_Target))
                            me->CastSpell(l_Target, eSpells::Burn, false);

                        m_Events.ScheduleEvent(eEvent::EventBurn, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_slag_elementalAI(p_Creature);
        }
};

/// Firecaller - 76821
class npc_foundry_firecaller : public CreatureScript
{
    public:
        npc_foundry_firecaller() : CreatureScript("npc_foundry_firecaller") { }

        enum eSpells
        {
            /// Others
            Dormant         = 155233,
            /// Fight
            CauterizeWounds = 155186,
            LavaBurst       = 159408,
            VolatileFire    = 176121
        };

        enum eEvents
        {
            EventCauterizeWounds = 1,
            EventLavaBurst,
            EventVolatileFire
        };

        enum eCreature
        {
            SlagElemental = 78463
        };

        enum eTalk
        {
            TalkVolatileFire = 9
        };

        struct npc_foundry_firecallerAI : public ScriptedAI
        {
            npc_foundry_firecallerAI(Creature* p_Creature) : ScriptedAI(p_Creature) { }

            EventMap m_Events;

            void Reset() override
            {
                m_Events.Reset();
            }

            void EnterCombat(Unit* p_Attacker) override
            {
                m_Events.ScheduleEvent(eEvents::EventCauterizeWounds, 8 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventLavaBurst, 5 * TimeConstants::IN_MILLISECONDS);
                m_Events.ScheduleEvent(eEvents::EventVolatileFire, 2 * TimeConstants::IN_MILLISECONDS);
            }

            void SpellHitTarget(Unit* p_Target, SpellInfo const* p_SpellInfo) override
            {
                if (p_Target == nullptr)
                    return;

                if (p_SpellInfo->Id == eSpells::VolatileFire)
                {
                    if (InstanceScript* l_Instance = me->GetInstanceScript())
                    {
                        if (Creature* l_Fury = Creature::GetCreature(*me, l_Instance->GetData64(eFoundryCreatures::HeartOfTheMountain)))
                        {
                            if (l_Fury->IsAIEnabled)
                                l_Fury->AI()->Talk(eTalk::TalkVolatileFire, p_Target->GetGUID(), TextRange::TEXT_RANGE_NORMAL);
                        }
                    }
                }
            }

            void UpdateAI(uint32 const p_Diff) override
            {
                if (!UpdateVictim())
                    return;

                m_Events.Update(p_Diff);

                if (me->HasUnitState(UnitState::UNIT_STATE_CASTING))
                    return;

                switch (m_Events.ExecuteEvent())
                {
                    case eEvents::EventCauterizeWounds:
                    {
                        if (Unit* l_Ally = me->SelectNearbyMostInjuredAlly(me, 30.0f, eCreature::SlagElemental))
                            me->CastSpell(l_Ally, eSpells::CauterizeWounds, false);

                        m_Events.ScheduleEvent(eEvents::EventCauterizeWounds, 15 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventLavaBurst:
                    {
                        me->CastSpell(me, eSpells::LavaBurst, false);
                        m_Events.ScheduleEvent(eEvents::EventLavaBurst, 5 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    case eEvents::EventVolatileFire:
                    {
                        me->CastSpell(me, eSpells::VolatileFire, false);
                        m_Events.ScheduleEvent(eEvents::EventVolatileFire, 10 * TimeConstants::IN_MILLISECONDS);
                        break;
                    }
                    default:
                        break;
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* p_Creature) const override
        {
            return new npc_foundry_firecallerAI(p_Creature);
        }
};

/// Defense - 160379
class spell_foundry_defense_aura : public SpellScriptLoader
{
    public:
        spell_foundry_defense_aura() : SpellScriptLoader("spell_foundry_defense_aura") { }

        class spell_foundry_defense_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_defense_aura_AuraScript);

            enum eSpells
            {
                DefenseAura = 160379,
                DefenseProc = 160382
            };

            uint32 m_DamageTimer;

            bool Load()
            {
                m_DamageTimer = 500;
                return true;
            }

            void OnUpdate(uint32 p_Diff)
            {
                if (m_DamageTimer)
                {
                    if (m_DamageTimer <= p_Diff)
                    {
                        if (Unit* l_Caster = GetCaster())
                        {
                            std::list<Unit*> l_TargetList;
                            float l_Radius = 7.0f;

                            JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                            JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                            for (Unit* l_Iter : l_TargetList)
                            {
                                if (!l_Iter->HasAura(eSpells::DefenseProc))
                                    l_Caster->CastSpell(l_Iter, eSpells::DefenseProc, true);
                            }
                        }

                        m_DamageTimer = 500;
                    }
                    else
                        m_DamageTimer -= p_Diff;
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_foundry_defense_aura_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_defense_aura_AuraScript();
        }
};

/// Bomb (overrider) - 155192
/// Bomb (overrider - second) - 174716
class spell_foundry_bomb_overrider : public SpellScriptLoader
{
    public:
        spell_foundry_bomb_overrider() : SpellScriptLoader("spell_foundry_bomb_overrider") { }

        class spell_foundry_bomb_overrider_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_bomb_overrider_AuraScript);

            enum eSpell
            {
                BombAoE = 155187
            };

            void AfterApply(constAuraEffectPtr p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (Unit* l_Target = GetTarget())
                {
                    AuraPtr l_Aura = p_AurEff->GetBase();
                    if (l_Target->GetMap()->IsHeroic())
                    {
                        l_Aura->SetDuration(10 * TimeConstants::IN_MILLISECONDS);
                        l_Aura->SetMaxDuration(10 * TimeConstants::IN_MILLISECONDS);
                    }
                    else if (l_Target->GetMap()->IsMythic())
                    {
                        l_Aura->SetDuration(8 * TimeConstants::IN_MILLISECONDS);
                        l_Aura->SetMaxDuration(8 * TimeConstants::IN_MILLISECONDS);
                    }
                }
            }

            void AfterRemove(constAuraEffectPtr p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (Unit* l_Target = GetTarget())
                    l_Target->CastSpell(*l_Target, eSpell::BombAoE, true);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_foundry_bomb_overrider_AuraScript::AfterApply, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
                AfterEffectRemove += AuraEffectRemoveFn(spell_foundry_bomb_overrider_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_OVERRIDE_SPELLS, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_bomb_overrider_AuraScript();
        }
};

/// Rupture - 156934
class spell_foundry_rupture_aura : public SpellScriptLoader
{
    public:
        spell_foundry_rupture_aura() : SpellScriptLoader("spell_foundry_rupture_aura") { }

        class spell_foundry_rupture_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_rupture_aura_AuraScript);

            enum eSpell
            {
                RuptureAreatrigger = 156933
            };

            void AfterRemove(constAuraEffectPtr p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(*l_Target, eSpell::RuptureAreatrigger, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_foundry_rupture_aura_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_rupture_aura_AuraScript();
        }
};

/// Hot Blooded - 158247
class spell_foundry_hot_blooded_aura : public SpellScriptLoader
{
    public:
        spell_foundry_hot_blooded_aura() : SpellScriptLoader("spell_foundry_hot_blooded_aura") { }

        class spell_foundry_hot_blooded_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_hot_blooded_aura_AuraScript);

            enum eSpells
            {
                HotBloodedDoT = 158246
            };

            uint32 m_DamageTimer;

            bool Load()
            {
                m_DamageTimer = 500;
                return true;
            }

            void OnUpdate(uint32 p_Diff)
            {
                if (m_DamageTimer)
                {
                    if (m_DamageTimer <= p_Diff)
                    {
                        if (Unit* l_Caster = GetCaster())
                        {
                            std::list<Unit*> l_TargetList;
                            float l_Radius = 20.0f;

                            JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(l_Caster, l_Caster, l_Radius);
                            JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(l_Caster, l_TargetList, l_Check);
                            l_Caster->VisitNearbyObject(l_Radius, l_Searcher);

                            for (Unit* l_Iter : l_TargetList)
                                l_Caster->CastSpell(l_Iter, eSpells::HotBloodedDoT, true);
                        }

                        m_DamageTimer = 500;
                    }
                    else
                        m_DamageTimer -= p_Diff;
                }
            }

            void Register() override
            {
                OnAuraUpdate += AuraUpdateFn(spell_foundry_hot_blooded_aura_AuraScript::OnUpdate);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_hot_blooded_aura_AuraScript();
        }
};

/// Damage Shield - 155176
class spell_foundry_damage_shield : public SpellScriptLoader
{
    public:
        spell_foundry_damage_shield() : SpellScriptLoader("spell_foundry_damage_shield") { }

        class spell_foundry_damage_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_damage_shield_AuraScript);

            enum eSpell
            {
                ShieldsDown = 158345
            };

            void AfterRemove(constAuraEffectPtr p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Creature* l_Target = GetTarget()->ToCreature())
                {
                    if (!l_Target->IsAIEnabled)
                        return;

                    if (npc_foundry_primal_elementalist::npc_foundry_primal_elementalistAI* l_AI = CAST_AI(npc_foundry_primal_elementalist::npc_foundry_primal_elementalistAI, l_Target->GetAI()))
                    {
                        uint64 l_Guid = l_Target->GetGUID();
                        l_AI->AddTimedDelayedOperation(50, [this, l_Guid]() -> void
                        {
                            if (Unit* l_Target = sObjectAccessor->FindUnit(l_Guid))
                                l_Target->CastSpell(l_Target, eSpell::ShieldsDown, true);
                        });
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_foundry_damage_shield_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DAMAGE_IMMUNITY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_damage_shield_AuraScript();
        }
};

/// Shields Down - 158345
class spell_foundry_shields_down : public SpellScriptLoader
{
    public:
        spell_foundry_shields_down() : SpellScriptLoader("spell_foundry_shields_down") { }

        class spell_foundry_shields_down_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_shields_down_AuraScript);

            enum eSpell
            {
                DamageShield = 155176
            };

            void AfterRemove(constAuraEffectPtr p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (GetTarget() == nullptr)
                    return;

                if (Creature* l_Target = GetTarget()->ToCreature())
                {
                    if (!l_Target->IsAIEnabled)
                        return;

                    if (npc_foundry_primal_elementalist::npc_foundry_primal_elementalistAI* l_AI = CAST_AI(npc_foundry_primal_elementalist::npc_foundry_primal_elementalistAI, l_Target->GetAI()))
                    {
                        uint64 l_Guid = l_Target->GetGUID();
                        l_AI->AddTimedDelayedOperation(50, [this, l_Guid]() -> void
                        {
                            if (Unit* l_Target = sObjectAccessor->FindUnit(l_Guid))
                                l_Target->CastSpell(l_Target, eSpell::DamageShield, true);
                        });
                    }
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_foundry_shields_down_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_shields_down_AuraScript();
        }
};

/// Volatile Fire - 176121
class spell_foundry_volatile_fire : public SpellScriptLoader
{
    public:
        spell_foundry_volatile_fire() : SpellScriptLoader("spell_foundry_volatile_fire") { }

        class spell_foundry_volatile_fire_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_volatile_fire_AuraScript);

            void OnApply(constAuraEffectPtr p_AurEff, AuraEffectHandleModes p_Mode)
            {
                int32 l_NewDuration = p_AurEff->GetAmplitude();

                if (AuraPtr l_Base = p_AurEff->GetBase())
                {
                    l_Base->SetMaxDuration(l_NewDuration);
                    l_Base->SetDuration(l_NewDuration);
                }
            }

            void OnTick(constAuraEffectPtr p_AurEff)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                    {
                        uint32 l_TriggerID = GetSpellInfo()->Effects[EFFECT_0].TriggerSpell;
                        l_Caster->CastSpell(l_Target, l_TriggerID, true);
                    }
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_foundry_volatile_fire_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_foundry_volatile_fire_AuraScript::OnTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_volatile_fire_AuraScript();
        }
};

/// Melt - 155225
class spell_foundry_melt_aura : public SpellScriptLoader
{
    public:
        spell_foundry_melt_aura() : SpellScriptLoader("spell_foundry_melt_aura") { }

        class spell_foundry_melt_aura_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_foundry_melt_aura_AuraScript);

            enum eSpell
            {
                MeltAreatrigger = 155224
            };

            void AfterRemove(constAuraEffectPtr p_AurEff, AuraEffectHandleModes p_Mode)
            {
                if (Unit* l_Caster = GetCaster())
                {
                    if (Unit* l_Target = GetTarget())
                        l_Caster->CastSpell(*l_Target, eSpell::MeltAreatrigger, true);
                }
            }

            void Register() override
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_foundry_melt_aura_AuraScript::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_foundry_melt_aura_AuraScript();
        }
};

/// Crucible (Left) - 233757
/// Crucible (Right) - 233758
class go_founrdy_crucible : public GameObjectScript
{
    public:
        go_founrdy_crucible() : GameObjectScript("go_founrdy_crucible_left") { }

        struct go_founrdy_crucibleAI : public GameObjectAI
        {
            go_founrdy_crucibleAI(GameObject* p_GameObject) : GameObjectAI(p_GameObject) { }

            EventMap m_Events;

            enum eGameObjects
            {
                CrucibleSecLeft     = 237303,
                CrucibleSecRight    = 237302
            };

            enum eEvent
            {
                EventInit = 1
            };

            void Reset() override
            {
                uint32 l_Time = urand(1 * TimeConstants::IN_MILLISECONDS, 10 * TimeConstants::IN_MILLISECONDS);

                GameObject* l_CrucibleSec = go->FindNearestGameObject(eGameObjects::CrucibleSecLeft, 50.0f);
                if (l_CrucibleSec == nullptr)
                    l_CrucibleSec = go->FindNearestGameObject(eGameObjects::CrucibleSecRight, 50.0f);

                m_Events.ScheduleEvent(eEvent::EventInit, 2 * TimeConstants::IN_MILLISECONDS);

                if (l_CrucibleSec != nullptr)
                {
                    m_Events.Reset();

                    uint64 l_Guid = l_CrucibleSec->GetGUID();
                    AddTimedDelayedOperation(l_Time, [this, l_Guid]() -> void
                    {
                        if (GameObject* l_CrucibleSec = GameObject::GetGameObject(*go, l_Guid))
                            l_CrucibleSec->SetGoState(GOState::GO_STATE_ACTIVE);
                    });

                    l_Time += 6 * TimeConstants::IN_MILLISECONDS;

                    AddTimedDelayedOperation(l_Time, [this, l_Guid]() -> void
                    {
                        if (GameObject* l_CrucibleSec = GameObject::GetGameObject(*go, l_Guid))
                            l_CrucibleSec->SetGoState(GOState::GO_STATE_READY);
                    });

                    l_Time += 5 * TimeConstants::IN_MILLISECONDS;

                    AddTimedDelayedOperation(l_Time, [this]() -> void
                    {
                        /// Start animation
                        go->SetGoState(GOState::GO_STATE_ACTIVE);
                    });

                    l_Time += 5 * TimeConstants::IN_MILLISECONDS;

                    AddTimedDelayedOperation(l_Time, [this]() -> void
                    {
                        /// End animation
                        go->SetGoState(GOState::GO_STATE_READY);
                    });
                }
            }

            void LastOperationCalled() override
            {
                Reset();
            }

            void UpdateAI(uint32 p_Diff) override
            {
                UpdateOperations(p_Diff);

                m_Events.Update(p_Diff);

                if (m_Events.ExecuteEvent() == eEvent::EventInit)
                    Reset();
            }
        };

        GameObjectAI* GetAI(GameObject* p_GameObject) const override
        {
            return new go_founrdy_crucibleAI(p_GameObject);
        }
};

/// Rupture - 156933
class areatrigger_foundry_rupture : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_rupture() : AreaTriggerEntityScript("areatrigger_foundry_rupture") { }

        enum eSpell
        {
            RuptureDoT = 156932
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 6.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (l_Unit->GetDistance(p_AreaTrigger) <= 2.5f)
                    {
                        if (!l_Unit->HasAura(eSpell::RuptureDoT))
                            l_Caster->CastSpell(l_Unit, eSpell::RuptureDoT, true);
                    }
                    else if (!l_Unit->FindNearestAreaTrigger(p_AreaTrigger->GetSpellId(), 2.5f))
                    {
                        if (l_Unit->HasAura(eSpell::RuptureDoT))
                            l_Unit->RemoveAura(eSpell::RuptureDoT);
                    }
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 2.5f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (!l_Unit->FindNearestAreaTrigger(p_AreaTrigger->GetSpellId(), l_Radius))
                    {
                        if (l_Unit->HasAura(eSpell::RuptureDoT))
                            l_Unit->RemoveAura(eSpell::RuptureDoT);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_rupture();
        }
};

/// Slag Pool - 155738
class areatrigger_foundry_slag_pool : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_slag_pool() : AreaTriggerEntityScript("areatrigger_foundry_slag_pool") { }

        enum eSpells
        {
            SlagPoolDoTAura     = 155743,
            SlagPoolEnergizer   = 163532
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                AreaTriggerTemplateList const& l_Templates = p_AreaTrigger->GetTemplates();
                std::vector<G3D::Vector2> l_CheckPoints;

                std::list<Unit*> l_TargetList;
                float l_Radius = 50.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                float l_X = p_AreaTrigger->GetPositionX();
                float l_Y = p_AreaTrigger->GetPositionY();
                float l_Z = p_AreaTrigger->GetPositionZ();
                float l_O = p_AreaTrigger->GetOrientation();

                Position const l_Pos = { l_X, l_Y, l_Z, 0.0f };

                for (AreaTriggerTemplate l_Template : l_Templates)
                {
                    /// Absolute position according to the current position and the template
                    float l_AbsX = l_X + l_Template.m_PolygonDatas.m_Vertices[0];
                    float l_AbsY = l_Y + l_Template.m_PolygonDatas.m_Vertices[1];

                    /// Calculating the distance between the origin and the absolute position, and the angle between them
                    Position const l_VirtualPos = { l_AbsX, l_AbsY, l_Z, 0.0f };
                    float l_Dist = l_Pos.GetExactDist2d(&l_VirtualPos);
                    float l_Angle = l_Pos.GetAngle(&l_VirtualPos);

                    /// Reporting the distance and the angle according to the current orientation
                    float l_NewX = l_X + l_Dist * cos(l_O - l_Angle + M_PI / 2.0f);
                    float l_NewY = l_Y + l_Dist * sin(l_O - l_Angle + M_PI / 2.0f);

                    /// Adding the point to the vector
                    G3D::Vector2 l_Point = G3D::Vector2(l_NewX, l_NewY);
                    l_CheckPoints.push_back(l_Point);
                }

                if (p_AreaTrigger->IsInAreaTriggerPolygon(l_CheckPoints, G3D::Vector2(l_Caster->m_positionX, l_Caster->m_positionY)))
                {
                    if (!l_Caster->HasAura(eSpells::SlagPoolEnergizer))
                        l_Caster->CastSpell(l_Caster, eSpells::SlagPoolEnergizer, true);
                }
                else
                {
                    if (l_Caster->HasAura(eSpells::SlagPoolEnergizer))
                        l_Caster->RemoveAura(eSpells::SlagPoolEnergizer);
                }

                for (Unit* l_Unit : l_TargetList)
                {
                    /// Target not in lightning lines
                    if (!p_AreaTrigger->IsInAreaTriggerPolygon(l_CheckPoints, G3D::Vector2(l_Unit->m_positionX, l_Unit->m_positionY)))
                    {
                        if (l_Unit->HasAura(eSpells::SlagPoolDoTAura))
                        {
                            l_Unit->RemoveAura(eSpells::SlagPoolDoTAura);
                            continue;
                        }
                    }
                    else
                    {
                        if (!l_Unit->HasAura(eSpells::SlagPoolDoTAura))
                            l_Caster->CastSpell(l_Unit, eSpells::SlagPoolDoTAura, true);
                    }
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                if (l_Caster->HasAura(eSpells::SlagPoolEnergizer))
                    l_Caster->RemoveAura(eSpells::SlagPoolEnergizer);

                std::list<Unit*> l_TargetList;
                float l_Radius = 50.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (l_Unit->HasAura(eSpells::SlagPoolDoTAura))
                        l_Unit->RemoveAura(eSpells::SlagPoolDoTAura);
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_slag_pool();
        }
};

/// Melt - 155224
class areatrigger_foundry_melt : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_melt() : AreaTriggerEntityScript("areatrigger_foundry_melt") { }

        enum eSpell
        {
            MeltDoT = 155223
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 15.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (l_Unit->GetDistance(p_AreaTrigger) <= 5.0f)
                    {
                        if (!l_Unit->HasAura(eSpell::MeltDoT))
                            l_Caster->CastSpell(l_Unit, eSpell::MeltDoT, true);
                    }
                    else if (!l_Unit->FindNearestAreaTrigger(p_AreaTrigger->GetSpellId(), 5.0f))
                    {
                        if (l_Unit->HasAura(eSpell::MeltDoT))
                            l_Unit->RemoveAura(eSpell::MeltDoT);
                    }
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 5.0f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (!l_Unit->FindNearestAreaTrigger(p_AreaTrigger->GetSpellId(), l_Radius))
                    {
                        if (l_Unit->HasAura(eSpell::MeltDoT))
                            l_Unit->RemoveAura(eSpell::MeltDoT);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_melt();
        }
};

/// Defense - 177772
class areatrigger_foundry_defense : public AreaTriggerEntityScript
{
    public:
        areatrigger_foundry_defense() : AreaTriggerEntityScript("areatrigger_foundry_defense") { }

        enum eSpell
        {
            DefenseAura = 160382
        };

        void OnUpdate(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 15.0f;

                JadeCore::AnyFriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyFriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (l_Unit->GetDistance(p_AreaTrigger) <= 3.5f)
                    {
                        if (!l_Unit->HasAura(eSpell::DefenseAura))
                            l_Caster->CastSpell(l_Unit, eSpell::DefenseAura, true);
                    }
                    else if (!l_Unit->FindNearestAreaTrigger(p_AreaTrigger->GetSpellId(), 3.5f))
                    {
                        if (l_Unit->HasAura(eSpell::DefenseAura))
                            l_Unit->RemoveAura(eSpell::DefenseAura);
                    }
                }
            }
        }

        void OnRemove(AreaTrigger* p_AreaTrigger, uint32 p_Time) override
        {
            if (Unit* l_Caster = p_AreaTrigger->GetCaster())
            {
                std::list<Unit*> l_TargetList;
                float l_Radius = 3.5f;

                JadeCore::AnyUnfriendlyUnitInObjectRangeCheck l_Check(p_AreaTrigger, l_Caster, l_Radius);
                JadeCore::UnitListSearcher<JadeCore::AnyUnfriendlyUnitInObjectRangeCheck> l_Searcher(p_AreaTrigger, l_TargetList, l_Check);
                p_AreaTrigger->VisitNearbyObject(l_Radius, l_Searcher);

                for (Unit* l_Unit : l_TargetList)
                {
                    if (!l_Unit->FindNearestAreaTrigger(p_AreaTrigger->GetSpellId(), l_Radius))
                    {
                        if (l_Unit->HasAura(eSpell::DefenseAura))
                            l_Unit->RemoveAura(eSpell::DefenseAura);
                    }
                }
            }
        }

        AreaTriggerEntityScript* GetAI() const override
        {
            return new areatrigger_foundry_defense();
        }
};

void AddSC_boss_blast_furnace()
{
    /// Bosses
    new boss_heart_of_the_mountain();
    new boss_foreman_feldspar();

    /// Creatures
    new npc_foundry_blackhand_cosmetic();
    new npc_foundry_primal_elementalist();
    new npc_foundry_bellows_operator();
    new npc_foundry_heat_regulator();
    new npc_foundry_security_guard();
    new npc_foundry_furnace_engineer();
    new npc_foundry_cluster_of_lit_bombs();
    new npc_foundry_slag_elemental();
    new npc_foundry_firecaller();

    /// Spells
    new spell_foundry_defense_aura();
    new spell_foundry_bomb_overrider();
    new spell_foundry_rupture_aura();
    new spell_foundry_hot_blooded_aura();
    new spell_foundry_damage_shield();
    new spell_foundry_shields_down();
    new spell_foundry_volatile_fire();
    new spell_foundry_melt_aura();

    /// GameObject
    new go_founrdy_crucible();

    /// AreaTriggers
    new areatrigger_foundry_rupture();
    new areatrigger_foundry_slag_pool();
    new areatrigger_foundry_melt();
    new areatrigger_foundry_defense();
}