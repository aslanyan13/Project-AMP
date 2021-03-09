#include "mainwindow.h"
#include "ui_mainwindow.h"

QColor mainColor(255, 37, 79);
string mainColorStr = "rgb(255, 37, 79)";

// rgb(255, 37, 79)  - Raspberry color
// rgb(37, 255, 20)  - Neon green color
// rgb(255, 0, 0)    - Red
// rgb(91, 192, 222) - Blue

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    for (int i = 0; i < 1024; i++)
    {
        prerenderedFft[i] = 3;
    }

    int id = QFontDatabase::addApplicationFont(":/Font Awesome 5 Pro Solid.ttf");
    cout << id << endl;
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont fontAwesome(family);

    channel = NULL;
    timer = new QTimer();
    timer->setInterval(6);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();

    // Window transparency
    // this->setWindowFlags(Qt::CustomizeWindowHint);
    // this->setWindowFlags(Qt::FramelessWindowHint);

    ui->setupUi(this);

    this->setStyleSheet("QMainWindow { background-color: #141414; }");
    this->setWindowTitle("AMPlayer v1.0a");

    /*
    QPushButton * menuBtn = new QPushButton(this);
    menuBtn->setGeometry(15, 20, 30, 30);
    menuBtn->setStyleSheet("border: 1px solid silver; background-color: #212121; color: silver;");
    menuBtn->setText("Menu");
    menuBtn->show();
    */

    pauseBtn = new QPushButton(this);
    pauseBtn->setFont(fontAwesome);
    pauseBtn->setGeometry(380, 285, 50, 50);
    pauseBtn->setStyleSheet("font-size: 36px; border: 0px solid silver; background-color: #141414; color: silver;");
    pauseBtn->setText("\uf04c"); // f04b - play
    pauseBtn->setCursor(Qt::PointingHandCursor);
    pauseBtn->show();

    QPushButton * forwardBtn = new QPushButton(this);
    forwardBtn->setFont(fontAwesome);
    forwardBtn->setGeometry(445, 290, 40, 40);
    forwardBtn->setStyleSheet("vertical-align: middle; border: 0px solid silver; font-size: 26px; background-color: #141414; color: silver;");
    forwardBtn->setText("\uf04e");
    forwardBtn->setCursor(Qt::PointingHandCursor);
    forwardBtn->show();

    QPushButton * backwardBtn = new QPushButton(this);
    backwardBtn->setFont(fontAwesome);
    backwardBtn->setGeometry(325, 290, 40, 40);
    backwardBtn->setStyleSheet("vertical-align: middle; border: 0px solid silver; font-size: 26px; background-color: #141414; color: silver;");
    backwardBtn->setText("\uf04a");
    backwardBtn->setCursor(Qt::PointingHandCursor);
    backwardBtn->show();

    repeatBtn = new QPushButton(this);
    repeatBtn->setFont(fontAwesome);
    repeatBtn->setGeometry(500, 291, 30, 30);
    repeatBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: silver;");
    repeatBtn->setText("\uf363");
    repeatBtn->setCursor(Qt::PointingHandCursor);
    repeatBtn->show();

    equoBtn = new QPushButton(this);
    equoBtn->setFont(fontAwesome);
    equoBtn->setGeometry(235, 291, 30, 30);
    equoBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: silver;");
    equoBtn->setText("\uf3f1");
    equoBtn->setCursor(Qt::PointingHandCursor);
    equoBtn->show();

    shuffleBtn = new QPushButton(this);
    shuffleBtn->setFont(fontAwesome);
    shuffleBtn->setGeometry(275, 291, 30, 30);
    shuffleBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: silver;");
    shuffleBtn->setText("\uf074");
    shuffleBtn->setCursor(Qt::PointingHandCursor);
    shuffleBtn->show();

    metronomeBtn = new QPushButton(this);
    metronomeBtn->setFont(fontAwesome);
    metronomeBtn->setGeometry(195, 291, 30, 30);
    metronomeBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: silver;");
    metronomeBtn->setText("\uf001");
    metronomeBtn->setCursor(Qt::PointingHandCursor);
    metronomeBtn->show();

    audio3dBtn = new QPushButton(this);
    audio3dBtn->setFont(fontAwesome);
    audio3dBtn->setGeometry(540, 291, 30, 30);
    audio3dBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: silver;");
    audio3dBtn->setText("\uf1b2");
    audio3dBtn->setCursor(Qt::PointingHandCursor);
    audio3dBtn->show();

    visualBtn = new QPushButton(this);
    visualBtn->setFont(fontAwesome);
    visualBtn->setGeometry(580, 291, 30, 30);
    visualBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: silver;");
    visualBtn->setText("\uf26c");
    visualBtn->setCursor(Qt::PointingHandCursor);
    visualBtn->show();

    songTitle = new QLabel(this);
    songTitle->setText("");
    songTitle->setGeometry(200, 220, 400, 30);
    songTitle->setAlignment(Qt::AlignCenter);
    songTitle->setStyleSheet("/* border: 1px solid silver; */ font-size: 20px; background-color: #141414; color: silver;");
    songTitle->show();

    QLabel * songInfo = new QLabel(this);
    songInfo->setText("Genre, 44.1k MP3");
    songInfo->setGeometry(200, 255, 400, 16);
    songInfo->setAlignment(Qt::AlignCenter);
    songInfo->setStyleSheet("/* border: 1px solid silver; */ background-color: #141414; color: gray;");
    songInfo->show();

    timecode = new QLabel (this);
    timecode->raise();
    timecode->setGeometry(0, 0, 0, 0);
    timecode->setAlignment(Qt::AlignCenter);
    timecode->setMouseTracking(true);
    timecode->installEventFilter(this);
    timecode->setStyleSheet("font-size: 10px; border: 1px solid silver; background-color: #141414; color: silver;");

    playlistWidget = new QListWidget (this);

    this->playlistWidget->setMouseTracking(true);
    playlistWidget->setGeometry(15, 400, 770, 160);
    playlistWidget->lower();
    playlistWidget->setStyleSheet("QListWidget { outline: 0; padding: 5px; font-size: 14px; /*border: 1px solid silver; */ border-radius: 5px; background-color: #181818; color: silver; }" \
                                  "QListWidget::item { outline: none; color: silver; border: 0px solid black; background: rgba(0, 0, 0, 0); }" \
                                  "QListWidget::item:selected { outline: none; border: 0px solid black; color: " + tr(mainColorStr.c_str()) + "; }");
    playlistWidget->show();

    QScrollBar *vbar = playlistWidget->verticalScrollBar();
    vbar->setStyle( new QCommonStyle );
    vbar->setStyleSheet("QScrollBar:vertical { outline: 0; border-radius: 20px; border: 0px solid black; width: 5px; background: #141414; }" \
                        "QScrollBar::add-line:vertical { height: 0; }" \
                        "QScrollBar::sub-line:vertical { height: 0; }" \
                        "QScrollBar::handle:vertical { border-radius: 20px; width: 5px; background: gray; }" \
                        "QScrollBar::handle:vertical:hover { border-radius: 20px; width: 5px; background: " + tr(mainColorStr.c_str()) + "; }" \
                        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { height: 0px; }");

    QPushButton * addSong = new QPushButton (this);
    addSong->setFont(fontAwesome);
    addSong->setGeometry(15, 570, 100, 20);
    addSong->setText("\uf319 Add Song");
    addSong->setStyleSheet("border-radius: 5px; border: 1px solid silver; background-color: #141414; color: silver;");
    addSong->show();

    QPushButton * addFolder = new QPushButton (this);
    addFolder->setFont(fontAwesome);
    addFolder->setGeometry(125, 570, 100, 20);
    addFolder->setText("\uf07c Add Folder");
    addFolder->setStyleSheet("border-radius: 5px; border: 1px solid silver; background-color: #141414; color: silver;");
    addFolder->show();

    songPosition = new QLabel(this);
    songPosition->setMouseTracking(true);
    songPosition->setText("00:00");
    songPosition->setGeometry(15, 363, 200, 20);
    songPosition->setAlignment(Qt::AlignLeft);
    songPosition->setStyleSheet("/* border: 1px solid silver; background-color: #141414; */ color: gray;");
    songPosition->show();

    songDuration = new QLabel(this);
    songDuration->setMouseTracking(true);
    songDuration->setText("00:00");
    songDuration->setGeometry(605, 363, 180, 20);
    songDuration->setAlignment(Qt::AlignRight);
    songDuration->setStyleSheet("/* border: 1px solid silver; background-color: #141414; */ color: gray;");
    songDuration->show();

    volumeSlider = new QSlider (Qt::Horizontal, this);
    volumeSlider->setMaximum(100);
    volumeSlider->setMinimum(0);
    volumeSlider->setValue(100);
    volumeSlider->setGeometry(633, 301, 150, 20);
    volumeSlider->setStyleSheet("QSlider::groove:horizontal {" \
                                    "border: 1px solid #999999; " \
                                    "border-radius: 20px;" \
                                    "background: #181818;"\
                                    "margin: 7px 0;"\
                                "}" \
                               "QSlider::handle:horizontal {" \
                                    "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f); "\
                                    "border: 1px solid #5c5c5c; "\
                                    "width: 5px; " \
                                    "margin: -5px -2px; /* handle is placed by default on the contents rect of the groove. Expand outside the groove */ " \
                                    "border-radius: 20px; "\
                                "}"
                                "QSlider::sub-page:horizontal {" \
                                    "border-radius: 20px;" \
                                    "margin: 7px 0;" \
                                    "background: " + tr(mainColorStr.c_str()) + "; " \
                                "}");

    volumeSlider->show();

    connect(playlistWidget, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(setActive(QListWidgetItem *)));

    // connect (menuBtn, SIGNAL(clicked()), this, SLOT(test()));
    connect (addSong, SIGNAL(clicked()), this, SLOT(openFile()));
    connect (addFolder, SIGNAL(clicked()), this, SLOT(openFolder()));

    connect (forwardBtn, SIGNAL(clicked()), this, SLOT(forward()));
    connect (backwardBtn, SIGNAL(clicked()), this, SLOT(backward()));
    connect (pauseBtn, SIGNAL(clicked()), this, SLOT(pause()));
    connect (repeatBtn, SIGNAL(clicked()), this, SLOT(changeRepeat()));
    connect (shuffleBtn, SIGNAL(clicked()), this, SLOT(changeShuffle()));
    connect (audio3dBtn, SIGNAL(clicked()), this, SLOT(audio3D()));
    connect (equoBtn, SIGNAL(clicked()), this, SLOT(equalizer()));
    connect (visualBtn, SIGNAL(clicked()), this, SLOT(visualizations()));
    connect (metronomeBtn, SIGNAL(clicked()), this, SLOT(metronome()));

    connect (volumeSlider, SIGNAL(valueChanged(int)), this, SLOT(changeVolume(int)));

    this->setMouseTracking(true);
    this->centralWidget()->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::test()
{
    cout << "Some event test..." << endl;
}
bool MainWindow::openFile ()
{
    HWND win = NULL;
    wchar_t file[MAX_PATH] = L"";

    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = win;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFile = file;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
    ofn.lpstrTitle = L"Select a file to play";
    ofn.lpstrFilter = L"Audio Files\0*.mo3;*.xm;*.mod;*.s3m;*.it;*.mtm;*.umx;*.mp3;*.mp2;*.mp1;*.ogg;*.wav;*.aif)\0All files\0*.*\0\0";

    if (!GetOpenFileName(&ofn)) return false;
    else {
        Song temp(file);
        temp.setName(file);
        temp.setNameFromPath();
        wcout << temp.path << endl;

        int iterIndex = current - playlist.begin();

        playlist.push_back(temp);
        cout << playlist.size() << endl;

        current = playlist.begin() + iterIndex;

        drawPlaylist();
        if (playlist.size() == 1)
            setActive (0);
    }

    return true;
}
bool MainWindow::openFolder () {
    QString folder = QFileDialog::getExistingDirectory(0, ("Select Folder with Songs"), QDir::homePath());
    cout << folder.toStdString() << endl;

    if (folder.toStdString() == "")
        return false;

    QDir directory(folder);
    QStringList musicFiles = directory.entryList(QStringList() << "*.mp3" << "*.MP3",QDir::Files|QDir::Readable);

    for (int i = 0; i < musicFiles.length(); i++)
    {
        wchar_t tmp[MAX_PATH];
        QString fullpath = folder + "/" + musicFiles[i];
        wcscpy(tmp, fullpath.toStdWString().c_str());

        Song temp(tmp);
        temp.setName(musicFiles[i].toStdWString());
        temp.setNameFromPath();
        playlist.push_back(temp);

        // cout << musicFiles[i].toStdString() << endl;
    }

    QMessageBox msgBox;
    msgBox.setWindowTitle("Files adding");

    if (musicFiles.length() > 0) {
        msgBox.setText(tr(to_string(musicFiles.length()).c_str()) + " files added to playlist successfully!");
    } else {
        msgBox.setText("There are no audio files in this folder!");
    }

    msgBox.setStyleSheet("background-color: #141414; color: silver;");
    msgBox.exec();

    drawPlaylist();

    return true;
}

void MainWindow::drawPlaylist() {
    playlistWidget->clear();

    for (int i = 0; i < playlist.size(); i++)
    {
        QListWidgetItem *  songItem = new QListWidgetItem(playlistWidget);
        QString path = QString::fromStdWString(wstring(playlist[i].path));
        QString name = QString::fromStdWString(to_wstring(i + 1) + L". " + playlist[i].getName());

        //if (current == playlist.begin() + i)
        //    songItem->setBackgroundColor(QColor(38, 160, 218));

        songItem->setData(Qt::UserRole, i);
        songItem->setData(1, path);
        songItem->setText(name);

        playlistWidget->addItem(songItem);
   }
}
void MainWindow::setTitle () {
    wstring name = current->getName();
    if (name.length() > 32)
        name = name.substr(0, 32) + L"...";

    songTitle->setText(QString::fromStdWString(name));
}
void MainWindow::setActive(QListWidgetItem * item) {
    paused = true;
    pauseBtn->setText("\uf04c"); // Set symbol to pause

    setActive (item->data(Qt::UserRole).toInt());
    item->setForeground(mainColor);
}
void MainWindow::setActive (int index) {
    current = playlist.begin() + index;

    if (channel != NULL)
        BASS_ChannelStop(channel);

    channel = BASS_StreamCreateFile(FALSE, current->path, 0, 0, 0);
    prerenderFft();

    BASS_ChannelPlay(channel, false);
    BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume);

    QString pos = QString::fromStdString(seconds2string(getPosition()));
    songPosition->setText(pos);
    QString len = QString::fromStdString(seconds2string(getDuration()));
    songDuration->setText(len);

    setTitle();
}
void MainWindow::forward () {
    if (channel == NULL || playlist.size() == 1)
        return;

    cout << "Distance - " << distance (playlist.end(), current) << endl;
    current++;
    cout << "Distance - " << distance (playlist.end(), current) << endl;
    if (distance (playlist.end(), current) >= 0) current = playlist.begin();

    BASS_ChannelStop(channel);
    channel = BASS_StreamCreateFile(FALSE, current->path, 0, 0, 0);

    prerenderFft();

    BASS_ChannelPlay(channel, true);
    BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume);

    QString pos = QString::fromStdString(seconds2string(getPosition()));
    songPosition->setText(pos);
    QString len = QString::fromStdString(seconds2string(getDuration()));
    songDuration->setText(len);

    setTitle();
}
void MainWindow::backward () {
    if (channel == NULL || playlist.size() == 1)
        return;

    current--;
    cout << "Distance - " << distance (playlist.begin(), current) << endl;
    if (distance (playlist.begin(), current) < 0) current = playlist.end() - 1;

    BASS_ChannelStop(channel);
    channel = BASS_StreamCreateFile(FALSE, current->path, 0, 0, 0);

    prerenderFft();

    BASS_ChannelPlay(channel, true);
    BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume);

    QString pos = QString::fromStdString(seconds2string(getPosition()));
    songPosition->setText(pos);
    QString len = QString::fromStdString(seconds2string(getDuration()));
    songDuration->setText(len);

    setTitle();
}
void MainWindow::pause ()
{
    if (channel == NULL) return;

    paused = !paused;

    if (paused) {
        BASS_ChannelPlay(channel, false);
        pauseBtn->setText("\uf04c"); // Set symbol to pause
    }
    else {
        BASS_ChannelPause(channel);
        pauseBtn->setText("\uf04b"); // Set symbol to play
    }
}
void MainWindow::changeVolume (int vol)
{
    cout << vol << endl;
    volume = vol / 100.0f;
    BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume);
}
void MainWindow::changeRepeat () {
    if (shuffle == true)
        changeShuffle();

    repeat = !repeat;
    cout << "Repeat - " << repeat << endl;
    if (repeat) repeatBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: " + tr(mainColorStr.c_str()) + ";");
    else repeatBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: silver;");
}
void MainWindow::changeShuffle () {
    if (repeat == true)
        changeRepeat();

    shuffle = !shuffle;
    cout << "Shuffle - " << shuffle << endl;
    if (shuffle) shuffleBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: " + tr(mainColorStr.c_str()) + ";");
    else shuffleBtn->setStyleSheet("font-size: 14px; margin-top: 10px; border: 0px solid silver; background-color: #141414; color: silver;");
}

string MainWindow::seconds2string (float seconds) {
    int hours = (seconds > 3600) ? (int)seconds / 3600 : 0;
    int minutes = (seconds > 60) ? ((int)seconds - hours * 3600) / 60 : 0;
    int secs = (int)seconds % 60;

    string result = "";
    if (hours != 0)
    {
        if (hours < 10) result += "0";
        result += to_string (hours);
        result += ":";
    }

    if (minutes < 10) result += "0";
    result += to_string (minutes);
    result += ":";

    if (secs < 10) result += "0";
    result += to_string (secs);

    return result;
}

void MainWindow::updateTime() {
    if (channel != NULL) {
        QString pos = QString::fromStdString(seconds2string(getPosition()));
        songPosition->setText(pos);

        if (getPosition() >= getDuration())
        {
            if (repeat) {
                BASS_ChannelStop(channel);
                channel = BASS_StreamCreateFile(FALSE, current->path, 0, 0, 0);
                BASS_ChannelPlay(channel, true);
                BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume);
            }
            else if (shuffle && playlist.size() > 1) {
                int songID;
                do {
                    songID = rand() % playlist.size();
                } while (songID == distance(playlist.begin(), current));

                current = playlist.begin() + songID;

                BASS_ChannelStop(channel);
                channel = BASS_StreamCreateFile(FALSE, current->path, 0, 0, 0);

                prerenderFft();

                BASS_ChannelPlay(channel, true);
                BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume);

                QString pos = QString::fromStdString(seconds2string(getPosition()));
                songPosition->setText(pos);
                QString len = QString::fromStdString(seconds2string(getDuration()));
                songDuration->setText(len);

                setTitle();
            } else {
                forward();
            }
        }
    }

    repaint();
}

void MainWindow::paintEvent(QPaintEvent * event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Spectrum background
    QColor color = QColor(20, 20, 20);

    painter.setBrush(QBrush(color, Qt::SolidPattern));
    painter.setPen(QPen(Qt::transparent, 1, Qt::SolidLine, Qt::FlatCap));

    painter.drawRect (50, 350, 700, 40);

    QImageReader reader(":/Images/cover-placeholder.png");
    QImage cover = reader.read();

    if (cover.isNull()) {
        cout << reader.errorString().toStdString() << endl;
    }
    else {
        QBrush brush(cover);
        QTransform transform;
        transform.scale(0.3, 0.3);
        transform.translate(80, 200);
        brush.setTransform(transform);
        painter.setBrush(brush);
        painter.drawRoundedRect(325, 60, 150, 150, 5, 5);
    }

    color = mainColor;

    if (liveSpec) {
        float fft[1024];
        BASS_ChannelGetData(channel, fft, BASS_DATA_FFT2048);

        for (int i = 0; i < 140; i++) {
            int h = sqrt(fft[i + 1]) * 3 * 40 - 4;
            if (h < 3) h = 3;
            if (h > 40) h = 40;

            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing);
            QPainterPath path;
            path.addRoundedRect(QRectF(50 + 5 * i, 350 + 10 + (20 - h) / 2, 3, h), 2, 2);
            QPen pen(Qt::transparent, 0);
            p.setPen(pen);
            if (i * (getDuration() / 140) <= getPosition())
                p.fillPath(path, color);
            else
                p.fillPath(path, QColor (192, 192, 192));
            p.drawPath(path);
        }
    } else {
        for (int i = 0; i < 140; i++) {

            QPainter p(this);
            p.setRenderHint(QPainter::Antialiasing);
            QPainterPath path;
            path.addRoundedRect(QRectF(50 + 5 * i, 350 + 10 + (20 - prerenderedFft[i]) / 2, 3, prerenderedFft[i]), 2, 2);
            QPen pen(Qt::transparent, 0);
            p.setPen(pen);

            if (i * (getDuration() / 140) <= getPosition())
                p.fillPath(path, color);
            else
                p.fillPath(path, QColor (192, 192, 192));

            p.drawPath(path);
        }
    }
}

void MainWindow::prerenderFft ()
{
    if (channel == NULL) return;

    int k = 0;
    float time = getDuration();

    int avgLen = 16;

    BASS_ChannelPlay(channel, FALSE);
    float fft[1024];
    for (float i = 0; i < time; i+= time / 140, k++)
    {
        BASS_ChannelGetData(channel, fft, BASS_DATA_FFT2048);

        float max = 0;
        for (int j = 1; j <= avgLen; j++)
            max += sqrt(fft[j]) * 3 * 40 - 4;

        max /= avgLen;
        max *= 2;

        if (max <= 3) max = 3;
        else if (max > 40) max = 40;

        prerenderedFft[(int)k] = max;
        cout << max << " ";

        BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, i), BASS_POS_BYTE);
    }

    BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, 0), BASS_POS_BYTE);
}
// Need to fix child widget tracking issue
void MainWindow::mouseMoveEvent (QMouseEvent * event) {
    float mouseX = event->pos().x();
    float mouseY = event->pos().y();

    cout << mouseX << " " << mouseY << endl;

    if (mouseX > 50 && mouseX < 750 && mouseY > 350 && mouseY < 390) {
        this->setCursor(Qt::PointingHandCursor);

        timecode->repaint();
        timecode->clear();
        timecode->setGeometry (mouseX + 10, mouseY + 10, 50, 20);
        timecode->setText(QString::fromStdString(seconds2string(((mouseX - 50) / (700.0f)) * getDuration())));
        timecode->show();
    }
    else {
        this->setCursor(Qt::ArrowCursor);
        timecode->hide();
    }

    if (mouseX > 15 && mouseX < 785 && mouseY > 400 && mouseY < 570)
    {
        playlistWidget->raise();
    } else {
        playlistWidget->lower();
    }
}
void MainWindow::mousePressEvent (QMouseEvent * event) {
    float mouseX = event->pos().x();
    float mouseY = event->pos().y();

    if (mouseX > 50 && mouseX < 750 && mouseY > 350 && mouseY < 390) {
        if (event->button() == Qt::LeftButton)
            BASS_ChannelSetPosition(channel, BASS_ChannelSeconds2Bytes(channel, ((mouseX - 50) / (700.0f)) * getDuration()), BASS_POS_BYTE);
        else if (event->button() == Qt::RightButton) {
            liveSpec = !liveSpec;
        }
    }
}
