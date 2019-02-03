// This code is based on Sabberstone project.
// Copyright (c) 2017-2018 SabberStone Team, darkfriend77 & rnilva
// Hearthstone++ is hearthstone simulator using C++ with reinforcement learning.
// Copyright (c) 2018 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

#include <hspp/Policy/Policy.hpp>
#include <hspp/Tasks/PlayerTasks/PlayMinionTask.hpp>
#include <hspp/Tasks/SimpleTasks/ModifyManaTask.hpp>

#include <algorithm>

using namespace Hearthstonepp::SimpleTasks;

namespace Hearthstonepp::PlayerTasks
{
PlayMinionTask::PlayMinionTask(Entity* source, int fieldPos, Entity* target)
    : ITask(source, target), m_fieldPos(fieldPos)
{
    // Do nothing
}

TaskID PlayMinionTask::GetTaskID() const
{
    return TaskID::PLAY_MINION;
}

TaskStatus PlayMinionTask::Impl(Player& player)
{
    BYTE position;

    if (m_fieldPos == -1)
    {
        const auto fieldIter = std::find(player.GetField().begin(),
                                         player.GetField().end(), nullptr);
        position = static_cast<BYTE>(
            std::distance(player.GetField().begin(), fieldIter));
    }
    else
    {
        TaskMeta req = player.GetPolicy().Require(player, TaskID::PLAY_MINION);
        if (!req.HasObjects())
        {
            return TaskStatus::PLAY_MINION_FLATBUFFER_NULLPTR;
        }
        position = req.GetObject<BYTE>();
    }

    // Verify field position
    if (position > player.GetField().size())
    {
        return TaskStatus::PLAY_MINION_POSITION_OUT_OF_RANGE;
    }

    // Verify character casting
    const auto character = dynamic_cast<Character*>(m_source);
    if (character == nullptr)
    {
        return TaskStatus::PLAY_MINION_CANNOT_CONVERT_ENTITY;
    }

    // Summon minion
    if (player.GetField().empty())
    {
        player.GetField().emplace_back(character);
    }
    else
    {
        player.GetField().insert(player.GetField().begin() + position,
                                 character);
    }

    // Apply card mechanics tags
    for (const auto tags : m_source->card->mechanics)
    {
        m_source->SetGameTag(tags, 1);
    }

    const auto cost = static_cast<BYTE>(m_source->card->cost);
    const TaskStatus modified =
        ModifyManaTask(ManaOperator::SUB, ManaType::AVAILABLE, cost)
            .Run(player);

    // Process PowerTasks
    if (m_source->card->power != nullptr)
    {
        for (auto& power : m_source->card->power->GetPowerTask())
        {
            power->Run(player);
        }
    }

    if (modified == TaskStatus::MODIFY_MANA_SUCCESS)
    {
        return TaskStatus::PLAY_MINION_SUCCESS;
    }

    return TaskStatus::PLAY_MINION_MODIFY_MANA_FAIL;
}
}  // namespace Hearthstonepp::PlayerTasks