#include "ShaderCompiler.h"

bool CreatePShader(ID3D11Device* dev, char* filename,char* shaderName, ID3D11PixelShader** PxShader)
{
	ID3DBlob* psBuffer = 0;
	HRESULT d3dResult;
	bool compileResult;

	//Kompilacia PS
	compileResult = CompileD3DShader( filename, shaderName, "ps_5_0", &psBuffer );

	if( compileResult == false )
	{
        std::cerr << "CreatePixelShader - compilation failed!\n";
		return false;
	}

	//Pixel shader objekt
	d3dResult = dev->CreatePixelShader( psBuffer->GetBufferPointer( ),
		psBuffer->GetBufferSize( ), 0, PxShader );

    psBuffer->Release();

	if( FAILED( d3dResult ) )
	{
		std::cerr << "CreatePixelShader failed!\n";
		
		return false;
	}

	return true;
}

//Vytvori objekt Geometry Shadera
bool CreateGShader(ID3D11Device* dev, char* filename,char* shaderName, ID3D11GeometryShader** GShader)
{
	ID3DBlob* psBuffer = 0;
	HRESULT d3dResult;
	bool compileResult;

	//Kompilacia PS
	compileResult = CompileD3DShader( filename, shaderName, "gs_5_0", &psBuffer );

	if( compileResult == false )
	{
		std::cerr << "Geometry shader compilation error\n";
		return false;
	}

	//Shader objekt
	d3dResult = dev->CreateGeometryShader( psBuffer->GetBufferPointer( ),
		psBuffer->GetBufferSize( ), 0, GShader );

    psBuffer->Release();

	if( FAILED( d3dResult ) )
	{
		std::cerr << "CreateGeometryShader failed!\n";
		return false;
	}

	return true;
}

//Vytvori objekt Vertex Shadera
//Ak je zadany createLayout parameter, vytvori tiez layout
bool CreateVShaderAndLayout(ID3D11Device* dev, char* filename,char* shaderName, ID3D11VertexShader** shader, ID3D11InputLayout** layout, D3D11_INPUT_ELEMENT_DESC* desc, unsigned int desc_size)
{
	ID3DBlob* vsBuffer = NULL;
	HRESULT d3dResult;

	//Kompilacia shadera
	bool compileResult = CompileD3DShader( filename, shaderName, "vs_5_0", &vsBuffer );

	if( compileResult == false )
	{
        std::cerr << "VS Shader compilation error\n";
		return false;
	}

	//Vytvorenie shader objektu
	d3dResult = dev->CreateVertexShader( vsBuffer->GetBufferPointer( ),
		vsBuffer->GetBufferSize( ), 0, shader );

	if( FAILED( d3dResult ) )
	{
		if( vsBuffer )
			vsBuffer->Release( );
		std::cerr << "CreateVertexShader failed!\n";
		return false;
	}

	//Vytvorenie VS Layoutu
	if(desc)
	{
		unsigned int totalLayoutElements = desc_size;;

		d3dResult = dev->CreateInputLayout( desc, totalLayoutElements,
			vsBuffer->GetBufferPointer( ), vsBuffer->GetBufferSize( ), layout );

		if( FAILED( d3dResult ) )
		{
			std::cerr << "CreateInputLayout failed!\n";
            vsBuffer->Release();
			return false;
		}
	}

    vsBuffer->Release();

	return true;
}

bool CreateCShader(ID3D11Device* dev, char* filename,char* shaderName, ID3D11ComputeShader** CShader)
{
	ID3DBlob* psBuffer = 0;
	HRESULT d3dResult;
	bool compileResult;

	//Kompilacia PS
	char* pProfile = ( dev->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0 ) ? "cs_5_0" : "cs_4_0";
	compileResult = CompileD3DShader( filename, shaderName, pProfile, &psBuffer );

	if( compileResult == false )
	{
		std::cerr << "Compute shader compilation error!\n";
		return false;
	}

	//Shader objekt
	d3dResult = dev->CreateComputeShader( psBuffer->GetBufferPointer( ),
		psBuffer->GetBufferSize( ), 0, CShader );

    psBuffer->Release();

	if( FAILED( d3dResult ) )
	{
		std::cerr << "CreateComputeShader failed!\n";
		
		return false;
	}

	return true;
}


//Compiles shader
bool CompileD3DShader(char* filePath, char* entry, char* shaderModel, ID3DBlob** buffer )
{
#ifdef _DEBUG
	DWORD shaderFlags = D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;// | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#else
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	ID3DBlob* errorBuffer = 0;
	HRESULT result;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wFilePath = converter.from_bytes(filePath);

	result = D3DCompileFromFile( wFilePath.c_str(), NULL, NULL, entry, shaderModel,
		shaderFlags, 0, buffer, &errorBuffer);

	if( FAILED( result ) )
	{
		if( errorBuffer != 0 )
		{
            std::cerr << std::string((char*)errorBuffer->GetBufferPointer()) << std::endl;
			errorBuffer->Release( );
		}

		return false;
	}

	if( errorBuffer != 0 )
		errorBuffer->Release( );

	return true;
}