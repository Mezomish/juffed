#ifndef __DOC_LIST_PLUGIN_PANEL_H__
#define __DOC_LIST_PLUGIN_PANEL_H__

#include <QWidget>
#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QFileIconProvider>

class QTreeView;
class QLineEdit;
class JuffAPI;

#if QT_VERSION < 0x050200
static const int BtnSize = 24;

#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>

class FilterLineEdit : public QWidget {
public:
	FilterLineEdit(QWidget* parent = 0) : QWidget(parent) {
		lineEd_ = new QLineEdit("", this);
		clearBtn_ = new QPushButton(QIcon(":clear"), "", this);
		clearBtn_->setFlat(true);
		clearBtn_->setFocusPolicy(Qt::NoFocus);
		clearBtn_->setCursor(Qt::ArrowCursor);
		clearBtn_->setMaximumWidth(BtnSize);
		
		QHBoxLayout* hBox = new QHBoxLayout(this);
		hBox->setMargin(0);
		hBox->setSpacing(0);
		hBox->addWidget(lineEd_);
#if QT_VERSION >= 0x040500
		lineEd_->setTextMargins(0, 0, BtnSize, 0);
		clearBtn_->setParent(lineEd_);
#else
		hBox->addWidget(clearBtn_);
#endif
	}
	
#if QT_VERSION >= 0x040500
	virtual void resizeEvent(QResizeEvent*) {
		clearBtn_->setGeometry(lineEd_->width() - BtnSize, (lineEd_->height() - BtnSize) / 2, BtnSize, BtnSize);
	}
#endif
	
	QLineEdit* lineEd_;
	QPushButton* clearBtn_;
};
#endif


class FileListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    FileListModel(QObject *parent);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QString file(const QModelIndex &index);

    void docOpened(const QString &fname, const QString &title);
    void docActivated(const QString &fname);
    void docClosed(const QString &fname);
    void docRenamed(const QString &fname, const QString &title, const QString &oldName);
    void docModified(const QString &fname, const QString &title);

private:
    // filename -> doc title
    QHash<QString,QString> m_files;
    QString m_currentFile;
    QFileIconProvider m_icons;
};


class FileListFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    FileListFilterModel(QWidget *parent);
};

class DocListPanel : public QWidget {
Q_OBJECT
public:
    DocListPanel(JuffAPI *api);

    void docOpened(const QString &fname, const QString &title);
    void docActivated(const QString &fname);
    void docClosed(const QString &fname);
    void docRenamed(const QString &fname, const QString &title, const QString &oldName);
    void docModified(const QString &fname, const QString &title);

public slots:
#if QT_VERSION < 0x050200
	void clear();
#endif

private:
    JuffAPI *api_;
    QTreeView* tree_;
    FileListFilterModel *proxy_;
    FileListModel *model_;

#if QT_VERSION < 0x050200
    FilterLineEdit* filter_;
#else
    QLineEdit* filter_;
#endif

private slots:
    void docClicked(const QModelIndex &index);
};

#endif
