// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

// Attribute에 대하여 Getter, Setter, Init 함수를 자동으로 지원하는 매크로
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UAuraAttributeSet();

	// Replicated 지정자로 지정된 모든 프로퍼티를 리플리케이트하며
	// 프로퍼티의 리플리케이트 방식을 구성하도록 지원하는 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Replicated와 같은 역할을 하지만 
	// Replicated된 데이터를 성공적으로 수신하면 호출되는 RepNotify 함수의 설정을 지원하는 ReplicatedUsing 지시자
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Mana Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Mana Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	// Health가 변동되면 호출되는 함수
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData OldHealth) const;

	// MaxHealth가 변동되면 호출되는 함수
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData OldMaxHealth) const;

	// Mana가 변동되면 호출호디는 함수
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData OldMana) const;

	// MaxMana가 변동되면 호출되는 함수
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData OldMaxMana) const;
};
