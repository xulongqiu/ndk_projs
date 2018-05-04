#ifndef ANDROID_TESTDELTAMEDIAPOLICY_H
#define ANDROID_TESEDELTAMEDIAPOLICY_H

#include <system/audio.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>

#include <binder/Parcel.h>

namespace android {

class DeltaMediaPolicy;
class PolicyProxyListener;

enum policy_user_notify{
	USER_AUDIO_MEDIA_CONTROL_PLAY,
	USER_AUDIO_MEDIA_CONTROL_PAUSE,
	USER_AUDIO_MEDIA_CONTROL_STOP,
	USER_AUDIO_MEDIA_CONTROL_NEXT,
	USER_AUDIO_MEDIA_CONTROL_PREV,
	USER_AUDIO_MEDIA_CONTROL_RESUME,
	USER_AUDIO_MEDIA_CONTROL_MUTE,
	USER_AUDIO_MEDIA_CONTROL_UNMUTE,
	USER_AUDIO_MEDIA_CONTROL_REPEAT,
	USER_AUDIO_MEDIA_CONTROL_VOLUME,
};

enum policy_user_status{
	UserPlayStatus_Playing			= 0,
	UserPlayStatus_Stopped			= 1,
	UserPlayStatus_Paused			= 2,
	UserPlayStatus_Transitioning	= 3,
	UserPlayStatus_Finished     	= 4,
};

enum policy_vr_status{
	Vr_Online_Stop = 0,
	Vr_Online_Start = 1,
	Vr_Online_Return = 2,
};


class PolicyUserListener
{
public:
	//Listener
	virtual ~PolicyUserListener(){}
	virtual void policyNotify(int msg, int ext1, int ext2) = 0;
};


class TestDeltaPolicy
{
public:
	TestDeltaPolicy(int32_t id);
	~TestDeltaPolicy();
	void policyUserNotify(int msg, int ext1, int ext2);
	void setPolicyUserListener(PolicyUserListener *listener);
	void release();
	void setStatus(int32_t status);
	void setVolume(int32_t volume);
	void requestAudioPlay(int32_t capability);
	void setVrStatus(int32_t status);
private:
	sp<DeltaMediaPolicy> mPolicy;
	sp<PolicyProxyListener> mPolicyProxyListener;

	PolicyUserListener *mPolicyUserListener;
	int32_t mId;
};

}

#endif
