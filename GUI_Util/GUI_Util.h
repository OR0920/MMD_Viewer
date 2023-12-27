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
#include<dxgi1_4.h>

// myLib
#include"MathUtil.h"

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


	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;


	struct Color
	{
		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
		float a = 1.f;

		Color();
		Color(float _r, float _g, float _b, float _a = 1.f);
	};

	class Model
	{
	public:
		Model();
		Model(const Model& other);
		const Model& operator=(const Model& other);

		Result Load(const char* const filepath);
		void Reset();
		void Draw();

	private:
		Result LoadAsPMD(const char* const filepath);
		Result LoadAsPMX(const char* const filepath);

		ComPtr<ID3D12Resource> mVB_Resource;
		D3D12_VERTEX_BUFFER_VIEW mVB_View;
		ComPtr<ID3D12Resource> mIB_Resource;
		D3D12_INDEX_BUFFER_VIEW mIB_View;

		
	};

	class GraphicsDevice
	{
		friend Model;
	public:
		// �`���̃E�B���h�E�ƃo�b�N�o�b�t�@�����w�肷��
		static Result Init(const ParentWindow& window, const int frameCount);
		// �V���O���g��
		static GraphicsDevice& Instance();
		// �I������
		static void Tern();

		// �`��̏������s��
		void BeginDraw();
		// ��ʃN���A
		void Clear(const Color& clearColor = Color(0.f, 0.f, 1.f));

		// �J�����Z�b�g
		void SetCamera
		(
			const MathUtil::float3 eye,
			const MathUtil::float3 target,
			const MathUtil::float3 up
		);

		// �`�揈�������s����
		void EndDraw();
		
	private:
		static GraphicsDevice* sCanvas;
		GraphicsDevice(const ParentWindow& window, const int frameCount); ~GraphicsDevice();

		Result InitDirect3D();
		Result InitConstantResource();

		Result mIsSuccessInit;
		const int mFrameCount;
		const ParentWindow& mWindow;
		const int mWidth, mHeight;

		static ComPtr<ID3D12Device> GetDevice();

		ComPtr<ID3D12Device> mDevice;
		ComPtr<ID3D12CommandQueue> mCommandQueue;

		ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> mCommandList;
		ComPtr<IDXGISwapChain3> mSwapChain;

		// RT : Render Target, RTV : Render Target View
		ComPtr<ID3D12DescriptorHeap> mRTV_Heap;
		std::vector<ComPtr<ID3D12Resource>>	mRT_Resouces;
		int mCurrentBufferID;

		D3D12_CPU_DESCRIPTOR_HANDLE mRTV_Handle;

		// DSB : Depth Stencil Buffer, DSV : Depth Stencil View
		ComPtr<ID3D12DescriptorHeap> mDSV_Heap;
		ComPtr<ID3D12Resource> mDSB_Resouce;

		D3D12_CPU_DESCRIPTOR_HANDLE mDSV_Handle;

		ComPtr<ID3D12Fence> mFence;
		UINT mFenceValue;

		struct ConstantBuffer
		{
			MathUtil::Matrix world;
			MathUtil::Matrix view;
			MathUtil::Matrix projection;
		};
		ComPtr<ID3D12Resource> mCB_Resource;
		ComPtr<ID3D12DescriptorHeap> mCB_Heap;
		ConstantBuffer* mappedCB;


	};
}

#endif // !_GUI_UTIL_H_


