// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interface/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(PlayerInputMappingContext);

	// LocalPlayerSubSystem은 Action과 키를 연결하는 Mapping Context를 로컬 플레이어에게 적용함
	// 그리고 Mapping Context간의 우선순위를 지정하여 여러 액션 간의 충돌을 해결
	// LocalPlayerSubSystem은 싱글톤 클래스로서 하나의 인스턴스만 존재함
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(PlayerInputMappingContext, 0);
	}

	// 마우스 커서에 대한 설정
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// UI에 대한 사용자 입력을 정의 -> 이는 뷰포트에도 동일하며 뷰포트에서 마우스를 숨기지 않음
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	
	if (!CursorHit.bBlockingHit)
	{
		return;
	}

	// TScriptInterface를 사용한 경우 따로 캐스팅할 필요가 없음
	// 이전 프레임에서의 커서가 가리킨 Actor와 현재 프레임에서 커서가 가리킨 Actor를 초기화
	LastCursorHit = CurrentCursorHit;
	CurrentCursorHit = CursorHit.GetActor();

	if (LastCursorHit == nullptr)
	{
		// 지난 프레임에서의 커서가 가리킨 Actor가 null이고 현재 프레임에서의 커서가 가리킨 Actor가 유효한 경우
		if (CurrentCursorHit != nullptr)
		{
			CurrentCursorHit->HighlightActor();
		}
		// 지난 프레임에서의 커서가 가리킨 Actor가 null이고 현재 프레임에서의 커서가 가리킨 Actor도 null인 경우
		else
		{
			return;
		}
	}
	else
	{
		// 지난 프레임에서의 커서가 가리킨 Actor가 유효하고 현재 프레임에서의 커서가 가리킨 Actor가 null인 경우
		if (CurrentCursorHit == nullptr)
		{
			LastCursorHit->UnHighlightActor();
		}
		// 지난 프레임에서의 커서가 가리킨 Actor가 유효하고 현재 프레임에서의 커서가 가리킨 Actor도 유효한 경우
		// 두 가지로 분기할 수 있음 -> 두 Actor는 같다! 또는 두 Actor는 다르다!
		else
		{
			// 두 Actor는 다르다!
			if (LastCursorHit != CurrentCursorHit)
			{
				LastCursorHit->UnHighlightActor();
				CurrentCursorHit->HighlightActor();
			}
			// 두 Actor는 같다!
			else
			{
				return;
			}
		}
	}
}
