#include "Music.h"
#include "main.h"

Music::Music(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->setWindowIcon(QIcon(":/player.ico"));
    ui.play->setCheckable(true);

    ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    audioOutput = new QAudioOutput(this);
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioOutput(audioOutput);

    connect(mediaPlayer,&QMediaPlayer::durationChanged,this,[=](qint64 duration){
        ui.time_aft->setText(QString("%1:%2").arg(duration/1000/60,2,10,
                                                  QChar('0')).arg(duration/1000%60,2,10,QChar('0')));
        ui.horizontalSlider->setRange(0,duration);
    });
    connect(mediaPlayer,&QMediaPlayer::positionChanged,this,[=](qint64 position){
        ui.time_pre->setText(QString("%1:%2").arg(position/1000/60,2,10,
                                                  QChar('0')).arg(position/1000%60,2,10,QChar('0')));
        ui.horizontalSlider->setValue(position);
    });

    connect(ui.horizontalSlider,&QSlider::sliderMoved,mediaPlayer,
            &QMediaPlayer::setPosition);

    next = new QAction("下一曲",this);
    next->setShortcut(Qt::Key_Right|Qt::Key_Right);
    previous = new QAction("上一曲",this);
    previous->setShortcut(Qt::CTRL|Qt::Key_Left);
    playPause = new QAction("播放&暂停",this);
    playPause->setShortcut(Qt::CTRL|Qt::Key_Space);

    connect(playPause,&QAction::triggered,this,&Music::on_play_clicked);
    connect(next,&QAction::triggered,this,&Music::on_next_clicked);
    connect(previous,&QAction::triggered,this,&Music::on_pre_clicked);

    rightMenu = new QMenu;
    deleteFunc = new QAction("删除",this);
    deleteFunc->setShortcut(Qt::Key_Delete);
    favorFunc = new QAction("喜欢",this);
    favorFunc->setShortcut(Qt::CTRL|Qt::Key_L);
    modifyFunc = new QAction("修改",this);
    modifyFunc->setShortcut(Qt::CTRL|Qt::Key_M);
    rightMenu->addAction(deleteFunc);
    rightMenu->addAction(favorFunc);
    rightMenu->addAction(modifyFunc);

    connect(favorFunc,&QAction::triggered,this,&Music::favorFunction);
    connect(deleteFunc,&QAction::triggered,this,&Music::deleteFunction);
    connect(modifyFunc,&QAction::triggered,this,&Music::modifyFunction);

    connect(ui.listWidget,&QListWidget::customContextMenuRequested,[&](const QPoint &pos){
        QPoint globalPos = ui.listWidget->mapToGlobal(pos);
        rightMenu->exec(globalPos);
    });
}

Music::~Music()
{

}

void Music::on_dir_clicked()
{
    int i;
    qInfo()<< "Hello!";
    auto path = QFileDialog::getExistingDirectory(this,"请选择文件夹","./source");
    qInfo()<<path;
    QDir dir(path);
    auto List = dir.entryList(QStringList()<<"*.mp3"<<"*.wav"<<"*.flac");
    for (i = 0;i<List.size();i++){
        if(playListFormer.contains(List[i],Qt::CaseInsensitive)){
            continue;
        }else{
            playListFormer.append(List[i]);
            ui.listWidget->addItem(List[i]);
            auto file = List[i];
            playList.append(QUrl::fromLocalFile(path+"/"+file));
        }
    }
    ui.listWidget->setCurrentRow(0);

    playTimes = new int[playList.size()];
}


void Music::on_play_clicked()
{
    if (playList.empty())
        return;
    switch (mediaPlayer->playbackState()){
    case QMediaPlayer::PlaybackState::StoppedState:
    {
        curPlayIndex = ui.listWidget->currentRow();
        mediaPlayer->setSource(playList[curPlayIndex]);
        mediaPlayer->play();
        playTimes[curPlayIndex]++;
        break;
    }
    case QMediaPlayer::PlaybackState::PlayingState:
        mediaPlayer->pause();
        break;
    case QMediaPlayer::PlaybackState::PausedState:
        mediaPlayer->play();
        playTimes[ui.listWidget->currentRow()]++;
        break;
    }
}

void Music::on_pre_clicked()
{
    if (playList.empty())
        return;
    curPlayIndex = abs(((curPlayIndex-1)% playList.size()));
    ui.listWidget->setCurrentRow(curPlayIndex);
    mediaPlayer->setSource(playList[curPlayIndex]);
    mediaPlayer->play();
    playTimes[curPlayIndex]++;
}


void Music::on_next_clicked()
{
    if (playList.empty())
        return;
    curPlayIndex = ((curPlayIndex+1)% playList.size());
    ui.listWidget->setCurrentRow(curPlayIndex);
    mediaPlayer->setSource(playList[curPlayIndex]);
    mediaPlayer->play();
    playTimes[curPlayIndex]++;
}


void Music::on_listWidget_doubleClicked(const QModelIndex &index)
{
    if (playList.empty())
        return;
    curPlayIndex = index.row();
    mediaPlayer->setSource(playList[curPlayIndex]);
    mediaPlayer->play();
    playTimes[curPlayIndex]++;
}

void Music::on_settings_clicked()
{
    Setting mySetting;
    mySetting.show();
    mySetting.exec();
}

void Music::on_search_clicked()
{
    searchList.clear();
    searchListFormer.clear();
    QString content = ui.lineEdit->text();
    if(content.size()!=0){
        QStringListModel model;
        model.setStringList(playListFormer);
        QCompleter* completer = new QCompleter;
        completer->setModel(&model);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        completer->setFilterMode(Qt::MatchContains);
        completer->setCompletionMode(QCompleter::PopupCompletion);
        ui.lineEdit->setCompleter(completer);

        delete completer;

        for (int i = 0;i<playListFormer.size();i++){
            if(playListFormer[i].contains(content,Qt::CaseInsensitive)){
                searchListFormer.append(playListFormer[i]);
                searchList.append(playList[i]);
            }
        }

        ui.listWidget->clear();
        ui.listWidget->addItems(searchListFormer);
        ui.listWidget->setCurrentRow(0);
        tempList = playList;
        qInfo() << tempList;
        playList = searchList;
    }
    if(content.size()==0){
        playList = tempList;
        qInfo() << "0";
        searchList.clear();
        searchListFormer.clear();
        ui.listWidget->clear();
        ui.listWidget->addItems(playListFormer);
        ui.listWidget->setCurrentRow(0);
        tempList.clear();
    }

    content.clear();
}

void Music::on_favorite_clicked()
{
    check ++;
    if (check%2==1){
        tempList = playList;
        tempListFormer = playListFormer;
        playList = favorList;
        playListFormer = favorListFormer;
        ui.listWidget->clear();
        ui.listWidget->addItems(playListFormer);
        ui.listWidget->setCurrentRow(0);
    }
    if (check%2==0){
        favorList = playList;
        favorListFormer = playListFormer;
        playList = tempList;
        playListFormer = tempListFormer;
        tempList.clear();
        tempListFormer.clear();
        ui.listWidget->clear();
        ui.listWidget->addItems(playListFormer);
        ui.listWidget->setCurrentRow(0);
    }
    if (check == 3){
        check = 1;
    }
}

void Music::deleteFunction()
{
    int i;
    int t = ui.listWidget->currentRow();
    for(i = 0;i<playList.size()-1;i++){
        if(t==0){
            deleteList.append(playList[i+1]);
            deleteListFormer.append(playListFormer[i+1]);
        }else{
            if(i<t){
                deleteList.append(playList[i]);
                deleteListFormer.append(playListFormer[i]);
            }
            if(i>=t){
                i = i+1;
                deleteList.append(playList[i]);
                deleteListFormer.append(playListFormer[i]);
            }
        }

    }

    playList = deleteList;
    playListFormer = deleteListFormer;
    deleteList.clear();
    deleteListFormer.clear();

    ui.listWidget->clear();
    ui.listWidget->addItems(playListFormer);
    ui.listWidget->setCurrentRow(0);
}

void Music::favorFunction()
{
    // if(favorList.contains(ui.listWidget->currentRow())){
    //     return;
    // }else{
        favorList.append(playList[ui.listWidget->currentRow()]);
        favorListFormer.append((playListFormer[ui.listWidget->currentRow()]));
    //}
}

void Music::on_rank_clicked()
{
    rankList.clear();
    rankListFormer.clear();
    int i , j;
    rankList = playList;
    rankListFormer = playListFormer;
    for (i = 0;i<playList.size();i++){
        for (j = 0;j<playList.size()-1;j++){
            if (playTimes[j]<playTimes[j+1]){
                qSwap(rankList[j],rankList[j+1]);
                qSwap(rankListFormer[j],rankListFormer[j+1]);
            }
        }
    }
    ui.rank_2->clear();
    ui.rank_2->addItems(rankListFormer);
}

void Music::modifyFunction()
{
    QListWidgetItem *selectedItem = ui.listWidget->currentItem();
    QString content = QInputDialog::getText(this,
                                            "请输入修改的歌名",
                                            "例如：Text.mp3",
                                            QLineEdit::Normal,
                                            selectedItem->text());
    playListFormer[ui.listWidget->currentRow()] = content;
    ui.listWidget->clear();
    ui.listWidget->addItems(playListFormer);
    ui.listWidget->setCurrentRow(0);
}
