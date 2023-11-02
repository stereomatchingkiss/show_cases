#include "alert_sound_manager.hpp"

#include <QFile>
#include <QDebug>

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QTimer>

namespace flt::mm{

alert_sound_manager::alert_sound_manager(QUrl const &filename, QObject *parent) :
    QObject(parent),
    audio_(std::make_unique<QAudioOutput>()),
    file_name_(filename),
    sound_(std::make_unique<QMediaPlayer>()),
    timer_(new QTimer(this))
{
    sound_->setAudioOutput(audio_.get());
    sound_->setLoops(QMediaPlayer::Infinite);
    sound_->setSource(file_name_);
    connect(sound_.get(), &QMediaPlayer::errorOccurred,
          [=](QMediaPlayer::Error error, const QString &error_string)
            { qInfo()<<"alert sound manager error = "<<error<<":"<<error_string; });

    timer_->setSingleShot(true);
    timer_->setInterval(7000);

    connect(timer_, &QTimer::timeout, [this]()
    {
        stop();
    });
}

alert_sound_manager::~alert_sound_manager()
{

}

void alert_sound_manager::play(QUrl const &file_name)
{
    if(file_name_ != file_name){
        qInfo()<<__func__<<": playlist_->currentMedia().canonicalUrl() != file_name";
        file_name_ = file_name;
        sound_->setSource(file_name_);
        audio_->setVolume(80);
        sound_->play();
        timer_->start();
    }else{
        if(sound_->playbackState() == QMediaPlayer::StoppedState){
            qInfo()<<__func__<<": state == stoppedState";
            sound_->play();
            timer_->start();
        }
    }
}

void alert_sound_manager::play()
{
    play(file_name_);
}

void alert_sound_manager::set_alarm_duration(int msec)
{
    timer_->setInterval(msec);
}

void alert_sound_manager::set_file_name(const QUrl &file_name)
{
    file_name_ = file_name;
}

void alert_sound_manager::stop()
{
    sound_->stop();
    timer_->stop();
}

}
