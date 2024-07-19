#pragma once

#include <QtWidgets/QWidget>
#include "ui_Music.h"
#include "main.h"
#include "setting.h"

class Music : public QWidget
{
    Q_OBJECT

public:
    Ui::MusicClass ui;

    int check = 0;
    int* playTimes;

    QList<QUrl> playList;       // = char[Url]
    QList<QUrl> favorList;
    QList<QUrl> searchList;
    QList<QUrl> tempList;
    QList<QUrl> deleteList;
    QList<QUrl> rankList;

    QStringList playListFormer; // = char[]
    QStringList favorListFormer;
    QStringList searchListFormer;
    QStringList tempListFormer;
    QStringList deleteListFormer;
    QStringList rankListFormer;

    int curPlayIndex = 0;
    QAudioOutput* audioOutput;
    QMediaPlayer* mediaPlayer;
    Music(QWidget *parent = nullptr);
    ~Music();

    QMenu* rightMenu;
    QAction* next;
    QAction* previous;
    QAction* playPause;
    QAction* deleteFunc;
    QAction* modifyFunc;
    QAction* favorFunc;
private slots:

    void on_dir_clicked();
    void on_play_clicked();
    void on_pre_clicked();
    void on_next_clicked();
    void on_listWidget_doubleClicked(const QModelIndex &index);
    void on_settings_clicked();
    void on_search_clicked();
    void on_favorite_clicked();
    void deleteFunction();
    void modifyFunction();
    void favorFunction();
    void on_rank_clicked();
};
