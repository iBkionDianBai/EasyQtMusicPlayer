#ifndef SETTING_H
#define SETTING_H

#include <QDialog>
#include "main.h"

namespace Ui {
class Setting;
}

class Setting : public QDialog
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();

private slots:
    void initBox();
    void setPageTheme();
private:
    QList<QUrl> themeList;
    QStringList themeListFormer;
    QString path;
    Ui::Setting *ui;
};

#endif // SETTING_H
