#include "SafeHouseVolume.h"

#include "Components/BoxComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "StealthSubsystem.h"

ASafeHouseVolume::ASafeHouseVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Volume = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeHouseVolume"));
	Volume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Volume->SetCollisionResponseToAllChannels(ECR_Ignore);
	Volume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Volume->SetGenerateOverlapEvents(true);
	SetRootComponent(Volume);
}

void ASafeHouseVolume::BeginPlay()
{
	Super::BeginPlay();

	Volume->OnComponentBeginOverlap.AddDynamic(this, &ASafeHouseVolume::OnVolumeBeginOverlap);
	Volume->OnComponentEndOverlap.AddDynamic(this, &ASafeHouseVolume::OnVolumeEndOverlap);
}

void ASafeHouseVolume::OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
	if (OtherActor != PlayerPawn)
	{
		return;
	}

	if (UStealthSubsystem* Stealth = World->GetSubsystem<UStealthSubsystem>())
	{
		Stealth->SetPlayerIsInSafeHouse(true);
	}
}

void ASafeHouseVolume::OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	APawn* PlayerPawn = PC ? PC->GetPawn() : nullptr;
	if (OtherActor != PlayerPawn)
	{
		return;
	}

	if (UStealthSubsystem* Stealth = World->GetSubsystem<UStealthSubsystem>())
	{
		Stealth->SetPlayerIsInSafeHouse(false);
	}
}
