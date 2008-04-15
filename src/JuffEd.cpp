/*
JuffEd - A simple text editor
Copyright 2007-2008 Murzin Mikhail

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License 
version 2 as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "JuffEd.h"

//	Qt headers
#include <QtCore/QMap>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QtGui/QCloseEvent>
#include <QtGui/QIcon>
#include <QtGui/QLabel>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>		
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>

//	local headers
#include "AboutDlg.h"
#include "AppInfo.h"
#include "CharsetsSettings.h"
#include "CommandStorage.h"
#include "DocHandler.h"
#include "DocViewer.h"
#include "FindDlg.h"
#include "IconManager.h"
#include "LexerStorage.h"
#include "license.h"
#include "Log.h"
#include "MainSettings.h"
#include "Settings.h"
#include "SettingsDlg.h"
#include "TextDoc.h"
#include "TextDocView.h"

#include "juffed.xpm"

class JuffEdInterior {
public:
	JuffEdInterior(DocHandler* h, QWidget* parent) : 
		charsetsMenu_(0), 
		syntaxMenu_(0), 
		markersMenu_(0), 
		recentFilesMenu_(0), 
		lastCharsetAction_(0),
		lastSyntaxAction_(0) {
		
/*		licenseText_ = 
		

"	GNU GENERAL PUBLIC LICENSE"
"	   Version 2, June 1991"
""
" Copyright (C) 1989, 1991 Free Software Foundation, Inc."
"                          675 Mass Ave, Cambridge, MA 02139, USA"
" Everyone is permitted to copy and distribute verbatim copies"
" of this license document, but changing it is not allowed."
""
"		Preamble"
""
"  The licenses for most software are designed to take away your"
"freedom to share and change it.  By contrast, the GNU General Public"
"License is intended to guarantee your freedom to share and change free"
"software--to make sure the software is free for all its users.  This"
"General Public License applies to most of the Free Software"
"Foundation's software and to any other program whose authors commit to"
"using it.  (Some other Free Software Foundation software is covered by"
"the GNU Library General Public License instead.)  You can apply it to"
"your programs, too."
""
"  When we speak of free software, we are referring to freedom, not"
"price.  Our General Public Licenses are designed to make sure that you"
"have the freedom to distribute copies of free software (and charge for"
"this service if you wish), that you receive source code or can get it"
"if you want it, that you can change the software or use pieces of it"
"in new free programs; and that you know you can do these things."
""
"  To protect your rights, we need to make restrictions that forbid"
"anyone to deny you these rights or to ask you to surrender the rights."
"These restrictions translate to certain responsibilities for you if you"
"distribute copies of the software, or if you modify it."
""
"  For example, if you distribute copies of such a program, whether"
"gratis or for a fee, you must give the recipients all the rights that"
"you have.  You must make sure that they, too, receive or can get the"
"source code.  And you must show them these terms so they know their"
"rights."
""
"  We protect your rights with two steps: (1) copyright the software, and"
"(2) offer you this license which gives you legal permission to copy,"
"distribute and/or modify the software."
""
"  Also, for each author's protection and ours, we want to make certain"
"that everyone understands that there is no warranty for this free"
"software.  If the software is modified by someone else and passed on, we"
"want its recipients to know that what they have is not the original, so"
"that any problems introduced by others will not reflect on the original"
"authors' reputations."
""
"  Finally, any free program is threatened constantly by software"
"patents.  We wish to avoid the danger that redistributors of a free"
"program will individually obtain patent licenses, in effect making the"
"program proprietary.  To prevent this, we have made it clear that any"
"patent must be licensed for everyone's free use or not licensed at all."
""
"  The precise terms and conditions for copying, distribution and"
"modification follow."
""
"	    GNU GENERAL PUBLIC LICENSE"
"   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION"
""
"  0. This License applies to any program or other work which contains"
"a notice placed by the copyright holder saying it may be distributed"
"under the terms of this General Public License.  The 'Program', below,"
"refers to any such program or work, and a 'work based on the Program'"
"means either the Program or any derivative work under copyright law:"
"that is to say, a work containing the Program or a portion of it,"
"either verbatim or with modifications and/or translated into another"
"language.  (Hereinafter, translation is included without limitation in"
"the term 'modification'.)  Each licensee is addressed as 'you'."
""
"Activities other than copying, distribution and modification are not"
"covered by this License; they are outside its scope.  The act of"
"running the Program is not restricted, and the output from the Program"
"is covered only if its contents constitute a work based on the"
"Program (independent of having been made by running the Program)."
"Whether that is true depends on what the Program does."
""
"  1. You may copy and distribute verbatim copies of the Program's"
"source code as you receive it, in any medium, provided that you"
"conspicuously and appropriately publish on each copy an appropriate"
"copyright notice and disclaimer of warranty; keep intact all the"
"notices that refer to this License and to the absence of any warranty;"
"and give any other recipients of the Program a copy of this License"
"along with the Program."
""
"You may charge a fee for the physical act of transferring a copy, and"
"you may at your option offer warranty protection in exchange for a fee."
""
"  2. You may modify your copy or copies of the Program or any portion"
"of it, thus forming a work based on the Program, and copy and"
"distribute such modifications or work under the terms of Section 1"
"above, provided that you also meet all of these conditions:"
""
"    a) You must cause the modified files to carry prominent notices"
"    stating that you changed the files and the date of any change."
""
"    b) You must cause any work that you distribute or publish, that in"
"    whole or in part contains or is derived from the Program or any"
"    part thereof, to be licensed as a whole at no charge to all third"
"    parties under the terms of this License."
""
"    c) If the modified program normally reads commands interactively"
"    when run, you must cause it, when started running for such"
"    interactive use in the most ordinary way, to print or display an"
"    announcement including an appropriate copyright notice and a"
"    notice that there is no warranty (or else, saying that you provide"
"    a warranty) and that users may redistribute the program under"
"    these conditions, and telling the user how to view a copy of this"
"    License.  (Exception: if the Program itself is interactive but"
"    does not normally print such an announcement, your work based on"
"    the Program is not required to print an announcement.)"
""
"These requirements apply to the modified work as a whole.  If"
"identifiable sections of that work are not derived from the Program,"
"and can be reasonably considered independent and separate works in"
"themselves, then this License, and its terms, do not apply to those"
"sections when you distribute them as separate works.  But when you"
"distribute the same sections as part of a whole which is a work based"
"on the Program, the distribution of the whole must be on the terms of"
"this License, whose permissions for other licensees extend to the"
"entire whole, and thus to each and every part regardless of who wrote it."
""
"Thus, it is not the intent of this section to claim rights or contest"
"your rights to work written entirely by you; rather, the intent is to"
"exercise the right to control the distribution of derivative or"
"collective works based on the Program."
""
"In addition, mere aggregation of another work not based on the Program"
"with the Program (or with a work based on the Program) on a volume of"
"a storage or distribution medium does not bring the other work under"
"the scope of this License."
""
"  3. You may copy and distribute the Program (or a work based on it,"
"under Section 2) in object code or executable form under the terms of"
"Sections 1 and 2 above provided that you also do one of the following:"
""
"    a) Accompany it with the complete corresponding machine-readable"
"    source code, which must be distributed under the terms of Sections"
"    1 and 2 above on a medium customarily used for software interchange; or,"
""
"    b) Accompany it with a written offer, valid for at least three"
"    years, to give any third party, for a charge no more than your"
"    cost of physically performing source distribution, a complete"
"    machine-readable copy of the corresponding source code, to be"
"    distributed under the terms of Sections 1 and 2 above on a medium"
"    customarily used for software interchange; or,"
""
"    c) Accompany it with the information you received as to the offer"
"    to distribute corresponding source code.  (This alternative is"
"    allowed only for noncommercial distribution and only if you"
"    received the program in object code or executable form with such"
"    an offer, in accord with Subsection b above.)"
""
"The source code for a work means the preferred form of the work for"
"making modifications to it.  For an executable work, complete source"
"code means all the source code for all modules it contains, plus any"
"associated interface definition files, plus the scripts used to"
"control compilation and installation of the executable.  However, as a"
"special exception, the source code distributed need not include"
"anything that is normally distributed (in either source or binary"
"form) with the major components (compiler, kernel, and so on) of the"
"operating system on which the executable runs, unless that component"
"itself accompanies the executable."
""
"If distribution of executable or object code is made by offering"
"access to copy from a designated place, then offering equivalent"
"access to copy the source code from the same place counts as"
"distribution of the source code, even though third parties are not"
"compelled to copy the source along with the object code."
""
"  4. You may not copy, modify, sublicense, or distribute the Program"
"except as expressly provided under this License.  Any attempt"
"otherwise to copy, modify, sublicense or distribute the Program is"
"void, and will automatically terminate your rights under this License."
"However, parties who have received copies, or rights, from you under"
"this License will not have their licenses terminated so long as such"
"parties remain in full compliance."
""
"  5. You are not required to accept this License, since you have not"
"signed it.  However, nothing else grants you permission to modify or"
"distribute the Program or its derivative works.  These actions are"
"prohibited by law if you do not accept this License.  Therefore, by"
"modifying or distributing the Program (or any work based on the"
"Program), you indicate your acceptance of this License to do so, and"
"all its terms and conditions for copying, distributing or modifying"
"the Program or works based on it."
""
"  6. Each time you redistribute the Program (or any work based on the"
"Program), the recipient automatically receives a license from the"
"original licensor to copy, distribute or modify the Program subject to"
"these terms and conditions.  You may not impose any further"
"restrictions on the recipients' exercise of the rights granted herein."
"You are not responsible for enforcing compliance by third parties to"
"this License."
""
"  7. If, as a consequence of a court judgment or allegation of patent"
"infringement or for any other reason (not limited to patent issues),"
"conditions are imposed on you (whether by court order, agreement or"
"otherwise) that contradict the conditions of this License, they do not"
"excuse you from the conditions of this License.  If you cannot"
"distribute so as to satisfy simultaneously your obligations under this"
"License and any other pertinent obligations, then as a consequence you"
"may not distribute the Program at all.  For example, if a patent"
"license would not permit royalty-free redistribution of the Program by"
"all those who receive copies directly or indirectly through you, then"
"the only way you could satisfy both it and this License would be to"
"refrain entirely from distribution of the Program."
""
"If any portion of this section is held invalid or unenforceable under"
"any particular circumstance, the balance of the section is intended to"
"apply and the section as a whole is intended to apply in other"
"circumstances."
""
"It is not the purpose of this section to induce you to infringe any"
"patents or other property right claims or to contest validity of any"
"such claims; this section has the sole purpose of protecting the"
"integrity of the free software distribution system, which is"
"implemented by public license practices.  Many people have made"
"generous contributions to the wide range of software distributed"
"through that system in reliance on consistent application of that"
"system; it is up to the author/donor to decide if he or she is willing"
"to distribute software through any other system and a licensee cannot"
"impose that choice."
""
"This section is intended to make thoroughly clear what is believed to"
"be a consequence of the rest of this License."
""
"  8. If the distribution and/or use of the Program is restricted in"
"certain countries either by patents or by copyrighted interfaces, the"
"original copyright holder who places the Program under this License"
"may add an explicit geographical distribution limitation excluding"
"those countries, so that distribution is permitted only in or among"
"countries not thus excluded.  In such case, this License incorporates"
"the limitation as if written in the body of this License."
""
"  9. The Free Software Foundation may publish revised and/or new versions"
"of the General Public License from time to time.  Such new versions will"
"be similar in spirit to the present version, but may differ in detail to"
"address new problems or concerns."
""
"Each version is given a distinguishing version number.  If the Program"
"specifies a version number of this License which applies to it and 'any"
"later version', you have the option of following the terms and conditions"
"either of that version or of any later version published by the Free"
"Software Foundation.  If the Program does not specify a version number of"
"this License, you may choose any version ever published by the Free Software"
"Foundation."
""
"  10. If you wish to incorporate parts of the Program into other free"
"programs whose distribution conditions are different, write to the author"
"to ask for permission.  For software which is copyrighted by the Free"
"Software Foundation, write to the Free Software Foundation; we sometimes"
"make exceptions for this.  Our decision will be guided by the two goals"
"of preserving the free status of all derivatives of our free software and"
"of promoting the sharing and reuse of software generally."
""
"		NO WARRANTY"
""
"  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY"
"FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN"
"OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES"
"PROVIDE THE PROGRAM 'AS IS' WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED"
"OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF"
"MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS"
"TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE"
"PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,"
"REPAIR OR CORRECTION."
""
"  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING"
"WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR"
"REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,"
"INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING"
"OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED"
"TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY"
"YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER"
"PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE"
"POSSIBILITY OF SUCH DAMAGES."
""
"		END OF TERMS AND CONDITIONS";*/
		
		handler_ = h;
		viewer_ = new DocViewer(parent);
		handler_->addViewer(viewer_);

		settingsDlg_ = new SettingsDlg(parent);
		aboutDlg_ = new AboutDlg(parent);
		aboutDlg_->setWindowTitle(QObject::tr("About"));
		aboutDlg_->setProgramName(AppInfo::name() + " v" + AppInfo::version());
		QString newLine("<br>"), margin("&nbsp;&nbsp;");
		QString text = QString("   %1   \n\n   Copyright (c) 2007-2008 Mikhail Murzin   ").arg(QObject::tr("Simple text editor with syntax highlighting"));
		QString auth("<br>&nbsp;Mikhail Murzin a.k.a. Mezomish<br>&nbsp;&nbsp;<a href='mailto:mezomish@gmail.com'>mezomish@gmail.com</a>");
		QString thanks = QString("<br>&nbsp;Jarek") 
						+ newLine + margin
						+ QString("<a href='mailto:ajep9691@wp.pl'>ajep9691@wp.pl</a>") 
						+ newLine + margin
						+ QObject::tr("Polish translation")
						+ newLine + newLine;
						
		thanks += QString("&nbsp;Michael Gangolf")
						+ newLine + margin
						+ QString("<a href='mailto:miga@migaweb.de'>miga@migaweb.de</a>")
						+ newLine + margin
						+ QObject::tr("German translation");

		aboutDlg_->setText(text);
		aboutDlg_->setAuthors(auth);
		aboutDlg_->setThanks(thanks);
//		aboutDlg_->setLicense(QCoreApplication::applicationDirPath() + "/COPYING");
		aboutDlg_->setLicense(License::licenseText, false);
		aboutDlg_->resize(500, 250);
		aboutDlg_->setIcon(QIcon(juffed_px));
		
		toolBar_ = 0;
	}
	~JuffEdInterior() {
		delete aboutDlg_;
		delete settingsDlg_;
		delete viewer_;
	}

//	QMap<QString, QString> icons_;
	DocHandler* handler_;
	DocViewer* viewer_;
	SettingsDlg* settingsDlg_;
	AboutDlg* aboutDlg_;
	
	QLabel* cursorPosL_;
	QLabel* fileNameL_;
	QLabel* charsetL_;
	QLabel* syntaxL_;
	QToolBar* toolBar_;
	QMap<QString, QMenu*> mainMenuItems_;
	QMap<QString, QAction*> charsetActions_;
	QMap<QString, QAction*> syntaxActions_;
	QMenu* charsetsMenu_;
	QMenu* syntaxMenu_;
	QMenu* markersMenu_;
	QMenu* recentFilesMenu_;
	QAction* lastCharsetAction_;
	QAction* lastSyntaxAction_;
//	QString licenseText_;
};

JuffEd::JuffEd(DocHandler* handler) : QMainWindow() {
	setWindowTitle("JuffEd");

	jInt_ = new JuffEdInterior(handler, this);
	setAcceptDrops(true);
	
	//	settings
	setGeometry(MainSettings::windowRect());
	if (MainSettings::isMaximized())
		showMaximized();
	applySettings();

	connect(jInt_->settingsDlg_, SIGNAL(applied()), SLOT(applySettings()));

	//	menus and toolbar	
	createCommands();
	createMenuBar();
	createToolBar();

	//	statusbar
	jInt_->cursorPosL_ = new QLabel("");
	jInt_->fileNameL_ = new QLabel("");
	jInt_->charsetL_ = new QLabel("");
	jInt_->syntaxL_ = new QLabel("");
	statusBar()->addWidget(jInt_->cursorPosL_);
	statusBar()->addWidget(jInt_->fileNameL_);
	statusBar()->addWidget(jInt_->charsetL_);
	statusBar()->addWidget(jInt_->syntaxL_);
	
	setCentralWidget(jInt_->viewer_->widget());

	connect(jInt_->viewer_, SIGNAL(docViewChanged(QWidget*)), SLOT(docSwitched(QWidget*)));
	connect(jInt_->viewer_, SIGNAL(docViewCloseRequested(QWidget*)), SLOT(docCloseRequested(QWidget*)));
	connect(jInt_->handler_, SIGNAL(fileNameChanged(Document*)), SLOT(docFileNameChanged(Document*)));
	connect(jInt_->handler_, SIGNAL(recentFileAdded()), SLOT(initRecentFilesMenu()));
	connect(jInt_->handler_, SIGNAL(cursorPositionChanged(int, int)), SLOT(displayCursorPos(int, int)));
}

JuffEd::~JuffEd() {
	MainSettings::setWindowRect(geometry());
	MainSettings::setMaximized(isMaximized());
	if (!jInt_->handler_->sessionName().isEmpty()) {
		MainSettings::setLastSessionName(jInt_->handler_->sessionName());
	}

	delete jInt_;

}

void JuffEd::closeEvent(QCloseEvent* e) {
	if (jInt_->handler_->closeAllDocs())
		e->accept();
	else
		e->ignore();
}

void JuffEd::about() {
	jInt_->aboutDlg_->exec();
}

void JuffEd::aboutQt() {
	QMessageBox::aboutQt(this, tr("About Qt"));
}

void JuffEd::exit() {
	close();
}

void JuffEd::applySettings() {
	jInt_->handler_->applySettings();
	jInt_->viewer_->applySettings();

	setupToolBarStyle();	
	IconManager::instance()->loadTheme(MainSettings::iconTheme());
	createCommands();
	initCharsetsMenu();
}

void JuffEd::createCommands() {
	CommandStorage* st = CommandStorage::instance();
	DocHandler* h = jInt_->handler_;
	IconManager* im = IconManager::instance();
	
	Command cmds[] = {
		Command(ID_FILE_NEW,	tr("New"),		im->icon("fileNew"),	QKeySequence("Ctrl+N"), h, SLOT(docNew())),
		Command(ID_FILE_OPEN,	tr("Open"),		im->icon("fileOpen"),	QKeySequence("Ctrl+O"), h, SLOT(docOpen())),
		Command(ID_FILE_SAVE,	tr("Save"),		im->icon("fileSave"),	QKeySequence("Ctrl+S"), h, SLOT(docSave())),
		Command(ID_FILE_SAVE_AS,tr("Save as"),	im->icon("fileSaveAs"),	QKeySequence("Ctrl+Shift+S"),			h, SLOT(docSaveAs())),
		Command(ID_FILE_RELOAD, tr("Reload"),	im->icon("reload"),		QKeySequence("F5"),		h, SLOT(docReload())),
		Command(ID_FILE_CLOSE,	tr("Close"),	im->icon("fileClose"),	QKeySequence("Ctrl+W"), h, SLOT(docClose())),
		Command(ID_FILE_CLOSE_ALL, tr("Close all"), im->icon("fileCloseAll"), QKeySequence(),	h, SLOT(docCloseAll())),
		Command(ID_EXIT,		tr("Exit"),		im->icon("exit"),		QKeySequence(),			this, SLOT(exit())),
		//
		Command(ID_EDIT_CUT,	tr("Cut"),		im->icon("editCut"),	QKeySequence("Ctrl+X"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_COPY,	tr("Copy"),		im->icon("editCopy"),	QKeySequence("Ctrl+C"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_PASTE,	tr("Paste"),	im->icon("editPaste"),	QKeySequence("Ctrl+V"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_UNDO,	tr("Undo"),		im->icon("editUndo"),	QKeySequence("Ctrl+Z"), h, SLOT(processTheCommand())),
		Command(ID_EDIT_REDO,	tr("Redo"),		im->icon("editRedo"),	QKeySequence("Ctrl+R"), h, SLOT(processTheCommand())),
		Command(ID_FIND,		tr("Find"),		im->icon("find"),		QKeySequence("Ctrl+F"), h, SLOT(processTheCommand())),
		Command(ID_FIND_NEXT,	tr("Find next"), im->icon("findNext"),	QKeySequence("F3"), h, SLOT(processTheCommand())),
		Command(ID_FIND_PREV,	tr("Find previous"), im->icon("findPrev"), QKeySequence("Shift+F3"), h, SLOT(processTheCommand())),
		Command(ID_GOTO_LINE,	tr("Go to line"), im->icon("gotoLine"),	QKeySequence("Ctrl+G"), h, SLOT(processTheCommand())),
		//
		Command(ID_VIEW_SHOW_LINE_NUMBERS,	tr("Show line numbers"),	QIcon(),	QKeySequence("F11"), h, SLOT(processTheCommand())),
		Command(ID_VIEW_WIDTH_ADJUST,		tr("Adjust text by width"),	QIcon(),	QKeySequence("F10"), h, SLOT(processTheCommand())),
		//
		Command(ID_MARKER_TOGGLE,			tr("Add/Remove marker"),	im->icon("addRemoveMarker"),	QKeySequence("Ctrl+B"), this, SLOT(toggleMarker())),                    
		Command(ID_MARKER_NEXT,				tr("Next marker"),			im->icon("nextMarker"),			QKeySequence("Ctrl+Alt+PgDown"),this, SLOT(nextMarker())),
		Command(ID_MARKER_PREV,				tr("Previous marker"),		im->icon("prevMarker"),			QKeySequence("Ctrl+Alt+PgUp"),this, SLOT(prevMarker())),
		Command(ID_MARKER_REMOVE_ALL,		tr("Remove all markers"),	im->icon("removeAllMarkers"),	QKeySequence(), 		this, SLOT(removeAllMarkers())),
		//
		Command(ID_DOC_PREV,	tr("Previous"),	im->icon("docPrevious"),	QKeySequence("Alt+Left"), h, SLOT(processTheCommand())),
		Command(ID_DOC_NEXT,	tr("Next"),	im->icon("docNext"),	QKeySequence("Alt+Right"), h, SLOT(processTheCommand())),
		//
		Command(ID_SETTINGS,	tr("Settings"),	im->icon("settings"),	QKeySequence(), this, SLOT(settings())),
		//
		Command(ID_SESSION_NEW,	tr("New session"),	im->icon("newSession"),	QKeySequence(), h, SLOT(processTheCommand())),
		Command(ID_SESSION_OPEN, tr("Open session"), im->icon("openSession"), QKeySequence(), h, SLOT(processTheCommand())),
		Command(ID_SESSION_SAVE, tr("Save session"), im->icon("saveSession"), QKeySequence(), h, SLOT(processTheCommand())),
		Command(ID_SESSION_SAVE_AS, tr("Save session as"), im->icon("saveSessionAs"), QKeySequence(), h, SLOT(processTheCommand())),
		//
		Command(ID_ABOUT,		tr("About"),	im->icon("about"),		QKeySequence(), this, SLOT(about())),
		Command(ID_ABOUT_QT,	tr("About Qt"),	im->icon("aboutQt"),	QKeySequence(), this, SLOT(aboutQt())),
		Command()
	};

	for (unsigned i = 0; cmds[i].id != 0; i++) {
		Command& cmd = cmds[i];
		if (st->action(cmd.id) == 0)
			st->registerCommand(cmds[i]);
		else
			st->action(cmd.id)->setIcon(cmd.icon);
	}

	CommandStorage::instance()->action(ID_VIEW_SHOW_LINE_NUMBERS)->setCheckable(true);
	CommandStorage::instance()->action(ID_VIEW_WIDTH_ADJUST)->setCheckable(true);

	if (jInt_->recentFilesMenu_ != 0)
		jInt_->recentFilesMenu_->setIcon(im->icon("fileOpen"));
}

void JuffEd::createMenuBar() {
	menuBar()->clear();

	CommandID fileMenu[] = { ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_FILE_SAVE_AS, 
					ID_FILE_RELOAD, ID_FILE_CLOSE, ID_FILE_CLOSE_ALL, ID_SEPARATOR, 
					ID_SESSION_NEW, ID_SESSION_OPEN, ID_SESSION_SAVE, ID_SESSION_SAVE_AS, 
					ID_SEPARATOR, ID_EXIT, ID_NONE };

	CommandID editMenu[] = { ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, ID_SEPARATOR, ID_EDIT_UNDO, 
					ID_EDIT_REDO, ID_SEPARATOR, ID_FIND, ID_FIND_NEXT, ID_FIND_PREV, 
					ID_SEPARATOR, ID_GOTO_LINE, ID_NONE };

	CommandID viewMenu[] = { ID_VIEW_SHOW_LINE_NUMBERS, ID_VIEW_WIDTH_ADJUST, ID_NONE };

	CommandID markersMenu[] = { ID_NONE };	

	CommandID toolsMenu[] = { ID_SETTINGS, ID_NONE };

	CommandID helpMenu[] = { ID_ABOUT, ID_ABOUT_QT, ID_NONE };

	CommandID* menu_ids[] = { fileMenu, editMenu, viewMenu, markersMenu, toolsMenu, helpMenu, 0 };
	QString menus[] = { tr("&File"), tr("&Edit"), tr("&View"), tr("&Markers"), tr("&Tools"), tr("&Help"), "" };


	for (unsigned i = 0; !menus[i].isEmpty(); i++) {
		QMenu* menu = menuBar()->addMenu(menus[i]);
		jInt_->mainMenuItems_[menus[i]] = menu;

		for (int j = 0; menu_ids[i][j] != ID_NONE; j++) {
			CommandID id = menu_ids[i][j];
			if (id == ID_SEPARATOR)
				menu->addSeparator();
			else {
				QAction* action = CommandStorage::instance()->action(id);
				if (action != 0) {
					menu->addAction(action);
				}
			}
		}
	}

	QAction* action = CommandStorage::instance()->action(ID_DOC_PREV);
	if (action != 0)
		addAction(action);
	action = CommandStorage::instance()->action(ID_DOC_NEXT);
	if (action != 0)
		addAction(action);

	//	charset menu
	jInt_->charsetsMenu_ = new QMenu(tr("Charset"));
	QMenu* vMenu = jInt_->mainMenuItems_.value(tr("&View"), 0);
	if (vMenu != 0) {
		vMenu->addSeparator();
		vMenu->addMenu(jInt_->charsetsMenu_);
	}
	initCharsetsMenu();

	//	lexers menu
	jInt_->syntaxMenu_ = new QMenu(tr("Syntax"));
	if (vMenu != 0) {
		vMenu->addMenu(jInt_->syntaxMenu_);
		QStringList sList;
		LexerStorage::instance()->getLexersList(sList);
		foreach (QString s, sList) {
			QAction* a = jInt_->syntaxMenu_->addAction(s, this, SLOT(syntaxSelected()));
			a->setCheckable(true);
			jInt_->syntaxActions_[s] = a;
		}	
	}
	
	//	markers menu
	initMarkersMenu();
	QMenu* markMenu = jInt_->mainMenuItems_.value(tr("&Markers"), 0);
	if (markMenu != 0) {
		connect(markMenu, SIGNAL(aboutToShow()), SLOT(initMarkersMenu()));
	}

	//	recent files
	jInt_->recentFilesMenu_ = new QMenu(tr("Recent files"));
	jInt_->recentFilesMenu_->setIcon(IconManager::instance()->icon("fileOpen"));
	QMenu* fMenu = jInt_->mainMenuItems_.value(tr("&File"), 0);
	QAction* openAct = CommandStorage::instance()->action(ID_FILE_SAVE);
	if (fMenu != 0 && openAct != 0) {
		fMenu->insertMenu(openAct, jInt_->recentFilesMenu_);
	}
	initRecentFilesMenu();
}

void JuffEd::createToolBar() {
	CommandID actions[] = {ID_FILE_NEW, ID_FILE_OPEN, ID_FILE_SAVE, ID_SEPARATOR, 
					ID_EDIT_CUT, ID_EDIT_COPY, ID_EDIT_PASTE, ID_SEPARATOR, 
					ID_EDIT_UNDO, ID_EDIT_REDO, ID_SEPARATOR, ID_FIND, ID_NONE};
	jInt_->toolBar_ = addToolBar("Main");
	for (unsigned i = 0; actions[i] != ID_NONE; i++) {
		CommandID id = actions[i];
		if (id == ID_SEPARATOR)
			jInt_->toolBar_->addSeparator();
		else
			jInt_->toolBar_->addAction(CommandStorage::instance()->action(actions[i]));
	}

	setupToolBarStyle();	
}

void JuffEd::initCharsetsMenu() {
	if (jInt_->charsetsMenu_ == 0)
		return;

	jInt_->charsetsMenu_->clear();
	jInt_->charsetActions_.clear();

	QStringList charsets = CharsetsSettings::getCharsetsList();
	foreach (QString charset, charsets) {
		if (CharsetsSettings::charsetEnabled(charset)) {
			QAction* action = jInt_->charsetsMenu_->addAction(charset, this, SLOT(charsetSelected()));
			action->setCheckable(true);
			jInt_->charsetActions_[charset] = action;
		}
	}
}

void JuffEd::initMarkersMenu() {
	QMenu* markersMenu = jInt_->mainMenuItems_.value(tr("&Markers"), 0);
	if (markersMenu != 0) {
		markersMenu->clear();
		markersMenu->addAction(CommandStorage::instance()->action(ID_MARKER_TOGGLE));
		markersMenu->addAction(CommandStorage::instance()->action(ID_MARKER_NEXT));
		markersMenu->addAction(CommandStorage::instance()->action(ID_MARKER_PREV));
		markersMenu->addAction(CommandStorage::instance()->action(ID_MARKER_REMOVE_ALL));
		markersMenu->addSeparator();
		
		TextDoc* doc = getCurrentTextDoc();
		if (doc == 0 || doc->isNull())
			return;

		TextDocView* tdView = qobject_cast<TextDocView*>(doc->view());		

		if (tdView == 0)
			return;

		IntList markers = tdView->markers();
		foreach (int m, markers) {
			QString str = tdView->markedLine(m).simplified();
			if (str.length() > 40)
				str = str.left(40) + " ...";
			markersMenu->addAction(QString("%1: %2").arg(m).arg(str), this, SLOT(gotoMarker()));
		}
	}
}

void JuffEd::initRecentFilesMenu() {
	if (jInt_->recentFilesMenu_ == 0)
		return;

	jInt_->recentFilesMenu_->clear();
	
	QStringList fileList;
	jInt_->handler_->getRecentFiles(fileList);
	foreach (QString fileName, fileList) {
		jInt_->recentFilesMenu_->addAction(fileName, jInt_->handler_, SLOT(docOpenRecent()));
	}
	if (fileList.count() == 0)
		jInt_->recentFilesMenu_->setEnabled(false);
	else
		jInt_->recentFilesMenu_->setEnabled(true);
}
	

void JuffEd::changeCurrentCharsetAction(QAction* a) {
	if (jInt_->lastCharsetAction_ != 0)
		jInt_->lastCharsetAction_->setChecked(false);

	if (a != 0) {
		jInt_->lastCharsetAction_ = a;
		a->setChecked(true);
	}
}

void JuffEd::changeCurrentSyntaxAction(QAction* a) {
	if (jInt_->lastSyntaxAction_ != 0)
		jInt_->lastSyntaxAction_->setChecked(false);

	if (a != 0) {
		jInt_->lastSyntaxAction_ = a;
		a->setChecked(true);
	}
}

void JuffEd::charsetSelected() {
	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		TextDoc* doc = getCurrentTextDoc();
		if (doc != 0 && !doc->isNull()) {
			if (doc->setCharset(a->text())) {
				displayCharset(doc->charset());
				changeCurrentCharsetAction(a);
			}
			else {
				a->setChecked(false);
			}
		}
		else {
			a->setChecked(false);
		}
	}
}

void JuffEd::syntaxSelected() {
	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		TextDoc* doc = getCurrentTextDoc();
		if (doc != 0 && !doc->isNull()) {
			TextDocView* tdView = qobject_cast<TextDocView*>(doc->view());
			if (tdView != 0) {
				tdView->setSyntax(a->text());
				displaySyntax(a->text());
				changeCurrentSyntaxAction(a);
			}
			else {
				a->setChecked(false);
			}
		}
		else {
			a->setChecked(false);
		}
	}
}

void JuffEd::settings() {
	jInt_->settingsDlg_->exec();
}

void JuffEd::setupToolBarStyle() {
	if (jInt_->toolBar_ != 0) {
		//	toolbar style
		int style = MainSettings::toolButtonStyle();
		if (style <= 4) {
			jInt_->toolBar_->show();
			jInt_->toolBar_->setToolButtonStyle((Qt::ToolButtonStyle)style);
		}
		else {
			jInt_->toolBar_->hide();
		}

		//	toolbar icon size
		int iconSize = MainSettings::iconSize();
		switch (iconSize) {
			case 0: jInt_->toolBar_->setIconSize(QSize(16, 16)); break;
			case 1: jInt_->toolBar_->setIconSize(QSize(24, 24)); break;
			case 2: jInt_->toolBar_->setIconSize(QSize(32, 32)); break;
			default: jInt_->toolBar_->setIconSize(QSize(24, 24)); break;
		}
	}
}

TextDoc* JuffEd::getCurrentTextDoc() {
	TextDoc* tDoc = qobject_cast<TextDoc*>(jInt_->handler_->currentDoc());
	return tDoc;
}

void JuffEd::displayCursorPos(int row, int col) {
	QString text(tr("Row: %1, Col: %2"));
	if (row == -1 && col == -1)
		jInt_->cursorPosL_->setText(" ");
	else
		jInt_->cursorPosL_->setText(text.arg(row + 1).arg(col + 1));
}

void JuffEd::displayFileName(const QString& fileName) {
	jInt_->fileNameL_->setText(QString(" %1 ").arg(fileName));
	QString title("JuffEd");
	if (!jInt_->handler_->sessionName().isEmpty())
		title += " - [" + jInt_->handler_->sessionName() + "]";
	if (!fileName.isEmpty())
		title += " - " + QFileInfo(fileName).fileName();
	setWindowTitle(title);
}

void JuffEd::displayCharset(const QString& charset) {
	jInt_->charsetL_->setText(QString(" %1 ").arg(charset));
	changeCurrentCharsetAction(jInt_->charsetActions_[charset]);
}

void JuffEd::displaySyntax(const QString& syntax) {
	jInt_->syntaxL_->setText(QString(" %1 ").arg(syntax));
	changeCurrentSyntaxAction(jInt_->syntaxActions_[syntax]);
}

void JuffEd::docFileNameChanged(Document* doc) {
	if (doc != 0 && !doc->isNull()) {
		displayFileName(doc->fileName());
	}
}

void JuffEd::docCloseRequested(QWidget* w) {
	TextDocView* tdView = qobject_cast<TextDocView*>(w);
	if (tdView != 0) {
		Document* doc = tdView->document();
		if (doc != 0) {
			jInt_->handler_->closeDocument(doc);
		}
	}
}

void JuffEd::docSwitched(QWidget* w) {
	TextDocView* tdView = qobject_cast<TextDocView*>(w);

	if (tdView == 0) {
		displayCursorPos(-1, -1);
		displayFileName("");
		displayCharset("");
		displaySyntax("none");
		return;
	}

	//	show line numbers menu item
	QAction* showLinesAction = CommandStorage::instance()->action(ID_VIEW_SHOW_LINE_NUMBERS);
	if (showLinesAction != 0) {
		showLinesAction->setChecked(tdView->lineNumIsVisible());
	}

	//	width adjust menu item
	QAction* adjustAction = CommandStorage::instance()->action(ID_VIEW_WIDTH_ADJUST);
	if (adjustAction != 0) {
		adjustAction->setChecked(tdView->isAdjustedByWidth());
	}

	displayFileName(tdView->document()->fileName());
	int row(0), col(0);
	tdView->getCursorPos(row, col);
	displayCursorPos(row, col);
	displaySyntax(tdView->syntax());

	//	charset menu item
	TextDoc* doc = qobject_cast<TextDoc*>(tdView->document());
	if (doc == 0) {
		displayCharset("");
	}
	else {
		displayCharset(doc->charset());
		jInt_->handler_->docActivated(doc);
	}
}

void JuffEd::toggleMarker() {
	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->toggleMarker();
	}
}

void JuffEd::nextMarker() {
	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->gotoNextMarker();
	}
}

void JuffEd::prevMarker() {
	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->gotoPrevMarker();
	}
}

void JuffEd::removeAllMarkers() {
	QWidget* view = jInt_->viewer_->currentView();	
	TextDocView* tdView = qobject_cast<TextDocView*>(view);

	if (tdView != 0) {
		tdView->removeAllMarkers();
	}
}

void JuffEd::gotoMarker() {
	QAction* a = qobject_cast<QAction*>(sender());
	if (a != 0) {
		int line = a->text().section(':', 0, 0).toInt();
		QWidget* view = jInt_->viewer_->currentView();	
		TextDocView* tdView = qobject_cast<TextDocView*>(view);
		if (tdView != 0) {
			tdView->gotoLine(line);
		}
	}
}

///////////////////////////////////////////////////////////////////////
//	Drag & Drop
///////////////////////////////////////////////////////////////////////

void JuffEd::dragEnterEvent(QDragEnterEvent* e) {
	if (e->mimeData()->hasUrls()) {
		e->acceptProposedAction();
	}
}

void JuffEd::dropEvent(QDropEvent* e) {
	if (e->mimeData()->hasUrls()) {
		QList<QUrl> urls = e->mimeData()->urls();
		foreach (QUrl url, urls) {
			QString name = url.path();

#ifdef Q_OS_WIN32			
			//	hack to protect of strings with filenames like /C:/doc/file.txt
			if (name[0] == '/')
				name.remove(0, 1);
#endif

			if (!name.isEmpty()) {
				jInt_->handler_->docOpen(name);
			}
		}
	}
}
