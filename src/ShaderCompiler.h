#pragma once

#include <d3dcompiler.h>
#include <d3d11.h>

#include <iostream>
#include <locale>
#include <codecvt>
#include <string>

//Funkcie pre kompilaciu shaderov
bool CompileD3DShader(char* filePath, char* entry, char* shaderModel, ID3DBlob** buffer );

bool CreatePShader(ID3D11Device* dev, char* filename,char* shaderName, ID3D11PixelShader** shader);
bool CreateGShader(ID3D11Device* dev, char* filename,char* shaderName, ID3D11GeometryShader** shader);
bool CreateCShader(ID3D11Device* dev, char* filename,char* shaderName, ID3D11ComputeShader** CShader);
bool CreateVShaderAndLayout(ID3D11Device* dev, char* filename,char* shaderName, ID3D11VertexShader** shader, ID3D11InputLayout** layout, D3D11_INPUT_ELEMENT_DESC* desc, unsigned int desc_size);