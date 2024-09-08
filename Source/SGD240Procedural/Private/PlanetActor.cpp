#include "PlanetActor.h"
#include "ProceduralMeshComponent.h"
#include "MarchingCubesTable.h"
#include "Materials/MaterialInterface.h"
#include "KismetProceduralMeshLibrary.h"
#include "DrawDebugHelpers.h"

// Sets default values
APlanetActor::APlanetActor()
{
    // Create a ProceduralMeshComponent and set it as the root component
    PlanetMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = PlanetMesh;

    // Enable ticking
    PrimaryActorTick.bCanEverTick = true;

    // Set a default radius for the planet
    Radius = 400.0f;  // Adjustable in the editor
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
    // Center the grid around (0, 0, 0)
    FVector GridCenterOffset = FVector(GridSize / 2.0f) * VoxelSize;

    // Actor Transforms for Visualization
    //FTransform ActorTransform = GetActorTransform();
    //FQuat ActorRotation = ActorTransform.GetRotation();
    
    for (int x = 0; x < GridSize; x++)
    {
        for (int y = 0; y < GridSize; y++)
        {
            for (int z = 0; z < GridSize; z++)
            {
                FVoxel NewVoxel;
                FVector BasePosition = FVector(x, y, z) * VoxelSize;

                // Offset the base position to center the grid around (0, 0, 0)
                BasePosition -= GridCenterOffset;

                // Define the voxel corners based on the base position
                NewVoxel.CornerPositions[0] = BasePosition;
                NewVoxel.CornerPositions[1] = BasePosition + FVector(VoxelSize, 0, 0);
                NewVoxel.CornerPositions[2] = BasePosition + FVector(VoxelSize, VoxelSize, 0);
                NewVoxel.CornerPositions[3] = BasePosition + FVector(0, VoxelSize, 0);
                NewVoxel.CornerPositions[4] = BasePosition + FVector(0, 0, VoxelSize);
                NewVoxel.CornerPositions[5] = BasePosition + FVector(VoxelSize, 0, VoxelSize);
                NewVoxel.CornerPositions[6] = BasePosition + FVector(VoxelSize, VoxelSize, VoxelSize);
                NewVoxel.CornerPositions[7] = BasePosition + FVector(0, VoxelSize, VoxelSize);

                OutVoxels.Add(NewVoxel);

                
                FVector Center = BasePosition + FVector(VoxelSize / 2.0f);
               // FVector TransformedCenter = ActorTransform.TransformPosition(Center);
                //FVector TransformedScale = ActorTransform.GetScale3D() * (VoxelSize / 2.0f); 
                //DrawDebugBox(GetWorld(), TransformedCenter, TransformedScale, ActorRotation, FColor::Green, true, 20.0f);
                

            }
        }
    }

    // Assign density values to the voxels
    AssignDensityValues(OutVoxels, GridSize, VoxelSize);
}

// Function to assign density values based on distance from the planet's center
void APlanetActor::AssignDensityValues(TArray<FVoxel>& Voxels, int GridSize, float VoxelSize)
{
    // Center the sphere at (0, 0, 0)
    FVector PlanetCenter = FVector(0, 0, 0);

    // Noise parameters
    float NoiseScale = 0.01f; // Needs to be Clamped
    float NoiseAmplitude = 75.0f; // Noise Strength

    for (FVoxel& Voxel : Voxels)
    {
        for (int CornerIndex = 0; CornerIndex < 8; CornerIndex++)
        {
            FVector CornerPosition = Voxel.CornerPositions[CornerIndex];

            // Calculate the distance from the planet's center to the voxel corner
            float Distance = FVector::Dist(CornerPosition, PlanetCenter);

            // Generate 3D Perlin noise based on the corner position
            float NoiseValue = FMath::PerlinNoise3D(CornerPosition * NoiseScale);

            // Adjust the noise amplitude to affect the terrain
            NoiseValue *= NoiseAmplitude;

            // Calculate the density value with added noise for surface variety
            Voxel.CornerValues[CornerIndex] = (Radius - Distance) + NoiseValue;
        }
    }
}

// Function to generate mesh using marching cubes
void APlanetActor::MarchingCubes(TArray<FVoxel>& Voxels, TArray<FVector>& Vertices, TArray<int32>& Triangles, int GridSize, float VoxelSize)
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
                FVector CornerA = Voxel.CornerPositions[MarchingCubesTable::EDGE_VERTICES[i][0]];
                FVector CornerB = Voxel.CornerPositions[MarchingCubesTable::EDGE_VERTICES[i][1]];
                float ValueA = Voxel.CornerValues[MarchingCubesTable::EDGE_VERTICES[i][0]];
                float ValueB = Voxel.CornerValues[MarchingCubesTable::EDGE_VERTICES[i][1]];

                EdgeVertices[i] = InterpolateEdge(CornerA, CornerB, ValueA, ValueB);
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
    int GridSize = 192;  // Size of bounds for voxel grid (increase for more detail)
    float VoxelSize = 16.0f;  // Size of each voxel - lower means more detail

    TArray<FVoxel> Voxels;
    GenerateVoxelGrid(GridSize, VoxelSize, Voxels);
    
    // Arrays to hold generated mesh data
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UVs;
    TArray<FLinearColor> VertexColors;
    TArray<FProcMeshTangent> Tangents;

    // Generate the mesh data using marching cubes
    MarchingCubes(Voxels, Vertices, Triangles, GridSize, VoxelSize);

    // Calculate normals and tangents
    UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

    // Update the procedural mesh component with the generated data
    PlanetMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, true);
    
    // Optional: Apply the material (if already set in the blueprint or elsewhere)
    if (PlanetMaterial)
    {
        PlanetMesh->SetMaterial(0, PlanetMaterial);
    } 
}