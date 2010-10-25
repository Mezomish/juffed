#include "../3rd_party/qtsingleapplication/qtsingleapplication.h"
#include "AppInfo.h"
#include "JuffEd.h"
#include "MainSettings.h"
#include "Settings.h"

#include <QFileInfo>
#include <QTranslator>

void loadTranslations(QApplication& app) {
	QString lng = MainSettings::get(MainSettings::Language);
	QTranslator* translator = new QTranslator();
	if ( translator->load("juffed_" + lng, AppInfo::translationPath()) ) {
		if ( !translator->isEmpty() ) {
			app.installTranslator(translator);
		}
	}
	
	// TODO : load plugins translations
}

void initApp(QApplication& app) {
	app.setOrganizationName("juff");
	app.setApplicationName("juffed");

#ifdef APPLEBUNDLE
    // If is the app built as an Apple Bundle, the libjuff is
    // staically linked into the exe. Resources has to be initialized
    // manually in this case. See Qt4 docs: The Qt Resource System.
    Q_INIT_RESOURCE(LibResources);
#endif

	loadTranslations(app);
}

void processParams(JuffEd& juffed, QStringList params) {
	params.removeFirst();
	foreach (QString param, params) {
		if ( QFileInfo(param).exists() )
			juffed.openDoc(QFileInfo(param).absoluteFilePath());
	}
}

int runSingle(int argc, char* argv[]) {
	QtSingleApplication app(argc, argv);
	initApp(app);

	// check if instance already exists
	QStringList fileList;
	foreach (QString param, app.arguments()) {
		fileList << QFileInfo(param).absoluteFilePath();
	}
	if ( app.sendMessage(fileList.join("\n")) )
		return 0;

	// instance doesn't exist yet
	JuffEd juffed;
	QObject::connect(&app, SIGNAL(messageReceived(const QString&)), &juffed, SLOT(onMessageReceived(const QString&)));
	app.setActivationWindow(juffed.mainWindow());

	juffed.mainWindow()->show();
	processParams(juffed, app.arguments());

	return app.exec();
}

int runNotSingle(int argc, char* argv[]) {
	QApplication app(argc, argv);
	initApp(app);

	JuffEd juffed;

	juffed.mainWindow()->show();
	processParams(juffed, app.arguments());

	return app.exec();
}

int main(int argc, char* argv[]) {
	Settings::instance()->read("juff", "juffed");
	if ( MainSettings::get(MainSettings::SingleInstance) )
		return runSingle(argc, argv);
	else 
		return runNotSingle(argc, argv);
}
