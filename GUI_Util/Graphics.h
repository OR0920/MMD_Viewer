#pragma once

// windows
#include<d3d12.h>
#include<dxgi1_6.h>

// other
#include"Result.h"

namespace GUI
{
	class ParentWindow;

	// D3D12の薄いラッパー
	namespace Graphics
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		struct Color
		{
			float r = 0.f;
			float g = 0.f;
			float b = 0.f;
			float a = 0.f;

			Color
			(
				float _r,
				float _g,
				float _b,
				float _a = 1.f
			);

			Color();
		};

		enum Format : unsigned int
		{
			COLOR_8_4 = 0,	// 色 32bitColor 
			FLOAT_32_4,		// float4
			FORMAT_COUNT
		};

		// デバッグモードを有効にする
		Result EnalbleDebugLayer();

		class GraphicsCommand;
		class SwapChain;
		class RenderTarget;
		class DepthStencilBuffer;
		class RootSignature;
		class InputElementDesc;
		class GraphicsPipeline;
		class VertexBuffer;
		class IndexBuffer;
		class SignateBuffer;
		class ConstantBuffer;
		class Texture2D;


		class DescriptorHeap;

		// デバイス
		// デフォルトのグラフィックデバイスが指定される
		class Device
		{
		public:
			Device(); ~Device();

			Result Create();

			// 各種インターフェイス生成関数
			Result CreateGraphicsCommand(GraphicsCommand& graphicsCommand);

			// レンダーターゲット作成
			Result CreateRenderTarget
			(
				RenderTarget& renderTarget,
				const SwapChain& swapChain
			);


			// 深度バッファのみ使用する場合
			Result CreateDepthBuffer
			(
				DepthStencilBuffer& depthStencilBuffer,
				const SwapChain& swapChain
			);

			Result CreateRootSignature(RootSignature& rootSignature);
			Result CreateGraphicsPipeline(GraphicsPipeline& pipeline);

			// 頂点バッファを生成する
			// vertexBuffer		: 出力	: 頂点バッファのインターフェイス
			// vertexTypeSize	: 入力	: 頂点構造体のサイズ
			// vertexCount		: 入力	: 頂点数
			Result CreateVertexBuffer
			(
				VertexBuffer& vertexBuffer,
				const unsigned int vertexTypeSize,
				const unsigned int vertexCount
			);

			// インデックスバッファを生成する
			// indexBuffer		: 出力	: インデックスバッファのインターフェイス
			// indexTypeSize	: 入力	: 一つのインデックスのサイズ
			// indexCount		: 入力	: インデックスの数
			Result CreateIndexBuffer
			(
				IndexBuffer& indexBuffer,
				const unsigned int indexTypeSize,
				const unsigned int indexCount
			);

			// 定数バッファを生成する
			// constantBuffer	: 出力		: 定数バッファのインターフェイス 
			// viewHeap			: 入力・出力	: 定数バッファの情報を格納する領域
			// bufferStructSize	: 入力		: 定数データ一つのサイズ
			// bufferCount		: 入力		: データ数 
			Result CreateConstantBuffer
			(
				ConstantBuffer& constantBuffer,
				DescriptorHeap& viewHeap,
				const unsigned int bufferStructSize,
				const unsigned int bufferCount = 1
			);

			Result CreateTexture2D
			(
				Texture2D& texture,
				DescriptorHeap& viewHeap
			);

			// 定数バッファ・テクスチャ用のディスクリプタヒープ
			// heap				: 出力	: ヒープのインターフェイス
			// resouceCount		: 入力	: 一度にバインドされる定数バッファ・テクスチャの数
			Result CreateDescriptorHeap
			(
				DescriptorHeap& heap,
				const unsigned int resourceCount
			);
		private:
			ComPtr<ID3D12Device> mDevice;
		};

		// ダブルバッファ以上のレンダーターゲットを管理する
		class SwapChain
		{
		public:
			SwapChain(); ~SwapChain();

			// 指定したウィンドウに描画される
			// command		: 入力	: コマンドのインターフェイス 
			// targetWindow	: 入力	: 描画先
			// frameCount	: 入力	: バッファ数
			Result Create
			(
				GraphicsCommand& command,
				const ParentWindow& targetWindow,
				const int frameCount
			);

			const float GetAspectRatio() const;

			// 現在のバックバッファを取得する際に使用する
			// 0 <= return < frameCount の値が返される
			int GetCurrentBackBufferIndex() const;

			// バッファを入れ替える
			void Present();

			// ライブラリから呼び出す関数
			Result GetDesc(DXGI_SWAP_CHAIN_DESC* desc) const;
			Result GetBuffer(const unsigned int bufferID, ID3D12Resource** resource) const;
		private:
			ComPtr<IDXGISwapChain4> mSwapChain;

			float mAspectRatio;
		};

		// 描画コマンドを発行する
		class GraphicsCommand
		{
			// 無暗にゲッターを作りたくないのでフレンド化
			friend Result Device::CreateGraphicsCommand(GraphicsCommand&);
			friend Result SwapChain::Create
			(
				GraphicsCommand&,
				const ParentWindow&,
				const int
			);
		public:
			GraphicsCommand(); ~GraphicsCommand();

			// 描画開始時に呼ぶ
			void BeginDraw();

			// パイプラインをセット
			void SetGraphicsPipeline(const GraphicsPipeline& pipeline);

			// レンダーターゲットを書き込み可能にする
			void UnlockRenderTarget(const RenderTarget& renderTarget);

			// ビューポートとシザー矩形をセットする。
			void SetViewportAndRect(const RenderTarget& renderTarget);

			// レンダーターゲットをセットする
			void SetRenderTarget
			(
				const RenderTarget& renderTarget
			);

			// レンダーターゲットと深度バッファをセットする
			void SetRenderTarget
			(
				const RenderTarget& renderTarget,
				const DepthStencilBuffer& depthStenilBuffer
			);

			// レンダーターゲットを塗りつぶす
			// デフォルトはグレー
			void ClearRenderTarget(const Color& color);

			// 深度バッファを初期化する
			void ClearDepthBuffer();

			// 定数バッファ、テクスチャ等をバインドする
			// ルートシグネチャで設定した値と対応させる必要がある
			void SetGraphicsRootSignature(const RootSignature& rootSignature);
			void SetDescriptorHeap(const DescriptorHeap& descHeap);
			void SetConstantBuffer
			(
				const ConstantBuffer& constBuffer,
				const int rootParamID,
				const int bufferID = 0
			);

			void SetDescriptorTable
			(
				const SignateBuffer& buffer,
				const int rootParamID,
				const int bufferID = 0
			);

			// 頂点のみで描画
			void SetVertexBuffer
			(
				const VertexBuffer& vertex
			);
			void DrawTriangle(const int vertexCount);

			// 頂点とインデックスで描画
			void SetVertexBuffer
			(
				const VertexBuffer& vertex,
				const IndexBuffer& intdex
			);
			void DrawTriangleList
			(
				const int indexCount, const int offs
			);

			// レンダーターゲットを書き込み不可にする
			void LockRenderTarget(const RenderTarget& renderTarget);

			// 描画終わりに呼び出す
			void EndDraw();

			// バックバッファに切り替え
			void Flip();
		private:

			ID3D12Device* mDevice;
			SwapChain* mSwapChain;

			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12CommandAllocator> mCommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;

			D3D12_CPU_DESCRIPTOR_HANDLE mRTV_Handle;
			D3D12_CPU_DESCRIPTOR_HANDLE mDSV_Handle;

			ComPtr<ID3D12Fence> mFence;
			UINT64 mFenceValue;
		};

		// 描画先を管理する
		// スワップチェインから、描画先の領域をもらう
		// ユーザー側からメンバを呼び出す必要はない
		class RenderTarget
		{
			friend Result Device::CreateRenderTarget(RenderTarget&, const SwapChain&);

		public:
			RenderTarget(); ~RenderTarget();

			const float GetAspectRatio() const;

			// ライブラリから呼び出す関数
			const D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(const int bufferID) const;
			const ComPtr<ID3D12Resource> GetRenderTargetResource(const int bufferID) const;

			const DXGI_FORMAT GetFormat() const;
			const int GetWidth() const;
			const int GetHeight() const;

			const D3D12_VIEWPORT& GetViewPort() const;
			const D3D12_RECT& GetRect() const;
		private:
			ComPtr<ID3D12DescriptorHeap> mHeaps;
			ComPtr<ID3D12Resource>* mResource;
			int mBufferCount;

			int mViewIncrementSize;

			D3D12_VIEWPORT mViewPort;
			D3D12_RECT mScissorRect;
			float mAspectRatio;
		};

		// 深度バッファ
		// レンダーターゲット同様、ユーザー側はこのインターフェイスを生成するだけでよい
		class DepthStencilBuffer
		{
			friend Result Device::CreateDepthBuffer
			(
				DepthStencilBuffer&, const SwapChain&
			);
		public:
			DepthStencilBuffer(); ~DepthStencilBuffer();

			// ライブラリから呼び出す関数
			const D3D12_CPU_DESCRIPTOR_HANDLE& GetDescriptorHandle() const;
		private:
			ComPtr<ID3D12Resource> mDSB_Resource;
			ComPtr<ID3D12DescriptorHeap> mDSV_Heap;
		};

		// ルートシグネチャ周りのインターフェイス
		// 参考: https://learn.microsoft.com/ja-jp/windows/win32/direct3d12/pipelines-and-shaders-with-directx-12A
		// 
		// パイプラインに渡されるデータ(リソース)は4種類で、すべてビューを介して渡される
		// 
		// 1.入力アセンブラ(IA)に渡されるもの
		//		・頂点バッファ
		//		・インデックスバッファ		(なくてもよい)
		// 2.出力マージャー(OM)に渡されるもの
		//		・レンダーターゲット
		//		・深度ステンシルバッファ		(なくてもよい)
		// 3.ストリーム出力(SO)の書き込み先	(なくてもよい)
		// 
		// 4.ルート引数に渡されるもの			(なくてもよい・最大64個)
		//		・定数										(ルート定数)
		//		・定数バッファ、テクスチャ等のディスクリプタ	(ルートディスクリプタ)
		//		・複数のディスクリプタをまとめたテーブル		(ディスクリプタテーブル)
		// 
		// このうち4をどのように使用するかを記述したものがルートシグネチャ
		//

		// ディスクリプタテーブルの設定を行う
		class DescriptorRange
		{
		public:
			DescriptorRange(); ~DescriptorRange();

			// テーブルがいくつか
			void SetRangeCount(const int rangeCount);
			// 何番目のテーブルが、何番のレジスタに対応し、ディスクリプタがいくつあるか
			void SetRangeForCBV(const int rangeID, const int registerID, const int descriptorCount);
			void SetRangeForSRV(const int rangeID, const int registerID, const int descriptorCount);

			// ライブラリから呼び出す関数
			int GetRangeCount() const;
			const D3D12_DESCRIPTOR_RANGE* const GetRange() const;
		private:
			D3D12_DESCRIPTOR_RANGE* mRange;
			int mRangeCount;
		};

		// ルートシグネチャ本体
		class RootSignature
		{
			friend Result Device::CreateRootSignature(RootSignature&);
		public:
			RootSignature(); ~RootSignature();

			// Create前に呼び出す

			// ルート引数がいくつあるか
			void SetParameterCount(const int count);

			// 何番目の引数に何を渡すか
			// ルートパラメータ(現状必要ないので必要になったら作る)

			// ルートディスクリプタ
			void SetParamForCBV(const int paramID, const int registerID);
			void SetParamForSRV(const int paramID, const int registerID);

			// ディスクリプタテーブル
			void SetParamForDescriptorTable
			(
				const int paramID,
				const DescriptorRange& range
			);

			// サンプラーがいくつあるか
			void SetStaticSamplerCount(const int count);
			void SetSamplerDefault(const int samplerID, const int registerID);
			void SetSamplerUV_Clamp(const int samplerID, const int registerID);

			// ライブラリから呼び出す関数
			const ComPtr<ID3D12RootSignature> GetRootSignature() const;
		private:
			ComPtr<ID3D12RootSignature> mRootSignature;
			D3D12_ROOT_SIGNATURE_DESC mDesc;

			D3D12_ROOT_PARAMETER* mRootParamter;
			D3D12_STATIC_SAMPLER_DESC* mSamplerDesc;

			bool IsParamSizeOver(const int i) const;
			bool IsSamplerSizeOver(const int i) const;
		};

		// 入力頂点のレイアウトを記述する
		class InputElementDesc
		{
		public:
			InputElementDesc(); ~InputElementDesc();

			// 構造体の要素数
			void SetElementCount(const int count);

			// 位置
			void SetDefaultPositionDesc(const char* const semantics = "POSITION");
			// 色
			void SetDefaultColorDesc(const char* const semantics = "COLOR");
			// 法線
			void SetDefaultNormalDesc(const char* const semantics = "NORMAL");
			// UV
			void SetDefaultUV_Desc(const char* const semantics = "UV");

			// 任意の4バイト浮動小数点
			void SetFloatParam(const char* const semantics);

			void DebugOutLayout() const;

			// ライブラリが呼び出す関数
			int GetDescCount() const;
			const D3D12_INPUT_ELEMENT_DESC* const GetElementDesc() const;
		private:
			bool IsSizeOver() const;

			int mCount;
			int mLastID;
			D3D12_INPUT_ELEMENT_DESC* mInputElementDesc;
		};

		// パイプライン全体の設定
		class GraphicsPipeline
		{
			friend Result Device::CreateGraphicsPipeline(GraphicsPipeline&);
		public:
			GraphicsPipeline(); ~GraphicsPipeline();

			// Create前に呼び出す


			// 深度バッファ有効化
			void SetDepthEnable();
			// ステンシルも使う場合。別のAPIを作る

			// 透過有効化　呼び出さない場合無効
			void SetAlphaEnable();

			// カリング無効化　呼び出さない場合背面カリング有効
			void SetCullDisable();

			// 前面カリング有効
			void SetFrontCullEnable();

			// 頂点レイアウト
			void SetInputLayout(const InputElementDesc& inputElementDesc);
			// ルートシグネチャ
			void SetRootSignature(const RootSignature& rootSignature);

			// シェーダーへのポインタ
			// ヘッダへ書き出したものをバインドする場合
			void SetVertexShader(const unsigned char* const vertexShader, const int length);
			void SetPixelShader(const unsigned char* const pixelShader, const int length);
#define SetShader(shader) shader, _countof(shader)


			// ファイルを呼び出す場合(未実装)

			// ライブラリから呼び出す関数
			const ComPtr<ID3D12PipelineState> GetPipelineState() const;
		private:
			ComPtr<ID3D12PipelineState> mPipelineState;
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
		};

		// 頂点バッファ
		class VertexBuffer
		{
			friend Result Device::CreateVertexBuffer
			(
				VertexBuffer&,
				const unsigned int,
				const unsigned int
			);
		public:
			VertexBuffer(); ~VertexBuffer();

			// 頂点データをコピー
			Result Copy(const void* const data);
			const int GetVertexCount() const;

			// ライブラリから呼び出す関数
			const D3D12_VERTEX_BUFFER_VIEW* const GetView() const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_VERTEX_BUFFER_VIEW mView;
			int mVertexCount = 0;
		};

		// インデックスバッファ
		class IndexBuffer
		{
			friend Result Device::CreateIndexBuffer
			(
				IndexBuffer&,
				unsigned int,
				unsigned int
			);
		public:
			IndexBuffer(); ~IndexBuffer();

			Result Copy(const void* const data);
			const int GetIndexCount() const;

			// ライブラリから呼び出す関数
			const D3D12_INDEX_BUFFER_VIEW* const GetView() const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_INDEX_BUFFER_VIEW mView;
			int mIndexCount;

		};

		// 定数バッファ、テクスチャ等、ルートシグネチャ経由でバインドされる
		// リソースのインターフェース
		class SignateBuffer
		{
		public:
			virtual ~SignateBuffer();

			virtual const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle(const int i = 0) const = 0;
		private:
		};

		// 定数バッファ
		class ConstantBuffer : public SignateBuffer
		{
			friend Result Device::CreateConstantBuffer
			(
				ConstantBuffer&,
				DescriptorHeap&,
				unsigned int, unsigned int
			);
		public:
			ConstantBuffer(); ~ConstantBuffer();

			// 構造体が定まらないのでポインタへマップ
			// アラインメントにより定数バッファの構造体のサイズにかかわらず
			// 256バイト単位となっているため
			// 定数バッファの構造体そのものの配列ではデータがずれる
			// 
			// unsigned char* mappedData;
			// *reinterpret_cast<Buffer*>(mappedData) = srcData[i];
			// mappedData += (256バイト単位へアラインメントされたバッファのサイズ);
			// 
			// このようにしないとデータがずれる。
			// マテリアルがおかしくなったらここを疑うこと
			Result Map(void** ptr);
			void Unmap();

			const int GetBufferIncrementSize() const;

			//ライブラリから呼び出す関数
			const D3D12_GPU_VIRTUAL_ADDRESS GetGPU_Address(const int i = 0) const;
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle(const int i = 0) const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_CONSTANT_BUFFER_VIEW_DESC mViewDesc;
			D3D12_CPU_DESCRIPTOR_HANDLE mCPU_Handle;
			D3D12_GPU_DESCRIPTOR_HANDLE mGPU_Handle;

			int mViewIncrementSize;
		};

		class TextureData;

		class Texture2D : public SignateBuffer
		{
			friend Result Device::CreateTexture2D
			(
				Texture2D&, DescriptorHeap&
			);
		public:
			Texture2D();
			~Texture2D();

			Result LoadFromFile(const wchar_t* const filePath);
			Result LoadFromFile(const char* const filepath);

			// ライブラリから呼び出す関数
			const D3D12_GPU_DESCRIPTOR_HANDLE GetGPU_Handle(const int i = 0) const;
		private:
			ComPtr<ID3D12Resource> mResource;
			D3D12_SHADER_RESOURCE_VIEW_DESC mViewDesc;
			D3D12_GPU_DESCRIPTOR_HANDLE mGPU_Handle;

			TextureData* mData;

			int mViewIncrementSize;

			Result WriteToSubresource();
		};

		// 定数バッファ、テクスチャのビューを格納するヒープ
		// 描画中に切り替えると処理が重くなるらしいので、できる限り少なく運用すること
		class DescriptorHeap
		{
			friend Result Device::CreateDescriptorHeap
			(
				DescriptorHeap&,
				const unsigned int
			);
			friend Result Device::CreateConstantBuffer
			(
				ConstantBuffer&,
				DescriptorHeap&,
				const unsigned int,
				const unsigned int
			);
			friend Result Device::CreateTexture2D
			(
				Texture2D&,
				DescriptorHeap&
			);
		public:
			DescriptorHeap();
			~DescriptorHeap();

			// ライブラリから呼び出す関数
			const D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentCPU_Handle();
			const D3D12_GPU_DESCRIPTOR_HANDLE GetCurrentGPU_Handle();

			const ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap() const;
		private:
			void MoveToNextHeapPos();

			ComPtr<ID3D12DescriptorHeap> mDescriptorHeap;
			int mDescriptorCount;
			int mViewIncrementSize;
			int mLastID;
		};
	}
}