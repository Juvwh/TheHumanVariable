#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Sound/SoundWaveProcedural.h"
#include "SoundWaveHelper.generated.h"

UCLASS()
class THEHUMANVARIABLE_API USoundWaveHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Audio")
	static USoundWaveProcedural* CreateProceduralSoundWaveFromWav(const TArray<uint8>& WavData);
};
