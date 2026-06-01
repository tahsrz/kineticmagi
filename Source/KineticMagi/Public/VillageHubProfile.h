#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VillageHubProfile.generated.h"

UENUM(BlueprintType)
enum class EVendorType : uint8
{
	GeneralGoods,
	FoodAndDrink,
	Blacksmith,
	Alchemist,
	Carpenter,
	Clothier,
	Banker,
	Appraiser
};

USTRUCT(BlueprintType)
struct FVendorSlotRule
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub")
	EVendorType VendorType = EVendorType::GeneralGoods;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub", meta = (ClampMin = "0", ClampMax = "100"))
	int32 MinCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub", meta = (ClampMin = "0", ClampMax = "100"))
	int32 MaxCount = 2;
};

UCLASS(BlueprintType)
class KINETICMAGI_API UVillageHubProfile : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub")
	bool bAlwaysSpawnCentralMarket = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub", meta = (ClampMin = "1000.0", ClampMax = "50000.0"))
	float MarketRadius = 7000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub", meta = (ClampMin = "1", ClampMax = "500"))
	int32 StallCount = 24;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float TradeTaxRate = 0.03f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PriceVolatility = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub")
	bool bHasQuestBoard = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub")
	bool bHasWarehouse = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub")
	bool bHasSecurityPost = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub", meta = (ClampMin = "0", ClampMax = "23"))
	int32 OpenHour = 7;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub", meta = (ClampMin = "0", ClampMax = "23"))
	int32 CloseHour = 22;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Village|Hub")
	TArray<FVendorSlotRule> VendorRules;
};
