#include "setting.h"
#include "ui_setting.h"

Setting::Setting(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Setting)
{
    ui->setupUi(this);


    connect(ui->themeBtn,SIGNAL(clicked()),ui->stackedWidget,SLOT(&Setting::setPageTheme));
    ui->stackedWidget->setCurrentIndex(0);
}

Setting::~Setting()
{
    delete ui;
}

void Setting::initBox(){
    path = ":/Qss";
    QDir dir(path);
    auto List = dir.entryList(QStringList() << "*.qss");
    themeListFormer.append(List);
    if (List.size()!=0) {
        for (auto file: List){
            themeList.append(QUrl::fromLocalFile(path+"/"+file));
        }
    }
    ui->themeChoose->clear();
    ui->themeChoose->addItems(themeListFormer);
}

void Setting::setPageTheme(){
    ui->stackedWidget->setCurrentIndex(0);
    initBox();
}

