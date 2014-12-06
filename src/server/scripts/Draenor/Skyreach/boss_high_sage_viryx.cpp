#include "instance_skyreach.h"
#include "Vehicle.h"

namespace MS
{
    namespace InstanceSkyreach
    {
        class mob_SkyreachShieldConstruct : public CreatureScript
        {
        public:
            // Entry: 76292
            mob_SkyreachShieldConstruct()
                : CreatureScript("mob_SkyreachShieldConstruct")
            {
            }

            enum class Spells : uint32
            {
                Shielding = 158641,
            };

            enum class Events : uint32
            {
                Shielding = 1,
            };

            CreatureAI* GetAI(Creature* creature) const
            {
                return new mob_SkyreachShieldConstructAI(creature);
            }

            struct mob_SkyreachShieldConstructAI : public ScriptedAI
            {
                mob_SkyreachShieldConstructAI(Creature* creature) : ScriptedAI(creature),
                m_Instance(creature->GetInstanceScript()),
                m_events()
                {
                }

                void Reset()
                {
                    m_events.Reset();
                }

                void EnterCombat(Unit* who)
                {
                    m_events.ScheduleEvent(uint32(Events::Shielding), urand(1000, 2000));
                }

                void UpdateAI(const uint32 diff)
                {
                    if (!UpdateVictim())
                        return;

                    m_events.Update(diff);

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    switch (m_events.ExecuteEvent())
                    {
                    case uint32(Events::Shielding):
                        m_events.ScheduleEvent(uint32(Events::Shielding), 5000);
                        me->CastSpell(me, uint32(Spells::Shielding));
                        break;
                    default:
                        break;
                    }

                    DoMeleeAttackIfReady();
                }

                InstanceScript* m_Instance;
                EventMap m_events;
            };
        };

        static const Position k_FallPoints[] =
        {
            { 1075.627f, 1721.043f, 263.72f },
            { 1085.533f, 1720.696f, 263.72f },
            { 1094.993f, 1721.900f, 263.72f },
            { 1104.928f, 1724.705f, 263.72f },
            { 1141.754f, 1729.700f, 263.72f },
            { 1123.270f, 1735.448f, 263.72f },
            { 1132.028f, 1744.517f, 263.72f },
            { 1138.780f, 1753.942f, 263.72f },
            { 1144.525f, 1764.297f, 263.72f },
            { 1147.478f, 1773.486f, 263.72f },
            { 1149.241f, 1785.068f, 263.72f },
            { 1148.452f, 1798.171f, 263.72f },
            { 1146.030f, 1811.055f, 263.72f },
            { 1140.494f, 1822.770f, 263.72f }
        };

        class mob_SolarZealot : public CreatureScript
        {
        public:
            // Entry: 76267
            mob_SolarZealot()
                : CreatureScript("mob_SolarZealot")
            {
            }

            enum class Spells : uint32
            {
                JumpChargeDemonCreatorRideMe = 165834,
                ForceDemonCreatorToRideMe = 136522,
            };

            CreatureAI* GetAI(Creature* creature) const
            {
                return new mob_SolarZealotAI(creature);
            }

            struct mob_SolarZealotAI : public ScriptedAI
            {
                mob_SolarZealotAI(Creature* creature) : ScriptedAI(creature),
                m_Reset(false)
                {
                    me->SetSpeed(UnitMoveType::MOVE_WALK, 1.f);
                    me->SetSpeed(UnitMoveType::MOVE_RUN, 1.f);
                    me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 1.f);
                }

                void Reset()
                {
                    events.Reset();
                    m_Reset = true;
                }

                void EnterCombat(Unit* who)
                {
                }

                void MovementInform(uint32 p_TypeId, uint32 p_PointId)
                {
                    if (p_TypeId == POINT_MOTION_TYPE && p_PointId == 0)
                    {
                        if (me->GetVehicle())
                            me->GetVehicle()->RemoveAllPassengers();
                        me->DespawnOrUnsummon();
                    }
                }

                void UpdateAI(const uint32 diff)
                {
                    events.Update(diff);

                    if (m_Reset)
                    {
                        m_Reset = false;
                        me->CastSpell(me->GetCharmerOrOwner(), uint32(Spells::JumpChargeDemonCreatorRideMe), true);
                        if (me->ToTempSummon() && me->ToTempSummon()->GetSummoner())
                            me->ToTempSummon()->GetSummoner()->EnterVehicle(me, 0);

                        int l_ClosestPoint = 0;
                        float l_ClosestDistance = k_FallPoints[0].GetExactDist2d(me);
                        for (int i = 1; i < 14; i++)
                        {
                            if (k_FallPoints[i].GetExactDist2d(me) < l_ClosestDistance)
                            {
                                l_ClosestDistance = k_FallPoints[i].GetExactDist2d(me);
                                l_ClosestPoint = i;
                            }
                        }

                        me->GetMotionMaster()->MovePoint(0, k_FallPoints[l_ClosestPoint]);
                    }
                }

                bool m_Reset;
            };
        };

        class mob_ArakkoaMagnifyingGlassFocus : public CreatureScript
        {
        public:
            // Entry: 76083
            mob_ArakkoaMagnifyingGlassFocus()
                : CreatureScript("mob_ArakkoaMagnifyingGlassFocus")
            {
            }

            enum class Spells : uint32
            {
                LensFlare = 154044,
            };

            enum class Events : uint32
            {
                LensFlare = 1,
            };

            CreatureAI* GetAI(Creature* creature) const
            {
                return new mob_ArakkoaMagnifyingGlassFocusI(creature);
            }

            struct mob_ArakkoaMagnifyingGlassFocusI : public ScriptedAI
            {
                mob_ArakkoaMagnifyingGlassFocusI(Creature* creature) : ScriptedAI(creature)
                {
                    me->SetSpeed(UnitMoveType::MOVE_WALK, 0.5f);
                    me->SetSpeed(UnitMoveType::MOVE_RUN, 0.5f);
                    me->SetSpeed(UnitMoveType::MOVE_FLIGHT, 0.5f);
                }

                void Reset()
                {
                    events.Reset();
                    events.ScheduleEvent(uint32(Events::LensFlare), 500);

                    if (me->GetInstanceScript())
                        me->GetInstanceScript()->SetData(Data::StartingLensFlare, 0);
                }

                void UpdateAI(const uint32 diff)
                {
                    events.Update(diff);

                    if (events.ExecuteEvent() == uint32(Events::LensFlare))
                    {
                        if (me->ToTempSummon())
                        {
                            if (me->ToTempSummon()->GetSummoner())
                            {
                                Position l_Pos;
                                me->ToTempSummon()->GetSummoner()->GetPosition(&l_Pos);
                                l_Pos.m_positionZ -= 1.5f;
                                me->GetMotionMaster()->MovePoint(0, l_Pos);
                            }
                        }
                        events.ScheduleEvent(uint32(Events::LensFlare), 500);
                    }
                }

                bool m_Reset;
            };
        };

        class boss_HighSageViryx : public CreatureScript
        {
        public:
            // Entry: 76266
            boss_HighSageViryx()
                : CreatureScript("boss_HighSageViryx")
            {
            }

            enum class Spells : uint32
            {
                CastDown = 156789,
                CastDown_Summon = 153955,
                CastDown_Visual = 165845,
                CastDown_2 = 153954,
                LensFlare = 154029,
                LensFlare_At = 154044,
                LensFlare_Dmg = 154043,
                SolarBurst = 154396,
                CallAdds = 154049,
            };

            enum class Texts : int32
            {
                COMBAT_START = -1603202,
                JUST_DIED = -1603203,
                FOUR_WINDS_1 = -1603204,
                FOUR_WINDS_2 = -1603205,
                KILL_PLAYER_1 = -1603206,
                KILL_PLAYER_2 = -1603207,
                VICTORY = -1603208
            };

            enum class Events : uint32
            {
                LensFlare = 1,
                CastDown = 2,
                SolarBurst = 3,
                CallAdds = 4,
            };

            CreatureAI* GetAI(Creature* creature) const
            {
                return new boss_HighSageViryxAI(creature);
            }

            struct boss_HighSageViryxAI : public BossAI
            {
                boss_HighSageViryxAI(Creature* creature) : BossAI(creature, Data::HighSageViryx)
                {
                    if (instance)
                        instance->SetBossState(Data::HighSageViryx, TO_BE_DECIDED);
                }

                void Reset()
                {
                    _Reset();
                }

                void JustReachedHome()
                {
                    _JustReachedHome();

                    if (instance)
                    {
                        instance->SetBossState(Data::HighSageViryx, FAIL);
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                    }
                }

                void JustDied(Unit* /*killer*/)
                {
                    _JustDied();

                    me->CastSpell(me, InstanceSkyreach::RandomSpells::DespawnAreaTriggers, true);

                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, me);
                }

                void KilledUnit(Unit* /*victim*/)
                {
                }

                void EnterCombat(Unit* who)
                {
                    _EnterCombat();

                    events.ScheduleEvent(uint32(Events::SolarBurst), 5000);
                    events.ScheduleEvent(uint32(Events::CastDown), 11000);
                    events.ScheduleEvent(uint32(Events::LensFlare), 27000);
                    events.ScheduleEvent(uint32(Events::CallAdds), 29000);

                    if (instance)
                        instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me);
                }

                void UpdateAI(const uint32 diff)
                {
                    if (!UpdateVictim())
                        return;

                    events.Update(diff);

                    if (me->HasUnitState(UNIT_STATE_CASTING))
                        return;

                    switch (events.ExecuteEvent())
                    {
                    case uint32(Events::CastDown):
                        events.ScheduleEvent(uint32(Events::CastDown), 35000);
                        if (Player* l_Plr = InstanceSkyreach::SelectRandomPlayerExcludedTank(me, 80.0f))
                            me->CastSpell(l_Plr, uint32(Spells::CastDown));
                        break;
                    case uint32(Events::LensFlare):
                        if (Player* l_Plr = InstanceSkyreach::SelectRandomPlayerIncludedTank(me, 80.0f))
                            l_Plr->CastSpell(l_Plr, uint32(Spells::LensFlare), true);
                        events.ScheduleEvent(uint32(Events::LensFlare), 27000);
                        break;
                    case uint32(Events::SolarBurst):
                        me->CastSpell(me->getVictim(), uint32(Spells::SolarBurst));
                        events.ScheduleEvent(uint32(Events::SolarBurst), 21000);
                        break;
                    case uint32(Events::CallAdds):
                        me->CastSpell(me, uint32(Spells::CallAdds), false);
                        events.ScheduleEvent(uint32(Events::CallAdds), 50000);
                        break;
                    default:
                        break;
                    }

                    DoMeleeAttackIfReady();
                }
            };
        };
    }
}

void AddSC_boss_HighSaveViryx()
{
    new MS::InstanceSkyreach::boss_HighSageViryx();
    new MS::InstanceSkyreach::mob_SolarZealot();
    new MS::InstanceSkyreach::mob_ArakkoaMagnifyingGlassFocus();
    new MS::InstanceSkyreach::mob_SkyreachShieldConstruct();
}