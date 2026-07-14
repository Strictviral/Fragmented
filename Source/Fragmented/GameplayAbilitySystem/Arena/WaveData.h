
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WaveData.generated.h"


USTRUCT(BlueprintType)
struct FWaveData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WaveNumber = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MeleeCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RangedCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RedSpecialCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GreenSpecialCount = 0;
};