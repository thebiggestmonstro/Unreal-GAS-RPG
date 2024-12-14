// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void PlayerTick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerInputMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	// 마우스 커서의 Trace를 구현하는 함수
	void CursorTrace();

	// 이전 프레임에서 커서가 가리킨 인터페이스와 현재 프레임에서 커서가 가리킨 인터페이스를 포인터로 저장
	// TScriptInterface는 Interface 포인터를 위한 래퍼
	TScriptInterface<IEnemyInterface> LastCursorHit;
	TScriptInterface<IEnemyInterface> CurrentCursorHit;
};
