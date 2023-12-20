#ifndef _GUI_UTIL_H_
#define _GUI_UTIL_H_

#include<windows.h>

namespace System
{
	enum Result;

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
		protected:
		};

		class MainWindow : public ParentWindow
		{
		public:
			// �T�C�Y���w�肵����
			Result Create(int width, int height);
			// ����{�^������������true��Ԃ�
			bool IsClose();

			// ���C���E�B���h�E�͈������z�肵�V���O���g��
			static MainWindow& Instance();


			// ���C�u����������Ăяo���֐�
			const HWND GetHandle() const final;
		private:
			bool isClose;
			MainWindow();
			~MainWindow() final;

			HWND mWindowHandle;
			WNDCLASSEX mWindowClass;
		};


		// �̈���Ƀh���b�v���ꂽ�E�B���h�E��F������E�B���h�E
		class FileCatcher
		{
		public:
			FileCatcher(); ~FileCatcher();

			// �e���w�肵����
			// �e�̃N���C�A���g�̈悢���ς��ɃT�C�Y���w�肳���
			// �e�̃T�C�Y�ύX�ɍ��킹�Ď����Œ��������
			Result Create(const ParentWindow& parent);

			bool Update();

			int GetLength() const;
			void GetPath(const char** str) const;
		private:

		};
	}
}

#endif // !_GUI_UTIL_H_


