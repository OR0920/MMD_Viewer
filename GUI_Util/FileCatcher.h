#ifndef _FILE_CATCHER_H_
#define _FILE_CATCHER_H_

namespace GUI_Util
{
	// �̈���Ƀh���b�v���ꂽ�t�@�C����F������E�B���h�E
	class FileCatcher
	{
	public:
		FileCatcher(); ~FileCatcher();

		// �e���w�肵����
		// �e�̃N���C�A���g�̈悢���ς��ɃT�C�Y���w�肳���
		// �e�̃T�C�Y�ύX�ɍ��킹�Ď����Œ��������
		Result Create(const ParentWindow& parent);

		bool Update();

		// �p�X�̒����A�p�X�A�h���b�v���ꂽ�ʒu���擾����
		int GetLength() const;
		int GetWideLength() const;
		const char* const GetPath() const;
		const wchar_t* const GetWidePath() const;

		// �t�@�C�����h���b�v���ꂽ�ꏊ
		struct DropPos
		{
			long x;
			long y;
		};
		const DropPos& GetDropPos() const;

	private:
		FileCatcher(const FileCatcher&) = delete;
		const FileCatcher& operator=(const FileCatcher&) = delete;


		std::string mFilePath;
		bool mIsUpdated;
		wchar_t mWideFilePath[MAX_PATH];
		DropPos mDropPos;


		// ���̃R�[���o�b�N�֐��ɁA�f�[�^���X�V���Ă��炤�B
		static LRESULT CALLBACK FileCatcherProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
		static WNDCLASSEX mWindowClass;
	};
}

#endif // !_FILE_CATCHER_H_
