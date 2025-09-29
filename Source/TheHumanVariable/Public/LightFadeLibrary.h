#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"
#include "Components/LightComponent.h" 

#include "LightFadeLibrary.generated.h"

UCLASS()
class THEHUMANVARIABLE_API ULightFadeLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", Latent, LatentInfo = "LatentInfo"), Category = "Light|Fade")
    static void FadeLightsToIntensity(UObject* WorldContextObject, FLatentActionInfo LatentInfo,
        const TArray<ULightComponent*>& Lights,
        float TargetIntensity, float Duration);
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", Latent, LatentInfo = "LatentInfo"), Category = "Lighting")
    static void TurnLightsOnWithFlicker(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, const TArray<ULightComponent*>& Lights, float TargetIntensity, float Duration);

};
