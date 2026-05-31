// Copyright (c) TahWorld. All rights reserved.

#include "GaspMoverGaitComponent.h"
#include "LBP_PhysicsSpringMode.h"
#include "MoverComponent.h"
#include "MovementMode.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DefaultMovementSet/InstantMovementEffects/BasicInstantMovementEffects.h"

UGaspMoverGaitComponent::UGaspMoverGaitComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// Set Dash Slide mechanics configuration defaults
	RequiredSprintTime = 0.5f;
	DashDuration = 0.25f;
	SlideDuration = 1.0f;
	DashVelocityBoost = 1200.0f;

	SprintStatusEffectString = TEXT("Sprint");
	DashStatusEffectString = TEXT("Dash");
	SlideStatusEffectString = TEXT("Slide");

	bAutoRegisterLBPPhysicsSpringMode = true;
	LBPPhysicsSpringModeClass = ULBP_PhysicsSpringMode::StaticClass();

	CurrentDashSlideState = EDashSlideState::None;
	SprintStartTimestamp = 0.0f;
}

void UGaspMoverGaitComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize runtime mappings with predefined editor defaults
	RegisteredGaitMappings = DefaultGaitMappings;

	// Eagerly cache the owner's Mover Component
	GetMoverComponent();

	if (bAutoRegisterLBPPhysicsSpringMode)
	{
		RegisterLBPPhysicsSpringMode();
	}
}

void UGaspMoverGaitComponent::RegisterGaitTagMapping(const FString& StatusEffect, const FGameplayTag& GaitTag)
{
	if (StatusEffect.IsEmpty() || !GaitTag.IsValid())
	{
		return;
	}

	RegisteredGaitMappings.Add(StatusEffect, GaitTag);
}

void UGaspMoverGaitComponent::UnregisterGaitTagMapping(const FString& StatusEffect)
{
	if (IsStatusEffectActive(StatusEffect))
	{
		RemoveStatusEffect(StatusEffect);
	}

	RegisteredGaitMappings.Remove(StatusEffect);
}

bool UGaspMoverGaitComponent::ApplyStatusEffect(const FString& StatusEffect)
{
	if (IsStatusEffectActive(StatusEffect))
	{
		return false; // Already applied
	}

	const FGameplayTag* MappedTag = RegisteredGaitMappings.Find(StatusEffect);
	if (!MappedTag || !MappedTag->IsValid())
	{
		return false; // No valid mapping registered for this status effect
	}

	UMoverComponent* MoverComp = GetMoverComponent();
	if (!MoverComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: Failed to apply status effect '%s'. Owner does not have a UMoverComponent."), 
			*GetName(), *StatusEffect);
		return false;
	}

	// Add tag directly to the native Mover component
	MoverComp->AddGameplayTag(*MappedTag);
	ActiveStatusEffects.Add(StatusEffect, *MappedTag);

	// If this is our Sprint status effect, track when sprinting started
	if (StatusEffect.Equals(SprintStatusEffectString, ESearchCase::IgnoreCase))
	{
		if (UWorld* World = GetWorld())
		{
			SprintStartTimestamp = World->GetTimeSeconds();
		}
	}

	return true;
}

bool UGaspMoverGaitComponent::RemoveStatusEffect(const FString& StatusEffect)
{
	const FGameplayTag* ActiveTag = ActiveStatusEffects.Find(StatusEffect);
	if (!ActiveTag)
	{
		return false; // Not currently active
	}

	UMoverComponent* MoverComp = GetMoverComponent();
	if (MoverComp)
	{
		// Remove tag directly from the native Mover component
		MoverComp->RemoveGameplayTag(*ActiveTag);
	}

	ActiveStatusEffects.Remove(StatusEffect);

	// If this is our Sprint status effect, clear the start timestamp
	if (StatusEffect.Equals(SprintStatusEffectString, ESearchCase::IgnoreCase))
	{
		SprintStartTimestamp = 0.0f;
	}

	return true;
}

void UGaspMoverGaitComponent::ClearAllStatusEffects()
{
	// If in a Dash Slide sequence, cancel it first to clean up states and timers safely
	if (CurrentDashSlideState != EDashSlideState::None)
	{
		CancelDashSlide();
	}

	UMoverComponent* MoverComp = GetMoverComponent();
	if (MoverComp)
	{
		for (const TPair<FString, FGameplayTag>& Pair : ActiveStatusEffects)
		{
			MoverComp->RemoveGameplayTag(Pair.Value);
		}
	}

	ActiveStatusEffects.Empty();
	SprintStartTimestamp = 0.0f;
}

bool UGaspMoverGaitComponent::IsStatusEffectActive(const FString& StatusEffect) const
{
	return ActiveStatusEffects.Contains(StatusEffect);
}
bool UGaspMoverGaitComponent::RegisterLBPPhysicsSpringMode()
{
	UMoverComponent* MoverComp = GetMoverComponent();
	if (!MoverComp || !LBPPhysicsSpringModeClass)
	{
		return false;
	}

	if (MoverComp->FindMode_Mutable(ULBP_PhysicsSpringMode::StaticClass(), ULBP_PhysicsSpringMode::ModeName, false))
	{
		return true;
	}

	UBaseMovementMode* RegisteredMode = MoverComp->AddMovementModeFromClass(
		ULBP_PhysicsSpringMode::ModeName,
		LBPPhysicsSpringModeClass);

	return RegisteredMode != nullptr;
}

// ==========================================
// DASH SLIDE LOGIC IMPLEMENTATION
// ==========================================

bool UGaspMoverGaitComponent::TryDashSlide()
{
	if (CurrentDashSlideState != EDashSlideState::None)
	{
		return false; // Already performing a Dash Slide sequence
	}

	// 1. Prerequisites Check: Must be actively sprinting
	if (!IsStatusEffectActive(SprintStatusEffectString))
	{
		return false;
	}

	// 2. Sprint Duration Check: Must have been sprinting for at least the required duration
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float SprintDuration = World->GetTimeSeconds() - SprintStartTimestamp;
	if (SprintDuration < RequiredSprintTime)
	{
		return false;
	}

	UMoverComponent* MoverComp = GetMoverComponent();
	if (!MoverComp)
	{
		return false;
	}

	// 3. Initiate Dash State
	CurrentDashSlideState = EDashSlideState::Dashing;

	// Transition status effects
	RemoveStatusEffect(SprintStatusEffectString);
	ApplyStatusEffect(DashStatusEffectString);

	// 4. Calculate and apply Dash forward velocity boost
	if (AActor* Owner = GetOwner())
	{
		const FVector DashDirection = Owner->GetActorForwardVector();
		
		FApplyVelocityEffect DashVelocityEffect;
		DashVelocityEffect.VelocityToApply = DashDirection * DashVelocityBoost;
		DashVelocityEffect.bAdditiveVelocity = false; // Override velocity for a sudden, aggressive dash burst
		DashVelocityEffect.ForceMovementMode = TEXT("Walking"); // Ensure Mover is in ground simulation mode

		MoverComp->QueueInstantMovementEffect(MakeShared<FApplyVelocityEffect>(DashVelocityEffect));
	}

	// Notify observers
	OnDashSlidePhaseChanged.Broadcast(CurrentDashSlideState);

	// Set timer to transition into the Slide phase
	World->GetTimerManager().SetTimer(DashTimerHandle, this, &UGaspMoverGaitComponent::OnDashCompleted, DashDuration, false);

	return true;
}

void UGaspMoverGaitComponent::CancelDashSlide()
{
	if (CurrentDashSlideState == EDashSlideState::None)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(DashTimerHandle);
		World->GetTimerManager().ClearTimer(SlideTimerHandle);
	}

	// Safely remove any active dash/slide statuses
	if (CurrentDashSlideState == EDashSlideState::Dashing)
	{
		RemoveStatusEffect(DashStatusEffectString);
	}
	else if (CurrentDashSlideState == EDashSlideState::Sliding)
	{
		RemoveStatusEffect(SlideStatusEffectString);
	}

	CurrentDashSlideState = EDashSlideState::None;

	// Broadcast sequence end
	OnDashSlideEnded.Broadcast();
}

void UGaspMoverGaitComponent::OnDashCompleted()
{
	if (CurrentDashSlideState != EDashSlideState::Dashing)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		CancelDashSlide();
		return;
	}

	// Transition to Slide State
	CurrentDashSlideState = EDashSlideState::Sliding;

	// Transition status effects
	RemoveStatusEffect(DashStatusEffectString);
	ApplyStatusEffect(SlideStatusEffectString);

	// Notify observers of Slide phase entry
	OnDashSlidePhaseChanged.Broadcast(CurrentDashSlideState);

	// Set timer to finish the slide phase and return to normal locomotion
	World->GetTimerManager().SetTimer(SlideTimerHandle, this, &UGaspMoverGaitComponent::OnSlideCompleted, SlideDuration, false);
}

void UGaspMoverGaitComponent::OnSlideCompleted()
{
	if (CurrentDashSlideState != EDashSlideState::Sliding)
	{
		return;
	}

	// Exit Slide phase
	RemoveStatusEffect(SlideStatusEffectString);

	CurrentDashSlideState = EDashSlideState::None;

	// Broadcast complete sequence end
	OnDashSlideEnded.Broadcast();
}

UMoverComponent* UGaspMoverGaitComponent::GetMoverComponent()
{
	if (!CachedMoverComponent)
	{
		if (AActor* Owner = GetOwner())
		{
			CachedMoverComponent = Owner->FindComponentByClass<UMoverComponent>();
		}
	}

	return CachedMoverComponent;
}

