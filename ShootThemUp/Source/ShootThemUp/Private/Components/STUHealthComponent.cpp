// Shoot Them Up Game, All Rights Reserved.

#include "Components/STUHealthComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogHealthComponent, All, All);

USTUHealthComponent::USTUHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTUHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	check(MaxHealth > 0);

	SetHealth(MaxHealth);

	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &USTUHealthComponent::OnTakeAnyDamage);
	}
}

float USTUHealthComponent::GetHealth() const
{
	return Health;
}

bool USTUHealthComponent::TryToAddHealth(float Amount)
{
	if (IsDead() || IsHealthFull())
		return false;

	SetHealth(Health + Amount);
	UE_LOG(LogHealthComponent, Display, TEXT("Health was added"));

	return true;
}

bool USTUHealthComponent::IsDead() const
{
	return FMath::IsNearlyZero(Health);
}

float USTUHealthComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void USTUHealthComponent::OnTakeAnyDamage(
	AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0 || IsDead() || !GetWorld())
		return;

	SetHealth(Health - Damage);

	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
	else if (AutoHeal)
	{
		GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &USTUHealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
	}
}

void USTUHealthComponent::HealUpdate()
{
	SetHealth(Health + HealModifier);

	if (IsHealthFull() && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
	}
}

void USTUHealthComponent::SetHealth(float NewHealth)
{
	Health = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(Health);
}

bool USTUHealthComponent::IsHealthFull()
{
	return FMath::IsNearlyEqual(Health, MaxHealth);
}
