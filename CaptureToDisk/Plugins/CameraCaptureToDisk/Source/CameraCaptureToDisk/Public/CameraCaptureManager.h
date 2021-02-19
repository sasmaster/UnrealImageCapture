// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ASceneCapture2D;

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraCaptureManager.generated.h"


USTRUCT()
struct FRenderRequestStruct{
    GENERATED_BODY()

    TArray<FColor> Image;
    FRenderCommandFence RenderFence;
    bool isPNG;

    FRenderRequestStruct(){
        isPNG = false;
    }
};


UCLASS(Blueprintable)
class CAMERACAPTURETODISK_API ACameraCaptureManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraCaptureManager();

	// Color Capture  Components
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
    ASceneCapture2D* ColorCaptureComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Capture")
    ASceneCapture2D* SegmentationCapture = nullptr;

    // PostProcessMaterial used for segmentation
    UPROPERTY(EditAnywhere, Category="Segmentation Setup")
    UMaterial* PostProcessMaterial = nullptr;

protected:
	// RenderRequest Queue
    TQueue<FRenderRequestStruct*> RenderRequestQueue;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupColorCaptureComponent(ASceneCapture2D* captureComponent);

    // Creates an async task that will save the captured image to disk
    void RunAsyncImageSaveTask(TArray<uint8> Image, FString ImageName);

    void SpawnSegmentationCaptureComponent(ASceneCapture2D* ColorCapture);
    void SetupSegmentationCaptureComponent(ASceneCapture2D* ColorCapture);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ImageCapture")
    void CaptureColorNonBlocking(ASceneCapture2D* CaptureComponent, bool IsSegmentation=false);
};




class AsyncSaveImageToDiskTask : public FNonAbandonableTask{
    public:
        AsyncSaveImageToDiskTask(TArray<uint8> Image, FString ImageName);
        ~AsyncSaveImageToDiskTask();

    // Required by UE4!
    FORCEINLINE TStatId GetStatId() const{
        RETURN_QUICK_DECLARE_CYCLE_STAT(AsyncSaveImageToDiskTask, STATGROUP_ThreadPoolAsyncTasks);
    }

protected:
    TArray<uint8> ImageCopy;
    FString FileName = "";

public:
    void DoWork();
};
