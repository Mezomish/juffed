#ifndef __SEARCH_DLG_H__
#define __SEARCH_DLG_H__

#include "ui_SearchDlg.h"

#include <QtGui/QDirModel>

class SearchDlg : public QDialog {
Q_OBJECT
public:
	SearchDlg(QWidget*);

	// getters
	QString findText()       const;
	QString startDir()       const;
	bool    searchInFiles()  const;
	bool    recursive()      const;
	QString filePatterns()   const;
	int     patternVariant() const;

	// setters
	void setFindText      (const QString&);
	void setStartDir      (const QString&);
	void setSearchInFiles (bool);
	void setRecursive     (bool);
	void setFilePatterns  (const QString&);
	void setPatternVariant(int);

private slots:
	void slotBrowse();

private:
	Ui::SearchDlg ui;
	QDirModel model_;
};

#endif /* __SEARCH_DLG_H__ */
