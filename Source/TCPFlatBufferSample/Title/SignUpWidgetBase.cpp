// Fill out your copyright notice in the Description page of Project Settings.


#include "SignUpWidgetBase.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "../TCPClientSubSystem.h"
#include "TitlePC.h"
#include "TitleWidgetBase.h"

void USignUpWidgetBase::NativeConstruct()
{
	UserIDTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("UserIDTextBox")));
	if (UserIDTextBox)
	{

	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &USignUpWidgetBase::OnClickedBack);
	}

	if (SignUpButton)
	{
		SignUpButton->OnClicked.RemoveAll(this);
		SignUpButton->OnClicked.AddDynamic(this, &USignUpWidgetBase::OnClickedSignUp);
	}
}

void USignUpWidgetBase::OnClickedBack()
{
	ATitlePC* PC = Cast<ATitlePC>(GetOwningPlayer());
	if (PC)
	{
		PC->SignUpWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
		PC->TitleWidgetObject->SetVisibility(ESlateVisibility::Visible);
	}
}


void USignUpWidgetBase::OnClickedSignUp()
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnClickedLogin"));
		UTCPClientSubsystem* TCP = GI->GetSubsystem<UTCPClientSubsystem>();
		TCP->OnSignUp.RemoveDynamic(this, &USignUpWidgetBase::ProcessSignUp);
		TCP->OnSignUp.AddDynamic(this, &USignUpWidgetBase::ProcessSignUp);

		TCP->SendSignUp(UserIDTextBox->GetText().ToString(), PasswordTextBox->GetText().ToString(), NameTextBox->GetText().ToString());
	}
}


void USignUpWidgetBase::ProcessSignUp(bool bSuccess, const FString& Message)
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		UTCPClientSubsystem* TCP = GI->GetSubsystem<UTCPClientSubsystem>();
		MessageBox->SetText(FText::FromString(Message));
	}
}


