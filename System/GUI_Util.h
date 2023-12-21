#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

// std
#include<string>

// windows
#include<windows.h>
#include<wrl.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_4.h>

namespace System
{
	enum Result;

	// GUI���쐬����N���X��������
	namespace GUI
	{
		// �q�E�B���h�E�����Ă�E�B���h�E�̃C���^�[�t�F�C�X
		// ���̃C���^�[�t�F�C�X�Ő錾����Ă��郁�\�b�h�́A
		// ���[�U�[������͌Ăяo���Ȃ�
		class ParentWindow
		{
		public:
			virtual ~ParentWindow();
			virtual const HWND GetHandle() const = 0;
			virtual const int GetWindowWidth() const = 0;
			virtual const int GetWindowHeight() const = 0;
		protected:
		};

		// ���C���E�B���h�E�@���j���[�͍��̂Ƃ������Ă��Ȃ�
		class MainWindow : public ParentWindow
		{
		public:
			// �T�C�Y���w�肵����
			Result Create(int width, int height);
			// ����{�^������������true��Ԃ�
			bool IsClose();


			const int GetWindowWidth() const;
			const int GetWindowHeight() const;

			// ���C���E�B���h�E�͈������z�肵�V���O���g��
			static MainWindow& Instance();

			// ���C�u����������Ăяo���֐�
			const HWND GetHandle() const;
		private:
			bool isClose;
			MainWindow();
			~MainWindow();

			int mWidth;
			int mHeight;

			HWND mWindowHandle;
			WNDCLASSEX mWindowClass;
		};


		// �̈���Ƀh���b�v���ꂽ�E�B���h�E��F������E�B���h�E
		class FileCatcher
		{
		public:
			struct DropPos
			{
				long x;
				long y;
			};

			// �e���w�肵����
			// �e�̃N���C�A���g�̈悢���ς��ɃT�C�Y���w�肳���
			// �e�̃T�C�Y�ύX�ɍ��킹�Ď����Œ��������
			Result Create(const ParentWindow& parent);

			bool Update();

			// �p�X�̒����A�p�X�A�h���b�v���ꂽ�ʒu���擾����
			int GetLength() const;
			const char* const GetPath() const;
			const DropPos& GetDropPos() const;

			// �V���O���g���p�֐�
			static FileCatcher& Instance();

			// �R�[���o�b�N�֐��B�@���[�U�[����͌Ăяo���Ȃ��B
			static LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
		private:
			// �R�[���o�b�N�֐�����p�X���擾����s����A
			// ��������Ɩʓ|�Ȃ̂ŃV���O���g��
			FileCatcher(); ~FileCatcher();

			WNDCLASSEX mWindowClass;

			std::string mFilePath;

			static bool sIsUpdated;
			static TCHAR sFilePath[MAX_PATH];
			static DropPos sDropPos;
		};


		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		// parent �Ŏw�肵���E�B���h�E��Direct3D�ł̕`���L���ɂ���
		static const int gFrameCount = 2;
		class GraphicsEngine
		{
		public:
			GraphicsEngine(); ~GraphicsEngine();

			Result Init(const ParentWindow& parent);

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

			// �`��̒P��
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

#endif // !_GUI_UTIL_H_


