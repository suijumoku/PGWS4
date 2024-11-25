#include<Windows.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<vector>
#include<d3dcompiler.h>
#include<algorithm>
#ifdef _DEBUG
#include<iostream>
#endif

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace std;
using namespace DirectX;

// @brief �R���\�[����ʈ�t�H�[�}�b�g�t���������\��
// @param format �t�H�[�}�b�g�i��d�@�Ƃ��@��f�@�Ƃ��́j
// @param �ϒ�����
// @remarks ���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�܂���

// �ʓ|�����Ǐ����Ȃ���΂����Ȃ��֐�
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ��Ă΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);// OS�ɑ΂��āu���̃A�v���͏I���v�Ɠ`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);// ����̏������s��
}

void DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf(format, valist);
	va_end(valist);
#endif
}

#ifdef _DEBUG
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	if (!SUCCEEDED(result))return;

	debugLayer->EnableDebugLayer(); // �f�o�b�O���C���[��L��������
	debugLayer->Release(); // �L����������C���^�[�t�F�C�X���������
}

#endif // _DEBUG


float GetTriangleNum(float time, float cycleTime)
{
	// ���Ԃ������Ŋ������]��������Ŋ���A�������Ԃ̔��������Ȃ�オ��A�ȏ�Ȃ牺����
	float cycle = fmodf(time, cycleTime) * 2;
	if (cycle / cycleTime < 1)
	{
		return cycle;
	}
	else
	{
		float n = (cycleTime - fmodf(time, cycleTime)) * 2;
		return n;
	}
}


#ifdef _DEBUG
int main()
{
#else
int WINAPI Winmain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;
	ID3D12CommandQueue* _cmdQueue = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;

	// �E�B���h�E�N���X�̐����U�o�^
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;// �R�[���o�b�N�֐��̎w��
	w.lpszClassName = _T("DX12Sample"); // �A�v���P�[�V�����N���X���i�K���ł悢�j
	w.hInstance = GetModuleHandle(nullptr);// �n���h���̎擾

	RegisterClassEx(&w);// �A�v���P�[�V�����N���X�i�E�B���h�E�N���X�̎w���OS�ɓ`����j

	RECT wrc = { 0,0,window_width,window_height };// �E�B���h�E�T�C�Y�����߂�

	// �֐����g���ăE�B���h�E�̃T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	// �E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,// �N���X���w��
		_T("DX12 �e�X�g"),// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,// �^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,// �\�� x���W��OS�ɂ��C��
		CW_USEDEFAULT,// �\�� y���W��OS�ɂ��C��
		wrc.right - wrc.left,// �E�B���h�E��
		wrc.bottom - wrc.top,// �E�B���h�E��
		nullptr,// �e�E�B���h�E�n���h��
		nullptr,// ���j���[�n���h��
		w.hInstance,// �Ăяo���A�v���P�[�V�����n���h��
		nullptr);// �ǉ��p�����[�^�[

#ifdef _DEBUG
	// �f�o�b�O���C���[���I����
	EnableDebugLayer();
#endif

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

#ifdef _DEBUG
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
#else
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif

	// �A�_�v�^�[�̗񋓗p
	vector<IDXGIAdapter*>adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0;_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;++i)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);// �A�_�v�^�[�̐����I�u�W�F�N�g�擾
		wstring strDesc = adesc.Description;
		// �T�������A�_�v�^�[�̖��O���m�F
		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	// Direct3D�f�o�C�X�̏�����
	D3D_FEATURE_LEVEL featureLevel;
	for (auto lv : levels)
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			break;// �����\�ȃo�[�W���������������烋�[�v��ł��؂�
		}
	}

	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&_cmdAllocator));
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		_cmdAllocator, nullptr,
		IID_PPV_ARGS(&_cmdList));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// �^�C���A�E�g����
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// �A�_�v�^�[���P�����g��Ȃ��Ƃ��͂O�ł悢
	cmdQueueDesc.NodeMask = 0;
	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// �R�}���h���X�g�ƍ��킹��
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// �L���[����
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.Width = window_width;
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;
	// �o�b�N�o�b�t�@�[�͐L�яk�݉\
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	// �t���b�v��͑��₩�ɔj��
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// ���Ɏw��Ȃ�
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// �E�B���h�E�̃t���X�N���[���؂�ւ��\
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue, hwnd,
		&swapchainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)&_swapchain);// �{����QueryInterface�Ȃǂ�p����
	// IDXGISwapChain4*�ւ̕ϊ��`�F�b�N�����邪�A
	// �����ł͕ς��₷���d���̂��߃L���X�g�őΉ�

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;// �����_�[�^�[�Q�b�g�r���[�Ȃ̂�RTV
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;// �\����2��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;// ���Ɏw��Ȃ�

	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);

	vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
	for (int idx = 0;idx < swcDesc.BufferCount;idx++)
	{
		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += idx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_dev->CreateRenderTargetView(_backBuffers[idx], nullptr, handle);
	}

	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	float frame = 120;
	float time = 0;
	float oneCycleTime = 6;
	float HalfCycleTime = oneCycleTime / 2;

	// �E�B���h�E�\��
	ShowWindow(hwnd, SW_SHOW);

	XMFLOAT3 vertices[] = {
		{-0.4f,-0.7f,0.0f}, // ����
		{-0.4f,+0.7f,0.0f}, // ����
		{+0.4f,-0.7f,0.0f}, // �E��
		{+0.4f,+0.7f,0.0f}, // �E��
	};

	D3D12_HEAP_PROPERTIES heapprop = {};
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC resdesc = {};
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resdesc.Width = sizeof(vertices); // ���_��񂪓��邾���̃T�C�Y
	resdesc.Height = 1;
	resdesc.DepthOrArraySize = 1;
	resdesc.MipLevels = 1;
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	resdesc.SampleDesc.Count = 1;
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ID3D12Resource* vertBuff = nullptr;
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	XMFLOAT3* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	std::copy(std::begin(vertices), std::end(vertices), vertMap);
	vertBuff->Unmap(0, nullptr);

	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress(); // �o�b�t�@�[�̉��z�A�h���X
	vbView.SizeInBytes = sizeof(vertices); // �S�o�C�g��
	vbView.StrideInBytes = sizeof(vertices[0]); // 1���_������̃o�C�g��

	unsigned short indices[] = {
	0,1,2,
	2,1,3
	};

	ID3D12Resource* idxBuff = nullptr;
	// �ݒ�́A�o�b�t�@�[�̃T�C�Y�ȊO�A���_�o�b�t�@�[�̐ݒ���g���܂킵�Ă��悢
	resdesc.Width = sizeof(indices);
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&idxBuff));
	
	// ������o�b�t�@�[�ɃC���f�b�N�X�f�[�^���R�s�[
	unsigned short* mappendIdx = nullptr;
	idxBuff->Map(0, nullptr, (void**)&mappendIdx);
	std::copy(std::begin(indices), std::end(indices), mappendIdx);
	idxBuff->Unmap(0, nullptr);

	// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);

	ID3DBlob* _vsBlob = nullptr;
	ID3DBlob* _psBlob = nullptr;

	ID3DBlob* errorBlob = nullptr;

	// _vsBlob�p
	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl", // �V�F�[�_�\��
		nullptr, // define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�̓f�t�H���g
		"BasicVS", "vs_5_0", // �֐���BasicVS�A�Ώ̃V�F�[�_�[��vs5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p����эœK���Ȃ�
		0,
		&_vsBlob, &errorBlob); // �G���[����errorBlob�Ƀ��b�Z�[�W������
	if (FAILED(result)) 
	{ 
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else
		{
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		exit(1);
	}
	// _psBlob�p
	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl", // �V�F�[�_�\��
		nullptr, // define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�̓f�t�H���g
		"BasicPS", "ps_5_0", // �֐���BasicPS�A�Ώ̃V�F�[�_�[��vs5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p����эœK���Ȃ�
		0,
		&_psBlob, &errorBlob); // �G���[����errorBlob�Ƀ��b�Z�[�W������
	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("�t�@�C������������܂���");
		}
		else
		{
			std::string errstr;
			errstr.resize(errorBlob->GetBufferSize());
			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(), errstr.begin());
			errstr += "\n";
			OutputDebugStringA(errstr.c_str());
		}
		exit(1);
	}

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

	gpipeline.pRootSignature = nullptr; // ��Őݒ肷��

	gpipeline.VS.pShaderBytecode = _vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = _vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = _psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = _psBlob->GetBufferSize();
	gpipeline.InputLayout.pInputElementDescs = inputLayout; // ���C�A�E�g�擪�A�h���X
	gpipeline.InputLayout.NumElements = _countof(inputLayout); // ���C�A�E�g�z��
	// �f�t�H���g�̃T���v���}�X�N��\���萔(0xffffffff)
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// �܂����u�G�C���A�X�͎g��Ȃ�����false
	gpipeline.RasterizerState.MultisampleEnable = false;

	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // �J�����O���Ȃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // ���g��h��Ԃ�
	gpipeline.RasterizerState.DepthClipEnable = true; // �[�x�����̃N���b�s���O�͗L����

	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlandDesc = {};
	// 
	renderTargetBlandDesc.BlendEnable = false;
	// 
	renderTargetBlandDesc.LogicOpEnable = false;
	renderTargetBlandDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlandDesc;

	gpipeline.InputLayout.pInputElementDescs = inputLayout; // ���C�A�E�g�擪�A�h���X
	gpipeline.InputLayout.NumElements = _countof(inputLayout); // ���C�A�E�g�z��

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1; // ���͂P�̂�
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~1�ɐ��K�����ꂽRGBA

	gpipeline.SampleDesc.Count = 1; // �T���v�����O��1�s�N�Z���ɂ�1
	gpipeline.SampleDesc.Quality = 0; // �N�I���e�B�͍Œ�

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc, // ���[�g�V�O�l�`���ݒ�
		D3D_ROOT_SIGNATURE_VERSION_1_0, // ���[�g�V�O�l�`���o�[�W����
		&rootSigBlob, // �V�F�[�_�[����������Ɠ���
		&errorBlob); // �G���[����������

	ID3D12RootSignature* rootsignature = nullptr;
	result = _dev->CreateRootSignature(
		0, // nodemask,0�ł悢
		rootSigBlob->GetBufferPointer(), // �V�F�[�_�[�̎��Ɠ��l
		rootSigBlob->GetBufferSize(), // �V�F�[�_�[�̎��Ɠ��l
		IID_PPV_ARGS(&rootsignature));
	rootSigBlob->Release(); // �s�v�ɂȂ����̂ŊJ��

	gpipeline.pRootSignature = rootsignature;

	ID3D12PipelineState* _pipelinestate = nullptr;
	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));
	D3D12_VIEWPORT viewport = {};
	viewport.Width = window_width; // �o�͐�̕�(�s�N�Z����)
	viewport.Height = window_height; // �o�͐�̍���(�s�N�Z����)
	viewport.TopLeftX = 0; // �o�͐�̍�����WX
	viewport.TopLeftY = 0; // �o�͐�̍�����WY
	viewport.MaxDepth = 1.0f; // �[�x�ő�l
	viewport.MinDepth = 0.0f; // �[�x�ŏ��l

	D3D12_RECT scissorrect = {};
	scissorrect.top = 0; // �؂蔲������W
	scissorrect.left = 0; // �؂蔲�������W
	scissorrect.right = scissorrect.left + window_width; // �؂蔲���E���W
	scissorrect.bottom = scissorrect.top + window_height; // �؂蔲�������W
	while (true)
	{
		MSG msg;
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// �A�v���P�[�V�������I��鎞��message��WM_QUIT�ɂȂ�
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// DirectX����
		// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾
		auto bbIdx = _swapchain->GetCurrentBackBufferIndex();

		_cmdList->SetPipelineState(_pipelinestate);

		//D3D12_RESOURCE_BARRIER  BarrierDesc = {};
		//BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;// �J��
		//BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;// ���Ɏw��Ȃ�
		//BarrierDesc.Transition.pResource = _backBuffers[bbIdx];// �o�b�N�o�b�t�@�[���\�[�X
		//BarrierDesc.Transition.Subresource = 0;
		//BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;// ���O��PRESENT���
		//BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;// ������RT���
		//_cmdList->ResourceBarrier(1, &BarrierDesc);// �o���A�w����s

		// �����_�[�^�[�Q�b�g���w��
		auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

		// ��ʃN���A
		float  clearColor[] = { 0.0f,0.0f,0.3f,1.0f };// ���F
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		
		//time += 1 / frame;

		//// �O�p�g(���ꂽ���Ԃ̔{�̎����̎O�p�g�A0~���ꂽ���Ԃ܂ł̒l�����)
		//float rt = GetTriangleNum(time + 3, HalfCycleTime);
		//float gt = GetTriangleNum(time + 2, HalfCycleTime);
		//float bt = GetTriangleNum(time + 4, HalfCycleTime);

		//float rc = rt - (HalfCycleTime - 1) / 2;
		//float gc = gt - (HalfCycleTime - 1) / 2;
		//float bc = bt - (HalfCycleTime - 1) / 2;

		//// Clamp���g���Ȃ��̂�
		//float R = min(max(rc, (float)0), (float)1);
		//float G = min(max(gc, (float)0), (float)1);
		//float B = min(max(bc, (float)0), (float)1);

		//float clearColor[] = { R ,G ,B,1.0f }; // ���F
		//_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		_cmdList->SetGraphicsRootSignature(rootsignature);

		_cmdList->RSSetViewports(1, &viewport);
		_cmdList->RSSetScissorRects(1, &scissorrect);

		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		_cmdList->IASetVertexBuffers(0, 1, &vbView);
		_cmdList->IASetIndexBuffer(&ibView);

		_cmdList->DrawIndexedInstanced(6, 1, 0, 0,0);

		// �O�ゾ������ւ���
	/*	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		_cmdList->ResourceBarrier(1, &BarrierDesc);*/

		// ���߂̃N���[�Y
		_cmdList->Close();

		// �R�}���h���X�g�̎��s
		ID3D12CommandList* cmdlists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(1, cmdlists);

		// �҂�
		_cmdQueue->Signal(_fence, ++_fenceVal);

		if (_fence->GetCompletedValue() != _fenceVal) {
			auto event = CreateEvent(nullptr, false, false, nullptr);
			_fence->SetEventOnCompletion(_fenceVal, event); // �C�x���g�n���h���̎擾
			WaitForSingleObject(event, INFINITE); // �C�x���g����������܂Ŗ����ɑ҂�
			CloseHandle(event); // �C�x���g�n���h�������

		}

		_cmdAllocator->Reset(); // �L���[���N���A
		_cmdList->Reset(_cmdAllocator, nullptr); // �ĂуR�}���h���X�g�����߂鏀��

		// �t���b�v
		_swapchain->Present(1, 0);
	}

	// �����N���X�͎g��Ȃ��̂œo�^��������
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}
