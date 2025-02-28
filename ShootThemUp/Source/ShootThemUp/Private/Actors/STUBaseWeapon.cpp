// Shoot Them Up Game, All Rights Reserved.

#include "Actors/STUBaseWeapon.h"
#include "Characters/STUBaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseWeapon, All, All);

ASTUBaseWeapon::ASTUBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
}

void ASTUBaseWeapon::StartFire() {}

void ASTUBaseWeapon::StopFire() {}

void ASTUBaseWeapon::ChangeClip()
{
	if (!CurrentAmmo.Infinite)
	{
		if (CurrentAmmo.Clips == 0)
		{
			UE_LOG(LogBaseWeapon, Warning, TEXT("No more clips"));
			return;
		}

		--CurrentAmmo.Clips;
	}

	CurrentAmmo.Bullets = DefaultAmmo.Bullets;

	UE_LOG(LogBaseWeapon, Display, TEXT("-----Change Clip-----"))
}

bool ASTUBaseWeapon::CanReload() const
{
	return CurrentAmmo.Bullets < DefaultAmmo.Bullets && CurrentAmmo.Clips > 0;
}

FWeaponUIData ASTUBaseWeapon::GetUIData() const
{
	return UIData;
}

FAmmoData ASTUBaseWeapon::GetAmmoData() const
{
	return CurrentAmmo;
}

void ASTUBaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	check(WeaponMesh);
	checkf(DefaultAmmo.Bullets > 0, TEXT("Bullets count couldn't be less or equal zero"));
	checkf(DefaultAmmo.Clips > 0, TEXT("Clips count couldn't be less or equal zero"));

	CurrentAmmo = DefaultAmmo;
}

void ASTUBaseWeapon::MakeShot() {}

APlayerController* ASTUBaseWeapon::GetPlayerController() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character)
		return nullptr;

	APlayerController* Controller = Character->GetController<APlayerController>();
	if (!Controller)
		return nullptr;

	return Controller;
}

bool ASTUBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation)
{
	const auto Controller = GetPlayerController();
	if (!Controller)
		return false;

	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	return true;
}

FVector ASTUBaseWeapon::GetMuzzleWorldLocation() const
{
	return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}

FVector ASTUBaseWeapon::GetMuzzleWorldRotation() const
{
	return WeaponMesh->GetSocketTransform(MuzzleSocketName).GetRotation().GetForwardVector();
}

bool ASTUBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd)
{
	FVector ViewLocation;
	FRotator ViewRotation;

	if (!GetPlayerViewPoint(ViewLocation, ViewRotation))
		return false;

	TraceStart = ViewLocation;
	const FVector ShootDirection = ViewRotation.Vector();
	TraceEnd = TraceStart + ShootDirection * TraceMaxDistance;
	return true;
}

void ASTUBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}

float ASTUBaseWeapon::ComputeDegreesToImpactPoint(const FHitResult& HitResult)
{
	FVector ToHitDirection = (HitResult.ImpactPoint - GetMuzzleWorldLocation()).GetSafeNormal();
	const auto AngleBetween = FMath::Acos(FVector::DotProduct(GetMuzzleWorldRotation(), ToHitDirection));
	return FMath::RadiansToDegrees(AngleBetween);
}

void ASTUBaseWeapon::DecreaseAmmo()
{
	if (CurrentAmmo.Bullets == 0)
	{
		UE_LOG(LogBaseWeapon, Warning, TEXT("Clip is empty"));
		return;
	}

	--CurrentAmmo.Bullets;

	if (IsClipEmpty() && !IsAmmoEmpty())
	{
		StopFire();
		OnClipEmpty.Broadcast();
	}
}

bool ASTUBaseWeapon::IsAmmoEmpty() const
{
	return !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}

bool ASTUBaseWeapon::IsClipEmpty() const
{
	return CurrentAmmo.Bullets == 0;
}

void ASTUBaseWeapon::LogAmmo()
{
	FString AmmoInfo = "Ammo: " + FString::FromInt(CurrentAmmo.Bullets) + " /";
	AmmoInfo += CurrentAmmo.Infinite ? "Infinite" : FString::FromInt(CurrentAmmo.Clips);

	UE_LOG(LogBaseWeapon, Display, TEXT("%s"), *AmmoInfo);
}
