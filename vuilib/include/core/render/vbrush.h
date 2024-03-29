#pragma once

#include "vcolor.h"
#include "vimage.h"
#include "vdirectxfactory.h"

VLIB_BEGIN_NAMESPACE

namespace Core {

class VPenBrush {
private:
	ID2D1SolidColorBrush* Brush{};
	ID2D1StrokeStyle*     StrokeStyle{};

	float                 Thickness;

public:
	ID2D1SolidColorBrush* GetDxBrush();
	ID2D1StrokeStyle* GetStrokeStyle();

	float GetThickness() const;
	void SetThickness(const float& TargetThickness);

public:
	VPenBrush(ID2D1RenderTarget* Target, const VColor& Color, const float& LineThickness = 1.f);
	~VPenBrush();
};
class VSolidBrush {
 private:
	 ID2D1SolidColorBrush* Brush{};

 public:
	 ID2D1SolidColorBrush* GetDxBrush();

 public:
	 VSolidBrush(ID2D1RenderTarget* Target, const VColor& Color);
	 ~VSolidBrush();
};
class VBitmapBrush {
 private:
     ID2D1BitmapBrush* Brush{};

 public:
     ID2D1BitmapBrush* GetDxBrush();

 public:
     VBitmapBrush(ID2D1RenderTarget* Target, VImage* Image);
     VBitmapBrush(ID2D1RenderTarget* Target, ID2D1Bitmap* Image);
	 ~VBitmapBrush();
};


}

VLIB_END_NAMESPACE