// This code is based on Sabberstone project.
// Copyright (c) 2017-2019 SabberStone Team, darkfriend77 & rnilva
// RosettaStone is hearthstone simulator using C++ with reinforcement learning.
// Copyright (c) 2019 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

#include <Rosetta/Actions/Attack.hpp>
#include <Rosetta/Games/Game.hpp>
#include <Rosetta/Models/Player.hpp>

namespace RosettaStone::Generic
{
void Attack(Player& player, Character* source, Character* target)
{
    // Check source can attack and target is valid
    if (!source->CanAttack() ||
        !source->IsValidCombatTarget(*player.opponent, target))
    {
        return;
    }

    // Process attack trigger
    player.GetGame()->taskQueue.StartEvent();
    player.GetGame()->triggerManager.OnAttackTrigger(&player, source);
    player.GetGame()->ProcessTasks();
    player.GetGame()->taskQueue.EndEvent();

    // Validate target trigger
    Trigger::ValidateTriggers(player.GetGame(), source, SequenceType::TARGET);

    // Process target trigger
    player.GetGame()->taskQueue.StartEvent();
    player.GetGame()->triggerManager.OnTargetTrigger(&player, source);
    player.GetGame()->ProcessTasks();
    player.GetGame()->taskQueue.EndEvent();

    // Set game step to MAIN_COMBAT
    player.GetGame()->step = Step::MAIN_COMBAT;

    // Get attack of source and target
    const int targetAttack = target->GetAttack();
    const int sourceAttack = source->GetAttack();

    // Take damage to target
    target->TakeDamage(*source, sourceAttack);

    // Ignore damage from defenders with 0 attack
    if (targetAttack > 0)
    {
        // Take damage to source
        source->TakeDamage(*target, targetAttack);
    }

    // Remove stealth ability if attacker has it
    if (source->GetGameTag(GameTag::STEALTH) == 1)
    {
        source->SetGameTag(GameTag::STEALTH, 0);
    }

    // Remove durability from weapon if hero attack
    Hero* hero = dynamic_cast<Hero*>(source);
    if (hero != nullptr && hero->weapon != nullptr &&
        hero->weapon->GetGameTag(GameTag::IMMUNE) == 0)
    {
        int prevValue = hero->weapon->GetDurability();
        hero->weapon->SetDurability(prevValue - 1);

        // Destroy weapon if durability is 0
        if (hero->weapon->GetDurability() == 0)
        {
            hero->RemoveWeapon();
        }
    }

    // Increase the number of attacked
    source->SetNumAttacksThisTurn(source->GetNumAttacksThisTurn() + 1);

    // Check source is exhausted
    if ((source->GetNumAttacksThisTurn() >= 1 &&
         source->GetGameTag(GameTag::WINDFURY) == 0) ||
        (source->GetNumAttacksThisTurn() >= 2 &&
         source->GetGameTag(GameTag::WINDFURY) == 1))
    {
        source->SetExhausted(true);
    }

    // Process after attack trigger
    player.GetGame()->taskQueue.StartEvent();
    if (source->afterAttackTrigger != nullptr)
    {
        source->afterAttackTrigger(&player, source);
    }
    player.GetGame()->ProcessTasks();
    player.GetGame()->taskQueue.EndEvent();

    // Process destroy and update aura
    player.GetGame()->ProcessDestroyAndUpdateAura();

    // Set game step to MAIN_ACTION
    player.GetGame()->step = Step::MAIN_ACTION;
}
}  // namespace RosettaStone::Generic
