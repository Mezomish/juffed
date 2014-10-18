#include "DocListPanel.h"

#include <QTreeView>
#include <QLineEdit>
#include <QVBoxLayout>
#include "JuffAPI.h"


FileListModel::FileListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FileListModel::rowCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return m_files.count();
}

int FileListModel::columnCount(const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant FileListModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        if (index.column() == 0)
            return m_files.values().at(index.row());
        else
            return m_files.keys().at(index.row());
        break;
    case Qt::DecorationRole:
        if (index.column() == 0)
            return m_icons.icon(QFileInfo(m_files.keys().at(index.row())));
        break;
    case Qt::BackgroundRole:
        if (m_files.keys().at(index.row()) == m_currentFile)
            return QPalette().highlight();
        break;
    case Qt::ToolTipRole:
        // upside-down of DisplayRole
        if (index.column() == 0)
            return m_files.keys().at(index.row());
        else
            return m_files.values().at(index.row());
        break;
    }

    return QVariant();
}

QVariant FileListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    if (orientation == Qt::Vertical)
        return QVariant();

    // QIcon(QString(":doc_icon"))
    // tooltip = name

    return section == 0 ? tr("Name") : tr("Full name");
}

QString FileListModel::file(const QModelIndex &index)
{
    if (!index.isValid())
        return QString();

    return m_files.keys().at(index.row());
}

void FileListModel::docOpened(const QString &fname, const QString &title)
{
    beginResetModel();
    m_files[fname] = title;
    endResetModel();
}

void FileListModel::docActivated(const QString &fname)
{
    beginResetModel();
    m_currentFile = fname;
    endResetModel();
}

void FileListModel::docClosed(const QString &fname)
{
    beginResetModel();
    m_files.remove(fname);
    endResetModel();
}

void FileListModel::docRenamed(const QString &fname, const QString &title, const QString &oldName)
{
    Q_UNUSED(oldName);
    beginResetModel();
    m_files[fname] = title;
    endResetModel();
}

void FileListModel::docModified(const QString &fname, const QString &title)
{
    beginResetModel();
    m_files[fname] = title;
    endResetModel();
}



FileListFilterModel::FileListFilterModel(QWidget *parent)
    : QSortFilterProxyModel(parent)
{
}

DocListPanel::DocListPanel(JuffAPI *api)
    : QWidget(0),
      api_(api)
{
    setWindowTitle(tr("Documents"));
	
    tree_ = new QTreeView(this);
    tree_->setAlternatingRowColors(true);

    model_ = new FileListModel(this);
    proxy_ = new FileListFilterModel(this);

    proxy_->setSourceModel(model_);
    tree_->setModel(proxy_);

    proxy_->setFilterKeyColumn(0);

#if QT_VERSION < 0x050200
    filter_ = new FilterLineEdit();
    filter_->setMaximumHeight(24);
#else
    filter_ = new QLineEdit;
    filter_->setPlaceholderText(tr("Filter"));
    filter_->setClearButtonEnabled(true);
#endif
    QVBoxLayout* vBox = new QVBoxLayout();
    vBox->setMargin(0);
    vBox->setSpacing(2);
    setLayout(vBox);
	
    vBox->addWidget(tree_);
    vBox->addWidget(filter_);
		
#if QT_VERSION < 0x050200
    connect(filter_->lineEd_, SIGNAL(textChanged(const QString&)),
            proxy_, SLOT(setFilterFixedString(const QString&)));
    connect(filter_->clearBtn_, SIGNAL(clicked()), SLOT(clear()));
#else
    connect(filter_, SIGNAL(textChanged(const QString&)),
            proxy_, SLOT(setFilterFixedString(const QString&)));
#endif

    connect(tree_, SIGNAL(activated(QModelIndex)),
            this, SLOT(docClicked(QModelIndex)));
}

void DocListPanel::docClicked(const QModelIndex &index) {
    //	LOGGER;
    if (!index.isValid())
        return;

    api_->openDoc(model_->file(proxy_->mapToSource(index)));
}

void DocListPanel::docOpened(const QString &fname, const QString &title)
{
    model_->docOpened(fname, title);
    tree_->resizeColumnToContents(0);
}

void DocListPanel::docActivated(const QString &fname)
{
    model_->docActivated(fname);
    tree_->resizeColumnToContents(0);
}

void DocListPanel::docClosed(const QString &fname)
{
    model_->docClosed(fname);
    tree_->resizeColumnToContents(0);
}

void DocListPanel::docRenamed(const QString &fname, const QString &title, const QString &oldName)
{
    model_->docRenamed(fname, title, oldName);
    tree_->resizeColumnToContents(0);
}

void DocListPanel::docModified(const QString &fname, const QString &title)
{
    model_->docModified(fname, title);
    tree_->resizeColumnToContents(0);
}

#if QT_VERSION < 0x050200
void DocListPanel::clear() {
#if 0
	filter_->lineEd_->setText("");
#endif
}
#endif
