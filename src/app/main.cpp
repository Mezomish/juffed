#include "AppInfo.h"
#include "JuffEd.h"
#include "MainSettings.h"
#include "Settings.h"

#include <QtGlobal>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTranslator>
#include <QLibraryInfo>
#include <qtsingleapplication.h>

#include <iostream>

void printHelp() {
	std::cout << "Usage:" << std::endl;
	std::cout << "  juffed --help              Print this message" << std::endl;
	std::cout << "  juffed --version           Print JuffEd version" << std::endl;
	std::cout << "  juffed [FILES]             Open [FILES] in JuffEd" << std::endl;
}

void printVersion() {
	std::cout << AppInfo::name().toLocal8Bit().constData() << " " << AppInfo::version().toLocal8Bit().constData() << std::endl;
}

void copyToLocalDir(const QString& subDirName) {
	QDir dir = QDir(AppInfo::dataDirPath() + "/" + subDirName);
	QString localPath = AppInfo::configDirPath() + "/" + subDirName;

	QDir localDir(localPath);
	if ( !localDir.exists() ) {
		localDir.mkpath(localPath);
	}
	foreach (QString file, dir.entryList(QDir::Files)) {
		if ( !QFileInfo(localPath + "/" + file).exists() ) {
			QFile::copy(dir.absolutePath() + "/" + file, localPath + "/" + file);
		}
	}
}

void checkForFirstRun() {
	//	highlight schemes and API lists
	copyToLocalDir("hlschemes");
	copyToLocalDir("apis");
	
	//	sessions
	QString configPath = AppInfo::configDirPath();
	QDir sessionDir(configPath + "/sessions/");
	if ( !sessionDir.exists() )
		sessionDir.mkpath(configPath + "/sessions/");

	//	create the log file
	QFile file(AppInfo::logFile());
	file.open(QIODevice::WriteOnly | QIODevice::Truncate);
	file.close();
}

QString detectLanguage() {
#ifdef Q_OS_UNIX
	QByteArray lang = qgetenv("LC_ALL");
	if ( lang.isEmpty() ) {
		lang = qgetenv("LC_MESSAGES");
	}
	if ( lang.isEmpty() ) {
		lang = qgetenv("LANG");
	}
	if ( !lang.isEmpty() ) {
		return QLocale(lang).name();
	}
#endif
	return QLocale::system().name();
}

void loadTranslations(QApplication& app) {
	QString lng = MainSettings::get(MainSettings::Language);
	if ( lng.compare("auto") == 0 ) {
		lng = detectLanguage();
	}
	QTranslator* translator = new QTranslator();
	if ( translator->load("juffed_" + lng, AppInfo::translationPath()) && !translator->isEmpty() ) {
		app.installTranslator(translator);
	}
	else {
		delete translator;
		translator = NULL;
	}
	
	QTranslator* qtTranslator = new QTranslator;
	qtTranslator->load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(qtTranslator);
	// TODO : load plugins translations
}

void initApp(QApplication& app) {
	app.setOrganizationName("juff");
	app.setApplicationName("juffed");

	checkForFirstRun();

#ifdef APPLEBUNDLE
    // If is the app built as an Apple Bundle, the libjuff is
    // staically linked into the exe. Resources has to be initialized
    // manually in this case. See Qt4 docs: The Qt Resource System.
    Q_INIT_RESOURCE(LibResources);
#endif

	loadTranslations(app);
}

void processParams(JuffEd& juffed, QStringList params) {
	// binary name itself
	params.removeFirst();
	foreach ( QString param, params ) {
// #17 Properly open non-existing files
// When opening a file that does not exist, juffed just focuses/opens, but
//  does not open the file. Juffed should virtually open the file, and create it on save.
//		if ( QFileInfo(param).exists() ) {
			juffed.openDoc(QFileInfo(param).absoluteFilePath());
		}
//	}
}

bool hasValidDoubleDashParam( const QCoreApplication& app ) {
	const QStringList& args = app.arguments();
	
	if ( args.size() == 2 ) {
		QString param = args[1];
		if ( param.compare("--help") == 0 ) {
			printHelp();
			return true;
		}
		else if ( param.compare("--version") == 0 ) {
			printVersion();
			return true;
		}
	}

	return false;
}

int runSingle(int argc, char* argv[]) {
	QtSingleApplication app(argc, argv);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
	initApp(app);

	if ( hasValidDoubleDashParam( app ) ) {
		return 0;
	}

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
#endif
	initApp(app);

	if ( hasValidDoubleDashParam( app ) ) {
		return 0;
	}

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
