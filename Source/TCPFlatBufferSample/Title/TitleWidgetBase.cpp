// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidgetBase.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "../TCPClientSubSystem.h"
#include "SignUpWidgetBase.h"
#include "TitlePC.h"


void UTitleWidgetBase::NativeConstruct()
{
	UserIDTextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("UserIDTextBox")));
	if (UserIDTextBox)
	{

	}

	if (LoginButton)
	{
		SignUpButton->OnClicked.RemoveDynamic(this, &UTitleWidgetBase::OnClickedLogin);
		LoginButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnClickedLogin);
	}

	if (SignUpButton)
	{
		SignUpButton->OnClicked.RemoveDynamic(this, &UTitleWidgetBase::OnClickedSignUp);
		SignUpButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnClickedSignUp);
	}
}

void UTitleWidgetBase::OnClickedLogin()
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnClickedLogin"));
		UTCPClientSubsystem* TCP = GI->GetSubsystem<UTCPClientSubsystem>();
		TCP->OnLogin.RemoveDynamic(this, &UTitleWidgetBase::ProcessLogin);
		TCP->OnLogin.AddDynamic(this, &UTitleWidgetBase::ProcessLogin);

		TCP->SendLogin(UserIDTextBox->GetText().ToString(), PasswordTextBox->GetText().ToString());
	}
}

void UTitleWidgetBase::OnClickedSignUp()
{
	ATitlePC* PC = Cast<ATitlePC>(GetOwningPlayer());
	if (PC)
	{
		PC->SignUpWidgetObject->SetVisibility(ESlateVisibility::Visible);
		PC->TitleWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	}
}



void UTitleWidgetBase::ProcessLogin(bool bSuccess, const FString& Message)
{
	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		UTCPClientSubsystem* TCP = GI->GetSubsystem<UTCPClientSubsystem>();
		//TCP->OnLogin.RemoveDynamic(this, &UTitleWidgetBase::ProcessLogin);
		MessageBox->SetText(FText::FromString(Message));
	}
}
