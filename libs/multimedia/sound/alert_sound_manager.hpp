#pragma once

#include <QObject>
#include <QString>
#include <QUrl>

#include <memory>

class QAudioOutput;
class QMediaPlayer;
class QMediaPlaylist;
class QString;
class QTimer;

namespace ocv::mm{

/**
 * Start, stop or mute alarm temporary
 */
class alert_sound_manager : public QObject
{
    Q_OBJECT
public:
    explicit alert_sound_manager(QObject *parent = nullptr);
    ~alert_sound_manager();

    /**
     * Play the sound
     * @param filename File name of the sound
     */
    void play(QUrl const &filename);
    /**
     * Overload of play. This api will play the sound if the sound
     * set by set_filename or play(QString const &filename)
     * could be opened
     */
    void play();

    void set_alarm_duration(int msec);

    /**
     * Set file name of the sound
     * @param filename File name of the sound
     */
    void set_file_name(QUrl const &file_name);
    /**
     * Stop the sound
     */
    void stop();

private:
    std::unique_ptr<QAudioOutput> audio_;
    QUrl file_name_;
    std::unique_ptr<QMediaPlayer> sound_;
    QTimer *timer_;
};

}
