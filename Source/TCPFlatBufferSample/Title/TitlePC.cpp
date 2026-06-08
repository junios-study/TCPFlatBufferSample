// Fill out your copyright notice in the Description page of Project Settings.


#include "TitlePC.h"
#include "TitleWidgetBase.h"
#include "../TCPClientSubsystem.h"
#include "SignUpWidgetBase.h"

void ATitlePC::BeginPlay()
{
	Super::BeginPlay();

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		UTCPClientSubsystem* TCP = GI->GetSubsystem<UTCPClientSubsystem>();
		if (!TCP->IsConncted())
		{
			TCP->Connect("127.0.0.1", 35000);
		}
	}

	//#include path
	FSoftClassPath TitleWidgetClass(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/Title/WBP_Title.WBP_Title_C'"));

	//#include 
	UClass* WidgetClass = TitleWidgetClass.TryLoadClass<UTitleWidgetBase>();

	if (WidgetClass && IsLocalPlayerController())
	{
		UE_LOG(LogTemp, Warning, TEXT("Make"));
		
		TitleWidgetObject = CreateWidget<UTitleWidgetBase>(this, WidgetClass);
		if (TitleWidgetObject)
		{
			TitleWidgetObject->AddToViewport();
		}
	}

	if (SignUpWidgetClass && IsLocalPlayerController())
	{
		SignUpWidgetObject = CreateWidget<USignUpWidgetBase>(this, SignUpWidgetClass);

		if (SignUpWidgetObject)
		{
			SignUpWidgetObject->AddToViewport();
			SignUpWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
		}

	}

	bShowMouseCursor = true;

	SetInputMode(FInputModeUIOnly());
	//SetInputMode(FInputModeGameOnly());

}
