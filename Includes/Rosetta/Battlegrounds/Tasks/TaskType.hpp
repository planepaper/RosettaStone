// Copyright (c) 2019 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef ROSETTASTONE_BATTLEGROUNDS_TASK_TYPE_HPP
#define ROSETTASTONE_BATTLEGROUNDS_TASK_TYPE_HPP

#include <Rosetta/Battlegrounds/Tasks/SimpleTasks/AddEnchantmentTask.hpp>
#include <Rosetta/Battlegrounds/Tasks/SimpleTasks/GetGameTagTask.hpp>
#include <Rosetta/Battlegrounds/Tasks/SimpleTasks/RandomTask.hpp>
#include <Rosetta/Battlegrounds/Tasks/SimpleTasks/SummonTask.hpp>

#include <variant>

namespace RosettaStone::Battlegrounds
{
using namespace SimpleTasks;
using TaskType =
    std::variant<AddEnchantmentTask, GetGameTagTask, RandomTask, SummonTask>;
}  // namespace RosettaStone::Battlegrounds

#endif  // ROSETTASTONE_BATTLEGROUNDS_TASK_TYPE_HPP
