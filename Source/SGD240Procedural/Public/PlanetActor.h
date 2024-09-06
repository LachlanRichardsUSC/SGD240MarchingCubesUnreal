#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "PlanetActor.generated.h"

USTRUCT()
struct FVoxel
{
	GENERATED_BODY()

	FVector CornerPositions[8]; // 8 corner positions of a voxel
	float CornerValues[8];       // Density values for each corner
};

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

	// Function to generate the voxel grid
	void GenerateVoxelGrid(int GridSize, float VoxelSize);

	// Function to assign density values
	void AssignDensityValues(TArray<FVoxel>& Voxels, int GridSize, float VoxelSize);

	// Marching Cubes algorithm to create the surface
	void MarchingCubes(TArray<FVoxel>& Voxels, TArray<FVector>& Vertices, TArray<int32>& Triangles, int GridSize);

	// Interpolates the surface position along a voxel edge
	FVector InterpolateEdge(const FVector& CornerA, const FVector& CornerB, float ValueA, float ValueB);

	// Procedural Mesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Planet")
	UProceduralMeshComponent* PlanetMesh;

	// Edge and triangle tables
	static const int EDGE_TABLE[256];
	static const int TRI_TABLE[256][16];

	// Planet Radius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet")
	float Radius;
};
