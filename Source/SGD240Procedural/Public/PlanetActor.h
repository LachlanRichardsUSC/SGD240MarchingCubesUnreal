#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MarchingCubesTable.h"
#include "PlanetActor.generated.h"

// Define FVoxel struct to store corner positions and values
struct FVoxel
{
 FVector CornerPositions[8]; // Positions of the 8 corners of a voxel
 float CornerValues[8];       // Density values at each corner
};

UCLASS()
class SGD240PROCEDURAL_API APlanetActor : public AActor
{
 GENERATED_BODY()

public:
 // Sets default values for this actor's properties
 APlanetActor();

protected:
 // Called when the game starts or when spawned
 virtual void BeginPlay() override;

public:
 // Called every frame
 virtual void Tick(float DeltaTime) override;
    
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Settings")
 UMaterialInterface* PlanetMaterial;

private:
 UPROPERTY(EditAnywhere, Category = "Planets")
 UProceduralMeshComponent* PlanetMesh;

 UPROPERTY(EditAnywhere, Category = "Planets")
 float Radius;

 void GeneratePlanet();
 void GenerateVoxelGrid(int GridSize, float VoxelSize, TArray<FVoxel>& OutVoxels);
 void AssignDensityValues(TArray<FVoxel>& Voxels, int GridSize, float VoxelSize);

 // Declare MarchingCubes function with the correct signature
 void MarchingCubes(TArray<FVoxel>& Voxels, TArray<FVector>& Vertices, TArray<int32>& Triangles, int GridSize, float VoxelSize);

 FVector InterpolateEdge(const FVector& CornerA, const FVector& CornerB, float ValueA, float ValueB);
};