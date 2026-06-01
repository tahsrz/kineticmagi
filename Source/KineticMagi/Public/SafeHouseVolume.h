#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SafeHouseVolume.generated.h"

class UBoxComponent;

UCLASS()
class KINETICMAGI_API ASafeHouseVolume : public AActor
{
	GENERATED_BODY()

public:
	ASafeHouseVolume();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "SafeHouse")
	TObjectPtr<UBoxComponent> Volume;
};
