//Resources

cbuffer Matrices           : register(b0)
{
	//matrix mvp;
	matrix inverseVP;
};

cbuffer Uniforms           : register(b1)
{
	float time;
	float screenWidth;
	float screenHeight;
	float dummy;
	float cameraPosX;
	float cameraPosY;
	float cameraPosZ;
	float cameraPosW;
};

//Structs
struct VS_INPUT
{
	float4 position : POSITION;
	//float2 texcoord : TEXCOORD;
};

struct GS_POINT
{
	float4 pos      : SV_POSITION;
	//float2 texcoord : TEXCOORD;
};

struct PS_OUT
{
	float4 position : SV_TARGET0;
	float4 normal   : SV_TARGET1;
	float4 color    : SV_TARGET2;
};

#define CLOUD_HEIGHT 200.0f

/*
//Perlin noise, original
#define NUM_GRADIENTS 12
static const float3 gradients[NUM_GRADIENTS] = {	float3(1, 1, 0), float3(-1, 1, 0), float3(1, -1, 0), float3(-1, -1, 0),
											float3(1, 0, 1), float3(-1, 0, 1), float3(1, 0, -1), float3(-1, 0, -1), 
											float3(0, 1, 1), float3(0, -1, 1), float3(0, 1, -1), float3(0, -1, -1)};


static const float permutation_table[512] =
{
	190, 12, 147, 105, 236, 196, 224, 36, 31, 18,
	72, 129, 198, 250, 200, 60, 79, 247, 233, 166,
	90, 178, 102, 39, 168, 88, 38, 170, 17, 16,
	151, 232, 219, 47, 169, 206, 229, 92, 52, 192,
	195, 74, 173, 176, 160, 131, 41, 217, 28, 68,
	30, 69, 103, 89, 58, 172, 159, 150, 66, 25,
	11, 167, 221, 56, 238, 255, 189, 110, 59, 80,
	109, 153, 203, 82, 228, 130, 209, 1, 237, 156,
	252, 242, 215, 4, 101, 8, 152, 29, 23, 62,
	124, 161, 253, 174, 33, 53, 86, 22, 146, 205,
	123, 106, 120, 235, 114, 2, 91, 234, 121, 197,
	135, 84, 81, 164, 183, 137, 5, 98, 181, 225,
	179, 44, 71, 216, 6, 32, 97, 115, 149, 24,
	175, 223, 99, 14, 111, 37, 154, 77, 95, 220,
	75, 112, 70, 40, 208, 171, 212, 19, 55, 226,
	9, 254, 113, 46, 187, 207, 133, 48, 241, 7,
	87, 65, 230, 10, 163, 108, 243, 143, 96, 127,
	125, 3, 227, 182, 61, 132, 140, 244, 116, 104,
	248, 126, 201, 246, 145, 177, 180, 26, 73, 214,
	78, 245, 188, 222, 100, 107, 139, 210, 193, 191,
	0, 50, 204, 64, 122, 251, 202, 35, 119, 34,
	15, 67, 157, 76, 27, 239, 118, 194, 83, 199,
	128, 54, 134, 155, 148, 49, 218, 136, 211, 21,
	94, 43, 231, 141, 13, 165, 144, 85, 45, 63,
	158, 249, 142, 184, 138, 57, 185, 93, 20, 51,
	117, 42, 240, 213, 162, 186,
	190, 12, 147, 105, 236, 196, 224, 36, 31, 18,
	72, 129, 198, 250, 200, 60, 79, 247, 233, 166,
	90, 178, 102, 39, 168, 88, 38, 170, 17, 16,
	151, 232, 219, 47, 169, 206, 229, 92, 52, 192,
	195, 74, 173, 176, 160, 131, 41, 217, 28, 68,
	30, 69, 103, 89, 58, 172, 159, 150, 66, 25,
	11, 167, 221, 56, 238, 255, 189, 110, 59, 80,
	109, 153, 203, 82, 228, 130, 209, 1, 237, 156,
	252, 242, 215, 4, 101, 8, 152, 29, 23, 62,
	124, 161, 253, 174, 33, 53, 86, 22, 146, 205,
	123, 106, 120, 235, 114, 2, 91, 234, 121, 197,
	135, 84, 81, 164, 183, 137, 5, 98, 181, 225,
	179, 44, 71, 216, 6, 32, 97, 115, 149, 24,
	175, 223, 99, 14, 111, 37, 154, 77, 95, 220,
	75, 112, 70, 40, 208, 171, 212, 19, 55, 226,
	9, 254, 113, 46, 187, 207, 133, 48, 241, 7,
	87, 65, 230, 10, 163, 108, 243, 143, 96, 127,
	125, 3, 227, 182, 61, 132, 140, 244, 116, 104,
	248, 126, 201, 246, 145, 177, 180, 26, 73, 214,
	78, 245, 188, 222, 100, 107, 139, 210, 193, 191,
	0, 50, 204, 64, 122, 251, 202, 35, 119, 34,
	15, 67, 157, 76, 27, 239, 118, 194, 83, 199,
	128, 54, 134, 155, 148, 49, 218, 136, 211, 21,
	94, 43, 231, 141, 13, 165, 144, 85, 45, 63,
	158, 249, 142, 184, 138, 57, 185, 93, 20, 51,
	117, 42, 240, 213, 162, 186
};

inline float fade(float x)
{
	return x*x*x*(x*(x * 6.0f - 15.0f) + 10.0f);
}

float noise3D(float Xp, float Yp, float Zp)
{
	int x = int(floor(Xp)) & int(255);
	int y = int(floor(Yp)) & int(255);
	int z = int(floor(Zp)) & int(255);

	//Calculate cell position
	float X = Xp - x;
	float Y = Yp - y;
	float Z = Zp - z;

	int indices[8];
	indices[0] = permutation_table[x + permutation_table[y + permutation_table[z]]] % NUM_GRADIENTS;
	indices[1] = permutation_table[x + permutation_table[y + permutation_table[z + 1]]] % NUM_GRADIENTS;
	indices[2] = permutation_table[x + permutation_table[y + 1 + permutation_table[z]]] % NUM_GRADIENTS;
	indices[3] = permutation_table[x + permutation_table[y + 1 + permutation_table[z + 1]]] % NUM_GRADIENTS;
	indices[4] = permutation_table[x + 1 + permutation_table[y + permutation_table[z]]] % NUM_GRADIENTS;
	indices[5] = permutation_table[x + 1 + permutation_table[y + permutation_table[z + 1]]] % NUM_GRADIENTS;
	indices[6] = permutation_table[x + 1 + permutation_table[y + 1 + permutation_table[z]]] % NUM_GRADIENTS;
	indices[7] = permutation_table[x + 1 + permutation_table[y + 1 + permutation_table[z + 1]]] % NUM_GRADIENTS;

	float grad[8];
	grad[0] = dot(gradients[indices[0]], float3(X, Y, Z));
	grad[1] = dot(gradients[indices[1]], float3(X-1, Y, Z));
	grad[2] = dot(gradients[indices[2]], float3(X, Y-1, Z));
	grad[3] = dot(gradients[indices[3]], float3(X-1, Y-1, Z));
	grad[4] = dot(gradients[indices[4]], float3(X, Y, Z-1));
	grad[5] = dot(gradients[indices[5]], float3(X-1, Y, Z-1));
	grad[6] = dot(gradients[indices[6]], float3(X, Y-1, Z-1));
	grad[7] = dot(gradients[indices[7]], float3(X-1, Y-1, Z-1));

	float3 f = float3(fade(X), fade(Y), fade(Z));
	
	return lerp(lerp(lerp(grad[0], grad[1], f.x), lerp(grad[2], grad[3], f.x), f.y), lerp(lerp(grad[4], grad[5], f.x), lerp(grad[6], grad[7], f.x), f.y), f.z);
}

float Noise3D(float3 x, float frequency, float persistence, uint numOctaves)
{
	float ret = 0.0f;
	float mmax = (pow(persistence, float(numOctaves)) - 1) / (persistence - 1);
	float amplitude = 1.0f;

	for (uint i = 0u; i<numOctaves; ++i)
	{
		ret += amplitude * noise3D(x.x*frequency, x.y*frequency, x.z*frequency);
		frequency *= 1.0f / persistence;
		amplitude *= persistence;
	}

	return ret / mmax;
}
//*/


//DORMONS
float Noise(in uint x)
{
	x += 10000;
	x = x*(x*(x*(x*(x + 1u) + 1u) + 1u) + 1u);
	return float(x) / float(0xffffffffu);///4294967296.;
}
float Noise(in uint3 c)
{
	return Noise((((c.z << 10) + c.y) << 11) + c.x);
}

float Noise(in uint4 c)
{
	return Noise((((((c.w << 8) + c.z) << 8) + c.y) << 8) + c.x);
}

float Noise(float3 x)
{
	x = ((x % 128) + 128) % 128;

	uint3 i = uint3(x);
	float3  t = x - i;
	float4  x0 = float4(Noise(i + uint3(0, 0, 0)), Noise(i + uint3(0, 1, 0)), Noise(i + uint3(0, 0, 1)), Noise(i + uint3(0, 1, 1)));
	float4  x1 = float4(Noise(i + uint3(1, 0, 0)), Noise(i + uint3(1, 1, 0)), Noise(i + uint3(1, 0, 1)), Noise(i + uint3(1, 1, 1)));
	float4  x01 = lerp(x0, x1, t.x);
	float2  x0123 = lerp(x01.xz, x01.yw, t.y);
	return lerp(x0123.x, x0123.y, t.z);
}

float Noise(float4 x)
{
	uint4 i = uint4(x);
	float4 t = x - i;
	float4 x0 = float4(Noise(i + uint4(0, 0, 0, 0)), Noise(i + uint4(0, 1, 0, 0)), Noise(i + uint4(0, 0, 1, 0)), Noise(i + uint4(0, 1, 1, 0)));
	float4 x1 = float4(Noise(i + uint4(1, 0, 0, 0)), Noise(i + uint4(1, 1, 0, 0)), Noise(i + uint4(1, 0, 1, 0)), Noise(i + uint4(1, 1, 1, 0)));
	float4 y0 = float4(Noise(i + uint4(0, 0, 0, 1)), Noise(i + uint4(0, 1, 0, 1)), Noise(i + uint4(0, 0, 1, 1)), Noise(i + uint4(0, 1, 1, 1)));
	float4 y1 = float4(Noise(i + uint4(1, 0, 0, 1)), Noise(i + uint4(1, 1, 0, 1)), Noise(i + uint4(1, 0, 1, 1)), Noise(i + uint4(1, 1, 1, 1)));
	float4 x01 = lerp(x0, x1, t.x);
	float4 y01 = lerp(y0, y1, t.x);
	float2 x0123 = lerp(x01.xz, x01.yw, t.y);
	float2 y0123 = lerp(y01.xz, y01.yw, t.y);
	float2 xy = lerp(float2(x0123.x, y0123.x), float2(x0123.y, y0123.y), t.z);
	return lerp(xy.x, xy.y, t.w);
}

float Noise3D(float3 x, float frequency, float persistence, uint numOctaves)
{
	//x += 1000;
	float ret = 0.0f;
	float mmax = (pow(persistence, float(numOctaves)) - 1) / (persistence - 1);
	float amplitude = 1.0f;
	
	for (uint i = 0u; i<numOctaves; ++i)
	{
		ret += amplitude * Noise(x*frequency+64);
		frequency *= 1.0f / persistence;
		amplitude *= persistence;
	}

	return ret / mmax;
}

float Noise4D(float4 x, float f, float p, uint N)
{
	float ret = 0;
	float mmax = (pow(p, float(N)) - 1) / (p - 1);
	float a = 1;
	
	for (uint i = 0u; i<N; ++i)
	{
		ret += a*Noise(x*f);
		f *= 1. / p;
		a *= p;
	}

	return ret / mmax;
}
//*/

/*
//Hugo Elias Version
// http://freespace.virgin.net/hugo.elias/models/m_perlin.htm
float Noise1(int x)
{
	//x = ((x << 13) ^ x);
	x = int((x % 1024.0f) + 1024.0f) % 1024.0f;
	return (x*x*(x*x*(x*x*5.8f - 15.0f) + 10.0f))/;

	//return (1.0 - ((x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float Noise(uint3 c)
{
	return Noise1((((c.z << 10) + c.y) << 11) + c.x);
}

float Noise(float3 x)
{
	uint3 i = uint3(x);
	float3  t = x - i;
	float4  x0 = float4(Noise(i + uint3(0, 0, 0)), Noise(i + uint3(0, 1, 0)), Noise(i + uint3(0, 0, 1)), Noise(i + uint3(0, 1, 1)));
	float4  x1 = float4(Noise(i + uint3(1, 0, 0)), Noise(i + uint3(1, 1, 0)), Noise(i + uint3(1, 0, 1)), Noise(i + uint3(1, 1, 1)));
	float4  x01 = lerp(x0, x1, t.x);
	float2  x0123 = lerp(x01.xz, x01.yw, t.y);
	return lerp(x0123.x, x0123.y, t.z);
}

float Noise3D(float3 x, float frequency, float persistence, uint numOctaves)
{
	float ret = 0.0f;
	float mmax = (pow(persistence, float(numOctaves)) - 1) / (persistence - 1);
	float amplitude = 1.0f;

	for (uint i = 0u; i<numOctaves; ++i)
	{
		ret += amplitude * Noise(x*frequency);
		frequency *= 1.0f / persistence;
		amplitude *= persistence;
	}

	return ret / mmax;
}
//*/

//VS
GS_POINT vertex_shader(VS_INPUT input)
{
	/*
	PS_INPUT output;
	output.pos = mul(input.position, mvp);
	output.texcoord = input.texcoord;

	return output;
	*/
	GS_POINT o;
	o.pos = float4(0, 0, 0, 0);
	return o;
}

//GS
[maxvertexcount(4)]
void geometry_shader(point GS_POINT input[1], inout TriangleStream<GS_POINT> outputStream)
{
	GS_POINT arr[4];
	arr[0].pos = float4(-1, -1, 0, 1);
	arr[1].pos = float4(-1, 1, 0, 1);
	arr[2].pos = float4(1, 1, 0, 1);
	arr[3].pos = float4(1, -1, 0, 1);

	outputStream.Append(arr[0]);
	outputStream.Append(arr[1]);
	outputStream.Append(arr[3]);
	outputStream.Append(arr[2]);
}


float3 calcNormal(float2 p)
{
	float xn = Noise3D(float3(p.x - 1.0f, 10*time, p.y), 0.2f, 0.5f, 4);
	float xp = Noise3D(float3(p.x + 1.0f, 10*time, p.y), 0.2f, 0.5f, 4);
	float zn = Noise3D(float3(p.x, 10*time, p.y - 1.0f), 0.2f, 0.5f, 4);
	float zp = Noise3D(float3(p.x, 10*time, p.y + 1.0f), 0.2f, 0.5f, 4);

	return normalize(float3(-(xp - xn), 1, -(zp - zn)));
}

PS_OUT pixel_shader_perlin(GS_POINT input)
{
	PS_OUT o;
	o.position = input.pos;
	o.normal = float4(0, 0, 0, 0);
	
	float4 cameraPos = float4(cameraPosX, cameraPosY, cameraPosZ, cameraPosW);
	float4 NDC = normalize(float4(-1.0f + (2.0f * input.pos.xy) / float2(screenWidth, screenHeight), 1, 1));
	NDC.y *= -1.0f; //Y in NDC raises oppositely to screen space

	float3 world = mul(NDC, inverseVP);
	float3 view = normalize(world.xyz);

	float t = (CLOUD_HEIGHT - cameraPos.y) / view.y;
	t = max(t, 0);

	float4 skyColor = lerp(float4(1, 1, 1, 1), float4(0, 0, 1, 1), pow(abs(view.y), 0.2f));

	if (t > 0)
	{
		float2 coords = cameraPos.xz + view.xz*t;
		float opacity = Noise3D(float3(coords-64, 0), 0.005, 0.5, 8);
		o.color = lerp(skyColor, float4(1, 1, 1, 1), opacity);
		//o.color = float4(opacity, opacity, opacity, 1);
	}
	else //sea
	{
		float t_sea = -cameraPos.y / view.y;
		float2 coords = cameraPos.xz + view.xz*t_sea;

		float3 normal = calcNormal(coords);

		const float3 sunPos = float3(0, 5000, 0);
		float lightAmount = dot(normalize(sunPos - float3(coords.x, 0, coords.y)), normal);

		o.color = lightAmount * float4(0.04f, 0.411f, 0.58f, 1.0f);
	}

	return o;

	
	//float3 samplePointCoord = float3(cameraPos.x + t*

	/*
	if (t == 0)
		o.color = float4(1,0,0,1);
	else
		o.color = float4(0,1,0,1);
	
	return o;
	*/

	/*
	float r = 0.0f;
	int i = 0;
	float frequency;
	float amplitude;
	r = Noise3D(float3(255.0f * input.pos.xy, 10 * time), 0.2, 0.5, 2);
	
	float cloudCover = 0.4;
	float cloudSharpness = 0.05;
	float c = max(r - cloudCover, 0);

	float f = 1 - pow(cloudSharpness, c);

	o.color = float4(f, f, f, 1);

	//o.color = float4(r, r, r, 1);

	//r = Noise3D(float3(255 * input.texcoord.xy, 10 * time[0]), 0.01, 0.5, 20);
	//o.color = float4(lerp(float3(0, 0, 1), float3(1, 1, 1), pow(input.texcoord.y + r,3)), 1);

	return o;*/
}
