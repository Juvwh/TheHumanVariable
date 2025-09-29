#include "LightFadeLibrary.h"
#include "Components/LightComponent.h"
#include "Engine/World.h"
#include "LatentActions.h"

class FLightFadeLatentAction : public FPendingLatentAction
{
public:
	enum class EPhase { Blink, Fade };

	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

	TArray<ULightComponent*> Lights;
	TArray<float> InitialIntensities;

	// Fade phase
	float TargetIntensity;
	float FadeDuration;
	float FadeElapsed;

	// Blink phase
	float BlinkDurationPerStep = 0.05f;
	int32 TotalBlinks = 4; // 3 flashes
	int32 BlinkIndex = 0;
	float BlinkTimer = 0.0f;
	float MinBlinkIntensity = 1.0f;
	float MaxBlinkIntensity = 25.0f;

	EPhase CurrentPhase = EPhase::Blink;

	FLightFadeLatentAction(const TArray<ULightComponent*>& InLights, float InTargetIntensity, float InFadeDuration,
		const FLatentActionInfo& LatentInfo)
		: ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, Lights(InLights)
		, TargetIntensity(InTargetIntensity)
		, FadeDuration(FMath::Max(InFadeDuration, 0.01f))
		, FadeElapsed(0.0f)
	{
		for (ULightComponent* Light : InLights)
		{
			InitialIntensities.Add(Light ? Light->Intensity : 0.0f);
		}
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		float DeltaTime = Response.ElapsedTime();

		if (CurrentPhase == EPhase::Blink)
		{
			BlinkTimer += DeltaTime;

			if (BlinkTimer >= BlinkDurationPerStep)
			{
				BlinkTimer = 0.0f;
				++BlinkIndex;

				float BlinkIntensity = (BlinkIndex % 2 == 0) ? MaxBlinkIntensity : MinBlinkIntensity;

				for (ULightComponent* Light : Lights)
				{
					if (Light)
						Light->SetIntensity(BlinkIntensity);
				}

				if (BlinkIndex >= TotalBlinks)
				{
					CurrentPhase = EPhase::Fade;
					FadeElapsed = 0.0f;

					for (int32 i = 0; i < Lights.Num(); ++i)
					{
						if (ULightComponent* Light = Lights[i])
						{
							Light->SetIntensity(InitialIntensities[i]);
						}
					}
				}
			}
		}
		else if (CurrentPhase == EPhase::Fade)
		{
			FadeElapsed += DeltaTime;
			float Alpha = FMath::Clamp(FadeElapsed / FadeDuration, 0.0f, 1.0f);

			for (int32 i = 0; i < Lights.Num(); ++i)
			{
				if (ULightComponent* Light = Lights[i])
				{
					float Start = InitialIntensities.IsValidIndex(i) ? InitialIntensities[i] : 0.0f;
					float NewIntensity = FMath::Lerp(Start, TargetIntensity, Alpha);
					Light->SetIntensity(NewIntensity);
				}
			}

			bool bFinished = (Alpha >= 1.0f);
			Response.FinishAndTriggerIf(bFinished, ExecutionFunction, OutputLink, CallbackTarget);
		}
	}
};

void ULightFadeLibrary::FadeLightsToIntensity(UObject* WorldContextObject, FLatentActionInfo LatentInfo,
	const TArray<ULightComponent*>& Lights, float TargetIntensity, float Duration)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FLightFadeLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID,
				new FLightFadeLatentAction(Lights, TargetIntensity, Duration, LatentInfo));
		}
	}
}

void ULightFadeLibrary::TurnLightsOnWithFlicker(UObject* WorldContextObject, FLatentActionInfo LatentInfo,
	const TArray<ULightComponent*>& Lights, float TargetIntensity, float Duration)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FLightFadeLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID,
				new FLightFadeLatentAction(Lights, TargetIntensity, Duration, LatentInfo));
		}
	}
}
