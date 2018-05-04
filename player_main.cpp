#define LOG_NDEBUG 0
#define LOG_TAG "TestDeltaPlayer_S"
#include "utils/Log.h"
#include <binder/ProcessState.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <media/testdeltaplayer.h>
#include <mediapolicy/testdeltapolicy.h>

namespace android{

class UserClassPlayerListener : public PlayerUserListener
{
public:
	UserClassPlayerListener() {}	
	~UserClassPlayerListener() {}
	
	void playerNotify(int msg, int ext1, int ext2)
	{
		switch(msg){
			//TODO player url buffering position	
			case       USER_NOTIFY_INFO_BUFFERING:
				{
					ALOGV("notify info buffering %d\n",ext1);
				}
				break;
			//TODO player url playing position	
			case       USER_NOTIFY_INFO_POSITION:
				{
					ALOGV("notify info position %d\n",ext1);
				}
				break;
			//TODO player url stream duration	
			case       USER_NOTIFY_INFO_DURATION:
				{
					ALOGV("notify info duration %d\n", ext1);
				}
				break;
			//TODO player url file size	
			case       USER_NOTIFY_INFO_SIZE:
				{
					ALOGV("notify info file size %d\n", ext1);
				}
				break;
			//TODO player url play complete	
			case       USER_NOTIFY_PLAYCOMPLETE:
				ALOGV(" notify info current item playback complete\n");
				//just for test
				break;
			//TODO player url seek complete	
			case       USER_NOTIFY_SEEKCOMPLETE:
				ALOGV(" notify info current item seek complete\n");
				break;
    			//TODO player url seek error	
			case       USER_NOTIFY_SEEKERROR:
				ALOGV(" notify info current item seek error\n");
				break;
    			//TODO player url play error	
			case       USER_NOTIFY_PLAYERROR:
				ALOGV(" notify info current item play error\n");
				//just for test
				break;
    			//TODO player url prepare complete	
			case       USER_NOTIFY_PREPARECOMPLETE:
				ALOGV(" notify info current item prepare complete\n");
				break;
    			//TODO player playing start buffing 
			case       USER_NOTIFY_START_BUFFERING:
				ALOGV(" notify info current item start buffering\n");
				break;
    			//TODO player playing end buffing 
			case       USER_NOTIFY_END_BUFFERING:
				ALOGV(" notify info current item end buffering\n");
				break;
			default:
				break;
		}
	}
};

class UserClassPolicyListener : public PolicyUserListener
{
public:
	UserClassPolicyListener() {}	
	~UserClassPolicyListener() {}
	void policyNotify(int msg, int ext1, int ext2)
	{
		switch(msg)
		{
			case USER_AUDIO_MEDIA_CONTROL_PLAY:
			{
				ALOGV("--->play<---\n");
				//start();
			}
			break;
			case USER_AUDIO_MEDIA_CONTROL_PAUSE:
			{
				ALOGV("--->pause<---\n");
				//pause();
			}
			break;
			case USER_AUDIO_MEDIA_CONTROL_STOP:
			{
				ALOGV("--->stop<---\n");
				//stop();
			}
			break;
			case USER_AUDIO_MEDIA_CONTROL_NEXT:
			{
				ALOGV("--->next<---\n");
			}
			break;
			case USER_AUDIO_MEDIA_CONTROL_PREV:
			{
				ALOGV("--->prev<---\n");
			}
			break;
			case USER_AUDIO_MEDIA_CONTROL_RESUME:
			{
				ALOGV("--->resume<---\n");
				//start();
			}
			break;
			case USER_AUDIO_MEDIA_CONTROL_MUTE:
			{
				ALOGV("--->mute<---\n");
			}
			break;
			case USER_AUDIO_MEDIA_CONTROL_UNMUTE:
			{
				ALOGV("--->unmute<---\n");
			}
			break;
			case USER_AUDIO_MEDIA_CONTROL_VOLUME:
			{
				ALOGV("--->volume %d<---\n",ext1);
			}
			break;
			default:
				ALOGV("--->default<----\n");
			break;
		}	
	}
	
};


class UserClass : public Thread
{
public:
	UserClass(const char* fileName,int32_t type, int32_t source, int32_t rate, int32_t ch)
	{
		ALOGV("UserClass::UserClass(%p)\n",this);
		ALOGV("fileName %s\n",fileName);
		mType = type;
		mFd = 0;

		if(mType == TEST_PLAYER_TYPE_STREAM)
		{
			mFd = open(fileName, O_RDONLY, 0777);
			//1k --> 500ms
			mBufferSize = 4096 * 4;
			mBuffer = (char *)malloc(mBufferSize + 4);
			if(mFd)
			{
				ALOGV("open streamfile ok\n");
				lseek64(mFd, 44, SEEK_SET);
			}
			else
				ALOGV("open streamfile failed\n");
		}
	
		mFileName = fileName;
		mRate = rate;
		mChannels = ch;
		
		//test source-->bt/aux
		mPaused = 1;	
		mPlayer = new TestDeltaMediaPlayer(type, source);
		mPlayer->setPlayerUserListener(new UserClassPlayerListener());
		mPolicy = new TestDeltaPolicy(source);
		mPolicy->setPolicyUserListener(new UserClassPolicyListener());
		run("UserClassStream", ANDROID_PRIORITY_URGENT_AUDIO);
		ALOGV("stream audio write thread start\n");
	}
	~UserClass()
	{
		ALOGV("UserClass::~UserClass(%p)\n",this);
		if(mFd)
			close(mFd);
		if(mBuffer)
		{
			free(mBuffer);
			mBuffer = NULL;
		}
		if(mPlayer != NULL)
		{
			mPlayer->release();
			delete mPlayer;
			mPlayer = NULL;
		}
		ALOGV("UserClass::~UserClass ok(%p)\n",this);
	}


	void init()
	{
	//	AutoMutex _l(mLock);
		if(mPlayer->init(mFileName, mRate, mChannels) != NO_ERROR)
			ALOGV("init  is error\n");
	}

	void prepare()
	{
	//	AutoMutex _l(mLock);
		if(!mPlayer->prepareAsync())
			ALOGV("prepare  is error\n");
	}

	void request()
	{
	//	AutoMutex _l(mLock);
		mPolicy->requestAudioPlay(0);	
	}

	void start()
	{
	//	AutoMutex _l(mLock);
		if(mPlayer->start())
		{
			ALOGV("mplayer start ok\n");

			mPolicy->setStatus(UserPlayStatus_Playing);
			mPaused = 0;
		}
		else
		{
			ALOGV("mplayer start failed\n");
		}

	}

	void vrStatus(int32_t status)
	{
		mPolicy->setVrStatus(status);
	}

	void pause()
	{
	//	AutoMutex _l(mLock);
		
		if(mPlayer->pause())
		{
			ALOGV("mplayer pause ok\n");
			mPolicy->setStatus(UserPlayStatus_Paused);
			mPaused = 1;
		}
		else
		{
			ALOGV("mplayer pause failed\n");
		}
	}
	
	void seekTo(int msec)
	{
//		AutoMutex _l(mLock);
		if(mPlayer->seekTo(msec) != NO_ERROR)
			ALOGV("seekTo msec %d error\n",msec);
	}
	
	void stop()
	{
//		AutoMutex _l(mLock);

		if(mPlayer->stop())
		{
			ALOGV("mplayer stop ok\n");
			mPolicy->setStatus(UserPlayStatus_Stopped);
			mPaused = 1;
		}
		else
		{
			ALOGV("mplayer stop failed\n");
		}
		
		if(mFd)
		{
			lseek64(mFd, 44, SEEK_SET);
		}
	}

	void reset()
	{
//		AutoMutex _l(mLock);

		mPolicy->setStatus(UserPlayStatus_Stopped);
		mPlayer->reset();
		mPaused = 1;
		
		if(mFd)
		{
			lseek64(mFd, 44, SEEK_SET);
		}
	}

	void release()
	{
		ALOGV("release");
//		AutoMutex _l(mLock);
		if(mType == TEST_PLAYER_TYPE_STREAM)
		{
			Thread::requestExit();
			if(mPaused)
			{
				mPaused = 0;
			//	mWaitWorkCV.signal(); 
			}
			//requestExitAndWait();
		}
		if(mPlayer != NULL)
		{
			mPlayer->release();
			delete mPlayer;
			mPlayer = NULL;
		}
		if(mPolicy != NULL)
		{
			mPolicy->release();
			delete mPolicy;
			mPolicy = NULL;
		}
	}

	//stream write audio data
	bool threadLoop()
	{
		int32_t len = 0;
		static int32_t logCnt = 0;
		static int64_t writeByte = 0;

		if(!logCnt)
		{
			logCnt = 1;
			ALOGV("threadloop run,,,,,,,,,,,,\n");
		}
		logCnt +=  1;	
		{
//			AutoMutex _l(mLock);
			if(mPaused)
			{
				if((logCnt & 0x01f) == 0)	
					ALOGV("*** synctrack thread paused ***\n");
				usleep(100*1000);
		//		mWaitWorkCV.wait(mLock);
				return true;
			}
			else
			{
				if((logCnt & 0x0ff) == 0)	
					ALOGV("*** synctrack thread started ***\n");
			}
		}

		if(mFd)
		{
			
			len = read(mFd, mBuffer, mBufferSize);
		
		
			if(len > 0)
			{
				int32_t ret;

				ret = mPlayer->write(mBuffer, len);
				if(ret != len)
				{
					ALOGV("player stop or pause? \n");
					usleep(100*1000);
				}
				else 
				{
					writeByte += len;					
					if((logCnt & 0x1f) == 0)
					{
						ALOGV("writeByte is %lld\n",writeByte);
					}
				}
			}
			else
			{
				// playback complete
				usleep(50000);
				if((logCnt & 0x1ff) == 0)
				{
					ALOGV("file read len <= 0\n");
				}
			}
		}
		else
		{
			usleep(50000);
			if((logCnt & 0x1ff) == 0)
			{
				logCnt = 1;
				ALOGV("is bug,,,, mFd == 0 but threadloop run\n");
			}
		}
		//100ms

		return true;
	}

private:
//    	Mutex	mLock;
//    	Condition mWaitWorkCV;

	int32_t mType;
	int32_t mSource;
	int32_t mPaused;
	int mFd;

	
	int mBufferSize;
	char* mBuffer;

	const char * mFileName;
	int32_t mRate;
	int32_t mChannels;

	TestDeltaMediaPlayer *mPlayer;
	TestDeltaPolicy *mPolicy;
};

}

static void usage(const char *me)
{
	fprintf(stderr,
			"usage:\n"
			"	%s -s streamfilename -r rate -c ch  | -u urlfilename \n",
			me);
}

int main(int argc, char**argv)
{
	using namespace android;

	ProcessState::self()->startThreadPool();

	int res;
	const char * fileName = NULL;
	bool isStream = false;
	bool isUrl = false;

	int source = 20;
	uint32_t fileRate = 48000;
	uint32_t fileCh = 2;
	
	fprintf(stderr, "test begin,,,,\n");
	char cmd[64];
	while((res = getopt(argc, argv, "s:u:r:c:h")) != -1 )
	{
		switch(res){
			case 'r':
			{
				fileRate = atoi(optarg);
				fprintf(stderr, "fileRate %d\n",fileRate);	
			}
				break;
			case 'c':
			{
				fileCh = atoi(optarg);
				fprintf(stderr, "filech %d\n",fileCh);	
			}
				break;
			case 's':
			{
				isStream = true;
				fileName = optarg;
				fprintf(stderr, "is stream test %s\n",fileName);
			}
				break;
			case 'u':
			{
				isUrl = true;
				fileName = optarg;
				fprintf(stderr, "is url test %s\n",fileName);
			}
				break;
			case '?':
			case 'h':
			default:
			{
				usage(argv[0]);
				exit(1);
			}
		}
	}
	if((fileName == NULL))
	{
		fprintf(stderr, "You need to select fileName\n");
		exit(1);
	}
	int32_t type;
	if(isStream)
		type = TEST_PLAYER_TYPE_STREAM;
	if(isUrl)
		type = TEST_PLAYER_TYPE_URL;

	fprintf(stderr,"fileName:%s,fileRate %d type %d, source:%d\n", fileName,fileRate, type, source);

	sp<UserClass> Player = new UserClass(fileName, type, source, fileRate, fileCh);

	char str1[64];
	char str2[64];
	do{
		memset(cmd, 0, sizeof(cmd));
		fprintf(stderr, "cmd=request|init|prepare|start|seek,ms|pause|stop|reset|exit|vrstart|vrstop|vrreturn\n");
		fprintf(stderr, "please input cmd: ");
		scanf("%s",cmd);

		fprintf(stderr, "cmd:%s\n",cmd);

		if(strcmp(cmd,"start") == 0)
		{
			Player->start();
		}
		else if(strcmp(cmd,"pause") == 0)
		{
			Player->pause();
		}
		else if(strcmp(cmd,"request") == 0)
		{
			Player->request();
		}
		else if(strcmp(cmd,"stop") == 0)
		{
			Player->stop();
		}
		else if(strcmp(cmd,"reset") == 0)
		{
			Player->reset();
		}
		else if(strcmp(cmd,"init") == 0)
		{
			Player->init();
		}
		else if(strcmp(cmd,"prepare") == 0)
		{
			Player->prepare();
		}
		else if(strcmp(cmd, "vrstart") == 0)
		{
			Player->vrStatus(Vr_Online_Start);
		}
		else if(strcmp(cmd, "vrstop") == 0)
		{
			Player->vrStatus(Vr_Online_Stop);
		}
		else if(strcmp(cmd, "vrreturn") == 0)
		{
			Player->vrStatus(Vr_Online_Return);
		}
		else if(strcmp(cmd,"exit") == 0)
		{
			break;
		}
		else
		{
			sscanf(cmd, "%[^,],%[^,]",str1,str2);
			fprintf(stderr, "str1:%s, str2:%s\n",str1,str2);
			if(strcmp(str1, "seek") == 0)
			{
				Player->seekTo(atoi(str2));
			}
	
		}
	}while(1);

	Player->release();
	Player.clear();

	fprintf(stderr, "player exit\n");
	return 0;
}
