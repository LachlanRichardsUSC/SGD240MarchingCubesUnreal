#include "PlanetActor.h"
#include "ProceduralMeshComponent.h"

// Precomputed edge and triangle tables for marching cubes (abbreviated here)
const int APlanetActor::EDGE_TABLE[256] = { /* Copy the full edge table here */ };
const int APlanetActor::TRI_TABLE[256][16] = { /* Copy the full triangle table here */ };

// Sets default values
APlanetActor::APlanetActor()
{
    // Create a ProceduralMeshComponent and set it as the root component
    PlanetMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
    RootComponent = PlanetMesh;

    // Enable ticking
    PrimaryActorTick.bCanEverTick = true;

    // Set a default radius for the planet
    Radius = 100.0f;
}

// Called when the game starts or when spawned
void APlanetActor::BeginPlay()
{
    Super::BeginPlay();
    GeneratePlanet();
}

// Called every frame
void APlanetActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Function to generate the voxel grid
void APlanetActor::GenerateVoxelGrid(int GridSize, float VoxelSize)
{
    TArray<FVoxel> Voxels;

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

                Voxels.Add(NewVoxel);
            }
        }
    }

    AssignDensityValues(Voxels, GridSize, VoxelSize);
}

// Assign density values based on distance from the planet's center
void APlanetActor::AssignDensityValues(TArray<FVoxel>& Voxels, int GridSize, float VoxelSize)

{
    FVector PlanetCenter = GetActorLocation();
    UE_LOG(LogTemp, Warning, TEXT("Planet Center: %s"), *PlanetCenter.ToString());

    for (FVoxel& Voxel : Voxels)
    {
        for (int CornerIndex = 0; CornerIndex < 8; CornerIndex++)
        {
            float Distance = FVector::Dist(Voxel.CornerPositions[CornerIndex], PlanetCenter);
            UE_LOG(LogTemp, Warning, TEXT("Distance from center: %f"), Distance);
           // Voxel.CornerValues[CornerIndex] = Radius - Distance;
            Voxel.CornerValues[CornerIndex] = 1.0f;

            // Log the density value for debugging
            //UE_LOG(LogTemp, Warning, TEXT("Voxel Corner %d Density: %f"), CornerIndex, Voxel.CornerValues[CornerIndex]);
        }
    }
}

// Marching cubes algorithm to create the surface
void APlanetActor::MarchingCubes(TArray<FVoxel>& Voxels, TArray<FVector>& Vertices, TArray<int32>& Triangles, int GridSize)
{
    int edgeToCorner[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

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

        if (EDGE_TABLE[VoxelConfig] == 0) continue;

        FVector EdgeVertices[12];
        for (int i = 0; i < 12; i++)
        {
            if (EDGE_TABLE[VoxelConfig] & (1 << i))
            {
                int cornerA = edgeToCorner[i][0];
                int cornerB = edgeToCorner[i][1];
                EdgeVertices[i] = InterpolateEdge(Voxel.CornerPositions[cornerA], Voxel.CornerPositions[cornerB], Voxel.CornerValues[cornerA], Voxel.CornerValues[cornerB]);
            }
        }

        for (int i = 0; TRI_TABLE[VoxelConfig][i] != -1; i += 3)
        {
            int32 VertexIndex = Vertices.Num();
            Vertices.Add(EdgeVertices[TRI_TABLE[VoxelConfig][i]]);
            Vertices.Add(EdgeVertices[TRI_TABLE[VoxelConfig][i + 1]]);
            Vertices.Add(EdgeVertices[TRI_TABLE[VoxelConfig][i + 2]]);

            Triangles.Add(VertexIndex);
            Triangles.Add(VertexIndex + 1);
            Triangles.Add(VertexIndex + 2);
        }
    }
}

// Function to interpolate the surface position along a voxel edge
FVector APlanetActor::InterpolateEdge(const FVector& CornerA, const FVector& CornerB, float ValueA, float ValueB)
{
    float t = ValueA / (ValueA - ValueB);
    return CornerA + t * (CornerB - CornerA);
}

// Generate the planet's mesh
// void APlanetActor::GeneratePlanet()
// {
//     int GridSize = 20;
//     float VoxelSize = 5.0f;
//
//     TArray<FVoxel> Voxels;
//     GenerateVoxelGrid(GridSize, VoxelSize);
//
//     TArray<FVector> Vertices;
//     TArray<int32> Triangles;
//     MarchingCubes(Voxels, Vertices, Triangles, GridSize);
//
//     // Log the number of vertices and triangles
//     UE_LOG(LogTemp, Warning, TEXT("Number of Vertices: %d"), Vertices.Num());
//     UE_LOG(LogTemp, Warning, TEXT("Number of Triangles: %d"), Triangles.Num() / 3);
//
//     PlanetMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
//     
//     UE_LOG(LogTemp, Warning, TEXT("Mesh section created with %d vertices and %d triangles"), Vertices.Num(), Triangles.Num() / 3);
//
//
//     // Assign a material to ensure visibility
//     //UMaterial* DefaultMaterial = UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface);
//
//     //PlanetMesh->SetMaterial(0, DefaultMaterial);
// }

void APlanetActor::GeneratePlanet()
{
    TArray<FVector> Vertices;
    TArray<int32> Triangles;

    int GridSize = 5;  // A small grid for testing (5x5x5 cubes)
    float VoxelSize = 100.0f;  // Size of each voxel (cube)
    float Gap = 10.0f;  // Introduce a gap to separate cubes

    // Calculate the center of the voxel grid
    FVector GridCenter = FVector(GridSize / 2.0f, GridSize / 2.0f, GridSize / 2.0f) * (VoxelSize + Gap);

    // Loop through the grid to generate each voxel (cube)
    for (int x = 0; x < GridSize; x++)
    {
        for (int y = 0; y < GridSize; y++)
        {
            for (int z = 0; z < GridSize; z++)
            {
                // Calculate the base position relative to the grid center
                FVector BasePosition = FVector(
                    x * (VoxelSize + Gap), 
                    y * (VoxelSize + Gap), 
                    z * (VoxelSize + Gap)
                ) - GridCenter;  // Subtract grid center to align with the origin

                int32 StartIndex = Vertices.Num();

                // Add cube vertices as before
                Vertices.Add(BasePosition);                                // Vertex 0
                Vertices.Add(BasePosition + FVector(VoxelSize, 0, 0));     // Vertex 1
                Vertices.Add(BasePosition + FVector(VoxelSize, VoxelSize, 0));  // Vertex 2
                Vertices.Add(BasePosition + FVector(0, VoxelSize, 0));     // Vertex 3
                Vertices.Add(BasePosition + FVector(0, 0, VoxelSize));     // Vertex 4
                Vertices.Add(BasePosition + FVector(VoxelSize, 0, VoxelSize));  // Vertex 5
                Vertices.Add(BasePosition + FVector(VoxelSize, VoxelSize, VoxelSize)); // Vertex 6
                Vertices.Add(BasePosition + FVector(0, VoxelSize, VoxelSize));  // Vertex 7

                // Check if there's a neighboring cube and skip internal faces
                if (x == 0) // No neighbor on the left
                {
                    Triangles.Add(StartIndex + 0); Triangles.Add(StartIndex + 3); Triangles.Add(StartIndex + 7);
                    Triangles.Add(StartIndex + 0); Triangles.Add(StartIndex + 7); Triangles.Add(StartIndex + 4);
                }
                if (x == GridSize - 1) // No neighbor on the right
                {
                    Triangles.Add(StartIndex + 1); Triangles.Add(StartIndex + 5); Triangles.Add(StartIndex + 6);
                    Triangles.Add(StartIndex + 1); Triangles.Add(StartIndex + 6); Triangles.Add(StartIndex + 2);
                }
                if (y == 0) // No neighbor below
                {
                    Triangles.Add(StartIndex + 0); Triangles.Add(StartIndex + 4); Triangles.Add(StartIndex + 5);
                    Triangles.Add(StartIndex + 0); Triangles.Add(StartIndex + 5); Triangles.Add(StartIndex + 1);
                }
                if (y == GridSize - 1) // No neighbor above
                {
                    Triangles.Add(StartIndex + 3); Triangles.Add(StartIndex + 2); Triangles.Add(StartIndex + 6);
                    Triangles.Add(StartIndex + 3); Triangles.Add(StartIndex + 6); Triangles.Add(StartIndex + 7);
                }
                if (z == 0) // No neighbor in front
                {
                    Triangles.Add(StartIndex + 0); Triangles.Add(StartIndex + 1); Triangles.Add(StartIndex + 2);
                    Triangles.Add(StartIndex + 0); Triangles.Add(StartIndex + 2); Triangles.Add(StartIndex + 3);
                }
                if (z == GridSize - 1) // No neighbor behind
                {
                    Triangles.Add(StartIndex + 4); Triangles.Add(StartIndex + 6); Triangles.Add(StartIndex + 5);
                    Triangles.Add(StartIndex + 4); Triangles.Add(StartIndex + 7); Triangles.Add(StartIndex + 6);
                }
            }
        }
    }

    // Create the mesh for the voxel grid
    PlanetMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);

    UE_LOG(LogTemp, Warning, TEXT("Voxel grid created with %d vertices and %d triangles"), Vertices.Num(), Triangles.Num() / 3);
}




