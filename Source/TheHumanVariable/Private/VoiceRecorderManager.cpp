// Fill out your copyright notice in the Description page of Project Settings.

#include "VoiceRecorderManager.h"
#include "AudioCapture.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/BufferArchive.h"
#include "Misc/Base64.h"
#include "Misc/FileHelper.h"

// Sets default values
AVoiceRecorderManager::AVoiceRecorderManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	AudioCapture = NewObject<UAudioCapture>();

}

// Called when the game starts or when spawned
void AVoiceRecorderManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AVoiceRecorderManager::StartRecording()
{
    if (AudioCapture && AudioCapture->OpenDefaultAudioStream())
    {
        AudioCapture->StartCapturingAudio();
    }
}

void AVoiceRecorderManager::StopAndSendRecording(const FString& ApiKey, const FString& ModelName)
{
    if (AudioCapture)
    {

    }
}


