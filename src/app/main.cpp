#include <QApplication>
#include "JuffEd.h"

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	app.setOrganizationName("juff");
	app.setApplicationName("juffed");

	JuffEd juffed;
//	JuffMW mw;
//	mw.show();
	juffed.mainWindow()->show();
	
	return app.exec();
}
