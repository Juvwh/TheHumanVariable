#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WavUtilsLibrary.generated.h"

UCLASS()
class THEHUMANVARIABLE_API UWavUtilsLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Retourne la durée en secondes d’un buffer WAV. Retourne -1 si le buffer est invalide. */
    UFUNCTION(BlueprintPure, Category = "Audio|Wav")
    static float GetWavDurationFromBuffer(const TArray<uint8>& WavData);
};
