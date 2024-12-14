// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();


}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC)
	{
		check(GameplayEffectClass);
		FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);
		const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);
		// Target의 ASC를 가져오고 GE를 적용한다 
		// 이때 적용된 GE를 현재 동작하는 GE를 나타내는 Wrapper 클래스인 FActiveGameplayEffectHandle의 변수로 저장한다
		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

		// GameplayEffectSpecHandle로부터 GameplayEffectSpec을 가져오고 GameplayEffectSpec으로부터 GameplayEffect를 가져옴
		// 가져온 GameplayEffect의 Duration이 Inifinite인지 확인
		const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
		if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{	
			// Duration이 Inifinite이고, GE의 종료 정책이 EndOverlap으로 설정되었다면
			// 해당 GE를 맵에 저장한다
			ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
		}
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	// Instant, Duration, Infinite GE의 적용 정책이 ApplyOnOverlap이라면 GE를 적용한다
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	// Instant, Duration, Infinite GE에 대한 적용 정책이 ApplyEndOverlap이라면 GE를 적용한다
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	// 이때 Infinite GE의 종료 정책이 RemoveOnEndOverlap이라면 다음의 로직을 수행한다
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		// 우선 GE의 적용 대상의 ASC를 가져온다
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (TargetASC)
		{
			// 대상의 ASC에서 삭제할 GE 핸들을 저장할 리스트를 만든다 
			TArray<FActiveGameplayEffectHandle> HandlesToRemove;
			// GameplayEffectHandle과 ASC를 매핑하는 Map을 튜플로 순회하면서
			for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
			{
				// 대상의 ASC가 튜플의 값 즉, GE의 현재 적용대상의 ASC와 동일하다면
				if (TargetASC == HandlePair.Value)
				{
					// 대상의 ASC로부터 현재 적용되는 GE를 삭제하고 Stack의 Count를 1 감소시킨다
					TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
					// 삭제할 GE 핸들을 저장할 리스트안에 삭제된 GE를 저장한다
					HandlesToRemove.Add(HandlePair.Key);
				}
			}
			// 순회와 삭제가 동시에 일어나면 null 크래시가 발생하므로
			// 순회 로직과 삭제 로직을 분리하였다
			for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
			{
				// 삭제할 GE 핸들을 저장한 리스트를 순화하면서 해당 핸들을 삭제한다
				ActiveEffectHandles.FindAndRemoveChecked(Handle);
			}
		}
	}
}