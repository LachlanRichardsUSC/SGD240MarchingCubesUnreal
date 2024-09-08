#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "MarchingCubesTable.h"
#include "PlanetActor.generated.h"

USTRUCT()
struct FVoxel
{
 GENERATED_BODY()

 FVector CornerPositions[8];  // 8 corners of a voxel
 float CornerValues[8];       // Corresponding density values at the corners
};

/**
 * Planet actor that generates procedural planets using marching cubes
 */
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
 
  // Planet material property, exposed to the editor
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Planet Settings")
     UMaterialInterface* PlanetMaterial;  // Declare PlanetMaterial here

private:
 // Procedural mesh component to hold the generated mesh
 UPROPERTY(EditAnywhere, Category = "Planets")
 UProceduralMeshComponent* PlanetMesh;

 // Radius of the planet
 UPROPERTY(EditAnywhere, Category = "Planets")
 float Radius;

 // Function to generate the planet mesh
 void GeneratePlanet();

 // Function to generate the voxel grid
 void GenerateVoxelGrid(int GridSize, float VoxelSize, TArray<FVoxel>& OutVoxels);

 // Function to assign density values to the voxels
 void AssignDensityValues(TArray<FVoxel>& Voxels, int GridSize, float VoxelSize);

 // Function to generate the mesh using marching cubes
 void MarchingCubes(TArray<FVoxel>& Voxels, TArray<FVector>& Vertices, TArray<int32>& Triangles, int GridSize);

 // Function to interpolate the edge position
 FVector InterpolateEdge(const FVector& CornerA, const FVector& CornerB, float ValueA, float ValueB);
};
