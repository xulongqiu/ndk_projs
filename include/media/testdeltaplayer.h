
#ifndef ANDROID_TESTDELTAMEDIAPLAYER_H
#define ANDROID_TESEDELTAMEDIAPLAYER_H

#include <system/audio.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>

#include <binder/Parcel.h>

namespace android {

class DeltaMediaPlayer;
class PlayerProxyListener;

enum player_user_notify{
	USER_NOTIFY_INFO_BUFFERING,
	USER_NOTIFY_INFO_POSITION,
	USER_NOTIFY_INFO_DURATION,
	USER_NOTIFY_PLAYCOMPLETE,
	USER_NOTIFY_PREPARECOMPLETE,
	USER_NOTIFY_SEEKCOMPLETE,
	USER_NOTIFY_START_BUFFERING,
	USER_NOTIFY_END_BUFFERING,
	USER_NOTIFY_SEEKERROR,
	USER_NOTIFY_PLAYERROR,
	USER_NOTIFY_INFO_SIZE,
	USER_NOTIFY_INFO_STARTPLAYING,
	USER_NOTIFY_INFO_PAUSED,
};

class PlayerUserListener
{
public:
	//Listener
	virtual ~PlayerUserListener() {}

	virtual void playerNotify(int msg, int ext1, int ext2) = 0;
};



enum test_player_type {
	TEST_PLAYER_TYPE_URL =      (1),
	TEST_PLAYER_TYPE_STREAM =    (1 << 1),
}; 


class TestDeltaMediaPlayer
{
public:

    TestDeltaMediaPlayer(int32_t type, int32_t source );
    ~TestDeltaMediaPlayer();
    status_t	init(const char *url, int32_t rate = 48000, int32_t channels = 2);
    
    size_t	write(const char *buf, int size, int64_t pts = 0);

    bool	start();
    bool	prepareAsync();
    bool 	pause();
    bool	stop();
    bool	reset();
    bool	flush();
    status_t	release();

    status_t	seekTo(int msec);


    void setPlayerUserListener(PlayerUserListener *listener);
    void playerUserNotify(int msg, int ext1, int ext2);

private:
    sp<DeltaMediaPlayer> mPlayer;
    sp<PlayerProxyListener> mPlayerProxyListener;

    PlayerUserListener *mPlayerUserListener;
};

}

#endif
