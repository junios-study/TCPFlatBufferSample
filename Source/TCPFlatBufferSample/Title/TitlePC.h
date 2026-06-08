// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TitlePC.generated.h"


class UTitleWidgetBase;
class USignUpWidgetBase;

/**
 * 
 */
UCLASS()
class TCPFLATBUFFERSAMPLE_API ATitlePC : public APlayerController
{
	GENERATED_BODY()
public:

	virtual void BeginPlay() override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UTitleWidgetBase> TitleWidgetObject;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<USignUpWidgetBase> SignUpWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<USignUpWidgetBase> SignUpWidgetObject;
};
