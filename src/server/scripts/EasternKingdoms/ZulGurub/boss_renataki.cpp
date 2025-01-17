/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Boss_Renataki
SD%Complete: 100
SDComment:
SDCategory: Zul'Gurub
EndScriptData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "zulgurub.h"

enum Spells
{
    SPELL_AMBUSH                = 34794,
    SPELL_THOUSANDBLADES        = 34799
};

enum Misc
{
    EQUIP_ID_MAIN_HAND          = 0  //was item display id 31818, but this id does not exist
};

class boss_renataki : public CreatureScript
{
public:
    boss_renataki() : CreatureScript("boss_renataki") { }

    struct boss_renatakiAI : public BossAI
    {
        boss_renatakiAI(Creature* creature) : BossAI(creature, DATA_EDGE_OF_MADNESS) { }

        uint32 Invisible_Timer;
        uint32 Ambush_Timer;
        uint32 Visible_Timer;
        uint32 Aggro_Timer;
        uint32 ThousandBlades_Timer;

        bool Invisible;
        bool Ambushed;

        void Reset() override
        {
            _Reset();
            Invisible_Timer = urand(8000, 18000);
            Ambush_Timer = 3000;
            Visible_Timer = 4000;
            Aggro_Timer = urand(15000, 25000);
            ThousandBlades_Timer = urand(4000, 8000);

            Invisible = false;
            Ambushed = false;
        }

        void JustDied(Unit* /*killer*/) override
        {
            _JustDied();
        }

        void EnterCombat(Unit* /*who*/) override
        {
            _EnterCombat();
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            //Invisible_Timer
            if (Invisible_Timer <= diff)
            {
                me->InterruptSpell(CURRENT_GENERIC_SPELL);

                SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
                me->SetDisplayId(11686);

                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                Invisible = true;

                Invisible_Timer = urand(15000, 30000);
            }
            else Invisible_Timer -= diff;

            if (Invisible)
            {
                if (Ambush_Timer <= diff)
                {
                    Unit* target = SelectTarget(SelectTargetMethod::Random, 0);
                    if (target)
                    {
                        me->NearTeleportTo(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), me->GetOrientation());
                        DoCast(target, SPELL_AMBUSH);
                    }

                    Ambushed = true;
                    Ambush_Timer = 3000;
                }
                else Ambush_Timer -= diff;
            }

            if (Ambushed)
            {
                if (Visible_Timer <= diff)
                {
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);

                    me->SetDisplayId(15268);
                    SetEquipmentSlots(false, EQUIP_ID_MAIN_HAND, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);

                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    Invisible = false;

                    Visible_Timer = 4000;
                }
                else Visible_Timer -= diff;
            }

            //Resetting some aggro so he attacks other gamers
            if (!Invisible)
            {
                if (Aggro_Timer <= diff)
                {
                    Unit* target = SelectTarget(SelectTargetMethod::Random, 1);

                    if (DoGetThreat(me->GetVictim()))
                    {
                        DoModifyThreatPercent(me->GetVictim(), -50);
                    }

                    if (target)
                    {
                        AttackStart(target);
                    }

                    Aggro_Timer = urand(7000, 20000);
                }
                else Aggro_Timer -= diff;

                if (ThousandBlades_Timer <= diff)
                {
                    DoCastVictim(SPELL_THOUSANDBLADES);
                    ThousandBlades_Timer = urand(7000, 12000);
                }
                else ThousandBlades_Timer -= diff;
            }

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return GetZulGurubAI<boss_renatakiAI>(creature);
    }
};

void AddSC_boss_renataki()
{
    new boss_renataki();
}
