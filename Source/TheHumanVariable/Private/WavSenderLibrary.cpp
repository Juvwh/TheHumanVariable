#include "WavSenderLibrary.h"
#include "Misc/FileHelper.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"
#include "Sound/SoundWave.h"
#include "Serialization/BufferArchive.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "LatentActions.h"
#include "Kismet/GameplayStatics.h"


class FSendGroqAction : public FPendingLatentAction
{
public:
    FString& TranscriptOut;
    ESendGroqResult& ResultOut;
    FName ExecutionFunction;
    int32 OutputLink;
    FWeakObjectPtr CallbackTarget;

    FSendGroqAction(FString& InTranscriptOut, ESendGroqResult& InResultOut, const FLatentActionInfo& LatentInfo)
        : TranscriptOut(InTranscriptOut), ResultOut(InResultOut),
        ExecutionFunction(LatentInfo.ExecutionFunction),
        OutputLink(LatentInfo.Linkage), CallbackTarget(LatentInfo.CallbackTarget)
    {
    }

    virtual void UpdateOperation(FLatentResponse& Response) override
    {
        if (bIsDone)
        {
            Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
        }
    }

    void MarkDone(FString Result, bool bSuccess)
    {
        TranscriptOut = Result;
        ResultOut = bSuccess ? ESendGroqResult::Success : ESendGroqResult::Failure;
        bIsDone = true;
    }

private:
    bool bIsDone = false;
};

void UWavSenderLibrary::SendWavToGroq(UObject* WorldContextObject, FLatentActionInfo LatentInfo, const FString& FilePath, const FString& ApiKey, const FString& ModelName, FString& TranscriptOut, ESendGroqResult& Result)
{
    if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
    {
        FLatentActionManager& LatentManager = World->GetLatentActionManager();
        if (LatentManager.FindExistingAction<FSendGroqAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
        {
            FSendGroqAction* NewAction = new FSendGroqAction(TranscriptOut, Result, LatentInfo);
            LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

            FString FullPath = FilePath;
            if (FPaths::IsRelative(FilePath))
            {
                FullPath = FPaths::Combine(FPaths::ProjectSavedDir(), FilePath);
            }

            if (!FPaths::FileExists(FullPath))
            {
                UE_LOG(LogTemp, Error, TEXT("WAV file does not exist: %s"), *FullPath);
                NewAction->MarkDone("", false);
                return;
            }

            TArray<uint8> FileBytes;
            if (!FFileHelper::LoadFileToArray(FileBytes, *FullPath))
            {
                UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FullPath);
                NewAction->MarkDone("", false);
                return;
            }

            FString Boundary = "----UEBoundary";
            FString BeginBoundary = FString::Printf(TEXT("--%s\r\nContent-Disposition: form-data; name=\"file\"; filename=\"audio.wav\"\r\nContent-Type: audio/wav\r\n\r\n"), *Boundary);
            FString EndBoundary = FString::Printf(TEXT("\r\n--%s\r\nContent-Disposition: form-data; name=\"model\"\r\n\r\n%s\r\n--%s--\r\n"), *Boundary, *ModelName, *Boundary);

            TArray<uint8> Payload;
            Payload.Append((uint8*)TCHAR_TO_UTF8(*BeginBoundary), BeginBoundary.Len());
            Payload.Append(FileBytes);
            Payload.Append((uint8*)TCHAR_TO_UTF8(*EndBoundary), EndBoundary.Len());

            TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
            Request->SetURL("https://api.groq.com/openai/v1/audio/transcriptions");
            Request->SetVerb("POST");
            Request->SetHeader("Authorization", FString::Printf(TEXT("Bearer %s"), *ApiKey));
            Request->SetHeader("Content-Type", FString::Printf(TEXT("multipart/form-data; boundary=%s"), *Boundary));
            Request->SetContent(Payload);

            Request->OnProcessRequestComplete().BindLambda([NewAction](FHttpRequestPtr Req, FHttpResponsePtr Response, bool bSuccess)
                {
                    if (bSuccess && Response.IsValid() && EHttpResponseCodes::IsOk(Response->GetResponseCode()))
                    {
                        FString JsonResponse = Response->GetContentAsString();
                        UE_LOG(LogTemp, Log, TEXT("Groq Response: %s"), *JsonResponse);

                        // Extraire uniquement le texte si besoin
                        TSharedPtr<FJsonObject> JsonObject;
                        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonResponse);
                        if (FJsonSerializer::Deserialize(Reader, JsonObject))
                        {
                            FString Transcript = JsonObject->GetStringField("text");
                            NewAction->MarkDone(Transcript, true);
                        }
                        else
                        {
                            NewAction->MarkDone("", false);
                        }
                    }
                    else
                    {
                        FString Err = Response.IsValid() ? Response->GetContentAsString() : TEXT("Pas de réponse");
                        UE_LOG(LogTemp, Error, TEXT("Échec de la requête Groq : %s"), *Err);
                        NewAction->MarkDone("", false);
                    }
                });

            Request->ProcessRequest();
        }
    }
}
