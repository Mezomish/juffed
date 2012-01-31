#include "QuickSettings.h"
#include <JuffPlugin.h>

QuickSettings::QuickSettings(QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);
  connect(ui.closeBtn, SIGNAL(clicked()), this, SLOT(close()));
  connect(ui.sortBtn, SIGNAL(clicked()), this, SLOT(sortNow()));
}

void QuickSettings::close()
{
  reject();
}

void QuickSettings::sortNow()
{
  accept();
}