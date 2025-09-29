#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWaveProcedural.h"
#include "WavPlaybackLibrary.generated.h"

UCLASS()
class THEHUMANVARIABLE_API UWavPlaybackLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Audio", meta = (WorldContext = "WorldContextObject", Latent, LatentInfo = "LatentInfo"))
    static void PlayWavFromBuffer(UObject* WorldContextObject,
        const TArray<uint8>& WavData,
        float& OutDuration,
        USoundWaveProcedural*& OutSoundWave,
        struct FLatentActionInfo LatentInfo);
};
