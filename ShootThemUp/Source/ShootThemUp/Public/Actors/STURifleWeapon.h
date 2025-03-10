// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/STUBaseWeapon.h"
#include "STURifleWeapon.generated.h"

class USTUWeaponFXComponent;

UCLASS()
class SHOOTTHEMUP_API ASTURifleWeapon : public ASTUBaseWeapon
{
	GENERATED_BODY()

public:
	ASTURifleWeapon();

	virtual void StartFire() override;
	virtual void StopFire() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "WeaponStats")
	float FireRate = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "WeaponStats")
	float BulletSpread = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeaponStats")
	float DamageAmount = 10.0f;

	UPROPERTY(VisibleAnywhere, Category = "VFX")
	USTUWeaponFXComponent* WeaponFXComponent;

	virtual void MakeShot() override;
	virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) override;

	void MakeDamage(const FHitResult& HitResult);

	virtual void BeginPlay() override;

private:
	FTimerHandle ShootTimerHandle;
};
