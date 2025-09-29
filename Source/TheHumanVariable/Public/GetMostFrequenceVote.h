#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GetMostFrequenceVote.generated.h"

UCLASS()
class THEHUMANVARIABLE_API UGetMostFrequenceVote : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable, Category = "Vote")
    static void GetIntArrayMode(
        const TArray<int32>& Votes,
        bool& OnlyOne,
        TArray<int32>& Modes,
        int32& VoteCount
    );
};
