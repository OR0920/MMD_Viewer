#pragma once

namespace GUI_Util
{
	// �q�E�B���h�E�����Ă�E�B���h�E�̃C���^�[�t�F�C�X
	// ������p�����Ă��Ȃ��E�B���h�E�͎q�E�B���h�E�����ĂȂ�
	class ParentWindow
	{
	public:
		virtual ~ParentWindow();
		virtual const HWND GetHandle() const = 0;
		virtual const int GetWindowWidth() const = 0;
		virtual const int GetWindowHeight() const = 0;
		virtual	const int GetClientWidth() const = 0;
		virtual const int GetClientHeight() const = 0;
	};
}
