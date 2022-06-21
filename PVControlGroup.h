#pragma once

#include "UI/Control/basic/VBasicControl/vuiobject.hpp"

class PVControlGroup {
private:
	std::vector<VUIObject*> GroupObjects;

public:
	PVControlGroup() {

	}

	void Hide() {
		for (auto& Element : GroupObjects) {
			Element->Hide();
		}
	}
	void Show() {
		for (auto& Element : GroupObjects) {
			Element->Show();
		}
	}

	void PushObject(VUIObject* Objects) {
		GroupObjects.push_back(Objects);
	}
};