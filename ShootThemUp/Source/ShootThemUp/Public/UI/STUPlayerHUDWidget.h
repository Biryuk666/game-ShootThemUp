// Shoot Them Up Game, All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STUCoreTypes.h"
#include "STUPlayerHUDWidget.generated.h"

UCLASS()
class SHOOTTHEMUP_API USTUPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, CAtegory = "UI")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintCallable, CAtegory = "UI")
	bool GetCurrentWeaponUIData(FWeaponUIData& UIData) const;

	UFUNCTION(BlueprintCallable, CAtegory = "UI")
	bool GetCurrentWeaponAmmoData(FAmmoData& AmmoData) const;

	UFUNCTION(BlueprintCallable, CAtegory = "UI")
	bool IsPlayerAlive() const;

	UFUNCTION(BlueprintCallable, CAtegory = "UI")
	bool IsPlayerSpectating() const;
};
