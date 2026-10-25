#pragma once
#include "Module/ModModule.h"

namespace TsabsPassenger
{
// Shared production decisions, not a simulated RecipeManager.
inline bool ShouldRegisterPassengerRecipe(ELifecyclePhase Phase, bool bValidWorld, bool bValidOwner, bool bAlreadyAttempted)
{
    return Phase == ELifecyclePhase::INITIALIZATION && bValidWorld && bValidOwner && !bAlreadyAttempted;
}
inline bool ShouldGrantPassengerRecipe(ELifecyclePhase Phase, bool bValidWorld, bool bClient, bool bManagerAuthority, bool bRegistered, bool bAlreadyAttempted)
{
    return Phase == ELifecyclePhase::POST_INITIALIZATION && bValidWorld && !bClient && bManagerAuthority && bRegistered && !bAlreadyAttempted;
}
}
