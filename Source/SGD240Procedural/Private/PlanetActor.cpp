#include "PlanetActor.h"
#include "ProceduralMeshComponent.h"
#include "MarchingCubesTable.h"
#include "Materials/MaterialInterface.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
APlanetActor::APlanetActor()
{
    // Create a ProceduralMeshComponent and set it as the root component
    PlanetMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = PlanetMesh;

    // Enable ticking
    PrimaryActorTick.bCanEverTick = true;

    // Set a default radius for the planet
    Radius = 200.0f;  // Adjustable in the editor
}

// Called when the game starts or when spawned
void APlanetActor::BeginPlay()
{
    Super::BeginPlay();

    // Generate the planet mesh
    GeneratePlanet();
}

// Called every frame
void APlanetActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Function to generate the voxel grid
void APlanetActor::GenerateVoxelGrid(int GridSize, float VoxelSize, TArray<FVoxel>& OutVoxels)
{
    for (int x = 0; x < GridSize; x++)
    {
        for (int y = 0; y < GridSize; y++)
        {
            for (int z = 0; z < GridSize; z++)
            {
                FVoxel NewVoxel;
                FVector BasePosition = FVector(x, y, z) * VoxelSize;

                NewVoxel.CornerPositions[0] = BasePosition;
                NewVoxel.CornerPositions[1] = BasePosition + FVector(VoxelSize, 0, 0);
                NewVoxel.CornerPositions[2] = BasePosition + FVector(VoxelSize, VoxelSize, 0);
                NewVoxel.CornerPositions[3] = BasePosition + FVector(0, VoxelSize, 0);
                NewVoxel.CornerPositions[4] = BasePosition + FVector(0, 0, VoxelSize);
                NewVoxel.CornerPositions[5] = BasePosition + FVector(VoxelSize, 0, VoxelSize);
                NewVoxel.CornerPositions[6] = BasePosition + FVector(VoxelSize, VoxelSize, VoxelSize);
                NewVoxel.CornerPositions[7] = BasePosition + FVector(0, VoxelSize, VoxelSize);

                OutVoxels.Add(NewVoxel);
            }
        }
    }

    // Assign density values to the voxels
    AssignDensityValues(OutVoxels, GridSize, VoxelSize);
}

// Function to assign density values based on distance from the planet's center
void APlanetActor::AssignDensityValues(TArray<FVoxel>& Voxels, int GridSize, float VoxelSize)
{
    FVector PlanetCenter = FVector(GridSize / 2.0f) * VoxelSize;

    for (FVoxel& Voxel : Voxels)
    {
        for (int CornerIndex = 0; CornerIndex < 8; CornerIndex++)
        {
            float Distance = FVector::Dist(Voxel.CornerPositions[CornerIndex], PlanetCenter);
            Voxel.CornerValues[CornerIndex] = Radius - Distance;
        }
    }
}

// Function to generate mesh using marching cubes
void APlanetActor::MarchingCubes(TArray<FVoxel>& Voxels, TArray<FVector>& Vertices, TArray<int32>& Triangles, int GridSize)
{
    for (const FVoxel& Voxel : Voxels)
    {
        int VoxelConfig = 0;
        for (int CornerIndex = 0; CornerIndex < 8; CornerIndex++)
        {
            if (Voxel.CornerValues[CornerIndex] > 0)
            {
                VoxelConfig |= (1 << CornerIndex);
            }
        }

        if (MarchingCubesTable::EDGE_TABLE[VoxelConfig] == 0)
        {
            continue;
        }

        FVector EdgeVertices[12];
        for (int i = 0; i < 12; i++)
        {
            if (MarchingCubesTable::EDGE_TABLE[VoxelConfig] & (1 << i))
            {
                EdgeVertices[i] = InterpolateEdge(
                    Voxel.CornerPositions[MarchingCubesTable::EDGE_VERTICES[i][0]],
                    Voxel.CornerPositions[MarchingCubesTable::EDGE_VERTICES[i][1]],
                    Voxel.CornerValues[MarchingCubesTable::EDGE_VERTICES[i][0]],
                    Voxel.CornerValues[MarchingCubesTable::EDGE_VERTICES[i][1]]);
            }
        }

        for (int i = 0; MarchingCubesTable::TRI_TABLE[VoxelConfig][i] != -1; i += 3)
        {
            int32 VertexIndex = Vertices.Num();
            Vertices.Add(EdgeVertices[MarchingCubesTable::TRI_TABLE[VoxelConfig][i]]);
            Vertices.Add(EdgeVertices[MarchingCubesTable::TRI_TABLE[VoxelConfig][i + 1]]);
            Vertices.Add(EdgeVertices[MarchingCubesTable::TRI_TABLE[VoxelConfig][i + 2]]);
            Triangles.Add(VertexIndex);
            Triangles.Add(VertexIndex + 1);
            Triangles.Add(VertexIndex + 2);
        }
    }
}

// Function to interpolate the edge between two corners
FVector APlanetActor::InterpolateEdge(const FVector& CornerA, const FVector& CornerB, float ValueA, float ValueB)
{
    float t = ValueA / (ValueA - ValueB);
    return CornerA + t * (CornerB - CornerA);
}

// Function to generate the planet
void APlanetActor::GeneratePlanet()
{
    int GridSize = 48; // Resolution
    float VoxelSize = 16.0f; // Size of Each Voxel

    TArray<FVoxel> Voxels;
    GenerateVoxelGrid(GridSize, VoxelSize, Voxels);
    
     TArray<FVector> Vertices;
        TArray<int32> Triangles;
        MarchingCubes(Voxels, Vertices, Triangles, GridSize);
       
        // Update the procedural mesh component with the calculated normals and no UVs
        PlanetMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, {}, {}, {}, {}, true);
    
        // Optional: Apply the material (if already set in the blueprint or elsewhere)
        if (PlanetMaterial)
        {
            PlanetMesh->SetMaterial(0, PlanetMaterial);
        } 
}
