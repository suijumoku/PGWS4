#include<Windows.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<vector>
#include<algorithm>
#ifdef _DEBUG
#include<iostream>
#endif

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace std;

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

	HRESULT D3D12CreateDevice(
		IUnknown * pAdapter,// �ЂƂ܂���nullptr��OK
		D3D_FEATURE_LEVEL MinimumFeatureLevel,// �Œ���K�v�ȃt�B�[�`���[���x��
		REFIID riid,// ��q�@�F�󂯎�肽���I�u�W�F�N�g�̌������ʂ��邽�߂�ID
		void** ppDevice//��q
	);

	HRESULT CreateSwapChainForHwnd(
		IUnknown * pDevice,// �R�}���h�L���[�I�u�W�F�N�g
		HWND hWnd,// �E�B���h�E�n���h��
		const DXGI_SWAP_CHAIN_DESC1 * pDesc,// �X���b�v�`�F�[���ݒ�
		const DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pFullscreenDesc,// �ЂƂ܂�nullptr�ł悢
		IDXGIOutput * pRestrictToOutput,// �����nullptr�ł悢
		IDXGISwapChain1 * *ppSwapChain// �X���b�v�`�F�[���I�u�W�F�N�g�擾�p
	);

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

		D3D12_RESOURCE_BARRIER  BarrierDesc = {};
		BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;// �J��
		BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;// ���Ɏw��Ȃ�
		BarrierDesc.Transition.pResource = _backBuffers[bbIdx];// �o�b�N�o�b�t�@�[���\�[�X
		BarrierDesc.Transition.Subresource = 0;
		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;// ���O��PRESENT���
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;// ������RT���
		_cmdList->ResourceBarrier(1, &BarrierDesc);// �o���A�w����s

		// �����_�[�^�[�Q�b�g���w��
		auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_cmdList->OMSetRenderTargets(1, &rtvH, true, nullptr);

		// ��ʃN���A
		time += 1 / frame;

		// �O�p�g(���ꂽ���Ԃ̔{�̎����̎O�p�g�A0~���ꂽ���Ԃ܂ł̒l�����)
		float rt = GetTriangleNum(time + 3, HalfCycleTime);
		float gt = GetTriangleNum(time + 2, HalfCycleTime);
		float bt = GetTriangleNum(time + 4, HalfCycleTime);

		float rc = rt - (HalfCycleTime - 1) / 2;
		float gc = gt - (HalfCycleTime - 1) / 2;
		float bc = bt - (HalfCycleTime - 1) / 2;

		// Clamp���g���Ȃ��̂�
		float R = min(max(rc, (float)0), (float)1);
		float G = min(max(gc, (float)0), (float)1);
		float B = min(max(bc, (float)0), (float)1);

		float clearColor[] = { R ,G ,B,1.0f }; // ���F
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

		// �O�ゾ������ւ���
		BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		_cmdList->ResourceBarrier(1, &BarrierDesc);

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
