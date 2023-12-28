#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

// std
#include<string>
#include<vector>

// windows
#include<windows.h>
#include<wrl.h>
#include<tchar.h>
#include<d3d12.h>
#include<dxgi1_6.h>

#ifdef _DEBUG
#define MAIN main 
#else
#define MAIN() WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif // _DEBUG


// GUI���쐬����N���X��������
namespace GUI
{
	enum Result
	{
		SUCCESS = 0,
		FAIL = 1,
		CONTINUE,
		QUIT,
	};

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
		// ���C���E�B���h�E�͈������z�肵�V���O���g��
		static MainWindow& Instance();

		// �T�C�Y���w�肵����
		Result Create(int width, int height);
		// ���b�Z�[�W����������
		// ����{�^������������false��Ԃ�
		Result ProcessMessage();

		const int GetWindowWidth() const;
		const int GetWindowHeight() const;

		// ���C�u����������Ăяo���֐�
		const HWND GetHandle() const;
	private:
		MainWindow();
		~MainWindow();

		int mWidth;
		int mHeight;

		HWND mWindowHandle;
		WNDCLASSEX mWindowClass;
	};

	void ErrorBox(const TCHAR* const message);

	// �̈���Ƀh���b�v���ꂽ�E�B���h�E��F������E�B���h�E
	class FileCatcher
	{
	public:
		// �V���O���g���p�֐�
		static FileCatcher& Instance();

		// �e���w�肵����
		// �e�̃N���C�A���g�̈悢���ς��ɃT�C�Y���w�肳���
		// �e�̃T�C�Y�ύX�ɍ��킹�Ď����Œ��������
		Result Create(const ParentWindow& parent);

		bool Update();

		// �p�X�̒����A�p�X�A�h���b�v���ꂽ�ʒu���擾����
		int GetLength() const;
		const char* const GetPath() const;

		struct DropPos
		{
			long x;
			long y;
		};
		const DropPos& GetDropPos() const;


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

	// D3D12�̔������b�p�[
	namespace Graphics
	{
		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

		Result EnalbleDebugLayer();

		class GraphicsCommand;
		class SwapChain;
		class RenderTarget;


		class Device
		{
		public:
			Device(); ~Device();

			Result Create();

			Result CreateGraphicsCommand(GraphicsCommand& graphicsCommand);
			Result CreateRenderTarget(RenderTarget& rendertarget, const SwapChain& swapchain);
		private:
			ComPtr<ID3D12Device> mDevice;
		};

		class SwapChain
		{
		public:
			SwapChain(); ~SwapChain();

			Result Create
			(
				const GraphicsCommand& device, 
				const ParentWindow& targetWindow, 
				const int frameCount
			);

			// ���C�u��������Ăяo���֐�
			Result GetDesc(void* desc) const;
			Result GetBuffer(const unsigned int bufferID, void** resource) const;
		private:
			ComPtr<IDXGISwapChain4> mSwapChain;

		};

		class GraphicsCommand
		{
			friend Result Device::CreateGraphicsCommand(GraphicsCommand&);
			friend Result SwapChain::Create
			(
				const GraphicsCommand&,
				const ParentWindow&,
				const int
			);
		public:
			GraphicsCommand(); ~GraphicsCommand();

		private:
			ComPtr<ID3D12CommandQueue> mCommandQueue;
			ComPtr<ID3D12CommandAllocator> mCommandAllocator;
			ComPtr<ID3D12GraphicsCommandList> mCommandList;
		};

		class RenderTarget
		{
			friend Result Device::CreateRenderTarget(RenderTarget&, const SwapChain&);
		public:
			RenderTarget(); ~RenderTarget();

		private:
			ComPtr<ID3D12DescriptorHeap> mRTV_Heaps;
			ComPtr<ID3D12Resource>* mRT_Resource;
			int mBufferCount;

			D3D12_VIEWPORT mViewPort;
			D3D12_RECT mScissorRect;
		};

		class DepthStencilBuffer
		{
		public:
			DepthStencilBuffer(); ~DepthStencilBuffer();

		private:
		};

	}
}

#endif // !_GUI_UTIL_H_


