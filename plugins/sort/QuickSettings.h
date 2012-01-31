#ifndef __QUICK_SETT_H__
#define __QUICK_SETT_H__

#include "ui_QuickSettings.h"

class JuffPlugin;

class QuickSettings : public QDialog {
Q_OBJECT
public:
  QuickSettings(QWidget * parent = 0);
  Ui::QuickSettings ui;

private slots:
  void close();
  void sortNow();

private:
};

#endif
