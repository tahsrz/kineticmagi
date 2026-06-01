#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DayNightProfile.generated.h"

UENUM(BlueprintType)
enum class EDayPhase : uint8
{
	Dawn,
	Day,
	Dusk,
	Night
};

UCLASS(BlueprintType)
class KINETICMAGI_API UDayNightProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DayNight", meta = (ClampMin = "1.0"))
	float DayLengthMinutes = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DayNight", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float DawnStartHour = 5.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DayNight", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float DayStartHour = 7.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DayNight", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float DuskStartHour = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DayNight", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float NightStartHour = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DayNight", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float InitialHour = 8.0f;
};
