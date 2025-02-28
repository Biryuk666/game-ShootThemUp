// Shoot Them Up Game, All Rights Reserved.

#include <Actors/STUBaseWeapon.h>
#include "Components/STUWeaponComponent.h"
#include "Dev/AnimUtils.h"
#include "Dev/STUEquipFinishedAnimNotify.h"
#include "Dev/STUReloadFinishedAnimNotify.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All);

constexpr static int32 WeaponNum = 2;

USTUWeaponComponent::USTUWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USTUWeaponComponent::StartFire()
{
	if (!CanFire())
		return;

	CurrentWeapon->StartFire();
}

void USTUWeaponComponent::StopFire()
{
	if (!CurrentWeapon)
		return;

	CurrentWeapon->StopFire();
}

void USTUWeaponComponent::TakeNextWeapon()
{
	if (!CanEquip())
		return;

	CurrentWeaponIndex = ++CurrentWeaponIndex % Weapons.Num();
	EquipWeapon(CurrentWeaponIndex);
}

void USTUWeaponComponent::Reload()
{
	ChangeClip();
}

bool USTUWeaponComponent::GetCurrentWeaponUIData(FWeaponUIData& UIData) const
{
	if (CurrentWeapon)
	{
		UIData = CurrentWeapon->GetUIData();
		return true;
	}

	return false;
}

bool USTUWeaponComponent::GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const
{
	if (CurrentWeapon)
	{
		AmmoData = CurrentWeapon->GetAmmoData();
		return true;
	}

	return false;
}

void USTUWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(WeaponData.Num() == WeaponNum, TEXT("Character can hold only %i weapon items"), WeaponNum);

	CurrentWeaponIndex = 0;
	InitAnimations();
	SpawnWeapons();
	EquipWeapon(CurrentWeaponIndex);
}

void USTUWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CurrentWeapon = nullptr;
	for (auto Weapon : Weapons)
	{
		Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		Weapon->Destroy();
	}

	Weapons.Empty();

	Super::EndPlay(EndPlayReason);
}

void USTUWeaponComponent::SpawnWeapons()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || !GetWorld())
		return;

	for (const FWeaponData& OneWeaponData : WeaponData)
	{
		auto Weapon = GetWorld()->SpawnActor<ASTUBaseWeapon>(OneWeaponData.WeaponClass);
		if (!Weapon)
			continue;

		Weapon->OnClipEmpty.AddUObject(this, &USTUWeaponComponent::OnClipEmpty);

		Weapon->SetOwner(Character);
		Weapons.Add(Weapon);
		AttachWeaponToSocket(Weapon, Character->GetMesh(), WeaponArmorySocketName);
	}
}

void USTUWeaponComponent::AttachWeaponToSocket(ASTUBaseWeapon* Weapon, USceneComponent* SkeletalMesh, const FName& SocketName)
{
	if (!Weapon || !SkeletalMesh)
		return;

	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	Weapon->AttachToComponent(SkeletalMesh, AttachmentRules, SocketName);
}

void USTUWeaponComponent::EquipWeapon(int32 WeaponIndex)
{
	if (WeaponIndex < 0 || WeaponIndex >= Weapons.Num())
	{
		UE_LOG(LogWeaponComponent, Warning, TEXT("Invalid Weapon Index"));
		return;
	}

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
		return;

	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
		AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponArmorySocketName);
	}

	CurrentWeapon = Weapons[WeaponIndex];
	// CurrentReloadAnimMontage = WeaponData[WeaponIndex].ReloadAnimMontage;
	const auto CurrentWeaponData = WeaponData.FindByPredicate([&](const FWeaponData& Data) { //
		return Data.WeaponClass == CurrentWeapon->GetClass();								 //
	});
	CurrentReloadAnimMontage = CurrentWeaponData ? CurrentWeaponData->ReloadAnimMontage : nullptr;

	AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
	EquipAnimInProgress = true;
	PlayAnimMontage(EquipAnimMontage);
}

void USTUWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
		return;

	Character->PlayAnimMontage(Animation);
}

void USTUWeaponComponent::InitAnimations()
{
	auto EquipFinishedNotify = AnimUtils::FindNotifyByClass<USTUEquipFinishedAnimNotify>(EquipAnimMontage);
	if (EquipFinishedNotify)
	{
		EquipFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnEquipFinished);
	}
	else
	{
		UE_LOG(LogWeaponComponent, Warning, TEXT("Equip anim notify is forgotten to set"));
		checkNoEntry();
	}
	for (const auto& OneWeaponData : WeaponData)
	{
		auto ReloadFinishedNotify = AnimUtils::FindNotifyByClass<USTUReloadFinishedAnimNotify>(OneWeaponData.ReloadAnimMontage);
		if (ReloadFinishedNotify)
		{
			ReloadFinishedNotify->OnNotified.AddUObject(this, &USTUWeaponComponent::OnReloadFinished);
		}
		else
		{
			UE_LOG(LogWeaponComponent, Warning, TEXT("Reload anim notify is forgotten to set"));
			checkNoEntry();
		}
	}
}

void USTUWeaponComponent::OnEquipFinished(USkeletalMeshComponent* MeshComponent)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || Character->GetMesh() != MeshComponent)
		return;

	EquipAnimInProgress = false;
	UE_LOG(LogWeaponComponent, Display, TEXT("Equip finished"));
}

void USTUWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character || Character->GetMesh() != MeshComponent)
		return;

	ReloadAnimInProgress = false;
	UE_LOG(LogWeaponComponent, Display, TEXT("Reload finished"));
}

bool USTUWeaponComponent::CanFire() const
{
	return CurrentWeapon && !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTUWeaponComponent::CanEquip() const
{
	return !EquipAnimInProgress && !ReloadAnimInProgress;
}

bool USTUWeaponComponent::CanReload() const
{
	return CurrentWeapon			//
		   && !EquipAnimInProgress	//
		   && !ReloadAnimInProgress //
		   && CurrentWeapon->CanReload();
}

void USTUWeaponComponent::OnClipEmpty()
{
	ChangeClip();
}

void USTUWeaponComponent::ChangeClip()
{
	if (!CanEquip() || !CanReload())
		return;

	CurrentWeapon->StopFire();
	ReloadAnimInProgress = true;
	CurrentWeapon->ChangeClip();
	PlayAnimMontage(CurrentReloadAnimMontage);
}
