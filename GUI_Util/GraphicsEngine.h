#ifndef _GRAPHICS_ENGINE_H_
#define _GRAPHCIS_ENGINE_H_

#include "GUI_Util.h"

namespace GUI
{
	namespace Graphics
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


		struct Color
		{
			float r = 0.f;
			float g = 0.f;
			float b = 0.f;
			float a = 1.f;

			static const Color Black;
			static const Color Gray;
			static const Color White;

			Color
			(
				float _r,
				float _g,
				float _b,
				float _a = 1.f
			);

			Color();
		};

		class Model
		{
		public:
			Model(); ~Model();

			Result Load(const char* const filepath);

			// ライブラリが呼び出す関数
			void Draw() const;
		private:
			Result LoadAsPMD(const char* const filepath);
			Result LoadAsPMX(const char* const filepath);

			void Reset();

			char* mModelName;
			

		};

		// 描画の単位
		class Scene
		{
		public:
			Scene(); ~Scene();

			void SetBackGroundColor
			(
				const Color clearColor
			);


			void PutModel(const Model& model);
			

			// ライブラリが呼び出す関数
			// ユーザーから呼び出せないようにインターフェイスの引数を指定する
			void Draw(ComPtr<ID3D12GraphicsCommandList> mCommandList, const D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle) const;
		private:
			Color mClearColor;
			const Model* mModel;
		};

		// parent で指定したウィンドウにDirect3Dでの描画を有効にする
		static const int gFrameCount = 2;
		class GraphicsEngine
		{
		public:

			Result Init(const GUI::ParentWindow& parent);
			
			void Draw(const Scene& scene);

			static GraphicsEngine& Instance();
		private:
			GraphicsEngine(); ~GraphicsEngine();
			GraphicsEngine(GraphicsEngine& engine);
			GraphicsEngine& operator=(const GraphicsEngine& engine) const;

			int mParentWidth;
			int mParentHeight;

			ComPtr<ID3D12Device> mDevice;
			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12CommandAllocator> mCommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;
			ComPtr<IDXGISwapChain3> mSwapChain;

			ComPtr<ID3D12DescriptorHeap> mRTV_Heap;
			ComPtr<ID3D12Resource> mRenderTargets[gFrameCount];
			ComPtr<ID3D12DescriptorHeap> mDSV_Heap;
			ComPtr<ID3D12Resource> mDepthBuffer;

			ComPtr<ID3D12Fence> mFence;

			uint64_t mFenceValue;

		};
	}
}
#endif // _GRAPHICS_ENGINE_H_