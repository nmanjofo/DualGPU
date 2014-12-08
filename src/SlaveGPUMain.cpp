#include "SlaveGPUMain.h"


typedef struct _tMatrices
{
	XMMATRIX XMInverseVPMatrices[6];
	XMVECTOR cameraPosition;
} tMatrices;

ID3D11Device*			g_pSlaveDevice = nullptr;
ID3D11DeviceContext*	g_pSlaveContext = nullptr;

ID3D11DepthStencilView* g_pSlaveDepthStencilView = nullptr;
D3D11_VIEWPORT			g_SlaveViewPort;

DWORD					g_dwMasterThreadID = 0;

//Env Map
ID3D11RenderTargetView* g_pRenderTargetViewMRT;
ID3D11Texture2D*		g_pTextureArrayMRT;
ID3D11Texture2D*		g_pTextureArrayMRTStaging;

ID3D11VertexShader*		g_pVSEnv;
ID3D11GeometryShader*	g_pGSEnv;
ID3D11PixelShader*		g_pPSEnv;

ID3D11Buffer*			g_pBuffer;
tMatrices				g_tMMatrices;

//CUDA
//CUdevice				g_CudaSlaveDevice;
//CUcontext				g_CudaSlaveContext;

//TODO - zamykat standardny vystup kvoli threadingu
void sendThreadMsg(DWORD receiver, UINT msg, LPARAM lpParam)
{
	BOOL wasSent = PostThreadMessage(receiver, msg, 0, lpParam);

	if (!wasSent)
	{
		std::cerr << "Message \"" << threadMsgToString(msg) << "\" to " << receiver << " failed to send!\n";
	}
}

std::string threadMsgToString(UINT msg)
{
	std::string s;
	switch (msg)
	{
	case TMSG_SLAVE_DATA_READY:
		s = "Slave Data Ready";
		break;
	case TMSG_SLAVE_INIT_FAILED:
		s = "Slave Init FAILED";
		break;
	case TMSG_SLAVE_INIT_OK:
		s = "Slave Init OK";
		break;
	case TMSG_TERMINATE:
		s = "Terminate";
		break;
	case TMSG_SLAVE_QUIT:
		s = "Slave is quitting";
		break;
	case TMSG_SLAVE_ID:
		s = "Slave ID";
		break;
	default:
		s = "UNKNOWN MESSAGE";
		break;
	}

	return s;
}



bool initSlaveD3D(IDXGIAdapter* adapter, unsigned int width, unsigned int height)
{
	UINT flags = 0;

#ifdef _DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif  

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL tmp = D3D_FEATURE_LEVEL_11_0;

	HRESULT ret = D3D11CreateDevice(adapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		flags,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&g_pSlaveDevice,
		&tmp,
		&g_pSlaveContext);

	if (FAILED(ret))
	{
		std::cout << "SLAVE: Error creating slave GPU device!\n";
	}

	//Render Target
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 6;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.MiscFlags = 0;

	ret = g_pSlaveDevice->CreateTexture2D(&td, nullptr, &g_pTextureArrayMRT);
	if (FAILED(ret))
	{
		std::cout << "SLAVE: Creating Render Target Texture failed!\n";
		return false;
	}

	td.Usage = D3D11_USAGE_STAGING;
	td.BindFlags = 0;
	td.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	
	ret = g_pSlaveDevice->CreateTexture2D(&td, nullptr, &g_pTextureArrayMRTStaging);
	if (FAILED(ret))
	{
		std::cout << "SLAVE: Creating Render Target Texture Staging failed!\n";
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtd;
	ZeroMemory(&rtd, sizeof(rtd));
	rtd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtd.Texture2DArray.ArraySize = 6;
	rtd.Texture2DArray.MipSlice = 0;
	rtd.Texture2DArray.FirstArraySlice = 0;

	ret = g_pSlaveDevice->CreateRenderTargetView(g_pTextureArrayMRT, &rtd, &g_pRenderTargetViewMRT);
	if (FAILED(ret))
	{
		std::cout << "SLAVE: Creating Render Target View failed!\n";
		return false;
	}
	
	//Create DepthStencilView
	D3D11_TEXTURE2D_DESC depthTexDesc;
	ZeroMemory(&depthTexDesc, sizeof(depthTexDesc));
	depthTexDesc.Width = width;
	depthTexDesc.Height = height;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 6;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D * depthTexture;
	ret = g_pSlaveDevice->CreateTexture2D(&depthTexDesc, nullptr, &depthTexture);

	if (FAILED(ret))
	{
		std::cout << "SLAVE: Creating depth texture failed!\n";
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = depthTexDesc.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Texture2DArray.ArraySize = 6;
	descDSV.Texture2DArray.FirstArraySlice = 0;

	ret = g_pSlaveDevice->CreateDepthStencilView(depthTexture, &descDSV, &g_pSlaveDepthStencilView);

	if (FAILED(ret))
	{
		std::cout << "SLAVE: DepthStencilView creation failed!\n";
		return false;
	}

	depthTexture->Release();
	
	//Viewport
	ZeroMemory(&g_SlaveViewPort, sizeof(g_SlaveViewPort));
	g_SlaveViewPort.Width = width;
	g_SlaveViewPort.Height = height;
	g_SlaveViewPort.MinDepth = 0.0f;
	g_SlaveViewPort.MaxDepth = 1.0f;
	g_SlaveViewPort.TopLeftX = 0.0f;
	g_SlaveViewPort.TopLeftY = 0.0f;

	//6 cameras
	XMMATRIX pM = XMMatrixPerspectiveFovLH(90.0f, (float)width / (float)height, 0.1f, 1000.0f);
	
	XMVECTOR camPos = XMLoadFloat3(&XMFLOAT3(0,1,0));
	XMVECTOR up = camPos;

	g_tMMatrices.XMInverseVPMatrices[0] = XMMatrixInverse(nullptr, pM * XMMatrixLookAtLH(camPos, XMLoadFloat3(&XMFLOAT3(1, 0, 0)), up));
	g_tMMatrices.XMInverseVPMatrices[1] = XMMatrixInverse(nullptr, pM * XMMatrixLookAtLH(camPos, XMLoadFloat3(&XMFLOAT3(-1, 0, 0)), up));
	g_tMMatrices.XMInverseVPMatrices[2] = XMMatrixInverse(nullptr, pM * XMMatrixLookAtLH(camPos, XMLoadFloat3(&XMFLOAT3(1, 0, 0)), XMLoadFloat3(&XMFLOAT3(0, 0, -1))));
	g_tMMatrices.XMInverseVPMatrices[3] = XMMatrixInverse(nullptr, pM * XMMatrixLookAtLH(camPos, XMLoadFloat3(&XMFLOAT3(1, 0, 0)), XMLoadFloat3(&XMFLOAT3(0, 0, 1))));
	g_tMMatrices.XMInverseVPMatrices[4] = XMMatrixInverse(nullptr, pM * XMMatrixLookAtLH(camPos, XMLoadFloat3(&XMFLOAT3(0, 0, 1)), up));
	g_tMMatrices.XMInverseVPMatrices[5] = XMMatrixInverse(nullptr, pM * XMMatrixLookAtLH(camPos, XMLoadFloat3(&XMFLOAT3(0, 0, -1)), up));

	g_tMMatrices.cameraPosition = camPos;

	//Matrix buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.ByteWidth = sizeof(tMatrices);
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.pSysMem = &g_tMMatrices;

	g_pSlaveDevice->CreateBuffer(&bd, &sd, &g_pBuffer);
	if (FAILED(ret))
	{
		std::cout << "SLAVE: Matrix Buffer creation failed!\n";
		return false;
	}

	//Setup
	g_pSlaveContext->OMSetRenderTargets(1, &g_pRenderTargetViewMRT, g_pSlaveDepthStencilView);
	g_pSlaveContext->RSSetViewports(1, &g_SlaveViewPort);

	return true;
}

bool initSceneSlave()
{

	return true;
}

void CleanSlave()
{
	if (g_pSlaveContext != NULL)
        g_pSlaveContext->Release();

    if (g_pSlaveDevice != NULL)
        g_pSlaveDevice->Release();
}

void DrawSlave()
{

}

void RunSlave()
{
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message > WM_USER)
            {
                switch (msg.message)
                {
				case ThreadMessage::TMSG_TERMINATE:
                    CleanSlave();
                    return;
                default:
                    break;
                }
            }
        }
        else
        {
            //uspat sa?
        }
    }
}

DWORD WINAPI SlaveGPUThreadMain(LPVOID lpParam)
{
	threadParam* tp = (threadParam*)lpParam;
	g_dwMasterThreadID = tp->masterThreadID;

	bool isOK = initSlaveD3D(tp->adapter, tp->texWidth, tp->texHeight);

	sendThreadMsg(g_dwMasterThreadID, TMSG_SLAVE_ID, GetCurrentThreadId());

	if (!isOK)
	{
		sendThreadMsg(g_dwMasterThreadID,TMSG_SLAVE_INIT_FAILED, NULL);
		return EXIT_FAILURE;
	}

	sendThreadMsg(g_dwMasterThreadID, TMSG_SLAVE_INIT_OK, NULL);
    RunSlave();

    return EXIT_SUCCESS;
}

