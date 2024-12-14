// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"
#include "UnrealWidgetFwd.h"
#include "UI/Widgets/AuraUserWidget.h"
#include "UI/WidgetController/AuraOverlayWidgetController.h"

// HUD에서 OverlayWidgetController를 가져오는 함수, 이때 가져오는 OverlayWidgetController는 항상 1개를 유지함
UAuraOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AuraOverlayWidgetController == nullptr)
	{
		AuraOverlayWidgetController = NewObject<UAuraOverlayWidgetController>(this, AuraOverlayWidgetControllerClass);
		AuraOverlayWidgetController->SetWidgetControllerParams(WCParams);
		AuraOverlayWidgetController->BindCallbacksToDependencies();
	}

	return AuraOverlayWidgetController;
}

// HUD에서 OverlayWidget을 설정하는 함수
void AAuraHUD::InitOverlay(APlayerController* InPlayerController, APlayerState* InPlayerState, UAbilitySystemComponent* InAbilitySystemComppnent, UAttributeSet* InAttributeSet)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass uninitialized, fill out BP_AuraHUD"));
	checkf(AuraOverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass uninitialized, fill out BP_AuraHUD"));

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);
	
	const FWidgetControllerParams WidgetControllerParams(InPlayerController, InPlayerState, InAbilitySystemComppnent, InAttributeSet);
	UAuraOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
}

void AAuraHUD::BeginPlay()
{
	Super::BeginPlay();
}
