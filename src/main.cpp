#include <Windows.h>

#include <d3d11.h>
#include <dxgi1_2.h>

#include <string>
#include <iostream>
#include <vector>

#include "ShaderCompiler.h"
#include "Camera.h"
#include "KMInputSingleton.h"


#include <IL\il.h>


//#define USE_MAP
#define USE_PERLIN

#define USE_MULTIGPU

#ifdef USE_MULTIGPU

//Slave GPU Thread handle and ID
HANDLE                    g_hSlaveThread;
DWORD					  g_dwSlaveThreadID;
#endif

//Global windows handle and instance
HWND                      g_hWnd;
HMODULE                   g_hInstance;
bool                      g_isWindowActive = true;

//CUDA stuff
//CUdevice				  g_CudaDevice;
//CUcontext				  g_CudaContext;

//ID3D11Device*             g_pDevice = nullptr;
//ID3D11DeviceContext*      g_pContext = nullptr;
IDXGISwapChain*           g_pSwapChain = nullptr;

ID3D11RenderTargetView*   g_pRenderTargetView = nullptr;
ID3D11DepthStencilView*   g_pDepthStencilView = nullptr;
D3D11_VIEWPORT            g_viewPort;

//Scene
ID3D11Buffer*             g_pVBO = nullptr;
ID3D11Buffer*             g_pIBO = nullptr;
ID3D11Buffer*             g_pMatrices = nullptr;
ID3D11Buffer*			  g_pUniforms = nullptr;
ID3D11PixelShader*        g_pPS = nullptr;
ID3D11GeometryShader*	  g_pGS = nullptr;
ID3D11VertexShader*       g_pVS = nullptr;
ID3D11InputLayout*        g_pInputLayout = nullptr;
float					  g_fBuf[8] = { 0.0f };

#ifndef USE_PERLIN
ID3D11Texture2D*          g_pTexture = nullptr;
ID3D11ShaderResourceView* g_pTextureSRV = nullptr;

unsigned char*            g_pTextureRawData[2] = { nullptr, nullptr };
unsigned int              g_iRowPitch = 0;
ID3D11SamplerState*       g_pTextureSampler = nullptr;
#endif

HRTimer					  g_Timer;

//Deferred shading
ID3D11InputLayout*        g_pDeferredInputLayout = nullptr;
ID3D11Texture2D*          g_pNormaltexture = nullptr;
ID3D11Texture2D*          g_pPositiontexture = nullptr;
ID3D11Texture2D*          g_pColortexture = nullptr;

ID3D11SamplerState*       g_pSampler = nullptr;

ID3D11RenderTargetView*   g_pMRT[3];
ID3D11ShaderResourceView* g_pShaderResourceViewMRT[3];

ID3D11Buffer*             g_pVBODeffered = nullptr;

ID3D11PixelShader*        g_pPSDeferred = nullptr;
ID3D11VertexShader*       g_pVSDeferred = nullptr;
ID3D11GeometryShader*     g_pGSDeferred = nullptr;

//Camera
Camera*                   g_pCamera = nullptr;

//Input processor
KMInputSingleton*         g_pKMInput = nullptr;

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define WM_USER_POOL  0x7FFF //MAX number of user message typedefs

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

#ifdef USE_MULTIGPU
	if (message < WM_USER)
	{
#endif
		switch (message)
		{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SYSCOMMAND:
			if ((wParam & 0xFFF0) == SC_MINIMIZE)
			{
				//g_isWindowActive = false;
				std::cout << "Minimalizujem sa\n";
				break;
			}
			else if ((wParam & 0xFFF0) == SC_RESTORE)
			{
				g_isWindowActive = true;
				std::cout << "Obnovujem sa\n";
				break;
			}
			break;

		case WM_SETFOCUS:
			std::cout << "Mam Focus\n";
			g_isWindowActive = true;
			break;

		case WM_KILLFOCUS:
			std::cout << "Stracam Focus\n";
			//g_isWindowActive = false;
			g_pCamera->ResetButtons();
			g_pKMInput->resetMouseButtons();
			break;

		case WM_INPUT:
		{
			if (!g_isWindowActive)
				break;

			const KMInputSingleton::KMInputMessage* msg = g_pKMInput->processInputMessage(lParam);

			if (msg != nullptr)
			{
				//Keyboard
				if (msg->messageType == KMInputSingleton::KMINPUT_MESSAGE_KEYBOARD)
				{
					if (msg->keyboard.state == KMInputSingleton::KMINPUT_KEY_PRESSED)
					{
						switch (msg->keyboard.key)
						{
						case Keys::W:
							g_pCamera->HandleInputMessage(Camera::CAMERA_FORWARD_DOWN);
							break;
						case Keys::S:
							g_pCamera->HandleInputMessage(Camera::CAMERA_BACKWARD_DOWN);
							break;
						case Keys::A:
							g_pCamera->HandleInputMessage(Camera::CAMERA_LEFT_DOWN);
							break;
						case Keys::D:
							g_pCamera->HandleInputMessage(Camera::CAMERA_RIGHT_DOWN);
							break;
						case Keys::SPACE:
							g_pCamera->HandleInputMessage(Camera::CAMERA_UP_DOWN);
							break;
						case Keys::CONTROL_LEFT:
							g_pCamera->HandleInputMessage(Camera::CAMERA_DOWN_DOWN);
							break;
						}
					}
					else
					{
						switch (msg->keyboard.key)
						{
						case Keys::W:
							g_pCamera->HandleInputMessage(Camera::CAMERA_FORWARD_UP);
							break;
						case Keys::S:
							g_pCamera->HandleInputMessage(Camera::CAMERA_BACKWARD_UP);
							break;
						case Keys::A:
							g_pCamera->HandleInputMessage(Camera::CAMERA_LEFT_UP);
							break;
						case Keys::D:
							g_pCamera->HandleInputMessage(Camera::CAMERA_RIGHT_UP);
							break;
						case Keys::SPACE:
							g_pCamera->HandleInputMessage(Camera::CAMERA_UP_UP);
							break;
						case Keys::CONTROL_LEFT:
							g_pCamera->HandleInputMessage(Camera::CAMERA_DOWN_UP);
							break;
						case Keys::ESCAPE:
							PostQuitMessage(0);
							break;
						}
					}
				}
				else if (msg->messageType == KMInputSingleton::KMINPUT_MESSAGE_MOUSE)
				{
					//Process only when in client area
					//ZATIAL ciastocny fix, len zaporne kooordinaty
					POINT p;
					GetCursorPos(&p);
					ScreenToClient(hWnd, &p);

					if (msg->mouse.left == KMInputSingleton::KMINPUT_MOUSE_BUTTON_DOWN && p.x >= 0 && p.y >= 0)
						g_pCamera->AdjustOrientation(0.01f*msg->mouse.deltaX, 0.01f*msg->mouse.deltaY);
				}
			}

			break;
		}
		}
#ifdef USE_MULTIGPU
	}
	else if (message > WM_USER && message < WM_USER_POOL)
	{
		std::cout << "MASTER - Message: " << threadMsgToString(message) << "\n";
		
		switch (message)
		{
		case TMSG_SLAVE_INIT_FAILED:
			std::wcerr << "MASTER - SLAVE INITIALIZATION FAILED!\n";
			break;
		case TMSG_SLAVE_INIT_OK:
			std::wcout << "MASTER - SLAVE INITIALIZATION OK!\n";
			break;
		}
	}
#endif

    return DefWindowProc(hWnd, message, wParam, lParam);
}

bool initWindowFromConsole(HWND console_hWnd, HINSTANCE program_instance)
{
    //Register windows class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = (WNDPROC)WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = program_instance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = TEXT("DGPUClass");
    wcex.hIconSm = 0;
    if (!RegisterClassEx(&wcex))
        return false;

    RECT console_rect = { 0, 0, 0, 0 };
    GetWindowRect(console_hWnd, &console_rect);

    // Create window
    RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        L"DGPUClass",
        L"DualGpuTestApp",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        console_rect.left + (console_rect.right - console_rect.left),
        console_rect.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        nullptr,
        nullptr,
        program_instance,
        nullptr);

    if (!g_hWnd)
        return false;

    ShowWindow(g_hWnd, SW_SHOWNORMAL);

    g_pKMInput = KMInputSingleton::getInstance();
    
    if (g_pKMInput == nullptr)
    {
        std::cerr << "Cannot initialize raw input!\n";
        return false;
    }

    g_pKMInput->setKeyboardMode(KMInputSingleton::KMIMPUT_KEYBOARD_MODE_SINGLE_DOWN);

    return true;
}

bool initD3D()
{
    IDXGIFactory1 * pFactory = nullptr;

    CreateDXGIFactory1(/*0,*/ __uuidof(IDXGIFactory1), (void**)&pFactory);

    UINT i = 0;
    IDXGIAdapter1 * pAdapter = nullptr;
    std::vector<IDXGIAdapter1*> vAdapters;
    std::vector<unsigned int> vSoftwareAdapters;

    while (pFactory->EnumAdapters1(i, &pAdapter) != DXGI_ERROR_NOT_FOUND)
    {
        vAdapters.push_back(pAdapter);
        ++i;
    }

    const unsigned int numAdapters = (int)vAdapters.size();
    for (unsigned int i = 0; i < numAdapters; ++i)
    {
        DXGI_ADAPTER_DESC1 adapterDesc;
        vAdapters[i]->GetDesc1(&adapterDesc);

        std::wcout << i << L": " << adapterDesc.Description;
        if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            std::cout << " (software adapter!)";

        std::cout << std::endl;

        unsigned int j = 0;
        IDXGIOutput1 * output = nullptr;
        std::vector<IDXGIOutput1*> outputs;
        while (vAdapters[i]->EnumOutputs(j, (IDXGIOutput**)&output) != DXGI_ERROR_NOT_FOUND)
        {
            outputs.push_back(output);
            ++j;
        }

        std::cout << "   Attached monitors:\n";
        const unsigned int o_size = (unsigned int)outputs.size();
        for (unsigned int k = 0; k < o_size; ++k)
        {
            DXGI_OUTPUT_DESC out_desc;
            outputs[k]->GetDesc(&out_desc);

            std::wcout << L"      " << out_desc.DeviceName << std::endl;
        }
    }

    if ((numAdapters < 2) && numAdapters!=0)
    {
        std::cerr << "ERROR Only " << numAdapters << " found!\n";
        return false;
    }

    //Select 0 as primary, 1 as secondary
    IDXGIAdapter * selectedAdapter = vAdapters[0];

    //Create device and context
    D3D11_CREATE_DEVICE_FLAG flags;
#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#else
    flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif

    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL tmp = D3D_FEATURE_LEVEL_11_0;
    HRESULT ret = D3D11CreateDevice(selectedAdapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        flags,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &g_pDevice,
        &tmp,
        &g_pContext);

    if (FAILED(ret))
    {
        std::cout << "Context Creation failed! " << ret << std::endl;
        return false;
    }

	//CUDA context 
	/*
	CUresult r;
	int q;
	CUdevice s;
	cudaGetDeviceCount(&q);

	cudaD3D11GetDevice(&g_CudaDevice, selectedAdapter);
	cudaD3D11GetDevice(&s, vAdapters[1]);

	cudaDeviceProp props;
	cudaGetDeviceProperties(&props, g_CudaDevice);

	int a, b;
	cudaError_t err;

	err = cudaSetDevice(g_CudaDevice);
	err = cudaDeviceEnablePeerAccess(s, 0);

	err = cudaSetDevice(s);
	err = cudaDeviceEnablePeerAccess(g_CudaDevice, 0);

	std::cout << cudaGetErrorString(err);

	err = cudaDeviceCanAccessPeer(&a, g_CudaDevice, s);
	err = cudaDeviceCanAccessPeer(&b, s, g_CudaDevice);
	*/
#ifdef USE_MULTIGPU
	//Spawn slave GPU thread
	threadParam tp;
	tp.adapter = vAdapters[1];
	tp.masterThreadID = GetWindowThreadProcessId(g_hWnd, nullptr);
	tp.texWidth = 800;
	tp.texHeight = 800;

    g_hSlaveThread = CreateThread(nullptr, 0, SlaveGPUThreadMain, &tp, 0, nullptr);

	g_dwSlaveThreadID = GetThreadId(g_hSlaveThread);
#endif

    //Swap chain
    DXGI_SWAP_CHAIN_DESC sc_desc;
    ZeroMemory(&sc_desc, sizeof(sc_desc));
    sc_desc.BufferCount = 2;
    sc_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sc_desc.BufferDesc.Height = WINDOW_HEIGHT;
    sc_desc.BufferDesc.RefreshRate.Numerator = 0; //SEM cez vsync
    sc_desc.BufferDesc.RefreshRate.Denominator = 1;
    sc_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sc_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sc_desc.BufferDesc.Width = WINDOW_WIDTH;
    sc_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sc_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; //potom destruktor musi prepnut do windowed
    sc_desc.OutputWindow = g_hWnd;
    sc_desc.SampleDesc.Count = 1; //Multisamplig
    sc_desc.SampleDesc.Quality = 0;
    sc_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sc_desc.Windowed = true;

    ret = pFactory->CreateSwapChain(g_pDevice, &sc_desc, &g_pSwapChain);
    if (FAILED(ret))
    {
        std::cout << "Swap Chain Creation Failed!\n";
        return false;
    }

    //Cleaning DXGI objects
    unsigned int p = (unsigned int)vAdapters.size();
    for (unsigned int i = 0; i < p; ++i)
    {
        vAdapters[i]->Release();
    }

    pFactory->Release();

    //Create default render target
    ID3D11Texture2D* backBufferTexture;
    ret = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferTexture);

    if (FAILED(ret))
    {
        std::cout << "Could not acquire backbuffer texture!\n";
        return false;
    }

    ret = g_pDevice->CreateRenderTargetView(backBufferTexture, 0, &g_pRenderTargetView);

    if (backBufferTexture)
        backBufferTexture->Release();

    if (FAILED(ret))
    {
        std::cout << "Could not create RenderTargetView!\n";
        return false;
    }

    //Create DepthStencilView
    D3D11_TEXTURE2D_DESC depthTexDesc;
    ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
    depthTexDesc.Width = WINDOW_WIDTH;
    depthTexDesc.Height = WINDOW_HEIGHT;
    depthTexDesc.MipLevels = 1;
    depthTexDesc.ArraySize = 1;
    depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthTexDesc.SampleDesc.Count = 1;
    depthTexDesc.SampleDesc.Quality = 0;
    depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
    depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthTexDesc.CPUAccessFlags = 0;
    depthTexDesc.MiscFlags = 0;

    ID3D11Texture2D * depthTexture;
    ret = g_pDevice->CreateTexture2D(&depthTexDesc, nullptr, &depthTexture);

    if (FAILED(ret))
    {
        std::cout <<  "Creating depth texture failed!\n";
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = depthTexDesc.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;

    ret = g_pDevice->CreateDepthStencilView(depthTexture, &descDSV, &g_pDepthStencilView);

    if (FAILED(ret))
    {
        std::cout << "DepthStencilView creation failed!\n";
        return false;
    }

    depthTexture->Release();

    //Viewport
    ZeroMemory(&g_viewPort, sizeof(g_viewPort));
    g_viewPort.Width = WINDOW_WIDTH;
    g_viewPort.Height = WINDOW_HEIGHT;
    g_viewPort.MinDepth = 0.0f;
    g_viewPort.MaxDepth = 1.0f;
    g_viewPort.TopLeftX = 0.0f;
    g_viewPort.TopLeftY = 0.0f;

    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    //Create MRT
    //Textures
    ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
    depthTexDesc.Width = WINDOW_WIDTH;
    depthTexDesc.Height = WINDOW_HEIGHT;
    depthTexDesc.MipLevels = 1;
    depthTexDesc.ArraySize = 1;
    depthTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    depthTexDesc.SampleDesc.Count = 1;
    depthTexDesc.SampleDesc.Quality = 0;
    depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
    depthTexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    depthTexDesc.CPUAccessFlags = 0;
    depthTexDesc.MiscFlags = 0;

    ret = g_pDevice->CreateTexture2D(&depthTexDesc, nullptr, &g_pNormaltexture);

    if (FAILED(ret))
    {
        std::cout << "Normal texture creation failed!\n";
        return false;
    }

    ret = g_pDevice->CreateTexture2D(&depthTexDesc, nullptr, &g_pPositiontexture);

    if (FAILED(ret))
    {
        std::cout << "Position texture creation failed!\n";
        return false;
    }

    ret = g_pDevice->CreateTexture2D(&depthTexDesc, nullptr, &g_pColortexture);

    if (FAILED(ret))
    {
        std::cout << "Color texture creation failed!\n";
        return false;
    }

    //Bind to MRTs
    //0 - position
    //1 - normal
    //2 - color
    
    D3D11_RENDER_TARGET_VIEW_DESC rtd;
    ZeroMemory(&rtd, sizeof(rtd));
    rtd.Texture2D.MipSlice = 0;
    rtd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    ret = g_pDevice->CreateRenderTargetView(g_pPositiontexture, &rtd, &g_pMRT[0]);

    if (FAILED(ret))
    {
        std::cout << "RT Position creation failed!\n";
        return false;
    }

    ret = g_pDevice->CreateRenderTargetView(g_pNormaltexture, &rtd, &g_pMRT[1]);

    if (FAILED(ret))
    {
        std::cout << "RT Normal creation failed!\n";
        return false;
    }

    ret = g_pDevice->CreateRenderTargetView(g_pColortexture, &rtd, &g_pMRT[2]);

    if (FAILED(ret))
    {
        std::cout << "RT Color creation failed!\n";
        return false;
    }

    //Bind to SRVs
    D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    ZeroMemory(&srvd, sizeof(srvd));
    srvd.Format = depthTexDesc.Format;
    srvd.Texture2D.MipLevels = 1;
    srvd.Texture2D.MostDetailedMip = 0;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

    ret = g_pDevice->CreateShaderResourceView(g_pPositiontexture, &srvd, &g_pShaderResourceViewMRT[0]);

    if (FAILED(ret))
    {
        std::cout << "SRV Position creation failed!\n";
        return false;
    }

    ret = g_pDevice->CreateShaderResourceView(g_pNormaltexture, &srvd, &g_pShaderResourceViewMRT[1]);

    if (FAILED(ret))
    {
        std::cout << "SRV Normal creation failed!\n";
        return false;
    }

    ret = g_pDevice->CreateShaderResourceView(g_pColortexture, &srvd, &g_pShaderResourceViewMRT[2]);

    if (FAILED(ret))
    {
        std::cout << "SRV Color creation failed!\n";
        return false;
    }

    //Shaders for final pass
    D3D11_INPUT_ELEMENT_DESC idesc;
    //position
    idesc.SemanticName = "POSITION";
    idesc.SemanticIndex = 0;
    idesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    idesc.InputSlot = 0;
    idesc.AlignedByteOffset = 0;
    idesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    idesc.InstanceDataStepRate = 0;

    if (!CreateVShaderAndLayout(g_pDevice, "shaders\\deferred.hlsl", "vs_quad", &g_pVSDeferred, &g_pDeferredInputLayout, &idesc, 1))
    {
        return false;
    }

    g_pContext->IASetInputLayout(g_pInputLayout);


    if (!CreatePShader(g_pDevice, "shaders\\deferred.hlsl", "ps_quad", &g_pPSDeferred))
    {
        return false;
    }

    if (!CreateGShader(g_pDevice, "shaders\\deferred.hlsl", "gs_quad", &g_pGSDeferred))
    {
        return false;
    }

    //Sampler
    D3D11_SAMPLER_DESC sdesc;
    ZeroMemory(&sdesc, sizeof(sdesc));
    sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sdesc.MaxAnisotropy = 1;
    sdesc.MaxLOD = D3D11_FLOAT32_MAX;
    sdesc.MinLOD = 0;
    sdesc.MipLODBias = 0;

    ret = g_pDevice->CreateSamplerState(&sdesc, &g_pSampler);
    if (FAILED(ret))
    {
        std::cout << "Sampler creation failed!\n";
        return false;
    }

    //Buffer for point as input
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.ByteWidth = 4*sizeof(float);
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

    const float singlePoint[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    D3D11_SUBRESOURCE_DATA bufferData;
    ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
    bufferData.pSysMem = singlePoint;
    bufferData.SysMemPitch = 0;
    bufferData.SysMemSlicePitch = 0;

    ret = g_pDevice->CreateBuffer(&bd, &bufferData, &g_pVBODeffered);

    if (FAILED(ret))
    {
        std::cout << "VBO Deferred creation failed!\n";
        return false;
    }

    //Constant buffer with matrix
    ZeroMemory(&bd, sizeof(bd));
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(XMMATRIX);
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

    ret = g_pDevice->CreateBuffer(&bd, nullptr, &g_pMatrices);

    if (FAILED(ret))
    {
        std::cout << "Matrix cbuffer creation failed!\n";
        return false;
    }

    return true;
}

void initCamera()
{
    g_pCamera = new Camera;
    g_pCamera->SetMovementSpeed(10.0f);
    g_pCamera->SetPerspectiveProjectionLH(90.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 1000.0f);
    g_pCamera->PositionCamera(XMFLOAT3(0, 0, -2), XMFLOAT3(0, 0, 1), XMFLOAT3(0, 1, 0));
}

bool initScene()
{
    //Vertex data
	
    float vertexData[] = {
        -5, -5, 0.5, 1,   1, 0,
        -5,  5, 0.5, 1,   0, 0,
         5,  5, 0.5, 1,   0, 1,
         5, -5, 0.5, 1,   1, 1,
         0,0,0,0,0,0,0,0 //padding, must be multiple of 16
         };

    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	
	float vertexData[] = { 0, 0, 0, 0 };
	unsigned int VBOSize = 4 * sizeof(float);

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = VBOSize;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    bd.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA bufferData;
    ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
    bufferData.pSysMem = vertexData;
    bufferData.SysMemPitch = 0;
    bufferData.SysMemSlicePitch = 0;

    HRESULT ret = g_pDevice->CreateBuffer(&bd, &bufferData, &g_pVBO);
    if (FAILED(ret))
    {
        std::cout << "Failed to allocate VBO!\n";
        return false;
    }

	/*
    //Index Buffer
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.ByteWidth = 6 * sizeof(int);

    bufferData.pSysMem = indices;

    ret = g_pDevice->CreateBuffer(&bd, &bufferData, &g_pIBO);
    if (FAILED(ret))
    {
        std::cout << "Failed to create IBO!\n";
        return false;
    }
    */

	//Uniforms
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = 8*sizeof(float);
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

	ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferData.pSysMem = g_fBuf;
	bufferData.SysMemPitch = 0;
	bufferData.SysMemSlicePitch = 0;

	ret = g_pDevice->CreateBuffer(&bd, &bufferData, &g_pUniforms);
	if (FAILED(ret))
	{
		std::cout << "Failed to allocate Uniforms buffer!\n";
		return false;
	}

    //Shaders
    D3D11_INPUT_ELEMENT_DESC idesc[1];
    //position
    idesc[0].SemanticName = "POSITION";
    idesc[0].SemanticIndex = 0;
    idesc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    idesc[0].InputSlot = 0;
    idesc[0].AlignedByteOffset = 0;
    idesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    idesc[0].InstanceDataStepRate = 0;
    /*
    //texcoord
    idesc[1].SemanticName = "TEXCOORD";
    idesc[1].SemanticIndex = 0;
    idesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    idesc[1].InputSlot = 0;
    idesc[1].AlignedByteOffset = 16;
    idesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    idesc[1].InstanceDataStepRate = 0;*/
    /*
    //Normal
    idesc[2].SemanticName = "NORMAL";
    idesc[2].SemanticIndex = 0;
    idesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    idesc[2].InputSlot = 0;
    idesc[2].AlignedByteOffset = 24;
    idesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    idesc[2].InstanceDataStepRate = 0;*/

#ifdef USE_PERLIN
	if (!CreateVShaderAndLayout(g_pDevice, "shaders\\skybox.hlsl", "vertex_shader", &g_pVS, &g_pInputLayout, idesc, 1))
	{
		return false;
	}

	g_pContext->IASetInputLayout(g_pInputLayout);

	if (!CreateGShader(g_pDevice, "shaders\\skybox.hlsl", "geometry_shader", &g_pGS))
	{
		return false;
	}

	if (!CreatePShader(g_pDevice, "shaders\\skybox.hlsl", "pixel_shader_perlin", &g_pPS))
	{
		return false;
	}
#else
	if (!CreateVShaderAndLayout(g_pDevice, "shaders\\basic.hlsl", "vertex_shader", &g_pVS, &g_pInputLayout, idesc, 2))
	{
		return false;
	}

	g_pContext->IASetInputLayout(g_pInputLayout);


	if (!CreatePShader(g_pDevice, "shaders\\basic.hlsl", "pixel_shader", &g_pPS))
	{
		return false;
	}
#endif

#ifndef USE_PERLIN
    //Texture
    ILuint img;
    ilGenImages(1, &img);
    ilBindImage(img);
    bool isOK = ilLoadImage(L"textures\\506.jpg");
    if (!isOK)
    {
        std::wcout << "Could not open texture file" << std::endl;
        return false;
    }

    isOK = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    if (!isOK)
    {
        std::wcout << "Could not convert texture file" << std::endl;
        return false;
    }

    g_pTextureRawData[0] = new unsigned char[ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 4];
	g_pTextureRawData[1] = new unsigned char[ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 4];
    memcpy(g_pTextureRawData[0], ilGetData(), ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 4);
	memcpy(g_pTextureRawData[1], ilGetData(), ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 4);

    D3D11_TEXTURE2D_DESC tdesc;
    ZeroMemory(&tdesc, sizeof(tdesc));
    tdesc.Width = ilGetInteger(IL_IMAGE_WIDTH);
    tdesc.Height = ilGetInteger(IL_IMAGE_HEIGHT);
    tdesc.MipLevels = 9;
    tdesc.ArraySize = 1;
    tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tdesc.SampleDesc.Count = 1;
    tdesc.SampleDesc.Quality = 0;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
#ifdef USE_MAP
	tdesc.Usage = D3D11_USAGE_DYNAMIC;
	tdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
#else
	tdesc.Usage = D3D11_USAGE_DEFAULT;
	tdesc.CPUAccessFlags = 0;
#endif
	tdesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	
    D3D11_SUBRESOURCE_DATA texData;
    ZeroMemory(&texData, sizeof(D3D11_SUBRESOURCE_DATA));
    texData.pSysMem = g_pTextureRawData[0];
    texData.SysMemPitch = g_iRowPitch = 4 * ilGetInteger(IL_IMAGE_WIDTH);
    texData.SysMemSlicePitch = 0;
	
    ret = g_pDevice->CreateTexture2D(&tdesc, /*&texData*/nullptr, &g_pTexture);

    if (FAILED(ret))
    {
        std::wcout << "Failed to create texture!\n";
        return false;
    }

    ilDeleteImages(1, &img); //nemoze to byt pred vytvorenim, inac je obsah nedefinovany (

    D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
    ZeroMemory(&srvdesc, sizeof(srvdesc));
    srvdesc.Format = tdesc.Format;
    srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvdesc.Texture2D.MipLevels = 1;
    srvdesc.Texture2D.MostDetailedMip = 0;

    ret = g_pDevice->CreateShaderResourceView(g_pTexture, &srvdesc, &g_pTextureSRV);
    if (FAILED(ret))
    {
        std::wcout << "Texture SRV creation failed!\n";
        return false;
    }

	//g_pContext->UpdateSubresource(g_pTexture, 0, nullptr, g_pTextureRawData[0], g_iRowPitch, 0);
	//g_pContext->GenerateMips(g_pTextureSRV);

    //Sampler
    D3D11_SAMPLER_DESC sdesc;
    ZeroMemory(&sdesc, sizeof(sdesc));
    sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sdesc.MaxAnisotropy = 1;
    sdesc.MaxLOD = D3D11_FLOAT32_MAX;
    sdesc.MinLOD = 0;
    sdesc.MipLODBias = 0;

    ret = g_pDevice->CreateSamplerState(&sdesc, &g_pTextureSampler);
    if (FAILED(ret))
    {
        std::cout << "Sampler creation failed!\n";
        return false;
    }
#endif

	//D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = 32 * sizeof(float);
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;

    return true;
}

bool initDevIL()
{
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
    {
        return false;
    }

    ilInit();

    return true;
}

void Draw()
{
	//static unsigned int index = 0;
	static HRTimer timer;

	g_fBuf[0] += timer.GetElapsedTimeSeconds();
	g_fBuf[1] = WINDOW_WIDTH;
	g_fBuf[2] = WINDOW_HEIGHT;
	
	//memcpy(g_fBuf+4, &g_pCamera->GetCameraPosition(), 3*sizeof(float));
	XMFLOAT3 cp = g_pCamera->GetCameraPosition();
	g_fBuf[4] = cp.x;
	g_fBuf[5] = cp.y;
	g_fBuf[6] = cp.z;
	g_fBuf[7] = 1;

	//XMFLOAT3 rr = g_pCamera->GetCameraViewVector();
	//std::wcout << "cam : " << rr.x << " " << rr.y << " " << rr.z << std::endl;

	timer.Reset();
    g_pCamera->Update();

    XMMATRIX inverseViewProjectionMatrix = XMMatrixInverse(nullptr,XMLoadFloat4x4(&g_pCamera->GetViewProjectionMatrix()));

	g_pContext->UpdateSubresource(g_pMatrices, 0, nullptr, &XMMatrixTranspose(inverseViewProjectionMatrix), 0, 0);
	g_pContext->UpdateSubresource(g_pUniforms, 0, nullptr, g_fBuf, 0, 0);

    //1st pass
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    g_pContext->ClearRenderTargetView(g_pRenderTargetView, clearColor);
    g_pContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    float clearColorMRT[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_pContext->ClearRenderTargetView(g_pMRT[0], clearColorMRT);
    g_pContext->ClearRenderTargetView(g_pMRT[1], clearColorMRT);
    g_pContext->ClearRenderTargetView(g_pMRT[2], clearColor);

    g_pContext->OMSetRenderTargets(3, g_pMRT, g_pDepthStencilView);
    g_pContext->RSSetViewports(1, &g_viewPort);

    g_pContext->VSSetShader(g_pVS, nullptr, 0);
    g_pContext->PSSetShader(g_pPS, nullptr, 0);
    g_pContext->GSSetShader(g_pGS, nullptr, 0);

    g_pContext->PSSetConstantBuffers(0, 1, &g_pMatrices);
	g_pContext->PSSetConstantBuffers(1, 1, &g_pUniforms);

#ifndef USE_PERLIN
    //TEXTURE COPY
	//g_Timer.Reset();
    g_pContext->UpdateSubresource(g_pTexture, 0, nullptr, g_pTextureRawData[index], g_iRowPitch, 0);
	g_pContext->GenerateMips(g_pTextureSRV);
	index ^= 1;
	//std::wcout << g_Timer.GetElapsedTimeMilliseconds() << "\n";
	
    g_pContext->PSSetShaderResources(0, 1, &g_pTextureSRV);
    g_pContext->PSSetSamplers(0, 1 ,&g_pTextureSampler);
#endif

    UINT strides = 4 * sizeof(float);
    UINT offsets = 0;

    g_pContext->IASetInputLayout(g_pInputLayout);
    //g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    g_pContext->IASetVertexBuffers(0, 1, &g_pVBO, &strides, &offsets);
    //g_pContext->IASetIndexBuffer(g_pIBO, DXGI_FORMAT_R32_UINT, 0);
    g_pContext->Draw(1, 0);

    //2nd pass
    g_pContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    g_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);
  
    g_pContext->VSSetShader(g_pVSDeferred, nullptr, 0);
    g_pContext->GSSetShader(g_pGSDeferred, nullptr, 0);
    g_pContext->PSSetShader(g_pPSDeferred, nullptr, 0);

    g_pContext->PSSetSamplers(0, 1, &g_pSampler);
    g_pContext->PSSetShaderResources(0, 1, &g_pShaderResourceViewMRT[0]);
    g_pContext->PSSetShaderResources(1, 1, &g_pShaderResourceViewMRT[1]);
    g_pContext->PSSetShaderResources(2, 1, &g_pShaderResourceViewMRT[2]);

    strides = 4*sizeof(float);
    g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    g_pContext->IASetInputLayout(g_pDeferredInputLayout);
    g_pContext->IASetVertexBuffers(0, 1, &g_pVBODeffered, &strides, &offsets);

    g_pContext->Draw(1, 0);

    //Unbind resource
    ID3D11ShaderResourceView* nullViews[1] = { nullptr };
    g_pContext->PSSetShaderResources(0, 1, nullViews);
    g_pContext->PSSetShaderResources(1, 1, nullViews);
    g_pContext->PSSetShaderResources(2, 1, nullViews);
    
    g_pSwapChain->Present(0, 0); //TU JE SYNC INTERVAL!
}

//TOTO do window managera
void Run()
{
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
			if (msg.hwnd != 0)
			{
				DispatchMessage(&msg);
			}
			else if (msg.message > WM_USER && msg.message < WM_USER_POOL)
			{
				WndProc(g_hWnd, msg.message, msg.wParam, msg.lParam); //Send thread messages (with msg.hwnd==0) also to WndProc
			}
        }
        else
        {
            //MOZE VZNIKNUT PROBLEM S CASOVACOM KAMERY! preverit
            if (g_isWindowActive)
                Draw();
        }
    }
}

void Clean()
{
#ifdef USE_MULTIGPU
	//Kill slave
	sendThreadMsg(GetThreadId(g_hSlaveThread), TMSG_TERMINATE, NULL);
	WaitForSingleObject(g_hSlaveThread, INFINITE);
#endif

    if (g_pCamera != nullptr)
        delete g_pCamera;

    SAFE_RELEASE(g_pNormaltexture);
    SAFE_RELEASE(g_pPositiontexture);
    SAFE_RELEASE(g_pColortexture);
    SAFE_RELEASE(g_pSampler);

    SAFE_RELEASE(g_pMRT[0]);
    SAFE_RELEASE(g_pMRT[1]);
    SAFE_RELEASE(g_pMRT[2]);

    SAFE_RELEASE(g_pShaderResourceViewMRT[0]);
    SAFE_RELEASE(g_pShaderResourceViewMRT[1]);
    SAFE_RELEASE(g_pShaderResourceViewMRT[2]);

    SAFE_RELEASE(g_pVBODeffered);

    SAFE_RELEASE(g_pPSDeferred);
    SAFE_RELEASE(g_pVSDeferred);
    SAFE_RELEASE(g_pGSDeferred);

    SAFE_RELEASE(g_pVBO);
    SAFE_RELEASE(g_pIBO);
    SAFE_RELEASE(g_pMatrices);
    SAFE_RELEASE(g_pPS);
	SAFE_RELEASE(g_pGS);
    SAFE_RELEASE(g_pVS);
    SAFE_RELEASE(g_pInputLayout);
    SAFE_RELEASE(g_pDeferredInputLayout);
	SAFE_RELEASE(g_pUniforms);

#ifndef USE_PERLIN
    SAFE_RELEASE(g_pTextureSampler);
    SAFE_RELEASE(g_pTextureSRV);
    SAFE_RELEASE(g_pTexture);

	if (g_pTextureRawData[0]!=nullptr)
		delete g_pTextureRawData[0];

	if (g_pTextureRawData[1]!=nullptr)
		delete g_pTextureRawData[1];
#endif

    if (g_pSwapChain!=nullptr)
        g_pSwapChain->SetFullscreenState(FALSE, nullptr);

    SAFE_RELEASE(g_pDepthStencilView);
    SAFE_RELEASE(g_pRenderTargetView);

    SAFE_RELEASE(g_pSwapChain);
    SAFE_RELEASE(g_pContext);
    SAFE_RELEASE(g_pDevice);
}

int main(int argc, char** argv)
{
	initGlobals();
	
	HWND hWnd_console = GetConsoleWindow();
    g_hInstance = GetModuleHandle(nullptr);
    
    if (!initWindowFromConsole(hWnd_console, g_hInstance))
    {
        return EXIT_FAILURE;
    }
    
    if (!initDevIL())
    {
        return EXIT_FAILURE;
    }

    initCamera();

    if (!initD3D())
    {
        return EXIT_FAILURE;
    }

    if (!initScene())
    {
        Clean();
        return EXIT_FAILURE;
    }

    Run();

    Clean();
    
    return EXIT_SUCCESS;
}