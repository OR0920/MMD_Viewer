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

		private:
			Result LoadAsPMD(const char* const filepath);
			Result LoadAsPMX(const char* const filepath);

			//// VB = Vertex Buffer 
			//ComPtr<ID3D12Resource> mVB_Resource;
			//D3D12_VERTEX_BUFFER_VIEW mVB_View;

			//// Index Buffer
			//ComPtr<ID3D12Resource> mIB_Resource;
			//D3D12_INDEX_BUFFER_VIEW mIB_View;
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

			const Color& GetBackGroundColor() const;
		private:
			Color mClearColor;
		};

		// parent で指定したウィンドウにDirect3Dでの描画を有効にする
		static const int gFrameCount = 2;
		class GraphicsEngine
		{
		public:
			GraphicsEngine(); ~GraphicsEngine();

			Result Init(const GUI::ParentWindow& parent);

			void Draw(const Scene& scene);

		private:
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