#include "VoiceGeneration.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "LatentActions.h"
#include "Json.h"

class FGetVoiceGroqAction : public FPendingLatentAction
{
public:
	TArray<uint8>& OutAudio;
	ESGetGroqVoiceEnum& ResultOut;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

	FGetVoiceGroqAction(TArray<uint8>& InAudioOut, ESGetGroqVoiceEnum& InResultOut, const FLatentActionInfo& LatentInfo)
		: OutAudio(InAudioOut), ResultOut(InResultOut), ExecutionFunction(LatentInfo.ExecutionFunction), OutputLink(LatentInfo.Linkage), CallbackTarget(LatentInfo.CallbackTarget)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		if (bIsDone)
		{
			Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		}
	}

	void MarkDone(const TArray<uint8>& AudioData, bool bSuccess)
	{
		OutAudio = AudioData;
		ResultOut = bSuccess ? ESGetGroqVoiceEnum::Success : ESGetGroqVoiceEnum::Failure;
		bIsDone = true;
	}

private:
	bool bIsDone = false;
};

void UVoiceGeneration::GetVoiceToGroq(UObject* WorldContextObject, FLatentActionInfo LatentInfo, const FString& Text, const FString& VoiceId, const FString& ApiKey, const FString& Model, TArray<uint8>& OutWavData, ESGetGroqVoiceEnum& Result)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();
		if (LatentManager.FindExistingAction<FGetVoiceGroqAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			FGetVoiceGroqAction* NewAction = new FGetVoiceGroqAction(OutWavData, Result, LatentInfo);
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

			TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
			Request->SetURL("https://api.groq.com/openai/v1/audio/speech");
			Request->SetVerb("POST");
			Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *ApiKey));
			Request->SetHeader("Content-Type", TEXT("application/json"));

			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			JsonObject->SetStringField("model", Model);
			JsonObject->SetStringField("voice", VoiceId);
			JsonObject->SetStringField("input", Text);
			JsonObject->SetStringField("response_format", TEXT("wav"));

			FString OutputString;
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
			FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

			Request->SetContentAsString(OutputString);

			Request->OnProcessRequestComplete().BindLambda([NewAction](FHttpRequestPtr Req, FHttpResponsePtr Resp, bool bSuccess)
				{
					if (bSuccess && Resp.IsValid() && EHttpResponseCodes::IsOk(Resp->GetResponseCode()))
					{
						NewAction->MarkDone(Resp->GetContent(), true);
					}
					else
					{
						NewAction->MarkDone({}, false);
					}
				});

			Request->ProcessRequest();
		}
	}
}
