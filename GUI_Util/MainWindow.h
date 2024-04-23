#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

namespace GUI_Util
{
	// ���C���E�B���h�E
	// �^�C�g���o�[�ƕ���Ȃǂ̃{�^�������E�B���h�E
	class MainWindow : public ParentWindow
	{
	public:
		// ���C���E�B���h�E�͈������z�肵�V���O���g��
		static MainWindow& Instance();

		// �T�C�Y���w�肵����
		Result Create(int width, int height, const LPCTSTR windowTitle);

		// ���b�Z�[�W����������
		// ����{�^������������false��Ԃ�
		// ���b�Z�[�W��҂�
		Result ProsessMessage();

		// ���b�Z�[�W��҂��Ȃ�
		Result ProcessMessageNoWait();

		// �E�B���h�E���̂��̂̃T�C�Y
		const int GetWindowWidth() const;
		const int GetWindowHeight() const;

		// �E�B���h�E�̕`��̈�̃T�C�Y
		const int GetClientWidth() const;
		const int GetClientHeight() const;

		// ���C�u����������Ăяo���֐�
		const HWND GetHandle() const;
	private:
		MainWindow();
		~MainWindow();

		HWND mWindowHandle;
		WNDCLASSEX mWindowClass;
	};
}

#endif // !_MAIN_WINDOW_H_

