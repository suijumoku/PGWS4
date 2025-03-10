﻿#include<Windows.h>
#include<tchar.h>
#include<vector>
#include<algorithm>

#include<d3d12.h>
#include<dxgi1_6.h>
#include<DirectXMath.h>
#include<d3dcompiler.h>
#include<DirectXTex.h>
#include<d3dx12.h>

#ifdef _DEBUG
#include<iostream>
#endif

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

using namespace std;
using namespace DirectX;

// @brief コンソール画面井フォーマット付き文字列を表示
// @param format フォーマット（％d　とか　％f　とかの）
// @param 可変長引数
// @remarks この関数はデバッグ用です。デバッグ時にしか動作しません

// 面倒だけど書かなければいけない関数
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ウィンドウが破棄されたら呼ばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);// OSに対して「このアプリは終わる」と伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);// 既定の処理を行う
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

// 
// 
// 
// 
size_t AlignmentedSize(size_t size, size_t alignment)
{
	return size + alignment - size % alignment;
}

#ifdef _DEBUG
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	HRESULT result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
	if (!SUCCEEDED(result))return;

	debugLayer->EnableDebugLayer(); // デバッグレイヤーを有効化する
	debugLayer->Release(); // 有効化したらインターフェイスを解放する
}

#endif // _DEBUG


float GetTriangleNum(float time, float cycleTime)
{
	// 時間を周期で割った余りを周期で割り、周期時間の半分未満なら上がり、以上なら下がる
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
void Rotation1(float& angle, bool& rotatePlus)
{
	if (angle < XM_PI && rotatePlus)
		angle += 0.1f;
	else if (angle < -XM_PI)
		rotatePlus = true;
	else
	{
		angle -= 0.1f;
		rotatePlus = false;

	}
}

void Rotation2(float& angle, bool& rotatePlus)
{
	if (angle < XM_PIDIV2 && rotatePlus)
		angle += 0.1f;
	else if (angle < -XM_PIDIV2)
		rotatePlus = true;
	else
	{
		angle -= 0.1f;
		rotatePlus = false;

	}
}

// 頂点データ構造体
struct Vertex {
	XMFLOAT3 pos; // xyz座標
	XMFLOAT2 uv; // uv座標
};

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

	// ウィンドウクラスの生成６登録
	WNDCLASSEX w = {};

	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;// コールバック関数の指定
	w.lpszClassName = _T("DX12Sample"); // アプリケーションクラス名（適当でよい）
	w.hInstance = GetModuleHandle(nullptr);// ハンドルの取得

	RegisterClassEx(&w);// アプリケーションクラス（ウィンドウクラスの指定をOSに伝える）

	RECT wrc = { 0,0,window_width,window_height };// ウィンドウサイズを決める

	// 関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	// ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(w.lpszClassName,// クラス名指定
		_T("DX12 テスト"),// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,// タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,// 表示 x座標はOSにお任せ
		CW_USEDEFAULT,// 表示 y座標はOSにお任せ
		wrc.right - wrc.left,// ウィンドウ幅
		wrc.bottom - wrc.top,// ウィンドウ高
		nullptr,// 親ウィンドウハンドル
		nullptr,// メニューハンドル
		w.hInstance,// 呼び出しアプリケーションハンドル
		nullptr);// 追加パラメーター

#ifdef _DEBUG
	// デバッグレイヤーをオンに
	EnableDebugLayer();
#endif

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));

	// アダプターの列挙用
	vector<IDXGIAdapter*>adapters;
	// ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0;_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;++i)
	{
		adapters.push_back(tmpAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);// アダプターの説明オブジェクト取得
		wstring strDesc = adesc.Description;
		// 探したいアダプターの名前を確認
		if (strDesc.find(L"NVIDIA") != string::npos)
		{
			tmpAdapter = adpt;
			break;
		}
	}

	// Direct3Dデバイスの初期化
	D3D_FEATURE_LEVEL featureLevel;
	for (auto lv : levels)
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK)
		{
			featureLevel = lv;
			break;// 生成可能なバージョンが見つかったらループを打ち切り
		}
	}

	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));
	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};
	// タイムアウト無し
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// アダプターを１つしか使わないときは０でよい
	cmdQueueDesc.NodeMask = 0;
	// プライオリティは特に指定なし
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// コマンドリストと合わせる
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// キュー生成
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
	// バックバッファーは伸び縮み可能
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
	// フリップ後は速やかに破棄
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	// 特に指定なし
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// ウィンドウ⇔フルスクリーン切り替え可能
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue, hwnd,
		&swapchainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)&_swapchain);// 本来はQueryInterfaceなどを用いて
	// IDXGISwapChain4*への変換チェックをするが、
	// ここでは変わりやすさ重視のためキャストで対応

	// PMDヘッダー構造体
	struct PMDHeader
	{
		float version; // 例:00 00 80 3F == 1.00
		char model_name[20]; // モデル名
		char comment[256]; // モデルコメント
	};

	char signature[3] = {}; // シグネチャ
	PMDHeader pmdheader = {};
	FILE* fp;
	fopen_s(&fp, "Model/初音ミク.pmd", "rb");

	fread(signature, sizeof(signature), 1, fp);
	fread(&pmdheader, sizeof(pmdheader), 1, fp);

	constexpr size_t pmdvertex_size = 38; // 頂点1つ当たりのサイズ

	unsigned int vertNum; // 調点数を取得
	fread(&vertNum, sizeof(vertNum), 1, fp);

#pragma pack(push, 1)
	struct PMD_VERTEX
	{
		XMFLOAT3 pos;
		XMFLOAT3 normal;
		XMFLOAT2 uv;
		uint16_t bone_no[2];
		uint8_t weight;
		uint8_t EdgeFlag;
		uint16_t dummy;
	};
#pragma pack(pop)

	vector<PMD_VERTEX>vertices(vertNum); // バッファーの確保
	for (unsigned int i = 0; i < vertNum; i++)
	{
		fread(&vertices[i], pmdvertex_size, 1, fp);
	}
	//	vector<unsigned char>vertices(vertNum* pmdvertex_size); // バッファーの確保
	//	fread(vertices.data(), vertices.size(), 1, fp); // 読み込み

	//	auto resdesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size());

	ID3D12Resource* vertBuff = nullptr;
	auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resdesc = CD3DX12_RESOURCE_DESC::Buffer(vertices.size() * sizeof(PMD_VERTEX));

	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc, // サイズ変更
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));

	PMD_VERTEX* vertMap = nullptr; // 型をVertexに変更
	//	unsigned char* vertMap = nullptr; // 
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);

	std::copy(std::begin(vertices), std::end(vertices), vertMap);

	vertBuff->Unmap(0, nullptr);


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;// レンダーターゲットビューなのでRTV
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;// 表裏の2つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;// 特に指定なし

	ID3D12DescriptorHeap* rtvHeaps = nullptr;
	result = _dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = _swapchain->GetDesc(&swcDesc);

	// SRGBレンダーターゲットビュー設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {}; // ガンマ補正有り（sRGB）
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	vector<ID3D12Resource*> _backBuffers(swcDesc.BufferCount);

	for (int idx = 0;idx < swcDesc.BufferCount;idx++)
	{
		result = _swapchain->GetBuffer(idx, IID_PPV_ARGS(&_backBuffers[idx]));
		D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += idx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_dev->CreateRenderTargetView(_backBuffers[idx], &rtvDesc, handle);
	}

	// 深度バッファーの作成
	D3D12_RESOURCE_DESC depthhResDesc = {};
	depthhResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元のテクスチャデータ
	depthhResDesc.Width = window_width; // 幅と高さはレンダーターゲットと同じ
	depthhResDesc.Height = window_height; // 同上
	depthhResDesc.DepthOrArraySize = 1; // テクスチャ配列でも、3Dテクスチャでもない
	depthhResDesc.Format = DXGI_FORMAT_D32_FLOAT; // 深度値書き込み用フォーマット
	depthhResDesc.SampleDesc.Count = 1; // サンプルは1ピクセル当たり1つ
	depthhResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // デプスステンシルとして使用

	// 
	D3D12_HEAP_PROPERTIES depthHeapProp = {};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT; // DEFAULTなので後はUNKNOWNでよい
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// このクリアする際の値が重要な意味を持つ
	D3D12_CLEAR_VALUE depthClearValue = {};
	depthClearValue.DepthStencil.Depth = 1.0f; // 負荷さ1.0f（最大値）でクリア
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT; // 32ビットfloat値としてクリア

	ID3D12Resource* depthBuffer = nullptr;
	result = _dev->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthhResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値書き込みに使用
		&depthClearValue,
		IID_PPV_ARGS(&depthBuffer));

	// 深度のためのディスクリプタヒープ制作
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {}; // 深度に使うことがわかればよい
	dsvHeapDesc.NumDescriptors = 1; // 深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // デプスステンシルビューとして使う
	ID3D12DescriptorHeap* dsvHeap = nullptr;
	result = _dev->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	// 深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // 深度値32ビット仕様
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE; // フラグとは特になし

	_dev->CreateDepthStencilView(
		depthBuffer,
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	ID3D12Fence* _fence = nullptr;
	UINT64 _fenceVal = 0;
	result = _dev->CreateFence(_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	float frame = 120;
	float time = 0;
	float oneCycleTime = 6;
	float HalfCycleTime = oneCycleTime / 2;

	// ウィンドウ表示
	ShowWindow(hwnd, SW_SHOW);

	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress(); // バッファーの仮想アドレス
	vbView.SizeInBytes = static_cast<UINT>(vertices.size() * sizeof(PMD_VERTEX)); // 全バイト数
	vbView.StrideInBytes = sizeof(PMD_VERTEX); // 1頂点当たりのバイト数

	unsigned int indicesNum; // インデックス数
	fread(&indicesNum, sizeof(indicesNum), 1, fp);

	std::vector<unsigned short>indices;
	indices.resize(indicesNum);
	fread(indices.data(), indices.size() * sizeof(indices[0]), 1, fp);

	fclose(fp);

	ID3D12Resource* idxBuff = nullptr;
	auto resourcepDesc = CD3DX12_RESOURCE_DESC::Buffer(indices.size() * sizeof(indices[0]));

	// 設定は、バッファーのサイズ以外、頂点バッファーの設定を使いまわしていよい
	result = _dev->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		//&resdesc,
		&resourcepDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&idxBuff));

	// 作ったバッファーにインデックスデータをコピー
	unsigned short* mappendIdx = nullptr;
	//vector<unsigned short> mappendIdx(0);
	idxBuff->Map(0, nullptr, (void**)&mappendIdx);
	//std::copy(indices.begin(), indices.end(), mappendIdx);
	std::copy(std::begin(indices), std::end(indices), mappendIdx);
	idxBuff->Unmap(0, nullptr);

	// インデックスバッファービューを作成
	D3D12_INDEX_BUFFER_VIEW ibView = {};
	ibView.BufferLocation = idxBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = indices.size() * sizeof(indices[0]);



	ID3DBlob* _vsBlob = nullptr;
	ID3DBlob* _psBlob = nullptr;

	ID3DBlob* errorBlob = nullptr;

	// _vsBlob用
	result = D3DCompileFromFile(
		L"BasicVertexShader.hlsl", // シェーダ―名
		nullptr, // defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルードはデフォルト
		"BasicVS", "vs_5_0", // 関数はBasicVS、対称シェーダーはvs5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用および最適化なし
		0,
		&_vsBlob, &errorBlob); // エラー時はerrorBlobにメッセージが入る
	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("ファイルが見当たりません");
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
	// _psBlob用
	result = D3DCompileFromFile(
		L"BasicPixelShader.hlsl", // シェーダ―名
		nullptr, // defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルードはデフォルト
		"BasicPS", "ps_5_0", // 関数はBasicPS、対称シェーダーはvs5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用および最適化なし
		0,
		&_psBlob, &errorBlob); // エラー時はerrorBlobにメッセージが入る
	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			::OutputDebugStringA("ファイルが見当たりません");
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
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{ // uv(追加)
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
			"BONE_NO",0,DXGI_FORMAT_R16G16_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
			"WEIGHT",0,DXGI_FORMAT_R8_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
			"EDGE_FLG",0,DXGI_FORMAT_R8_UINT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},

	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};

	gpipeline.pRootSignature = nullptr; // 後で設定する

	gpipeline.VS.pShaderBytecode = _vsBlob->GetBufferPointer();
	gpipeline.VS.BytecodeLength = _vsBlob->GetBufferSize();
	gpipeline.PS.pShaderBytecode = _psBlob->GetBufferPointer();
	gpipeline.PS.BytecodeLength = _psBlob->GetBufferSize();
	gpipeline.InputLayout.pInputElementDescs = inputLayout; // レイアウト先頭アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout); // レイアウト配列数
	// デフォルトのサンプルマスクを表す定数(0xffffffff)
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// まだ安置エイリアスは使わないためfalse
	gpipeline.RasterizerState.MultisampleEnable = false;

	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // カリングしない
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID; // 中身を塗りつぶす
	gpipeline.RasterizerState.DepthClipEnable = true; // 深度方向のクリッピングは有効に

	gpipeline.RasterizerState.FrontCounterClockwise = false;
	gpipeline.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	gpipeline.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	gpipeline.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	gpipeline.RasterizerState.AntialiasedLineEnable = false;
	gpipeline.RasterizerState.ForcedSampleCount = 0;
	gpipeline.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	// 深度ステンシル
	gpipeline.DepthStencilState.DepthEnable = true; // 深度バッファーを使う
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; // 書き込む
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // 小さい方を採用
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	gpipeline.DepthStencilState.StencilEnable = false;

	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlandDesc = {};
	// 
	renderTargetBlandDesc.BlendEnable = false;
	// 
	renderTargetBlandDesc.LogicOpEnable = false;
	renderTargetBlandDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	gpipeline.BlendState.RenderTarget[0] = renderTargetBlandDesc;

	gpipeline.InputLayout.pInputElementDescs = inputLayout; // レイアウト先頭アドレス
	gpipeline.InputLayout.NumElements = _countof(inputLayout); // レイアウト配列数

	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	gpipeline.NumRenderTargets = 1; // 今は１つのみ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0~1に正規化されたRGBA

	gpipeline.SampleDesc.Count = 1; // サンプリングは1ピクセルにつき1
	gpipeline.SampleDesc.Quality = 0; // クオリティは最低

	ID3D12RootSignature* rootsignature = nullptr;
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {}; // テクスチャと定数の2つ

	// テクスチャ用レジスター0番
	descTblRange[0].NumDescriptors = 1; // テクスチャ1つ
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // 種別はテクスチャ
	descTblRange[0].BaseShaderRegister = 0; // 0番スロットから
	descTblRange[0].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	// 定数用レジスター0番
	descTblRange[1].NumDescriptors = 1; // 定数1つ
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV; // 種別は定数
	descTblRange[1].BaseShaderRegister = 0; // 0番スロットから
	descTblRange[1].OffsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparam = {};

	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootparam.DescriptorTable.pDescriptorRanges = descTblRange; // ディスクリプタレンジのアドレス
	rootparam.DescriptorTable.NumDescriptorRanges = 2; // ディスクリプタレンジ数
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // 全てのシェーダーから見える

	rootSignatureDesc.pParameters = &rootparam; // ルートパラメーターの先頭アドレス
	rootSignatureDesc.NumParameters = 1; // ルートパラメーター数

	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 横方向の繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 縦方向の繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP; // 奥行きの繰り返し
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK; // ボーダーは黒
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; // 線形補間
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX; // ミップマップ最大値
	samplerDesc.MinLOD = 0.0f; // ミップマップ最小値
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // ピクセルシェーダーから見える
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; // リサンプリングしない

	rootSignatureDesc.pStaticSamplers = &samplerDesc; // ルートパラメーターの先頭アドレス
	rootSignatureDesc.NumStaticSamplers = 1; // ルートパラメーター数

	ID3DBlob* rootSigBlob = nullptr;
	result = D3D12SerializeRootSignature(
		&rootSignatureDesc, // ルートシグネチャ設定
		D3D_ROOT_SIGNATURE_VERSION_1_0, // ルートシグネチャバージョン
		&rootSigBlob, // シェーダーを作った時と同じ
		&errorBlob); // エラー処理も同じ

	result = _dev->CreateRootSignature(
		0, // nodemask,0でよい
		rootSigBlob->GetBufferPointer(), // シェーダーの時と同様
		rootSigBlob->GetBufferSize(), // シェーダーの時と同様
		IID_PPV_ARGS(&rootsignature));
	rootSigBlob->Release(); // 不要になったので開放

	gpipeline.pRootSignature = rootsignature;

	ID3D12PipelineState* _pipelinestate = nullptr;

	result = _dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&_pipelinestate));

	D3D12_VIEWPORT viewport = {};
	viewport.Width = window_width; // 出力先の幅(ピクセル数)
	viewport.Height = window_height; // 出力先の高さ(ピクセル数)
	viewport.TopLeftX = 0; // 出力先の左上座標X
	viewport.TopLeftY = 0; // 出力先の左上座標Y
	viewport.MaxDepth = 1.0f; // 深度最大値
	viewport.MinDepth = 0.0f; // 深度最小値

	D3D12_RECT scissorrect = {};
	scissorrect.top = 0; // 切り抜き上座標
	scissorrect.left = 0; // 切り抜き左座標
	scissorrect.right = scissorrect.left + window_width; // 切り抜き右座標
	scissorrect.bottom = scissorrect.top + window_height; // 切り抜き下座標

	// シェーダー側に渡すための基本的な行列データ
	struct MatricesData
	{
		XMMATRIX world; // モデル本体を回転させたり移動させたりする行列
		XMMATRIX viewproj; // ビューとプロジェクション合成行列
	};

	// WICテクスチャのロード
	TexMetadata metadata = {};
	ScratchImage scratchImg = {};

	result = LoadFromWICFile(
		L"img/textest.png", WIC_FLAGS_NONE,
		&metadata, scratchImg);

	auto img = scratchImg.GetImage(0, 0, 0); // 生データ抽出

	// 中間バッファーとしてのアップロードヒープ設定
	D3D12_HEAP_PROPERTIES uploadHeapProp = {};

	// マップ可能にするため、UPLOADにする
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;

	// アップロード用に使用すること前提なのでUNKNOWNでよい
	uploadHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	uploadHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	uploadHeapProp.CreationNodeMask = 0; // 単一アダプターのため0
	uploadHeapProp.VisibleNodeMask = 0; // 単一アダプターのため0

	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_UNKNOWN; // 単なるデータの塊なのでUNKNOWN
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; // 単なるバッファーとして指定
	resDesc.Width = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT) * img->height; // データサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;

	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR; // 連続したデータ
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE; // 特にフラグ無し

	resDesc.SampleDesc.Count = 1; // 通常テクスチャなのでアンチエイリアシングしない
	resDesc.SampleDesc.Quality = 0;

	// 中間バッファー制作
	ID3D12Resource* uploadbuff = nullptr;

	result = _dev->CreateCommittedResource(
		&uploadHeapProp,
		D3D12_HEAP_FLAG_NONE, // 特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadbuff)
	);

	// テクスチャのためのヒープ設定
	D3D12_HEAP_PROPERTIES texHeapProp = {};

	texHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT; // テクスチャ用
	texHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	texHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	texHeapProp.CreationNodeMask = 0; // 単一アダプターのため0
	texHeapProp.VisibleNodeMask = 0; // 単一アダプターのため0

	// リソース設定(変数は使いまわし)
	resDesc.Format = metadata.format;
	resDesc.Width = static_cast<UINT>(metadata.width); // 幅
	resDesc.Height = static_cast<UINT>(metadata.height); // 高さ
	resDesc.DepthOrArraySize = static_cast<uint16_t>(metadata.arraySize);
	resDesc.MipLevels = static_cast<uint16_t>(metadata.mipLevels);
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // レイアウトは決定しない

	ID3D12Resource* texbuff = nullptr;
	result = _dev->CreateCommittedResource(
		&texHeapProp,
		D3D12_HEAP_FLAG_NONE, // 特に指定なし
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST, // コピー先
		nullptr,
		IID_PPV_ARGS(&texbuff));

	uint8_t* mapforImg = nullptr; // image->pixelsと同じ型にする
	result = uploadbuff->Map(0, nullptr, (void**)&mapforImg); // マップ

	auto srcAddress = img->pixels;

	auto rowPitch = AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

	for (int y = 0;y < img->height;++y)
	{
		std::copy_n(srcAddress, rowPitch, mapforImg);

		// 1行ごとのつじつまを合わせる
		srcAddress += img->rowPitch;
		mapforImg += rowPitch;

	}

	uploadbuff->Unmap(0, nullptr); // アンマップ

	D3D12_TEXTURE_COPY_LOCATION src = {}, dst = {};

	// コピー先設定
	dst.pResource = texbuff;
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dst.SubresourceIndex = 0;

	src.pResource = uploadbuff; // 中間バッファー
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT; // フットプリント指定

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint = {};
	UINT nrow;
	UINT64 rowsize, size;
	auto desc = texbuff->GetDesc();
	_dev->GetCopyableFootprints(&desc, 0, 1, 0, &footprint, &nrow, &rowsize, &size);
	src.PlacedFootprint = footprint;
	// コピー元(アップロード側)設定
	src.PlacedFootprint.Offset = 0;
	src.PlacedFootprint.Footprint.Width = static_cast<UINT>(metadata.width);
	src.PlacedFootprint.Footprint.Height = static_cast<UINT>(metadata.height);
	src.PlacedFootprint.Footprint.Depth = static_cast<UINT>(metadata.depth);
	src.PlacedFootprint.Footprint.RowPitch =
		static_cast<UINT>(AlignmentedSize(img->rowPitch, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
	src.PlacedFootprint.Footprint.Format = img->format;

	_cmdList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);


	auto BarrierDesc = CD3DX12_RESOURCE_BARRIER::Transition(
		texbuff, D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//D3D12_RESOURCE_BARRIER BarrierDesc = {};
	//BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//BarrierDesc.Transition.pResource = texbuff;
	//BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	_cmdList->ResourceBarrier(1, &BarrierDesc); // バリア指定実行
	_cmdList->Close();

	// 定数バッファー作成
	XMMATRIX worldMat = XMMatrixRotationY(XM_PIDIV4);

	XMFLOAT3 eye(0, 10, -15);
	XMFLOAT3 target(0, 10, 0);
	XMFLOAT3 up(0, 1, 0);

	auto viewMat = XMMatrixLookAtLH(
		XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	auto projMat = XMMatrixPerspectiveFovLH(
		XM_PIDIV2, // 画角は90°
		static_cast<float>(window_width) / static_cast<float>(window_height), // アスペクト比
		1.0f, // 近い方
		100.0f // 遠い方
	);

	ID3D12Resource* constBuff = nullptr;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	resDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(MatricesData) + 0xff) & ~0xff);
	_dev->CreateCommittedResource(		 // (sizeof(matric) + 256 - 1) % 256 と同等
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff)
	);


	MatricesData* mapMatrix; // マップ先を示すポインター
	result = constBuff->Map(0, nullptr, (void**)&mapMatrix); // マップ
	mapMatrix->world = worldMat;
	mapMatrix->viewproj = viewMat * projMat;
	//*mapMatrix = matrix; // 行列の内容をコピー

	ID3D12CommandList* cmdlists[] = { _cmdList };
	_cmdQueue->ExecuteCommandLists(1, cmdlists);
	_cmdQueue->Signal(_fence, ++_fenceVal);


	if (_fence->GetCompletedValue() != _fenceVal)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		_fence->SetEventOnCompletion(_fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}	_cmdAllocator->Reset(); // 
	_cmdList->Reset(_cmdAllocator, nullptr);

	result = texbuff->WriteToSubresource(
		0,
		nullptr, // 全領域へコピー
		img->pixels, // 元データアドレス
		static_cast<UINT>(img->rowPitch), // 1ラインサイズ
		static_cast<UINT>(img->slicePitch) // 全サイズ
	);

	ID3D12DescriptorHeap* basicDescHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// シェーダーから見えるように
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// マスクは0
	descHeapDesc.NodeMask = 0;
	// SRV1つとCBV1つ
	descHeapDesc.NumDescriptors = 2;
	// シェーダーリソースビュー用
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// 生成
	result = _dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = metadata.format; // RGBA(0.0f~1.0fに正規化)
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // 後述
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1; // ミップマップは私用しないので1

	// デスクリプタの先頭ハンドルを取得して置く
	auto basicHeapHandle = basicDescHeap->GetCPUDescriptorHandleForHeapStart();

	_dev->CreateShaderResourceView(
		texbuff, // ビューと関連付けるバッファー
		&srvDesc, // 先ほど設定したテクスチャ設定情報
		basicHeapHandle // 先頭の場所を示すハンドル
	);

	// 次の場所に移動
	basicHeapHandle.ptr +=
		_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(constBuff->GetDesc().Width);

	// 定数バッファービューの作成
	_dev->CreateConstantBufferView(&cbvDesc, basicHeapHandle);

	float angle = 0.0f;
	bool rotatePlus = true;
	while (true)
	{
		MSG msg;
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			// アプリケーションが終わる時にmessageがWM_QUITになる
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		//Rotation1(angle, rotatePlus);
		//Rotation2(angle, rotatePlus);


		worldMat = XMMatrixRotationY(angle);
		mapMatrix->world = worldMat;
		mapMatrix->viewproj = viewMat * projMat;
		angle += 0.005f;
		// DirectX処理
		// バックバッファのインデックスを取得
		auto bbIdx = _swapchain->GetCurrentBackBufferIndex();


		auto BarrierDesc = CD3DX12_RESOURCE_BARRIER::Transition(
			_backBuffers[bbIdx], D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET); // これだけで済む

		_cmdList->ResourceBarrier(1, &BarrierDesc); // バリア指定実行

		_cmdList->SetPipelineState(_pipelinestate);


		// レンダーターゲットを指定
		auto rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx * _dev->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		auto dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);

		// 画面クリア
		float  clearColor[] = { 1.0f,1.0f,1.0f,1.0f };// 白色
		_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
		_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		_cmdList->RSSetViewports(1, &viewport);
		_cmdList->RSSetScissorRects(1, &scissorrect);
		_cmdList->SetGraphicsRootSignature(rootsignature);

		_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		_cmdList->IASetVertexBuffers(0, 1, &vbView);
		_cmdList->IASetIndexBuffer(&ibView);

		_cmdList->SetDescriptorHeaps(1, &basicDescHeap);

		_cmdList->SetGraphicsRootDescriptorTable(0,
			basicDescHeap->GetGPUDescriptorHandleForHeapStart());
		//_cmdList->SetGraphicsRootSignature(rootsignature);

		_cmdList->DrawIndexedInstanced(indicesNum, 1, 0, 0, 0);
		//_cmdList->DrawInstanced(vertNum, 1, 0, 0);

		_cmdList->ResourceBarrier(1, &BarrierDesc);

		// 命令のクローズ
		_cmdList->Close();

		// コマンドリストの実行
		ID3D12CommandList* cmdlists[] = { _cmdList };
		_cmdQueue->ExecuteCommandLists(1, cmdlists);

		// 待ち
		_cmdQueue->Signal(_fence, ++_fenceVal);

		if (_fence->GetCompletedValue() != _fenceVal) {
			auto event = CreateEvent(nullptr, false, false, nullptr);
			_fence->SetEventOnCompletion(_fenceVal, event); // イベントハンドルの取得
			WaitForSingleObject(event, INFINITE); // イベントが発生するまで無限に待つ
			CloseHandle(event); // イベントハンドルを閉じる

		}

		_cmdAllocator->Reset(); // キューをクリア
		_cmdList->Reset(_cmdAllocator, nullptr); // 再びコマンドリストをためる準備

		// フリップ
		_swapchain->Present(1, 0);
	}

	// もうクラスは使わないので登録解除する
	UnregisterClass(w.lpszClassName, w.hInstance);

	return 0;
}
