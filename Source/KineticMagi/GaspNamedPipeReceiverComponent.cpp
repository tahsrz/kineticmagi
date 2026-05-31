// Copyright (c) TahWorld. All rights reserved.

#include "GaspNamedPipeReceiverComponent.h"
#include "GaspMoverGaitComponent.h"
#include "MoverComponent.h"
#include "MoverTypes.h"
#include "HAL/RunnableThread.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "GameplayTagContainer.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

// =====================================================================
// FNamedPipeReaderWorker Implementation
// =====================================================================

FNamedPipeReaderWorker::FNamedPipeReaderWorker(const FString& InPipeName, TQueue<FTahStatePacket, EQueueMode::Mpsc>& InTargetQueue)
	: PipeName(InPipeName)
	, TargetQueue(InTargetQueue)
	, bKeepRunning(false)
{
#if PLATFORM_WINDOWS
	PipeHandle = INVALID_HANDLE_VALUE;
#endif
}

FNamedPipeReaderWorker::~FNamedPipeReaderWorker()
{
	Stop();
}

bool FNamedPipeReaderWorker::Init()
{
	bKeepRunning = true;

#if PLATFORM_WINDOWS
	FString FullPipePath = FString::Printf(TEXT("\\\\.\\pipe\\%s"), *PipeName);
	
	// Create Named Pipe with read-write access and block-wait modes
	PipeHandle = CreateNamedPipeW(
		*FullPipePath,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		1,              // Max instances
		1024,           // Output buffer size
		1024,           // Input buffer size
		0,              // Timeout (default)
		NULL            // Security attributes
	);

	if (PipeHandle == INVALID_HANDLE_VALUE)
	{
		UE_LOG(LogTemp, Error, TEXT("GaspPipeReader: Failed to create Named Pipe '%s'. Error code: %d"), 
			*FullPipePath, GetLastError());
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("GaspPipeReader: Named Pipe successfully initialized at '%s'"), *FullPipePath);
	return true;
#else
	UE_LOG(LogTemp, Warning, TEXT("GaspPipeReader: Named Pipes are only supported on Windows platforms."));
	return false;
#endif
}

uint32 FNamedPipeReaderWorker::Run()
{
#if PLATFORM_WINDOWS
	while (bKeepRunning)
	{
		// Wait for a client connection
		if (ConnectNamedPipe(PipeHandle, NULL) || GetLastError() == ERROR_PIPE_CONNECTED)
		{
			uint8 PacketBuffer[4];
			DWORD BytesRead = 0;

			// Read incoming 4-byte binary packets from the pipe client
			while (bKeepRunning && ReadFile(PipeHandle, PacketBuffer, 4, &BytesRead, NULL) && BytesRead == 4)
			{
				FTahStatePacket Packet(
					PacketBuffer[0], // Byte 0x00: Movement Mode
					PacketBuffer[1], // Byte 0x01: Gait Profile
					PacketBuffer[2], // Byte 0x02: Upper Overlay
					PacketBuffer[3]  // Byte 0x03: Modifier
				);
				
				// Push to the thread-safe Multi-Producer Single-Consumer queue
				TargetQueue.Enqueue(Packet);
			}

			// Clean up client connection and prepare for next client loop
			DisconnectNamedPipe(PipeHandle);
		}
		
		// Yield slightly to prevent thread hogging
		FPlatformProcess::Sleep(0.05f);
	}
#endif

	return 0;
}

void FNamedPipeReaderWorker::Stop()
{
	bKeepRunning = false;

#if PLATFORM_WINDOWS
	if (PipeHandle && PipeHandle != INVALID_HANDLE_VALUE)
	{
		// Close the handle to trigger immediate cancellation of blocking operations (Connect/ReadFile)
		CloseHandle(PipeHandle);
		PipeHandle = INVALID_HANDLE_VALUE;
	}
#endif
}

void FNamedPipeReaderWorker::Exit()
{
}


// =====================================================================
// UGaspNamedPipeReceiverComponent Implementation
// =====================================================================

UGaspNamedPipeReceiverComponent::UGaspNamedPipeReceiverComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	PipeName = TEXT("TahWorldGaspPipe");
	PipeThread = nullptr;
}

void UGaspNamedPipeReceiverComponent::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the cached native GASP Gameplay Tags
	Tag_Mode_Walking = FGameplayTag::RequestGameplayTag(FName("Movement.Mode.Walking"));
	Tag_Mode_Sliding = FGameplayTag::RequestGameplayTag(FName("Movement.Mode.Sliding"));
	Tag_Gait_Walk = FGameplayTag::RequestGameplayTag(FName("Movement.Gait.Walk"));
	Tag_Gait_Sprint = FGameplayTag::RequestGameplayTag(FName("Movement.Gait.Sprint"));
	Tag_Overlay_Unarmed = FGameplayTag::RequestGameplayTag(FName("Overlay.State.Unarmed"));
	Tag_Overlay_CapturedHex = FGameplayTag::RequestGameplayTag(FName("Overlay.State.CapturedHex"));
	Tag_Modifier_Injured = FGameplayTag::RequestGameplayTag(FName("Movement.Modifier.Injured"));
	Tag_Modifier_Stunned = FGameplayTag::RequestGameplayTag(FName("Movement.Modifier.Stunned"));

	// Cache the companion Gait Component and Mover Component on this Actor
	if (AActor* Owner = GetOwner())
	{
		GaitComponent = Owner->FindComponentByClass<UGaspMoverGaitComponent>();
		MoverComponent = Owner->FindComponentByClass<UMoverComponent>();
	}

	// Spin up the Named Pipe reader in a background worker thread
	PipeWorker = MakeUnique<FNamedPipeReaderWorker>(PipeName, ReceivedPacketQueue);
	PipeThread = FRunnableThread::Create(
		PipeWorker.Get(), 
		*FString::Printf(TEXT("NamedPipeReaderThread_%s"), *PipeName),
		0,
		TPri_BelowNormal
	);
}

void UGaspNamedPipeReceiverComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clean up thread resources
	if (PipeThread)
	{
		PipeThread->Kill(true);
		PipeThread = nullptr;
	}

	PipeWorker.Reset();

	Super::EndPlay(EndPlayReason);
}

void UGaspNamedPipeReceiverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Dequeue and process any incoming named pipe state packets on the Game Thread
	FTahStatePacket PendingPacket;
	while (ReceivedPacketQueue.Dequeue(PendingPacket))
	{
		UMoverComponent* MoverComp = GetMoverComponent();
		if (!MoverComp)
		{
			continue;
		}

		// 1. Resolve Byte 0x00 (Movement Mode) -> Movement.Mode.Walking, Movement.Mode.Sliding
		FGameplayTag NewModeTag = FGameplayTag();
		if (PendingPacket.MovementMode == 0)
		{
			NewModeTag = Tag_Mode_Walking;
		}
		else if (PendingPacket.MovementMode == 1)
		{
			NewModeTag = Tag_Mode_Sliding;
		}
		UpdateTagCategory(ActiveModeTag, NewModeTag);

		// 2. Resolve Byte 0x01 (Gait Profile) -> Movement.Gait.Walk, Movement.Gait.Sprint
		FGameplayTag NewGaitTag = FGameplayTag();
		if (PendingPacket.GaitProfile == 0)
		{
			NewGaitTag = Tag_Gait_Walk;
		}
		else if (PendingPacket.GaitProfile == 1)
		{
			NewGaitTag = Tag_Gait_Sprint;
		}
		UpdateTagCategory(ActiveGaitTag, NewGaitTag);

		// 3. Resolve Byte 0x02 (Upper Overlay) -> Overlay.State.Unarmed, Overlay.State.CapturedHex
		FGameplayTag NewOverlayTag = FGameplayTag();
		if (PendingPacket.UpperOverlay == 0)
		{
			NewOverlayTag = Tag_Overlay_Unarmed;
		}
		else if (PendingPacket.UpperOverlay == 1)
		{
			NewOverlayTag = Tag_Overlay_CapturedHex;
		}
		UpdateTagCategory(ActiveOverlayTag, NewOverlayTag);

		// 4. Resolve Byte 0x03 (Modifier) -> Movement.Modifier.Injured, Movement.Modifier.Stunned
		FGameplayTag NewModifierTag = FGameplayTag();
		if (PendingPacket.Modifier == 1)
		{
			NewModifierTag = Tag_Modifier_Injured;
		}
		else if (PendingPacket.Modifier == 2)
		{
			NewModifierTag = Tag_Modifier_Stunned;
		}
		UpdateTagCategory(ActiveModifierTag, NewModifierTag);
	}
}

UMoverComponent* UGaspNamedPipeReceiverComponent::GetMoverComponent()
{
	if (!MoverComponent)
	{
		if (AActor* Owner = GetOwner())
		{
			MoverComponent = Owner->FindComponentByClass<UMoverComponent>();
		}
	}
	return MoverComponent;
}

void UGaspNamedPipeReceiverComponent::UpdateTagCategory(FGameplayTag& ActiveTagField, const FGameplayTag& NewTag)
{
	UMoverComponent* MoverComp = GetMoverComponent();
	if (!MoverComp)
	{
		return;
	}

	if (ActiveTagField == NewTag)
	{
		return;
	}

	// Remove old tag from UMoverComponent if it was valid
	if (ActiveTagField.IsValid())
	{
		MoverComp->RemoveGameplayTag(ActiveTagField);
	}

	// Add new tag to UMoverComponent if valid
	if (NewTag.IsValid())
	{
		MoverComp->AddGameplayTag(NewTag);
	}

	ActiveTagField = NewTag;
}

void UGaspNamedPipeReceiverComponent::InjectStateIntoTickData(FGameplayTagContainer& Tags, const FTahStatePacket& Packet)
{
	// Remove any existing tags from these specific categories to avoid conflicts and maintain clean state tracking
	Tags.RemoveTag(Tag_Mode_Walking);
	Tags.RemoveTag(Tag_Mode_Sliding);
	Tags.RemoveTag(Tag_Gait_Walk);
	Tags.RemoveTag(Tag_Gait_Sprint);
	Tags.RemoveTag(Tag_Overlay_Unarmed);
	Tags.RemoveTag(Tag_Overlay_CapturedHex);
	Tags.RemoveTag(Tag_Modifier_Injured);
	Tags.RemoveTag(Tag_Modifier_Stunned);

	// Inject the gameplay tags dynamically based on the 4-byte binary state packet
	
	// Byte 0x00: Movement Mode
	if (Packet.MovementMode == 0)
	{
		Tags.AddTag(Tag_Mode_Walking);
	}
	else if (Packet.MovementMode == 1)
	{
		Tags.AddTag(Tag_Mode_Sliding);
	}

	// Byte 0x01: Gait Profile
	if (Packet.GaitProfile == 0)
	{
		Tags.AddTag(Tag_Gait_Walk);
	}
	else if (Packet.GaitProfile == 1)
	{
		Tags.AddTag(Tag_Gait_Sprint);
	}

	// Byte 0x02: Upper Overlay
	if (Packet.UpperOverlay == 0)
	{
		Tags.AddTag(Tag_Overlay_Unarmed);
	}
	else if (Packet.UpperOverlay == 1)
	{
		Tags.AddTag(Tag_Overlay_CapturedHex);
	}

	// Byte 0x03: Modifier
	if (Packet.Modifier == 1)
	{
		Tags.AddTag(Tag_Modifier_Injured);
	}
	else if (Packet.Modifier == 2)
	{
		Tags.AddTag(Tag_Modifier_Stunned);
	}
}
