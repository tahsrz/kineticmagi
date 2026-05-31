// Copyright (c) TahWorld. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "GaspMoverGaitComponent.generated.h"

// Forward declaration of Mover Component class
class UMoverComponent;
class ULBP_PhysicsSpringMode;

/** States of the Dash Slide movement sequence. */
UENUM(BlueprintType)
enum class EDashSlideState : uint8
{
	None,
	Sprinting,
	Dashing,
	Sliding
};

// Delegates for Dash-Slide notifications
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDashSlidePhaseChangedSignature, EDashSlideState, NewPhase);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashSlideEndedSignature);

/**
 * UGaspMoverGaitComponent
 * 
 * An Actor Component designed to map and register status effect strings 
 * to native GASP Mover component gameplay tags dynamically at runtime or via configuration.
 * Also acts as the high-level coordinator for the Dash Slide locomotion sequence.
 */
UCLASS(ClassGroup = (Movement), meta = (BlueprintSpawnableComponent))
class KINETICMAGI_API UGaspMoverGaitComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGaspMoverGaitComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	/**
	 * Registers a dynamic runtime mapping between a status effect string and a gait gameplay tag.
	 * 
	 * @param StatusEffect	The status effect identifier (e.g., "Sprint", "Slow", "Stumble").
	 * @param GaitTag		The native GASP Mover gait tag (e.g., Mover.Gait.Sprint, Mover.Gait.Slow).
	 */
	UFUNCTION(BlueprintCallable, Category = "GASP Mover | Gait Mapping")
	void RegisterGaitTagMapping(const FString& StatusEffect, const FGameplayTag& GaitTag);

	/**
	 * Removes a status effect registration mapping.
	 * 
	 * @param StatusEffect	The status effect identifier to unregister.
	 */
	UFUNCTION(BlueprintCallable, Category = "GASP Mover | Gait Mapping")
	void UnregisterGaitTagMapping(const FString& StatusEffect);

	/**
	 * Applies a registered status effect, adding its corresponding gait tag to the Mover component.
	 * 
	 * @param StatusEffect	The status effect string identifier.
	 * @return True if the status effect was successfully applied and mapped.
	 */
	UFUNCTION(BlueprintCallable, Category = "GASP Mover | Status Effects")
	bool ApplyStatusEffect(const FString& StatusEffect);

	/**
	 * Removes an active status effect, removing its corresponding gait tag from the Mover component.
	 * 
	 * @param StatusEffect	The status effect string identifier.
	 * @return True if the status effect was found and successfully removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "GASP Mover | Status Effects")
	bool RemoveStatusEffect(const FString& StatusEffect);

	/**
	 * Clears all active status effects and removes their corresponding tags from the Mover component.
	 */
	UFUNCTION(BlueprintCallable, Category = "GASP Mover | Status Effects")
	void ClearAllStatusEffects();

	/**
	 * Checks if a status effect is currently active on this component.
	 * 
	 * @param StatusEffect	The status effect string identifier.
	 */
	UFUNCTION(BlueprintPure, Category = "GASP Mover | Status Effects")
	bool IsStatusEffectActive(const FString& StatusEffect) const;

	/**
	 * Retrieves the active status effects and their mapped gait tags.
	 */
	UFUNCTION(BlueprintPure, Category = "GASP Mover | Status Effects")
	const TMap<FString, FGameplayTag>& GetActiveStatusEffects() const { return ActiveStatusEffects; }

	/** Registers the Chaos-driven LBP physics spring mode on the owner's Mover component. */
	UFUNCTION(BlueprintCallable, Category = "GASP Mover | Movement Modes")
	bool RegisterLBPPhysicsSpringMode();

	// ==========================================
	// DASH SLIDE MECHANIC API
	// ==========================================

	/** Current phase of the Dash Slide locomotion sequence. */
	UPROPERTY(BlueprintReadOnly, Category = "GASP Mover | Dash Slide")
	EDashSlideState CurrentDashSlideState;

	/** Triggers the Dash Slide sequence if prerequisites (sprinting duration) are met. */
	UFUNCTION(BlueprintCallable, Category = "GASP Mover | Dash Slide")
	bool TryDashSlide();

	/** Interrupts and cancels any active Dash Slide sequence immediately. */
	UFUNCTION(BlueprintCallable, Category = "GASP Mover | Dash Slide")
	void CancelDashSlide();

	/** Event fired when a new phase of the Dash Slide sequence starts (e.g., Sprint -> Dash or Dash -> Slide). */
	UPROPERTY(BlueprintAssignable, Category = "GASP Mover | Dash Slide")
	FDashSlidePhaseChangedSignature OnDashSlidePhaseChanged;

	/** Event fired when the Dash Slide sequence finishes or gets canceled. */
	UPROPERTY(BlueprintAssignable, Category = "GASP Mover | Dash Slide")
	FDashSlideEndedSignature OnDashSlideEnded;

protected:
	/** Default mappings defined in editor defaults between status effects and Mover gait tags. */
	UPROPERTY(EditDefaultsOnly, Category = "GASP Mover | Configuration", meta = (ForceMapLayout))
	TMap<FString, FGameplayTag> DefaultGaitMappings;

	// ==========================================
	// DASH SLIDE CONFIGURATION PROPERTIES
	// ==========================================

	/** Minimum time the character must be sprinting before a Dash Slide can be triggered. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Dash Slide Configuration", meta = (ClampMin = "0.0"))
	float RequiredSprintTime;

	/** Duration of the initial Dash impulse phase in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Dash Slide Configuration", meta = (ClampMin = "0.0"))
	float DashDuration;

	/** Duration of the follow-up Slide phase in seconds. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Dash Slide Configuration", meta = (ClampMin = "0.0"))
	float SlideDuration;

	/** Instant forward velocity boost applied to the Mover component at the start of the Dash. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Dash Slide Configuration", meta = (ClampMin = "0.0"))
	float DashVelocityBoost;

	/** Status effect string identifier corresponding to Sprinting. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Dash Slide Configuration")
	FString SprintStatusEffectString;

	/** Status effect string identifier corresponding to Dashing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Dash Slide Configuration")
	FString DashStatusEffectString;

	/** Status effect string identifier corresponding to Sliding. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Dash Slide Configuration")
	FString SlideStatusEffectString;

	/** Automatically register LBP_PhysicsSpringMode during BeginPlay. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Movement Modes")
	bool bAutoRegisterLBPPhysicsSpringMode;

	/** Movement mode class registered under the name LBP_PhysicsSpringMode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GASP Mover | Movement Modes")
	TSubclassOf<ULBP_PhysicsSpringMode> LBPPhysicsSpringModeClass;

private:
	/** Cached reference to the owner's Mover Component. */
	UPROPERTY()
	TObjectPtr<UMoverComponent> CachedMoverComponent;

	/** Currently registered status effect mappings. */
	TMap<FString, FGameplayTag> RegisteredGaitMappings;

	/** Currently active status effects on the actor and their applied gait tags. */
	TMap<FString, FGameplayTag> ActiveStatusEffects;

	/** Timestamp when the character began sprinting. */
	float SprintStartTimestamp;

	/** Timer handle for transitioning from Dash to Slide. */
	FTimerHandle DashTimerHandle;

	/** Timer handle for ending the Slide phase. */
	FTimerHandle SlideTimerHandle;

	/** Helper to resolve and cache the owner's Mover Component if not already done. */
	UMoverComponent* GetMoverComponent();

	/** Triggered when the Dash timer expires to initiate the Slide phase. */
	void OnDashCompleted();

	/** Triggered when the Slide timer expires to return to default locomotion. */
	void OnSlideCompleted();
};

