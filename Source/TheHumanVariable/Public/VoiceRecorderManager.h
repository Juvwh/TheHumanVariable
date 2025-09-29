// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AudioCapture.h"
#include "VoiceRecorderManager.generated.h"

UCLASS()
class THEHUMANVARIABLE_API AVoiceRecorderManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoiceRecorderManager();

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void StartRecording();

	UFUNCTION(BlueprintCallable, Category = "Voice")
	void StopAndSendRecording(const FString& ApiKey, const FString& ModelName);

	//UPROPERTY(BlueprintAssignable, Category = "Voice")
	//FOnTranscriptionReceived OnTranscriptionReceived;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void Tick(float DeltaTime) override;

private:	
	// Called every frame
	UPROPERTY()
	UAudioCapture* AudioCapture;
	TArray<float> CapturedAudio;
	void SendToGroq(const TArray<float>& AudioSamples, const FString& ApiKey, const FString& ModelName);
	TArray<uint8> EncodeWAV(const TArray<float>& AudioSamples);

};
