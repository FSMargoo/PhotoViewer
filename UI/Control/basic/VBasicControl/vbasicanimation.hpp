/*
 * VBasicAnimation.hpp
 *	@description  : The Basic Animation Interface
*/

#pragma once

#include "vuiobject.hpp"

VLIB_BEGIN_NAMESPACE

/*
 * VInterpolatorFunctional Functional:
 *	@description  : The Interpolator Functional
*/
namespace VInterpolatorFunctional {
	double AnticipateInterpolator(double x) {
		return x * x * (static_cast<double>(2 + 1) * x - 2);
	}
	double AccelerateDecelerateInterpolator(double x) {
		return (cos((x + 1) * 3.14) / 2.0) + 0.5;
	}
	double AccelerateInterpolator(double x) {
		return pow(x, 2);
	}
	double AnticipateOvershootInterpolator(double x) {
		if (x <= 0.5) {
			return 0.5 * 2 * x * 2 * x * (static_cast<double>(3 + 1) * 2 * x - 3);
		}
		else {
			return 0.5 * ((2 * x - 2) * (2 * x - 2) * (static_cast<double>(3 + 1) * (2 * x - 2) + 3) + 2);
		}
	}
	double DecelerateInterpolator(double x) {
		return 1.0 - pow((1.0 - x), 2 * 1);
	}
	double LinearInterpolator(double x) {
		return x;
	}
	double OvershootInterpolator(double x) {
		return (x - 1) * (x - 1) * (static_cast<double>(2 + 1) * (x - 1) + 2) + 1;
	}
	double CycleInterpolator(double x) {
		return sin(2 * 1 * 3.14 * x);
	}
}

/*
 * VInterpolatorType enum class:
 *	@description  : The Interpolator Type
*/
enum class VInterpolatorType {
	AnticipateInterpolator,           // x * x* ((2 + 1)* x - 2)
	AccelerateDecelerateInterpolator, // (cos((x+ 1) * π) / 2.0) + 0.5
	AccelerateInterpolator,           // pow(x, 2)
	AnticipateOvershootInterpolator,  // 0.5*2*x * 2*x * ((3 + 1) * 2*x - 3)             （0 < x < 0.5）
									  // 0.5 * ((2 * x - 2) * (2 * x - 2) * ((3 + 1) * (2 * x - 2) + 3) + 2)  （0.5 <= x < 1.0）
	DecelerateInterpolator,           // 1.0 - pow((1.0 - x), 2 * 1)
	LinearInterpolator,               // x
	OvershootInterpolator,            // (x-1) * (x-1) * ((2+ 1) *( x-1) + 2)+1
	CycleInterpolator                 // sin(2 * 1* π * x)
};

/*
 * VAnimationInterpolator class:
 *	@description  : The Basic Interpolator of Animation
*/
class VAnimationInterpolator {
private:
	double                        InterpolatorDuration;
	std::function<double(double)> InterpolatorFunction;

public:
	double                        LocalX = 0;

public:
	/*
	 * Build up
	*/

	VAnimationInterpolator(double Dx, VInterpolatorType Interpolator) {
		InterpolatorDuration = Dx;

		using namespace VInterpolatorFunctional;

		switch (Interpolator) {
		case VInterpolatorType::AnticipateInterpolator:
			InterpolatorFunction = AnticipateInterpolator;

			break;
		case VInterpolatorType::AccelerateDecelerateInterpolator:
			InterpolatorFunction = AccelerateDecelerateInterpolator;

			break;
		case VInterpolatorType::AccelerateInterpolator:
			InterpolatorFunction = AccelerateInterpolator;

			break;
		case VInterpolatorType::AnticipateOvershootInterpolator:
			InterpolatorFunction = AnticipateOvershootInterpolator;

			break;
		case VInterpolatorType::DecelerateInterpolator:
			InterpolatorFunction = DecelerateInterpolator;

			break;
		case VInterpolatorType::LinearInterpolator:
			InterpolatorFunction = LinearInterpolator;

			break;
		case VInterpolatorType::OvershootInterpolator:
			InterpolatorFunction = OvershootInterpolator;

			break;
		case VInterpolatorType::CycleInterpolator:
			InterpolatorFunction = CycleInterpolator;

			break;
		default:
			InterpolatorFunction = AnticipateInterpolator;

			break;
		}
	}
	VAnimationInterpolator(double Dx, std::function<double(double)> Interpolator) {
		InterpolatorDuration = Dx;
		InterpolatorFunction = Interpolator;
	}

public:
	/*
	 * InterpolatorEnd Functional:
	 *	@description  : Is the InterpolatorEnd End
	*/
	bool  InterpolatorEnd() {
		return (LocalX >= 1);
	}

	double GetOneFrame() {
		if (InterpolatorEnd() == true) {
			return 1.f;
		}

		auto Result = InterpolatorFunction(LocalX);
		LocalX += InterpolatorDuration;

		return Result;
	}

	void Reset() {
		LocalX = 0;
	}
};

/*
 * VColorInterpolator class:
 *	@description  : The Interpolator of VColor
*/
class VColorInterpolator {
private:
	VColor TargetColor;
	VColor SourceColor;

	VAnimationInterpolator Interpolator;

	bool   AnimationStart = false;

public:
	/*
	 * Build up Functional
	*/

	VColorInterpolator(double Dx, VInterpolatorType Interpolator)
		: Interpolator(Dx, Interpolator) {

	}
	void Start(VColor Source, VColor Target) {
		TargetColor = Target;
		SourceColor = Source;

		AnimationStart = true;

		Interpolator.Reset();
	}

	/*
	 * GetOneFrame Functional:
	 *	@description  : Get One Color
	*/
	VColor GetOneFrame() {
		auto InterpolatorFrame = Interpolator.GetOneFrame();

		int ResultR = SourceColor.GetRed()   + ( TargetColor.GetRed()   - SourceColor.GetRed()   ) * InterpolatorFrame;
		int ResultG = SourceColor.GetGreen() + ( TargetColor.GetGreen() - SourceColor.GetGreen() ) * InterpolatorFrame;
		int ResultB = SourceColor.GetBlue()  + ( TargetColor.GetBlue()  - SourceColor.GetBlue()  ) * InterpolatorFrame;
		int ResultA = SourceColor.GetAlpha() + ( TargetColor.GetAlpha() - SourceColor.GetAlpha() ) * InterpolatorFrame;

		return VColor(ResultR, ResultG, ResultB, ResultA);
	}

	/*
	 * IsAnimationEnd Functional:
	 *	@description  : Get the Stats of Animation
	*/
	bool IsAnimationEnd() {
		if (AnimationStart == true) {
			if (Interpolator.InterpolatorEnd() == true) {
				AnimationStart = false;

				return true;
			}
			else {
				return false;
			}
		}
		else {
			return true;
		}
	}
};

VLIB_END_NAMESPACE