//#include <Windows.h>//�E�B���h�E�̍쐬�⑀��A���b�Z�[�W�����Ȃǂ�OS���x���̑���
//#include <tchar.h>//_T�}�N���𗘗p���}���`�o�C�g�����Z�b�g��Unicode�����Z�b�g�̗������T�|�[�g���邽��
//#include <d3d12.h>
//#include <dxgi1_6.h>
//#include <DIrectXMath.h>
//#include <vector>
//#include <d3dcompiler.h>
//#include <algorithm> // std::max��std::min���g������
//#ifdef _DEBUG //�f�o�b�O�r���h���ɂ̂� _DEBUG �N��
//#include <iostream>
//#endif
//#pragma comment (lib, "d3d12.lib")
//#pragma comment (lib, "dxgi.lib")
//#pragma comment (lib, "d3dcompiler.lib")
//using namespace std;
//using namespace DirectX;
//
//// @brief �R���\�[����ʂɃt�H�[�}�b�g�t���������\��
//// @param format �t�H�[�}�b�g (%d �Ƃ� %f �Ƃ���)
//// @param �ϒ�����
//// @ remarks ���̊֐��̓f�o�b�O�p�ł��B�f�o�b�O���ɂ������삵�Ȃ�
//
////�E�B���h�E�ɑ����郁�b�Z�[�W����������֐�
//LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
//{
//	//�E�B���h�E���j�����ꂽ��Ă΂��
//	if (msg == WM_DESTROY)
//	{
//		PostQuitMessage(0);// OS�ɑ΂��āu�������̃A�v���͏I���v�Ɠ`����
//		return 0;
//	}
//	return DefWindowProc(hwnd, msg, wparam, lparam);//����̏������s��
//}
//
//void DebugOutputFormatString(const char* format, ...)
//{
//#ifdef _DEBUG
//	va_list valist;//�ϒ������̃��X�g���Ǘ�
//	va_start(valist, format);//valist�����������A�ǉ��̈����ւ̃A�N�Z�X���\�ɂ���
//	printf(format, valist);
//	va_end(valist);
//#endif
//}
//
////�f�o�b�O���[�h�ł̓R���\�[���A�v���P�[�V�����p�� main �֐����G���g���[�|�C���g
////�����[�X���[�h�ł�GUI�A�v���P�[�V�����̃G���g���[�|�C���g�ł��� WinMain
//#ifdef _DEBUG
//void EnableDebugLayer()
//{
//	ID3D12Debug* debugLayer = nullptr;
//	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
//	if (!SUCCEEDED(result)) return;
//	debugLayer->EnableDebugLayer();//�f�o�b�O���C���[��L����
//	debugLayer->Release();//�L����������C���^�[�t�F�[�X���
//}
//#endif//_DEBUG
////-----------------------------------------------------------
//// saturate�֐��̎���
//float saturate(float x) {
//	if (x < 0.0f) return 0.0f;
//	if (x > 1.0f) return 1.0f;
//	return x;
//}
////-----------------------------------------------------------
//#ifdef _DEBUG
//int main()
//{
//#else
//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
//{
//#endif
//	const unsigned int window_width = 1280;//�E�B���h�E�̕�
//	const unsigned int window_Height = 720;//�E�B���h�E�̍�
//	ID3D12Device* _dev = nullptr;
//	IDXGIFactory6* _dxgiFactory = nullptr;
//	ID3D12CommandAllocator* _cmdAllocator = nullptr;
//	ID3D12GraphicsCommandList* _cmdList = nullptr;
//	ID3D12CommandQueue* _cmdQueue = nullptr;
//	IDXGISwapChain4* _swapchain = nullptr;
//	//�E�B���h�E�N���X�̐������o�^
//	WNDCLASSEX w = {};
//	w.cbSize = sizeof(WNDCLASSEX);//�\���̂̃T�C�Y���w��
//	w.lpfnWndProc = (WNDPROC)WindowProcedure;//�R�[���o�b�N�֐��̎w��(���b�Z�[�W�����p)
//	w.lpszClassName = _T("DX12Sample");//�A�v���P�[�V�����N���X���i���ド�C�u���������Ƃ��ɂ���p�̖��O�ɕς���j
//	w.hInstance = GetModuleHandle(nullptr);//�n���h���̎擾
//	RegisterClassEx(&w);//�E�B���h�E�T�C�Y�����߂�
//	RECT wrc = { 0, 0, window_width, window_Height };//�`��\�̈�̃T�C�Y��ݒ�
//	//�֐����g���ăE�B���h�E�̃T�C�Y��␳����(����ɂ��A�^�C�g���o�[�⋫�E���̕����l�������T�C�Y��)
//	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
//	//�E�B���h�E�I�u�W�F�N�g�̐���
//	HWND hwnd = CreateWindow(w.lpszClassName,//�N���X���w��
//		_T("DX12 �e�X�g"),//�^�C�g���o�[����
//		WS_OVERLAPPEDWINDOW,//�^�C�g���o�[�Ƌ��E��������E�B���h�E
//		CW_USEDEFAULT,//�\�� x ���W�� OS �ɂ��܂���
//		CW_USEDEFAULT,//�\�� y ���W�� OS �ɂ��܂���
//		wrc.right - wrc.left,//�E�B���h�E��
//		wrc.bottom - wrc.top,//�E�B���h�E��
//		nullptr,//�e�E�B���h�E�n���h��
//		nullptr,//���j���[�n���h��
//		w.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
//		nullptr);//�ǉ��p�����[�^
//#ifdef _DEBUG
//	//�f�o�b�O���C���[ ON
//	EnableDebugLayer();
//#endif
//	D3D_FEATURE_LEVEL levels[] =
//	{
//		D3D_FEATURE_LEVEL_12_0,
//		D3D_FEATURE_LEVEL_12_1,
//		D3D_FEATURE_LEVEL_11_1,
//		D3D_FEATURE_LEVEL_11_0,
//	};
//#ifdef _DEBUG
//	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&_dxgiFactory));
//#else
//	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
//#endif
//	HRESULT D3D12CreateDevice(
//		IUnknown * pAdapter, //�ЂƂ܂��� nullptr ��OK
//		D3D_FEATURE_LEVEL MinimumFeatureLevel, //�Œ���K�v�ȃt���[�`���[���x��
//		REFIID riid,
//		void** ppDevice
//	);
//
//	HRESULT CreateSwapChainForHwnd(
//		IUnknown * pDevice,
//		HWND hWnd,
//		const DXGI_SWAP_CHAIN_DESC1 * pDesc,
//		const DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreenDesc,
//		IDXGIOutput * pRestricToOutput,
//		IDXGISwapChain1 * ppSwapChain
//	);
//	//�A�_�v�^�[�̗񋓗p
//	std::vector <IDXGIAdapter*> adapters;
//	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
//	IDXGIAdapter* tmpAdapter = nullptr;
//	for (int i = 0;
//		_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
//		++i)
//	{
//		adapters.push_back(tmpAdapter);
//	}
//	for (auto adpt : adapters)
//	{
//		DXGI_ADAPTER_DESC adesc = {};
//		adpt->GetDesc(&adesc);//�A�_�v�^�[�̐����I�u�W�F�N�g�擾
//		std::wstring strDesc = adesc.Description;
//		//�T�������A�_�v�^�[�̖��O���m�F
//		if (strDesc.find(L"NVIDIA") != std::string::npos)
//		{
//			tmpAdapter = adpt;
//			break;
//		}
//	}
//	//Direct3D�f�o�C�X�̏�����
//	D3D_FEATURE_LEVEL featureLevel;
//	for (auto lv : levels)
//	{
//		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK)
//		{
//			featureLevel = lv;
//			break;//�����\�ȃo�[�W���������������烋�[�v��ł��؂�
//		}
//	}
//	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
//		IID_PPV_ARGS(&_cmdAllocator));
//	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
//		_cmdAllocator, nullptr,
//		IID_PPV_ARGS(&_cmdList));
//	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
//	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//	cmdQueueDesc.NodeMask = 0;
//	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
//	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));
//	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
//	swapchainDesc.Width = window_width;
//	swapchainDesc.Height = window_Height;
//	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	swapchainDesc.Stereo = false;
//	swapchainDesc.SampleDesc.Count = 1;
//	swapchainDesc.SampleDesc.Quality = 0;
//	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
//	swapchainDesc.BufferCount = 2;
//	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
//	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
//	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
//	result = _dxgiFactory->CreateSwapChainForHwnd(
//		_cmdQueue, hwnd,
//		&swapchainDesc, nullptr, nullptr,
//		(IDXGISwapChain1**)&_swapchain);
//	D3D12_HEAP_PROPERTIES heapprop = {};
//	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
//	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
//	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
//
//	/*XMFLOAT3 vertices[] = {
//	{-0.5f, -0.5f, 0.0f}, // ���� (0)
//	{ 0.5f, -0.5f, 0.0f}, // �E�� (1)
//	{ 0.0f,  0.5f, 0.0f}, // ������ (2)
//	{-0.8f,  0.0f, 0.0f}, // ���� (3)
//	{ 0.8f,  0.0f, 0.0f}, // �E�� (4)
//	};*/
//
//	struct Vertex {
//		XMFLOAT3 pos;
//		XMFLOAT2 uv;
//	};
//
//	Vertex vertices[] = {
//		{{-0.4f,-0.7f,0.0f} , {0.0f, 1.0f}},
//		{{-0.4f,+0.7f,0.0f} , {0.0f, 0.0f}},
//		{{+0.4f,-0.7f,0.0f} , {1.0f, 1.0f}},
//		{{+0.4f,+0.7f,0.0f} , {1.0f, 0.0f}},
//	};
//
//
//	D3D12_RESOURCE_DESC resdesc = {};
//	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	resdesc.Width = sizeof(vertices);//���_�����邾���̃T�C�Y
//	resdesc.Height = 1;
//	resdesc.DepthOrArraySize = 1;
//	resdesc.MipLevels = 1;
//	resdesc.Format = DXGI_FORMAT_UNKNOWN;
//	resdesc.SampleDesc.Count = 1;
//	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
//	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//	ID3D12Resource* vertBuff = nullptr;
//	result = _dev->CreateCommittedResource(
//		&heapprop,
//		D3D12_HEAP_FLAG_NONE,
//		&resdesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&vertBuff));
//
//	Vertex* vertMap = nullptr;
//	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
//	std::copy(std::begin(vertices), std::end(vertices), vertMap);
//	vertBuff->Unmap(0, nullptr);
//	D3D12_VERTEX_BUFFER_VIEW vbView = {};
//	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();//�o�b�t�@�[�̉��z�A�h���X
//	vbView.SizeInBytes = sizeof(vertices);//�S�o�C�g��
//	vbView.StrideInBytes = sizeof(vertices[0]);//�P���_������̃o�C�g��
//
//
//	unsigned short indices[] = {
//		0, 1, 2,
//		2, 1, 3
//	};
//
//	/*unsigned short indices[] = {
//		0, 2, 3, // �����̎O�p�`
//		1, 2, 4, // �E���̎O�p�`
//		3, 2, 4, // ��̎O�p�`
//	};*/
//
//	ID3D12Resource* idxBuff = nullptr;
//	resdesc.Width = sizeof(indices);
//	result = _dev->CreateCommittedResource(
//		&heapprop,
//		D3D12_HEAP_FLAG_NONE,
//		&resdesc,
//		D3D12_RESOURCE_STATE_GENERIC_READ,
//		nullptr,
//		IID_PPV_ARGS(&idxBuff));
//
//	unsigned short* mappedIdx = nullptr;
//	idxBuff->Map(0, nullptr, (void**)&mappedIdx);
//	std::copy(std::begin(indices), std::end(indices), mappedIdx);
//
//	idxBuff->Unmap(0, nullptr);
//
//	D3D12_INDEX_BUFFER_VIEW ibView = {};
//	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
//	ibView.Format = DXGI_FORMAT_R16_UINT;
//	ibView.SizeInBytes = sizeof(indices);
//
//	struct TexRGBA
//	{
//		unsigned char R, G, B, A;
//	};
//
//	std::vector<TexRGBA> texturedata(256 * 256);
//	for (auto& rgba : texturedata)
//	{
//		rgba.R = rand() % 256;
//		rgba.G = rand() % 256;
//		rgba.B = rand() % 256;
//		rgba.A = 255;
//	}
//
//	D3D12_HEAP_PROPERTIES texHeapProp = {};
//	texHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
//	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
//	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
//	texHeapProp.CreationNodeMask = 0;
//	texHeapProp.VisibleNodeMask = 0;
//
//	D3D12_RESOURCE_DESC resDesc = {};
//	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	resDesc.Width = 256;
//	resDesc.Height = 256;
//	resDesc.DepthOrArraySize = 1;
//	resDesc.SampleDesc.Count = 1;
//	resDesc.SampleDesc.Quality = 0;
//	resDesc.MipLevels = 0;
//	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
//	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
//
//	ID3D12Resource* texbuff = nullptr;
//	result = _dev->CreateCommittedResource(
//		&texHeapProp,
//		D3D12_HEAP_FLAG_NONE,
//		&resDesc,
//		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
//		nullptr,
//		IID_PPV_ARGS(&texbuff));
//
//	result = texbuff->WriteToSubresource(
//		0,
//		nullptr,
//		texturedata.data(),
//		sizeof(TexRGBA) * 256,
//		sizeof(TexRGBA) * (UINT)texturedata.size()
//	);
//
//	ID3D12DescriptorHeap* texDescHeap = nullptr;
//	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
//	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	descHeapDesc.NodeMask = 0;
//	descHeapDesc.NumDescriptors = 1;
//	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//
//	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&texDescHeap));
//
//
//	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
//	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	srvDesc.Shader4ComponentMapping =
//		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	srvDesc.Texture2D.MipLevels = 1;
//
//	_dev->CreateShaderResourceView(
//		texbuff,
//		&srvDesc,
//		texDescHeap->GetCPUDescriptorHandleForHeapStart()
//	);
//
//	ID3DBlob* _vsBlob = nullptr;
//	ID3DBlob* _psBlob = nullptr;
//
//	ID3DBlob* errorBlob = nullptr;
//	result = D3DCompileFromFile(
//				L"BasicVertexShader.hlsl", // �V�F�[�_�\��
//				nullptr, // define�͂Ȃ�
//				D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�̓f�t�H���g
//				"BasicVS", "vs_5_0", // �֐���BasicVS�A�Ώ̃V�F�[�_�[��vs5_0
//				D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p����эœK���Ȃ�
//				0,
//				&_vsBlob, &errorBlob);//�G���[����errorBlob �Ƀ��b�Z�[�W������
//
//
//	if (FAILED(result)) {
//		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
//			::OutputDebugStringA("�t�@�C����������܂���");
//		}
//		else {
//			std::string errstr;
//			errstr.resize(errorBlob->GetBufferSize());
//			std::copy_n((char*)errorBlob->GetBufferPointer(),
//				errorBlob->GetBufferSize(), errstr.begin());
//			errstr += "\n";
//			OutputDebugStringA(errstr.c_str());
//		}
//		exit(1);
//	}
//		result = D3DCompileFromFile(
//			L"BasicPixelShader.hlsl", // �V�F�[�_�\��
//			nullptr, // define�͂Ȃ�
//			D3D_COMPILE_STANDARD_FILE_INCLUDE, // �C���N���[�h�̓f�t�H���g
//			"BasicPS", "ps_5_0", // �֐���BasicPS�A�Ώ̃V�F�[�_�[��vs5_0
//			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // �f�o�b�O�p����эœK���Ȃ�
//			0,
//			&_psBlob, &errorBlob);//�G���[����errorBlob �Ƀ��b�Z�[�W������
//	if (FAILED(result)) {
//		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
//			::OutputDebugStringA("�t�@�C����������܂���");
//		}
//		else {
//			std::string errstr;
//			errstr.resize(errorBlob->GetBufferSize());
//			std::copy_n((char*)errorBlob->GetBufferPointer(),
//				errorBlob->GetBufferSize(), errstr.begin());
//			errstr += "\n";
//			OutputDebugStringA(errstr.c_str());
//		}
//		exit(1);
//	}
//	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
//	{
//		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
//			D3D12_APPEND_ALIGNED_ELEMENT,
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
//		{//uv
//			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
//			0, D3D12_APPEND_ALIGNED_ELEMENT,
//			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
//		},
//	};
//
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
//
//	gpipeline.pRootSignature = nullptr;
//
//	gpipeline.VS.pShaderBytecode = _vsBlob->GetBufferPointer();
//	gpipeline.VS.BytecodeLength = _vsBlob->GetBufferSize();
//	gpipeline.PS.pShaderBytecode = _psBlob->GetBufferPointer();
//	gpipeline.PS.BytecodeLength = _psBlob->GetBufferSize();
//	//�f�t�H���g�̃T���v���}�X�N��\���萔 (0xffffffff)
//	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//	gpipeline.RasterizerState.MultisampleEnable = false;
//	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
//	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//���g��h��Ԃ�
//	gpipeline.RasterizerState.DepthClipEnable = true;//�[�x�����̃N���b�s���O�͗L����
//	gpipeline.BlendState.AlphaToCoverageEnable = false;
//	gpipeline.BlendState.IndependentBlendEnable = false;
//	D3D12_RENDER_TARGET_BLEND_DESC renderTrgetBlendDesc = {};
//	//�ЂƂ܂����Z���Z�⃿�u�����f�B���O�͎g�p���Ȃ�
//	renderTrgetBlendDesc.BlendEnable = false;
//	renderTrgetBlendDesc.LogicOpEnable = false;
//	renderTrgetBlendDesc.RenderTargetWriteMask =
//		D3D12_COLOR_WRITE_ENABLE_ALL;
//	gpipeline.BlendState.RenderTarget[0] = renderTrgetBlendDesc;
//	gpipeline.InputLayout.pInputElementDescs = inputLayout;//���C�A�E�g�擪�A�h���X
//	gpipeline.InputLayout.NumElements = _countof(inputLayout);//���C�A�E�g�z��̗v�f��
//	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
//	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;//�O�p�`�ō\��
//	gpipeline.NumRenderTargets = 1;
//	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;//0�`1�ɐ��K�����ꂽRGBA
//	gpipeline.SampleDesc.Count = 1;
//	gpipeline.SampleDesc.Quality = 0;
//	D3D12_ROOT_SIGNATURE_DESC rootSibnatureDesc = {};
//	rootSibnatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//	D3D12_DESCRIPTOR_RANGE descTblRange = {};
//	descTblRange.NumDescriptors = 1;
//	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	descTblRange.BaseShaderRegister = 0;
//	descTblRange.OffsetInDescriptorsFromTableStart =
//		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//
//	D3D12_ROOT_PARAMETER rootparam = {};
//	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
//	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
//	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;
//	rootparam.DescriptorTable.NumDescriptorRanges = 1;
//
//
//	rootSibnatureDesc.pParameters = &rootparam;
//	rootSibnatureDesc.NumParameters = 1;
//
//	D3D12_STATIC_SAMPLER_DESC sampleDesc = {};
//	sampleDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	sampleDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	sampleDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
//	sampleDesc.BorderColor =
//		D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
//	sampleDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
//	sampleDesc.MaxLOD = D3D12_FLOAT32_MAX;
//	sampleDesc.MinLOD = 0.0f;
//	sampleDesc.ShaderVisibility =
//		D3D12_SHADER_VISIBILITY_PIXEL;
//	sampleDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
//
//	rootSibnatureDesc.pStaticSamplers = &sampleDesc;
//	rootSibnatureDesc.NumStaticSamplers = 1;
//
//	ID3DBlob* rootSigBlob = nullptr;
//	result = D3D12SerializeRootSignature(
//		&rootSibnatureDesc,
//		D3D_ROOT_SIGNATURE_VERSION_1_0,
//		&rootSigBlob,
//		&errorBlob);
//	ID3D12RootSignature* rootsignature = nullptr;
//	result = _dev->CreateRootSignature(
//		0,
//		rootSigBlob->GetBufferPointer(),
//		rootSigBlob->GetBufferSize(),
//		IID_PPV_ARGS(&rootsignature));
//	rootSigBlob->Release();
//
//	gpipeline.pRootSignature = rootsignature;
//
//	ID3D12PipelineState* _pipelinestate = nullptr;
//
//	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));
//
//	D3D12_VIEWPORT viewport = {};
//	viewport.Width = window_width;
//	viewport.Height = window_Height;
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.MaxDepth = 1.0f;
//	viewport.MinDepth = 0.0f;
//	D3D12_RECT scissorrect = {};
//	scissorrect.top = 0;
//	scissorrect.left = 0;
//	scissorrect.right = scissorrect.left + window_width;
//	scissorrect.bottom = scissorrect.top + window_Height;
//	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
//	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�����_�\�^�[�Q�b�g�r���[
//	heapDesc.NodeMask = 0;
//	heapDesc.NumDescriptors = 2; //�\���̂Q��
//	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//
//	ID3D12DescriptorHeap* rtvHeaps = nullptr;
//	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));
//	DXGI_SWAP_CHAIN_DESC swcDesc = {};
//	result = _swapchain->GetDesc(&swcDesc);
//	std::vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);
//	for (int idx = 0; idx < swcDesc.BufferCount; ++idx)
//	{
//		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
//		D3D12_CPU_DESCRIPTOR_HANDLE handle
//			= rtvHeaps->GetCPUDescriptorHandleForHeapStart();
//		handle.ptr += idx * _dev->GetDescriptorHandleIncrementSize(
//			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//		_dev->CreateRenderTargetView(_backBuffers[idx], nullptr, handle);
//	}
//	ID3D12Fence* _fence = nullptr;
//	UINT64 _fenceVal = 0;
//	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
//	ShowWindow(hwnd, SW_SHOW);
//
//	//���b�Z�[�W���[�v
//	while (true)
//	{
//		MSG msg;//���b�Z�[�W�̎�ނ�E�B���h�E�n���h���A���M�҂Ȃǂ̏��
//		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//		{
//			//�A�v���P�[�V�������I���Ƃ��� message ���@WM_QUIT�@�ɂȂ�
//			if (msg.message == WM_QUIT)
//			{
//				break;
//			}
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//		//DirectX����
//		auto bbIdx = _swapchain->GetCurrentBackBufferIndex();
//		_cmdList->SetPipelineState(_pipelinestate);
//		D3D12_RESOURCE_BARRIER BarrierDesc = {};
//		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;//�J��
//		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//		BarrierDesc.Transition.pResource = _backBuffers[bbIdx];//�o�b�N�o�b�t�@�[���\�[�X
//		BarrierDesc.Transition.Subresource = 0;
//		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//���O��PRESENT���
//		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;//������ RT ���
//		_cmdList->ResourceBarrier(1, &BarrierDesc);//�o���A�w����s
//		auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
//		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(
//			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//		_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);
//		//-------------------------------------------------------------------
//
//		// �^�C�}�[�̐ݒ� (t�̍X�V)
//		static float t = 0.0f;
//		float deltaTime = 0.016f;
//		t += deltaTime; // ���t���[���i�߂�
//		// t �� 0.0 ���� 1.0 �͈̔͂ɐ����ifmod�Ŏ����I�Ƀ��Z�b�g�j
//		t = fmod(t, 1.0f);
//		// �F�̌v�Z
//		float red = saturate(abs(6 * t - 3) - 1);
//		float green = saturate(-abs(6 * t - 2) + 2);
//		float blue = saturate(-abs(6 * t - 4) + 2);
//		float clearColor[] = { red, green, blue, 1.0f };
//		//-------------------------------------------------------------------
//		//��ʃN���A
//		//float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
//		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
//		//�O�ゾ������ւ���
//		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
//		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
//		_cmdList->ResourceBarrier(1, &BarrierDesc);
//
//
//		_cmdList->SetGraphicsRootSignature(rootsignature);
//		_cmdList->SetDescriptorHeaps(1, &texDescHeap);
//		_cmdList->SetGraphicsRootDescriptorTable(
//			0,
//			texDescHeap->GetGPUDescriptorHandleForHeapStart());
//		_cmdList->RSSetViewports(1, &viewport);
//		_cmdList->RSSetScissorRects(1, &scissorrect);
//		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		_cmdList->IASetVertexBuffers(0, 1, &vbView);
//		_cmdList->DrawInstanced(3, 1, 0, 0);
//		_cmdList->IASetIndexBuffer(&ibView);
//		_cmdList->DrawIndexedInstanced(6, 1, 0, 0 , 0);
//
//
//		//���߂̃N���[�Y
//		_cmdList->Close();
//		//�R�}���h���X�g�̎��s
//		ID3D12CommandList* cmdList[] = { _cmdList };
//		_cmdQueue->ExecuteCommandLists(1, cmdList);
//		_cmdQueue->Signal(_fence, ++_fenceVal);
//		_cmdQueue->Signal(_fence, ++_fenceVal);
//		if (_fence->GetCompletedValue() != _fenceVal) {
//			auto event = CreateEvent(nullptr, false, false, nullptr);
//			_fence->SetEventOnCompletion(_fenceVal, event);
//			WaitForSingleObject(event, INFINITE);
//			CloseHandle(event);
//		}
//		_cmdAllocator->Reset();//�L���[���N���A
//		_cmdList->Reset(_cmdAllocator, nullptr);//�ĂуR�}���h���X�g�����߂鏀��
//		//�t���b�v
//		_swapchain->Present(1, 0);
//	}
//	//�������̃N���X�͎g��Ȃ��̂œo�^��������
//	UnregisterClass(w.lpszClassName, w.hInstance);
//	return 0;
//	}