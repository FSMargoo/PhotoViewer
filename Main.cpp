/*
 * PhotoViewer ( Version 1.0.0 )
 * <作   者>	: Margoo
 * <编译环境>	: VS2022 + EasyX20220610
 * <邮	 箱>	: 1683691371@qq.com
*/
#include "UI/Control/basic/VBasicControl/vapplication.hpp"
#include "PVMainWindow.hpp"

int main(int argc, char* argv[]) {
	if (argc == 1) {
		VApplication Applicaiton;
		PVMainWindow MainWindow(1113, 680, &Applicaiton);

		return Applicaiton.Exec();
	}
	else {
		VApplication Applicaiton;
		PVMainWindow MainWindow(1113, 680, CodeConvert(argv[1]), &Applicaiton);

		return Applicaiton.Exec();
	}
}