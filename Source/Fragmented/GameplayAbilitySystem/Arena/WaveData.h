
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WaveData.generated.h"

UENUM(BlueprintType)
enum class EWaveType : uint8
{
	Initial UMETA(DisplayName="Initial Wave"),
	HalfHealth UMETA(DisplayName="Half Health Wave"),
	Final UMETA(DisplayName="Final Wave")
};

USTRUCT(BlueprintType)
struct FWaveData : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWaveType WaveType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DifficultyTier = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MeleeCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RangedCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RedSpecialCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 GreenSpecialCount = 0;
};