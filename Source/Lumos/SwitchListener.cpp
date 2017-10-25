// Fill out your copyright notice in the Description page of Project Settings.

#include "Lumos.h"
#include "SwitchListener.h"


// Sets default values for this component's properties
USwitchListener::USwitchListener()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void USwitchListener::BeginPlay()
{
	OnPostSwitch.AddDynamic(this, &USwitchListener::DoPostSwitchEffect);
}

void USwitchListener::OnSwitch(USwitchListener* sl)
{
	if (IsActive == true)
	{
		bool beginState = IsOn();

		for (auto& switchListener : ListenerActors)
		{
			if (switchListener)
			{
				TArray<UActorComponent*> comps = switchListener->GetComponentsByClass(USwitchListener::StaticClass());
				for (auto& comp : comps)
				{
					//this is a slow check which could be optimized by assuming that the indices of ListenerActors and ListenerComponents are parallel; but both lists should be short anyway
					if (ListenerComponents.Contains(comp->GetName()))
					{
						auto switchComp = Cast<USwitchListener>(comp);
						switchComp->OnSwitch(this);
					}
				}
			}
		}

		OnPostSwitch.Broadcast(sl);

		if (IsOneShotSwitch == true)
		{
			//if the state of this switch changed as a result of OnSwitch(), deactivate it
			if (beginState != IsOn())
			{
				IsActive = false;
			}
		}
	}
}

void USwitchListener::OnSwitch(AActor* switchActor)
{
	TArray<UActorComponent*> comps = switchActor->GetComponentsByClass(USwitchListener::StaticClass());
	for (auto c : comps)
	{
		USwitchListener* sl = Cast<USwitchListener>(c);
		if (sl != nullptr)
		{
			OnSwitch(sl);
		}
	}
}

void USwitchListener::DoPostSwitchEffect(USwitchListener* switchListener)
{
	//do nothing by default; override in children
}
