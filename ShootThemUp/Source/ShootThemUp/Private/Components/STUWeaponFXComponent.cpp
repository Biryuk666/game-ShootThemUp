// Shoot Them Up Game, All Rights Reserved.

#include "Components/DecalComponent.h"
#include "Components/STUWeaponFXComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

USTUWeaponFXComponent::USTUWeaponFXComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USTUWeaponFXComponent::PlayImpactFX(const FHitResult& Hit)
{
	auto ImpactData = DefaultImpactData;

	if (Hit.PhysMaterial.IsValid())
	{
		const auto PhysMat = Hit.PhysMaterial.Get();
		if (ImpactDataMap.Contains(PhysMat))
		{
			ImpactData = ImpactDataMap[PhysMat];
		}
	}

	// niagara

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), //
		ImpactData.NiagaraEffect,							   //
		Hit.ImpactPoint,									   //
		Hit.ImpactNormal.Rotation());

	// decals

	auto DecalComponent = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), //
		ImpactData.DecalData.Material,										 //
		ImpactData.DecalData.Size,											 //
		Hit.ImpactPoint,													 //
		Hit.ImpactNormal.Rotation());

	DecalComponent->SetFadeOut(ImpactData.DecalData.LifeTime, ImpactData.DecalData.FadeOutTime);
}
