#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CoreMinimal.h"
#include "LightUtilityLibrary.generated.h"

class ULightComponent;

UCLASS()
class THEHUMANVARIABLE_API ULightUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Lights", meta = (WorldContext = "WorldContextObject"))
	static void GetAllLightComponents(UObject* WorldContextObject, TArray<ULightComponent*>& OutLightComponents);
};
