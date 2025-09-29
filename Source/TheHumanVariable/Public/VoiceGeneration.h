#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VoiceGeneration.generated.h"

UENUM(BlueprintType)
enum class ESGetGroqVoiceEnum : uint8
{
    Success     UMETA(DisplayName = "Success"),
    Failure     UMETA(DisplayName = "Failure")
};

DECLARE_DELEGATE_TwoParams(FGroqVoiceCallback, const TArray<uint8>& /*WavData*/, ESGetGroqVoiceEnum /*Result*/);

UCLASS()
class THEHUMANVARIABLE_API UVoiceGeneration : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Groq|Voice", meta = (WorldContext = "WorldContextObject", Latent, LatentInfo = "LatentInfo", ExpandEnumAsExecs = "Result"))
    static void GetVoiceToGroq(
        UObject* WorldContextObject,
        struct FLatentActionInfo LatentInfo,
        const FString& Text,
        const FString& VoiceID,
        const FString& ApiKey,
        const FString& Model,
        TArray<uint8>& WavData,
        ESGetGroqVoiceEnum& Result
    );
};