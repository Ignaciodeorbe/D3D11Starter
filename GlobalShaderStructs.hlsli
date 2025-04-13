#ifndef __SHADERSTRUCTS__ 
#define __SHADERSTRUCTS__


// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};


// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
    float3 tangent : TANGENT;

};

struct VertexToPixel_Sky
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};


//----------------
// Lighting Struct
//----------------

// A constant Fresnel value for non-metals (glass and plastic have values of about 0.04)
static const float F0_NON_METAL = 0.04f;

// Minimum roughness for when spec distribution function denominator goes to zero
static const float MIN_ROUGHNESS = 0.0000001f; // 6 zeros after decimal

// Handy to have this as a constant
static const float PI = 3.14159265359f;

#define MAX_SPECULAR_EXPONENT 256.0f

#define LIGHT_TYPE_DIRECTIONAL	0
#define LIGHT_TYPE_POINT		1
#define LIGHT_TYPE_SPOT			2

struct Light
{
    int Type;
    float3 Direction;    
    float Range;
    float3 Position;    
    float Intensity;
    float3 Color; 
    float SpotInnerAngle;
    float SpotOuterAngle;
    float2 Padding; 
};


//-----------------------
// Normal Mapping Methods
//-----------------------

float3 ComputeNormalMap(float3 normal, float3 tangent, Texture2D normalMap, SamplerState samplerState, float2 uv)
{
    float3 unpackedNormal = normalMap.Sample(samplerState, uv).rgb * 2 - 1;
    unpackedNormal = normalize(unpackedNormal);
    
    float3 N = normalize(normal); // Must be normalized here or before
    float3 T = normalize(tangent); // Must be normalized here or before
    T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
    return mul(unpackedNormal, TBN); // Note multiplication order!
}


//-----------------
// Lighting Methods
//-----------------

float3 Diffuse(float3 lightDirection, float3 normal, float3 surfaceColor)
{
    // Normalize light direction
    lightDirection = normalize(lightDirection);
    
    // Calculate lambertian reflectance with dot product
    float lambertianReflection = saturate(dot(normal, lightDirection));
    
    return surfaceColor * lambertianReflection;
}

float Phong(float3 lightDirection, float3 normal, float3 cameraPosition, float3 pixelWorldPosition, float roughness)
{
    
    lightDirection = normalize(lightDirection);
    
    float3 V = normalize(cameraPosition - pixelWorldPosition);
    float3 R = reflect(-lightDirection, normal);
    
    return pow(max(dot(V, R), 0), (1 - roughness) * MAX_SPECULAR_EXPONENT);
}

float Attenuation(Light light, float3 pixelWorldPosition)
{
    float dist = distance(light.Position, pixelWorldPosition);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}


// ------------------
// Light Type Methods
// ------------------

float3 ComputeDirectionalLighting(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 pixelWorldPosition, float roughness)
{
    // Compute light direction 
    float3 lightDirection = normalize(-light.Direction);

    // Compute diffuse and specular 
    float3 diffuse = Diffuse(lightDirection, normal, surfaceColor);
    float specular = Phong(lightDirection, normal, cameraPosition, pixelWorldPosition, roughness);
    return diffuse + specular;
}

float3 ComputePointLighting(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 pixelWorldPosition, float roughness)
{
    // Compute light direction to pixel
    float3 lightDirectionToPixel = normalize(light.Position - pixelWorldPosition);

    // Compute diffuse and specular
    float3 diffuse = Diffuse(lightDirectionToPixel, normal, surfaceColor);
    float specular = Phong(lightDirectionToPixel, normal, cameraPosition, pixelWorldPosition, roughness);
    
    // Apply attenuation 
    float attenuation = Attenuation(light, pixelWorldPosition);
    
    return (diffuse + specular) * attenuation;

}

float3 ComputeSpotLighting(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 pixelWorldPosition, float roughness)
{
    // Compute light direction 
    float3 lightDirection = normalize(-light.Direction);
    
    // Compute light direction to pixel
    float3 lightDirectionToPixel = normalize(light.Position - pixelWorldPosition);
    
    // Get cos(angle) between pixel and light direction
    float pixelAngle = saturate(dot(lightDirectionToPixel, lightDirection));
    
    // Get cosines of angles and calculate range
    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;
    
    // Linear falloff over the range, clamp 0-1, apply to light calc
    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);
    
    return ComputePointLighting(light, normal, surfaceColor, cameraPosition, pixelWorldPosition, roughness) * spotTerm;
}


// --------------------
// Main Lighting Method
// --------------------

float3 ComputeLighting(Light light, float3 normal, float3 surfaceColor, float3 cameraPosition, float3 pixelWorldPosition, float roughness)
{
    float3 result = 0.0f;

    // Compute the lighting for whatever type of light it is
    if (light.Type == LIGHT_TYPE_DIRECTIONAL)
    {
        result = ComputeDirectionalLighting(light, normal, surfaceColor, cameraPosition, pixelWorldPosition, roughness);
    }
    else if (light.Type == LIGHT_TYPE_POINT)
    {
        result = ComputePointLighting(light, normal, surfaceColor, cameraPosition, pixelWorldPosition, roughness);
    }
    else if (light.Type == LIGHT_TYPE_SPOT)
    {
        result = ComputeSpotLighting(light, normal, surfaceColor, cameraPosition, pixelWorldPosition, roughness);
    }

    return result * light.Color * light.Intensity;
}


// -----------
// PBR Methods
// -----------

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}


// Calculates diffuse amount based on energy conservation
//
// diffuse - Diffuse amount
// F - Fresnel result from microfacet BRDF
// metalness - surface metalness amount
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}


// Normal Distribution Function: GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector: (V + L)/2
// n - Normal
//
// D(h, n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float roughness)
{
    // Pre-calculations
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!
    
    // ((n dot h)^2 * (a^2 - 1) + 1)
    // Can go to zero if roughness is 0 and NdotH is 1; MIN_ROUGHNESS helps here
    float denomToSquare = NdotH2 * (a2 - 1) + 1;
    
    // Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}


// Fresnel term - Schlick approx.
//
// v - View vector
// h - Half vector
// f0 - Value when l = n
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
    // Pre-calculations
    float VdotH = saturate(dot(v, h));
    
    // Final value
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}


// Geometric Shadowing - Schlick-GGX
// - k is remapped to a / 2, roughness remapped to (r+1)/2 before squaring!
//
// n - Normal
// v - View vector
//
// G_Schlick(n,v,a) = (n dot v) / ((n dot v) * (1 - k) * k)
//
// Full G(n,v,l,a) term = G_SchlickGGX(n,v,a) * G_SchlickGGX(n,l,a)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
    // End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));
    
    // Final value
    // Note: Numerator should be NdotV (or NdotL depending on parameters).
    // However, these are also in the BRDF's denominator, so they'll cancel!
    // We're leaving them out here AND in the BRDF function as the
    // dot products can get VERY small and cause rounding errors.
    return 1 / (NdotV * (1 - k) + k);
}


// Cook-Torrance Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - parts of the denominator are canceled out by numerator (see below)
//
// D() - Normal Distribution Function - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 specColor, float3out F_out)
{
    // Other vectors
    float3 h = normalize(v + l); // That’s an L, not a 1! Careful copy/pasting from a PDF!
    
    // Run numerator functions
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
    
    // Pass F out of the function for diffuse balance
    F_out = F;
    
    // Final specular formula
    // Note: The denominator SHOULD contain (NdotV)(NdotL), but they'd be
    // canceled out by our G() term. As such, they have been removed
    // from BOTH places to prevent floating point rounding errors.
    float3 specularResult = (D * F * G) / 4;

    // One last non-obvious requirement: According to the rendering equation,
    // specular must have the same NdotL applied as diffuse! We'll apply
    // that here so that minimal changes are required elsewhere.
    return specularResult * max(dot(n, l), 0);
}
#endif