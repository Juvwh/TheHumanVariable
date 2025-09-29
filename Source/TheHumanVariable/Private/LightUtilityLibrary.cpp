#include "LightUtilityLibrary.h"
#include "Engine/World.h"
#include "Components/LightComponent.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h" 

void ULightUtilityLibrary::GetAllLightComponents(UObject* WorldContextObject, TArray<ULightComponent*>& OutLightComponents)
{
	OutLightComponents.Reset();

	if (!WorldContextObject) return;

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!World) return;

	for (TActorIterator<AActor> ActorIt(World); ActorIt; ++ActorIt)
	{
		AActor* Actor = *ActorIt;
		if (!IsValid(Actor)) continue;

		TArray<ULightComponent*> FoundLights;
		Actor->GetComponents<ULightComponent>(FoundLights);

		for (ULightComponent* Light : FoundLights)
		{
			if (IsValid(Light))
			{
				OutLightComponents.Add(Light);
			}
		}
	}
}
