#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWave.h"
#include "LatentActions.h"
#include "WavSenderLibrary.generated.h"

UENUM(BlueprintType)
enum class ESendGroqResult : uint8
{
    Success UMETA(DisplayName = "Success"),
    Failure UMETA(DisplayName = "Failure")
};

UCLASS()
class THEHUMANVARIABLE_API UWavSenderLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", Latent, LatentInfo = "LatentInfo", ExpandEnumAsExecs = "Result"), Category = "Audio|Groq")
    static void SendWavToGroq(UObject* WorldContextObject, FLatentActionInfo LatentInfo, const FString& FilePath, const FString& ApiKey, const FString& ModelName, FString& TranscriptOut, ESendGroqResult& Result);

};
