// Copyright (c) TahWorld. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HAL/Runnable.h"
#include "Containers/Queue.h"
#include "HAL/ThreadSafeBool.h"
#include "GameplayTagContainer.h"
#include "GaspNamedPipeReceiverComponent.generated.h"

class FRunnableThread;
class UGaspMoverGaitComponent;
class UMoverComponent;

/**
 * FTahStatePacket
 * 4-byte binary state packet received from the C# backend to update native GASP states.
 */
USTRUCT(BlueprintType)
struct FTahStatePacket
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "GASP Mover | State Packet")
	uint8 MovementMode; // Byte 0x00

	UPROPERTY(BlueprintReadOnly, Category = "GASP Mover | State Packet")
	uint8 GaitProfile;  // Byte 0x01

	UPROPERTY(BlueprintReadOnly, Category = "GASP Mover | State Packet")
	uint8 UpperOverlay; // Byte 0x02

	UPROPERTY(BlueprintReadOnly, Category = "GASP Mover | State Packet")
	uint8 Modifier;     // Byte 0x03

	FTahStatePacket()
		: MovementMode(0)
		, GaitProfile(0)
		, UpperOverlay(0)
		, Modifier(0)
	{}

	FTahStatePacket(uint8 InMovementMode, uint8 InGaitProfile, uint8 InUpperOverlay, uint8 InModifier)
		: MovementMode(InMovementMode)
		, GaitProfile(InGaitProfile)
		, UpperOverlay(InUpperOverlay)
		, Modifier(InModifier)
	{}
};

/**
 * FNamedPipeReaderWorker
 * Background thread worker responsible for reading 4-byte state packets from a Windows Named Pipe.
 */
class FNamedPipeReaderWorker : public FRunnable
{
public:
	FNamedPipeReaderWorker(const FString& InPipeName, TQueue<FTahStatePacket, EQueueMode::Mpsc>& InTargetQueue);
	virtual ~FNamedPipeReaderWorker() override;

	// FRunnable Interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	FString PipeName;
	TQueue<FTahStatePacket, EQueueMode::Mpsc>& TargetQueue;
	FThreadSafeBool bKeepRunning;
	
#if PLATFORM_WINDOWS
	void* PipeHandle; // Handle stored as void* to prevent exposing Windows.h types
#endif
};

/**
 * UGaspNamedPipeReceiverComponent
 * 
 * Modular Actor Component that creates a Named Pipe to receive 4-byte binary packets from the C# backend.
 * Decodes the packet values and maps them directly to native GASP/Mover gameplay tags.
 */
UCLASS(ClassGroup = (Movement), meta = (BlueprintSpawnableComponent))
class KINETICMAGI_API UGaspNamedPipeReceiverComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGaspNamedPipeReceiverComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Decodes any queued 4-byte binary packets and executes tag updates on the UMoverComponent
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Name of the Named Pipe to create (e.g. "TahWorldGaspPipe"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GASP Mover | Pipe Configuration")
	FString PipeName;

	/**
	 * Converts raw TAH packet bytes into the GameplayTagContainer labels consumed by Chooser Tables.
	 * Can be called from UMoverMode custom simulation logic before visual selection runs.
	 */
	void InjectStateIntoTickData(FGameplayTagContainer& Tags, const struct FTahStatePacket& Packet);

private:
	/** Background thread worker instance. */
	TUniquePtr<FNamedPipeReaderWorker> PipeWorker;

	/** Thread handle. */
	FRunnableThread* PipeThread;

	/** Thread-safe queue containing received binary state packets. */
	TQueue<FTahStatePacket, EQueueMode::Mpsc> ReceivedPacketQueue;

	/** Cached pointer to the companion UGaspMoverGaitComponent. */
	UPROPERTY()
	TObjectPtr<UGaspMoverGaitComponent> GaitComponent;

	/** Cached pointer to the Owner's UMoverComponent. */
	UPROPERTY()
	TObjectPtr<UMoverComponent> MoverComponent;

	// ==========================================
	// CACHED NATIVE GAMEPLAY TAGS
	// ==========================================
	FGameplayTag Tag_Mode_Walking;
	FGameplayTag Tag_Mode_Sliding;
	FGameplayTag Tag_Gait_Walk;
	FGameplayTag Tag_Gait_Sprint;
	FGameplayTag Tag_Overlay_Unarmed;
	FGameplayTag Tag_Overlay_CapturedHex;
	FGameplayTag Tag_Modifier_Injured;
	FGameplayTag Tag_Modifier_Stunned;

	// ==========================================
	// ACTIVE TAG STATE TRACKING
	// ==========================================
	FGameplayTag ActiveModeTag;
	FGameplayTag ActiveGaitTag;
	FGameplayTag ActiveOverlayTag;
	FGameplayTag ActiveModifierTag;

	/** Helper to resolve and cache the owner's Mover Component. */
	UMoverComponent* GetMoverComponent();

	/** Resolves and updates a specific gameplay tag category on the Mover component. */
	void UpdateTagCategory(FGameplayTag& ActiveTagField, const FGameplayTag& NewTag);

};
