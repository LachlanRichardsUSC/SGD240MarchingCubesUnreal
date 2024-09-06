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

    // Simple voxel grid (1x1x1) for debugging
    int GridSize = 1;  
    float VoxelSize = 100.0f;  // Large size to make it visible

    FVector BasePosition(0, 0, 0);

    // Create 8 corners of a single voxel (cube)
    Vertices.Add(BasePosition);                                // Vertex 0
    Vertices.Add(BasePosition + FVector(VoxelSize, 0, 0));     // Vertex 1
    Vertices.Add(BasePosition + FVector(VoxelSize, VoxelSize, 0));  // Vertex 2
    Vertices.Add(BasePosition + FVector(0, VoxelSize, 0));     // Vertex 3
    Vertices.Add(BasePosition + FVector(0, 0, VoxelSize));     // Vertex 4
    Vertices.Add(BasePosition + FVector(VoxelSize, 0, VoxelSize));  // Vertex 5
    Vertices.Add(BasePosition + FVector(VoxelSize, VoxelSize, VoxelSize)); // Vertex 6
    Vertices.Add(BasePosition + FVector(0, VoxelSize, VoxelSize));  // Vertex 7

    // Create triangles for the sides of the voxel (6 faces, each made of 2 triangles)
    // Example for one face (bottom face):
    // Front face
    Triangles.Add(0); Triangles.Add(1); Triangles.Add(2);  // First triangle
    Triangles.Add(0); Triangles.Add(2); Triangles.Add(3);  // Second triangle

    // Back face
    Triangles.Add(4); Triangles.Add(6); Triangles.Add(5);  // First triangle
    Triangles.Add(4); Triangles.Add(7); Triangles.Add(6);  // Second triangle

    // Left face
    Triangles.Add(0); Triangles.Add(3); Triangles.Add(7);  // First triangle
    Triangles.Add(0); Triangles.Add(7); Triangles.Add(4);  // Second triangle

    // Right face
    Triangles.Add(1); Triangles.Add(5); Triangles.Add(6);  // First triangle
    Triangles.Add(1); Triangles.Add(6); Triangles.Add(2);  // Second triangle

    // Top face
    Triangles.Add(3); Triangles.Add(2); Triangles.Add(6);  // First triangle
    Triangles.Add(3); Triangles.Add(6); Triangles.Add(7);  // Second triangle

    // Bottom face
    Triangles.Add(0); Triangles.Add(4); Triangles.Add(5);  // First triangle
    Triangles.Add(0); Triangles.Add(5); Triangles.Add(1);  // Second triangle

    // Add other faces if needed, but for now we'll test with just this.

    // Update the procedural mesh component
    PlanetMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);

    UE_LOG(LogTemp, Warning, TEXT("Voxel grid created with %d vertices and %d triangles"), Vertices.Num(), Triangles.Num() / 3);
}

