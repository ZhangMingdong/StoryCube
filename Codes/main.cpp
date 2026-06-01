#include <QApplication>
#include "MainWindow.h"


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName(app.translate("main", "Polygon"));
	app.setOrganizationName("Leoben Ltd.");
	app.setOrganizationDomain("Leoben.cn");
	app.setWindowIcon(QIcon(":images/icon.png"));

    MainWindow view;
    view.show();
    return app.exec();
}
