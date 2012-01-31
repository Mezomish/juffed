#include "PythonPlugin.h"
#include "Log.h"

#include <QtCore>

static Juff::Document* CurrentDoc = NULL;

bool addArgument(PyObject* args, int index, const char* argument);
bool addArgument(PyObject* args, int index, int argument);
bool addArgument(PyObject* args, int index, bool argument);

/*static PyObject* method_preved (PyObject *self, PyObject *args) {
	if ( !PyArg_ParseTuple(args, ":preved") ) {
		return NULL;
	}
	return Py_BuildValue("s", "Preved!");
}

static PyMethodDef MedvedMethods[] = {
	{ "preved", method_preved, METH_VARARGS, "Returns 123" },
	{ NULL, NULL, 0, NULL },
};*/

static PyObject* getCursorPos(PyObject* self, PyObject* args) {
	LOGGER;
	Q_UNUSED(self);
	Q_UNUSED(args);
	
	if ( CurrentDoc == NULL ) {
		return NULL;
	}
	
	int row, col;
	CurrentDoc->getCursorPos(row, col);
	return Py_BuildValue("[i,i]", row, col);
}

static PyObject* setCursorPos(PyObject* self, PyObject* args) {
	LOGGER;
	Q_UNUSED(self);
	
	if ( CurrentDoc == NULL ) {
		return NULL;
	}
	
	int row, col;
	if ( !PyArg_ParseTuple(args, "i|i", &row, &col) ) {
		return Py_BuildValue("s", "Error: can't parse arguments. Expecting 2 integer parameters.");
	}
	
//	qDebug() << row << "," << col;
	CurrentDoc->setCursorPos(row, col);
	return Py_BuildValue("s", "");
}

static PyMethodDef DocMethods[] = {
	{ "getCursorPos", getCursorPos, METH_VARARGS, "" },
	{ "setCursorPos", setCursorPos, METH_VARARGS, "" },
	{ NULL, NULL, 0, NULL },
};






PythonPlugin::PythonPlugin(): QObject(), JuffPlugin() {
	LOGGER;

	globalDict_ = NULL;
	
	// Open and execute the Python file
	FILE* file = fopen("/home/mrz/script_plugin.py", "r");

	Py_Initialize();
	Py_InitModule("juffed_doc", DocMethods);

	if ( file != NULL ) {
		PyRun_SimpleFile(file, "");

		// Get a reference to the main module and global dictionary
		PyObject* mainModule = PyImport_AddModule("__main__");
		if ( mainModule != NULL ) {
			globalDict_ = PyModule_GetDict(mainModule);
		}
		else {
			Log::debug("Couldn't get main module");
		}
	}
	else {
		Log::debug("Couldn't open script file");
	}
}

PythonPlugin::~PythonPlugin() {
	Py_DECREF(globalDict_);
	Py_Finalize();
}

void PythonPlugin::init() {
	LOGGER;
	connect(api(), SIGNAL(docOpened(Juff::Document*, Juff::PanelIndex)), SLOT(onDocOpened(Juff::Document*, Juff::PanelIndex)));
	connect(api(), SIGNAL(docActivated(Juff::Document*)), SLOT(onDocActivated(Juff::Document*)));
}

QString PythonPlugin::name() const {
	PyObject* func = getFunction("name");
	if ( func != NULL ) {
		PyObject* res = PyObject_CallObject(func, NULL);
		if ( res != NULL ) {
			return QString::fromUtf8(PyString_AsString(res));
		}
	}
	return "";
}

QString PythonPlugin::title() const {
	return tr("Python support");
}

QString PythonPlugin::description() const {
	return "";
}

QString PythonPlugin::targetEngine() const {
	return "all";
}

Juff::ActionList PythonPlugin::getActions() const {
	Juff::ActionList list;
	PyObject* func = getFunction("getAction");
	
	if ( func != NULL ) {
		int index = 0;
		PyObject* arg = PyTuple_New(1);
		PyObject* res;
		while ( true ) {
			addArgument(arg, 0, index);
			res = PyObject_CallObject(func, arg);
			
			QString str = QString::fromUtf8(PyString_AsString(res));
//			qDebug() << str;
			if ( str.isEmpty() ) {
				break;
			}
			
			QStringList fields = str.split('|');
			if ( fields.count() >= 3 ) {
				QAction* action = new QAction(fields[0], NULL);
				action->setShortcut(QKeySequence(fields[1]));
				action->setData(fields[2]);
				connect(action, SIGNAL(triggered()), SLOT(onAction()));
				list << action;
			}
			
			++index;
			Py_DECREF(res);
		}
		Py_DECREF(arg);
	}
	return list;
}

void PythonPlugin::onAction() {
	QAction* action = qobject_cast<QAction*>(sender());
	if ( action != 0 ) {
		QString funcName = action->data().toString();
		if ( !funcName.isEmpty() ) {
			PyObject* func = getFunction(funcName.toUtf8().constData());
			if ( func != NULL ) {
				PyObject_CallObject(func, NULL);
			}
		}
	}
}

Juff::ActionList PythonPlugin::mainMenuActions(Juff::MenuID id) const {
	Juff::ActionList list;
	if ( Juff::MenuTools == id ) {
		Juff::ActionList actions = getActions();
		list << actions;
//		list << act_;
	}
	return list;
}



void PythonPlugin::onDocOpened(Juff::Document* doc, Juff::PanelIndex index) {
	LOGGER;
	PyObject* func = getFunction("docOpened");
	if ( func != NULL ) {
		PyObject* args = PyTuple_New(2);

		if ( !addArgument(args, 0, doc->fileName().toLocal8Bit().constData()) )
			return;
		if ( !addArgument(args, 1, (int)index) )
			return;

		PyObject_CallObject(func, args);
		Py_DECREF(args);
	}
}

void PythonPlugin::onDocActivated(Juff::Document* doc) {
	LOGGER;
	
	CurrentDoc = doc;
	
	PyObject* func = getFunction("docActivated");
	if ( func != NULL ) {
		PyObject* args = PyTuple_New(1);

		if ( !addArgument(args, 0, doc->fileName().toLocal8Bit().constData()) )
			return;

		PyObject_CallObject(func, args);
		Py_DECREF(args);
	}
}

void PythonPlugin::onDocClosed(Juff::Document* doc) {
	LOGGER;
	PyObject* func = getFunction("docClosed");
	if ( func != NULL ) {
		PyObject* args = PyTuple_New(1);

		if ( !addArgument(args, 0, doc->fileName().toLocal8Bit().constData()) )
			return;

		PyObject_CallObject(func, args);
		Py_DECREF(args);
	}
}

void PythonPlugin::onDocRenamed(Juff::Document* doc, const QString& oldName) {
	Q_UNUSED(doc);
	Q_UNUSED(oldName);
}

void PythonPlugin::onDocModified(Juff::Document* doc) {
	Q_UNUSED(doc);
}



////////////////////////////////////////////////////////////////////////////////
// Helper functions implementation
////////////////////////////////////////////////////////////////////////////////

PyObject* PythonPlugin::getFunction(const char* funcName) const {
	LOGGER;
	PyObject* func = PyDict_GetItemString(globalDict_, funcName);
	if ( func != NULL ) {
		if ( PyCallable_Check(func) ) {
			return func;
		}
		else {
			Py_DECREF(func);
		}
	}
	return NULL;
}

bool reallyAddArgument(PyObject* args, int index, PyObject* value) {
	LOGGER;
	if ( value != NULL ) {
		PyTuple_SetItem(args, index, value);
		return true;
	}
	else {
		Py_DECREF(args);
		return false;
	}
}

bool addArgument(PyObject* args, int index, const char* argument) {
	LOGGER;
	return reallyAddArgument(args, index, PyString_FromString(argument));
//	return reallyAddArgument(args, index, Py_BuildValue("%s", argument));
}

bool addArgument(PyObject* args, int index, int argument) {
	LOGGER;
	return reallyAddArgument(args, index, PyInt_FromLong(argument));
//	return reallyAddArgument(args, index, Py_BuildValue("%i", argument));
}


Q_EXPORT_PLUGIN2(python, PythonPlugin)
