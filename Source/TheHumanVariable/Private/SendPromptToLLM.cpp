#include "SendPromptToLLM.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "LatentActions.h"
#include "Misc/ConfigCacheIni.h"

class FSendGroqLatentAction : public FPendingLatentAction
{
public:
    FString& TranscriptOut;
    ESendGroqResultText& ResultOut;
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;
    bool bIsDone = false;

    FSendGroqLatentAction(FString& InTranscriptOut, ESendGroqResultText& InResultOut, const FLatentActionInfo& LatentInfo)
        : TranscriptOut(InTranscriptOut), ResultOut(InResultOut),
        ExecutionFunction(LatentInfo.ExecutionFunction), OutputLink(LatentInfo.Linkage), CallbackTarget(LatentInfo.CallbackTarget)
    {
    }

    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        if (bIsDone)
        {
            Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
        }
    }

    void MarkDone(const FString& Result, bool bSuccess)
    {
        TranscriptOut = Result;
        ResultOut = bSuccess ? ESendGroqResultText::Success : ESendGroqResultText::Failure;
        bIsDone = true;
    }
};

void USendPromptToLLM::SendPromptToLLM(
    UObject* WorldContextObject,
    FLatentActionInfo LatentInfo,
    const FString& Prompt,
    const FString& Job,
    ELLMProvider Provider,
    FString& ResponseOut,
    ESendGroqResultText& Result
)
{
    FString ModelOrAgentID;
    FString URL;
    FString ApiKey;

    // Lecture depuis DefaultGame.ini
    const FString Section = TEXT("/Script/TheHumanVariable.SendPromptToLLM");

    switch (Provider)
    {
    case ELLMProvider::Groq_Llama:
        ModelOrAgentID = TEXT("llama3-70b-8192");
        URL = TEXT("https://api.groq.com/openai/v1/chat/completions");
        GConfig->GetString(*Section, TEXT("GroqApiKey"), ApiKey, GGameIni);
        break;

    case ELLMProvider::Groq_Gemma:
        ModelOrAgentID = TEXT("gemma2-9b-it");
        URL = TEXT("https://api.groq.com/openai/v1/chat/completions");
        GConfig->GetString(*Section, TEXT("GroqApiKey"), ApiKey, GGameIni);
        break;

    case ELLMProvider::Mistral:
        ModelOrAgentID = TEXT("ag:65bc52ef:20250507:vide:eebbc04f");
        URL = TEXT("https://api.mistral.ai/v1/agents/completions");
        GConfig->GetString(*Section, TEXT("MistralApiKey"), ApiKey, GGameIni);
        break;

    case ELLMProvider::OpenAI_GPT:
        ModelOrAgentID = TEXT("gpt-4o-mini");
        URL = TEXT("https://api.openai.com/v1/chat/completions");
        GConfig->GetString(*Section, TEXT("OpenAIApiKey"), ApiKey, GGameIni);
        break;
    }

    if (ApiKey.IsEmpty())
    {
        Result = ESendGroqResultText::Failure;
        ResponseOut = TEXT("API Key manquante ou non définie dans DefaultGame.ini.");
        return;
    }

    if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
    {
        FLatentActionManager& LatentManager = World->GetLatentActionManager();
        if (LatentManager.FindExistingAction<FSendGroqLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
        {
            FSendGroqLatentAction* NewAction = new FSendGroqLatentAction(ResponseOut, Result, LatentInfo);
            LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

            TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
            Request->SetURL(URL);
            Request->SetVerb(TEXT("POST"));
            Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
            Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));

            TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
            TArray<TSharedPtr<FJsonValue>> Messages;

            if (Provider == ELLMProvider::Mistral)
            {
                RootObject->SetStringField(TEXT("agent_id"), ModelOrAgentID);
                RootObject->SetNumberField(TEXT("random_seed"), FMath::RandRange(1, INT_MAX));

                // Ajout du message system (Job)
                TSharedPtr<FJsonObject> SystemMsg = MakeShareable(new FJsonObject);
                SystemMsg->SetStringField("role", "system");
                SystemMsg->SetStringField("content", Job);
                Messages.Add(MakeShareable(new FJsonValueObject(SystemMsg)));

                // Ajout du message utilisateur (Prompt)
                TSharedPtr<FJsonObject> UserMsg = MakeShareable(new FJsonObject);
                UserMsg->SetStringField("role", "user");
                UserMsg->SetStringField("content", Prompt);
                Messages.Add(MakeShareable(new FJsonValueObject(UserMsg)));
            }

            else
            {
                RootObject->SetStringField(TEXT("model"), ModelOrAgentID);
                RootObject->SetNumberField(TEXT("temperature"), 0.5f);
                RootObject->SetNumberField(TEXT("max_completion_tokens"), 1024);
                RootObject->SetNumberField(TEXT("top_p"), 1.0f);
                RootObject->SetBoolField(TEXT("stream"), false);

                TSharedPtr<FJsonObject> SystemMsg = MakeShareable(new FJsonObject);
                SystemMsg->SetStringField("role", "system");
                SystemMsg->SetStringField("content", Job);
                Messages.Add(MakeShareable(new FJsonValueObject(SystemMsg)));

                TSharedPtr<FJsonObject> UserMsg = MakeShareable(new FJsonObject);
                UserMsg->SetStringField("role", "user");
                UserMsg->SetStringField("content", Prompt);
                Messages.Add(MakeShareable(new FJsonValueObject(UserMsg)));
            }

            RootObject->SetArrayField(TEXT("messages"), Messages);

            FString JsonString;
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
            FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
            Request->SetContentAsString(JsonString);

            Request->OnProcessRequestComplete().BindLambda([NewAction](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bSuccess)
                {
                    if (bSuccess && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
                    {
                        FString JsonResponse = Response->GetContentAsString();

                        TSharedPtr<FJsonObject> JsonParsed;
                        TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JsonResponse);

                        if (FJsonSerializer::Deserialize(JsonReader, JsonParsed) && JsonParsed.IsValid())
                        {
                            const TArray<TSharedPtr<FJsonValue>>* Choices;
                            if (JsonParsed->TryGetArrayField("choices", Choices) && Choices->Num() > 0)
                            {
                                const TSharedPtr<FJsonObject> MessageObj = (*Choices)[0]->AsObject()->GetObjectField("message");
                                FString Content = MessageObj->GetStringField("content");
                                NewAction->MarkDone(Content, true);
                                return;
                            }
                        }

                        NewAction->MarkDone(TEXT("Erreur de parsing JSON."), false);
                    }
                    else
                    {
                        FString ErrorMsg = Response.IsValid() ? Response->GetContentAsString() : TEXT("Invalid response");
                        NewAction->MarkDone(ErrorMsg, false);
                    }
                });

            Request->ProcessRequest();
        }
    }
}
