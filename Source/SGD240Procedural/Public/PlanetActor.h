#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PlanetActor.generated.h"

UCLASS()
class SGD240PROCEDURAL_API APlanetActor : public AActor
{
	GENERATED_BODY()

public:
	// Constructor
	APlanetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function to generate the planet's mesh
	void GeneratePlanet();

	// Procedural Mesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Planet")
	UProceduralMeshComponent* PlanetMesh;
	
	
	// Expose Planet Radius to Editor and Blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Settings")
	float Radius;  // Editable radius for the planet

};
