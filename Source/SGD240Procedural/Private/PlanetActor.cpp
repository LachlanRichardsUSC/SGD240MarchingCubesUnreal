#include "PlanetActor.h"
#include "ProceduralMeshComponent.h"

// Sets default values
APlanetActor::APlanetActor()
{
	// Create a ProceduralMeshComponent and set it as the root component
	PlanetMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GeneratedMesh"));
	RootComponent = PlanetMesh;

	// Enable ticking
	PrimaryActorTick.bCanEverTick = true;

	// Set a default radius
	Radius = 100.0f;  // Default value, can be changed in the editor or via Blueprints
}

// Called when the game starts or when spawned
void APlanetActor::BeginPlay()
{
	Super::BeginPlay();

	// Call the GeneratePlanet function to create the planet's mesh
	GeneratePlanet();
}

// Called every frame
void APlanetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Function to generate the planet mesh
void APlanetActor::GeneratePlanet()
{
    // Arrays to hold the mesh data
    TArray<FVector> Vertices;
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;

    int LatSegments = 20;  // Latitude segments (number of rings)
    int LongSegments = 20;  // Longitude segments (number of slices)

    // Use this->Radius to avoid shadowing the class member
    for (int Lat = 0; Lat <= LatSegments; Lat++)
    {
        float Theta = (Lat * PI) / LatSegments;
        float SinTheta = FMath::Sin(Theta);
        float CosTheta = FMath::Cos(Theta);

        for (int Long = 0; Long <= LongSegments; Long++)
        {
            float Phi = (Long * 2 * PI) / LongSegments;
            float SinPhi = FMath::Sin(Phi);
            float CosPhi = FMath::Cos(Phi);

            FVector Vertex(
                this->Radius * SinTheta * CosPhi,  // X
                this->Radius * SinTheta * SinPhi,  // Y
                this->Radius * CosTheta            // Z
            );

            Vertices.Add(Vertex);
            Normals.Add(Vertex.GetSafeNormal());  // Normals are aligned with the vertex positions for a sphere

            // Add UV mapping (simple spherical projection)
            UV0.Add(FVector2D((float)Long / LongSegments, (float)Lat / LatSegments));
        }
    }

    // Generate triangles
    for (int Lat = 0; Lat < LatSegments; Lat++)
    {
        for (int Long = 0; Long < LongSegments; Long++)
        {
            int First = (Lat * (LongSegments + 1)) + Long;
            int Second = First + LongSegments + 1;

            // First triangle
            Triangles.Add(First);
            Triangles.Add(First + 1);
            Triangles.Add(Second);

            // Second triangle
            Triangles.Add(Second);
            Triangles.Add(First + 1);
            Triangles.Add(Second + 1);
        }
    }

    // Now apply the mesh data to the ProceduralMeshComponent
    PlanetMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), true);
    PlanetMesh->SetMaterial(0, nullptr);  // Optional: set a material if needed
}
