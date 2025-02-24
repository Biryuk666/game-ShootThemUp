// Shoot Them Up Game, All Rights Reserved.

#include "Dev/STUBaseAnimNotify.h"

void USTUBaseAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	OnNotified.Broadcast(MeshComp);
	Super::Notify(MeshComp, Animation);
}