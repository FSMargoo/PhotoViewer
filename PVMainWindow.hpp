#pragma once

#include "./UI/Control/basic/VBasicControl/vwidget.hpp"
#include "./UI/Control/basic/VBasicControl/vtextlabel.hpp"
#include "./UI/Control/basic/VBasicControl/vimagelabel.hpp"
#include "./UI/Control/basic/VBasicControl/vpushbutton.hpp"
#include "./UI/Control/basic/VBasicControl/viconbutton.hpp"

#include "PVControlGroup.h"

#include <comutil.h>
#include <direct.h>

#ifdef _DEBUG
#	pragma comment(lib, "comsuppwd.lib")
#else
#	pragma comment(lib, "comsuppw.lib")
#endif

std::wstring CodeConvert(char* String) {
	_bstr_t Temp(String);

	return Temp.operator const wchar_t*();
}

class PVImageLabel : public VImageLabel {
public:
	VSignal<> MouseDragBegin;
	VSignal<> MouseDragStop;

public:
	PVImageLabel(VImage* Image, VUIObject* Parent)
		: VImageLabel(Image, Parent) {
	}

	void OnPaint(VCanvas* Canvas) override {
		if (Theme->Image != nullptr) {
			VPainterDevice Device(Canvas);

			Device.DrawImage(Theme->Image, 0, 0);
		}
	}

	void MouseLeftClicked(VMouseClickedFlag Flag) override {
		if (Flag == VMouseClickedFlag::Down) {
			MouseDragBegin.Emit();
		}
		else {
			MouseDragStop.Emit();
		}
	}
	void LosedMouseFocus() override {
		MouseDragStop.Emit();
	}
};

enum class LocalUISurafce {
	StartupUI, MainUI
};

class PVMainWindow : public VMainWindow {
private:
	LocalUISurafce LocalSurface;

private:
	PVControlGroup StartupUI;
	PVControlGroup MainUI;

private:
	VTextLabel*    SoftwareNameLabel;
	VTextLabel*    WelcomeLabel;
	VPushButton*   OpenPictureFile;

private:
	VTextLabel *   PictureNameLabel;
	VTextLabel *   ZoomPercent;
	VIconButton*   ZoomUpButton;
	VIconButton*   ZoomDownButton;
	VIconButton*   ZoomOneToOne;
	PVImageLabel*  ViewPicture;
	VImage*        ViewImage   = nullptr;
	VImage*        ZoomedImage = nullptr;
	VImage         ZoomedCache;

	double         ZoomCoefficient;

	bool           AutoZoom = true;

	std::wstring   ViewImageName;
	std::wstring   ViewImagePath;

	VPoint         OriginOffset;
	VPoint         PositionOffset;

	bool           UserDraging   = false;
	bool           FirstLogMouse = false;

	time_t         LastZoomTime  = 0;

private:
	std::wstring GetASPercent() {
		if (ZoomCoefficient >= 1) {
			return std::to_wstring(ZoomCoefficient * 100).substr(0, 3) + L"%";
		}
		if (0.1 <= ZoomCoefficient && 
			ZoomCoefficient <= 1) {
			return std::to_wstring(ZoomCoefficient * 100).substr(0, 2) + L"%";
		}
		if (ZoomCoefficient <= 0.1) {
			return std::to_wstring(ZoomCoefficient * 100).substr(0, 1) + L"%";
		}
	}
	std::wstring GetASPercent(double Cofficient) {
		if (Cofficient >= 1) {
			return std::to_wstring(Cofficient * 100).substr(0, 3) + L"%";
		}
		if (0.1 <= Cofficient &&
			Cofficient <= 1) {
			return std::to_wstring(Cofficient * 100).substr(0, 2) + L"%";
		}
		if (Cofficient <= 0.1) {
			return std::to_wstring(Cofficient * 100).substr(0, 1) + L"%";
		}
	}

private:
	void InitPath() {
		wchar_t LocalPath[MAX_PATH + 1] = { 0 };
		GetModuleFileName(NULL, LocalPath, MAX_PATH);

		std::wstring PathString = LocalPath;

		PathString = PathString.substr(0, PathString.find_last_of(L"\\"));

		_wchdir(PathString.c_str());
	}

private:
	VImage* ZoomImage(VImage* ResourceImage, int Width, int Height) {
		VImage* Image = new VImage(Width, Height);
		VPainterDevice Device(Image);

		Device.DrawImage(ResourceImage, { 0, 0, Width, Height });

		return Image;
	}

private:
	void UserStartDrag() {
		UserDraging = true;
	}
	void UserEndDrag() {
		UserDraging   = false;
		FirstLogMouse = false;
	}

private:
	void MarginAutoBuild(int Width, int Height) {
		if (LocalSurface == LocalUISurafce::StartupUI) {
			SoftwareNameLabel->Resize(Width, 78);
			WelcomeLabel->Resize(Width, 33);
			WelcomeLabel->Move(0, Height / 2 - 33 / 2 - 100);
			OpenPictureFile->Move(Width / 2 - 189 / 2, Height / 2 - 52 / 2);
		}
		else {
			ZoomUpButton->Move(Width - 186, 21);
			ZoomDownButton->Move(Width - 246, 21);
			ZoomOneToOne->Move(Width - 306, 21);
			ZoomPercent->Move(Width - 147, 33);
			PictureNameLabel->Resize(Width, 78);

			LoadPicturePosition();
		}
	}

	void LoadPicturePosition() {
		if (AutoZoom == true) {
			if (ViewImage->GetWidth() < GetWidth() ||
				ViewImage->GetHeight() < (GetHeight() - 74)) {
				ViewPicture->Resize(ViewImage->GetWidth(), ViewImage->GetHeight());
				ViewPicture->Move(GetWidth() / 2 - ViewImage->GetWidth() / 2,
					GetHeight() / 2 - ViewImage->GetHeight() / 2 + 74);

				ZoomCoefficient = 1;

				ZoomPercent->SetPlaneText(GetASPercent());
			}
			else {
				ZoomCoefficient = static_cast<double>(GetHeight() - 74) / ViewImage->GetHeight();

				ZoomedImage = ZoomImage(ViewImage, ViewImage->GetWidth() * ZoomCoefficient, GetHeight() - 74);

				ViewPicture->SetImage(ZoomedImage);
				ViewPicture->Resize(ZoomedImage->GetWidth(), ZoomedImage->GetHeight());
				ViewPicture->Move(GetWidth() / 2 - ZoomedImage->GetWidth() / 2, 
					GetHeight() / 2 - ZoomedImage->GetHeight() / 2);

				ZoomPercent->SetPlaneText(GetASPercent());
			}
		}
		else {
			ViewPicture->Move(GetWidth() / 2 - ViewPicture->GetWidth() / 2 - PositionOffset.x,
				GetHeight() / 2 - ViewPicture->GetHeight() / 2 + 74 - PositionOffset.y);
		}
	}

	void SetNewPicture() {
		PictureNameLabel->SetPlaneText(ViewImageName);

		if (ViewImage != nullptr) {
			delete ViewImage;
		}
		ViewImage = new VImage(ViewImagePath);

		ViewPicture->SetImage(ViewImage);
		
		LoadPicturePosition();
	}

	void SelectPhoto() {
		OPENFILENAME* FileSelector = new OPENFILENAME{ 0 };
		TCHAR         FilePath[MAX_PATH];

		FileSelector->lStructSize = sizeof(OPENFILENAME);
		
		FileSelector->hwndOwner = GetHWnd();
		FileSelector->lpstrFile = FilePath;
		FileSelector->lpstrFile[0] = '\0';
		
		FileSelector->nMaxFile = sizeof(FilePath);
		
		FileSelector->lpstrFilter = L"All(*.*)\0*.*\0";
		FileSelector->nFilterIndex = 1;
		FileSelector->lpstrFileTitle = NULL;
		FileSelector->nMaxFileTitle = 0;
		FileSelector->Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileName(FileSelector)) {
			std::wstring PictureFilePath    = FilePath;
			int          LastNotDotPosition = static_cast<int>(PictureFilePath.find_last_of(L"\\")) + 1;
			ViewImageName                   = PictureFilePath.substr(LastNotDotPosition, PictureFilePath.size() - LastNotDotPosition);
			std::wstring NewWindowTitle     = L"PhotoViewer (" + ViewImageName + L")";

			ViewImagePath = FilePath;

			LocalSurface = LocalUISurafce::MainUI;

			SetWindowText(GetWinID(), NewWindowTitle.c_str());

			SetNewPicture();

			MarginAutoBuild(GetWidth(), GetHeight());

			StartupUI.Hide();
			MainUI.Show();
		}
	}

	void OneToOne() {
		AutoZoom = false;
		ZoomCoefficient = 1.f;

		PositionOffset = { 0, 0 };

		ZoomPercent->SetPlaneText(L"100%");

		ViewPicture->SetImage(ViewImage);
		ViewPicture->Resize(ViewImage->GetWidth(),
			ViewImage->GetHeight());

		LoadPicturePosition();
	}
	void ZoomUp() {
		AutoZoom = false;

		PositionOffset = { 0, 0 };

		auto Coefficient = ZoomCoefficient + 0.05;

		if (Coefficient >= 0.1 && Coefficient <= 8) {
			if (Coefficient >= 1) {
				ZoomPercent->SetPlaneText(GetASPercent(Coefficient));
			}
			else {
				ZoomPercent->SetPlaneText(GetASPercent(Coefficient));
			}

			if (ZoomedImage != nullptr) {
				delete ZoomedImage;
			}

			ZoomedImage = ZoomImage(ViewImage, ViewImage->GetWidth() * Coefficient,
				ViewImage->GetHeight() * Coefficient);

			ViewPicture->SetImage(ZoomedImage);
			ViewPicture->Resize(ZoomedImage->GetWidth(), ZoomedImage->GetHeight());

			ZoomCoefficient = Coefficient;

			LoadPicturePosition();
		}
	}
	void ZoomDown() {
		AutoZoom = false;

		PositionOffset = { 0, 0 };

		auto Coefficient = ZoomCoefficient - 0.05;

		if (Coefficient >= 0.1 && Coefficient <= 8) {
			if (Coefficient >= 1) {
				ZoomPercent->SetPlaneText(std::to_wstring(Coefficient * 100).substr(0, 3) + L"%");
			}
			else {
				ZoomPercent->SetPlaneText(std::to_wstring(Coefficient * 100).substr(0, 2) + L"%");
			}

			if (ZoomedImage != nullptr) {
				delete ZoomedImage;
			}

			ZoomedImage = ZoomImage(ViewImage, ViewImage->GetWidth() * Coefficient,
				ViewImage->GetHeight() * Coefficient);

			ViewPicture->SetImage(ZoomedImage);
			ViewPicture->Resize(ZoomedImage->GetWidth(), ZoomedImage->GetHeight());

			ZoomCoefficient = Coefficient;

			LoadPicturePosition();
		}
	}

	void DealyMessage(VMessage* Message) override {
		if (LocalSurface == LocalUISurafce::MainUI) {
			if (Message->GetType() == VMessageType::MouseWheelMessage) {
				if (LastZoomTime == 0) {
					LastZoomTime = clock();
				}

				if (clock() - LastZoomTime >= 30) {
					LastZoomTime = 0;

					AutoZoom = false;

					auto WheelMessage = static_cast<VMouseWheelMessage*>(Message);

					auto Coefficient = ZoomCoefficient + static_cast<double>(WheelMessage->WheelValue) / 12000 * 10;

					if (Coefficient >= 0.1 && Coefficient <= 8) {
						if (Coefficient >= 1) {
							ZoomPercent->SetPlaneText(std::to_wstring(Coefficient * 100).substr(0, 3) + L"%");
						}
						else {
							ZoomPercent->SetPlaneText(std::to_wstring(Coefficient * 100).substr(0, 2) + L"%");
						}

						if (ZoomedImage != nullptr) {
							delete ZoomedImage;
						}

						ZoomedImage = ZoomImage(ViewImage, ViewImage->GetWidth() * Coefficient,
							ViewImage->GetHeight() * Coefficient);

						ViewPicture->SetImage(ZoomedImage);
						ViewPicture->Resize(ZoomedImage->GetWidth(), ZoomedImage->GetHeight());

						ZoomCoefficient = Coefficient;

						LoadPicturePosition();
					}
				}
			}

			if (UserDraging == false) {
				return;
			}

			if (LastZoomTime == 0) {
				LastZoomTime = clock();
			}

			if (clock() - LastZoomTime >= 15) {
				LastZoomTime = clock();

				if (Message->GetType() == VMessageType::MouseMoveMessage) {
					AutoZoom = false;

					auto MouseMessage = static_cast<VMouseMoveMessage*>(Message);

					if (FirstLogMouse == false) {
						OriginOffset = MouseMessage->MousePosition;
						OriginOffset.Offset(PositionOffset.x, PositionOffset.y);

						FirstLogMouse = true;
					}
					else {
						PositionOffset.x = (OriginOffset.x - MouseMessage->MousePosition.x);
						PositionOffset.y = (OriginOffset.y - MouseMessage->MousePosition.y);

						LoadPicturePosition();
					}
				}
			}
		}
	}

private:
	void InitStartupUI() {
		SizeOnChange.Connect(this, &PVMainWindow::MarginAutoBuild);

		LocalSurface = LocalUISurafce::StartupUI;

		SetBackgroundColor(VColor(41, 42, 43));

		SoftwareNameLabel = new VTextLabel(this, L"Photo Viewer");
		WelcomeLabel      = new VTextLabel(this, L"欢迎来到 PhotoViewer");
		OpenPictureFile   = new VPushButton(this, L"打开您的图片文件");

		SoftwareNameLabel->SetTextSize(17);
		SoftwareNameLabel->SetBackgroundColor(VColor(57, 57, 57));
		SoftwareNameLabel->SetLineAlignment(VStringAlignment::AlignmentCenter);
		SoftwareNameLabel->SetTextColor(VColor(255, 255, 255));
		SoftwareNameLabel->SetAlignment(VStringAlignment::AlignmentCenter);
		SoftwareNameLabel->Resize(GetWidth(), 78);

		WelcomeLabel->SetTextSize(33);
		WelcomeLabel->Resize(GetWidth(), 33);
		WelcomeLabel->SetLineAlignment(VStringAlignment::AlignmentCenter);
		WelcomeLabel->SetAlignment(VStringAlignment::AlignmentCenter);
		WelcomeLabel->Move(0, GetHeight() / 2 - 33 / 2 - 100);
		WelcomeLabel->SetTextColor(VColor(255, 255, 255));

		OpenPictureFile->Resize(189, 52);
		OpenPictureFile->Move(GetWidth() / 2 - 189 / 2, GetHeight() / 2 - 52 / 2);

		OpenPictureFile->ButtonPushed.Connect(this, &PVMainWindow::SelectPhoto);

		StartupUI.PushObject(SoftwareNameLabel);
		StartupUI.PushObject(WelcomeLabel);
		StartupUI.PushObject(OpenPictureFile);
	}
	void InitMainUI() {
		ViewPicture      = new PVImageLabel(nullptr, this);
		PictureNameLabel = new VTextLabel(this, ViewImageName);

		PictureNameLabel->SetTextSize(17);
		PictureNameLabel->SetBackgroundColor(VColor(57, 57, 57));
		PictureNameLabel->SetLineAlignment(VStringAlignment::AlignmentCenter);
		PictureNameLabel->SetTextColor(VColor(255, 255, 255));
		PictureNameLabel->SetAlignment(VStringAlignment::AlignmentCenter);
		PictureNameLabel->Resize(GetWidth(), 78);

		VImage* OneToOneIcon = new VImage(L"./pv/OneToOne.png");
		VImage* ZoomDownIcon = new VImage(L"./pv/ZoomDown.png");
		VImage* ZoomUpIcon   = new VImage(L"./pv/ZoomUp.png");

		ZoomCoefficient      = 0;

		ZoomUpButton   = new VIconButton(this);
		ZoomDownButton = new VIconButton(this);
		ZoomOneToOne   = new VIconButton(this);

		ZoomUpButton->SetIcon(ZoomUpIcon);
		ZoomDownButton->SetIcon(ZoomDownIcon);
		ZoomOneToOne->SetIcon(OneToOneIcon);

		ZoomUpButton->ButtonPushed.Connect(this, &PVMainWindow::ZoomUp);
		ZoomDownButton->ButtonPushed.Connect(this, &PVMainWindow::ZoomDown);
		ZoomOneToOne->ButtonPushed.Connect(this, &PVMainWindow::OneToOne);

		ZoomUpButton->Resize(43, 43);
		ZoomDownButton->Resize(43, 43);
		ZoomOneToOne->Resize(43, 43);

		ZoomUpButton->Move(GetWidth() - 186, 21);
		ZoomDownButton->Move(GetWidth() - 246, 21);
		ZoomOneToOne->Move(GetWidth() - 306, 21);

		ZoomPercent = new VTextLabel(this, L"Picture Not Loaded");

		ZoomPercent->SetTextSize(18);
		ZoomPercent->Move(GetWidth() - 147, 33);
		ZoomPercent->Resize(100, 18);

		ViewPicture->MouseDragBegin.Connect(this, &PVMainWindow::UserStartDrag);
		ViewPicture->MouseDragStop.Connect(this, &PVMainWindow::UserEndDrag);

		MainUI.PushObject(PictureNameLabel);
		MainUI.PushObject(ViewPicture);
		MainUI.PushObject(ZoomPercent);
		MainUI.PushObject(ZoomUpButton);
		MainUI.PushObject(ZoomOneToOne);
		MainUI.PushObject(ZoomDownButton);

		MainUI.Hide();
	}

public:
	PVMainWindow(int Width, int Height, VApplication* Parent) : VMainWindow(Width, Height, Parent) {
		SetMinimalWidth(400);
		SetMinimalHeight(250);

		InitPath();
		InitMainUI();
		InitStartupUI();
	}
	PVMainWindow(int Width, int Height, std::wstring FilePath, VApplication* Parent) : VMainWindow(Width, Height, Parent) {
		SetMinimalWidth(400);
		SetMinimalHeight(250);

		InitPath();
		InitMainUI();
		InitStartupUI();

		int          LastNotDotPosition = static_cast<int>(FilePath.find_last_of(L"\\")) + 1;
		ViewImageName                   = FilePath.substr(LastNotDotPosition, FilePath.size() - LastNotDotPosition);
		std::wstring NewWindowTitle     = L"PhotoViewer (" + ViewImageName + L")";

		ViewImagePath = FilePath;

		LocalSurface = LocalUISurafce::MainUI;

		SetWindowText(GetWinID(), NewWindowTitle.c_str());

		SetNewPicture();

		MarginAutoBuild(GetWidth(), GetHeight());

		StartupUI.Hide();
		MainUI.Show();
	}
};