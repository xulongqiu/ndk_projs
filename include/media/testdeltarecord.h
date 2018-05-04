
#ifndef ANDROID_TESTDELTARECORD_H
#define ANDROID_TESEDELTARECORD_H

#include <system/audio.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>

#include <binder/Parcel.h>

namespace android {

class DeltaRecord;

class TestDeltaRecord : virtual public RefBase
{
public:
    /* TestDeltaRecord
     * In     : int id, ignore this parameter now.
     * Out    :
     * Return :
    */
    TestDeltaRecord(int id);
    ~TestDeltaRecord();
    /* init
     * In     : int rate, sample rate, 16k, 44.1k, 48k ...
     * In     : int chs, channels, 1, 2
     * In     : int isBlock, 0: 有数据则返回读取到的数据长度， 没有数据立即返回；非0：读取制定长度的数据或者出错返回
     * In     : int frames, 推荐使用默认值 
     * In     : int frameSize, 推荐使用默认值 
     * Out    : 
     * Return : int, 0: init 未出错， 非0： init 出错
    */
    int init(int rate, int chs, int isBlock, int frames=800, int frameSize=4);
    /* start
     * In     :
     * Out    :
     * Return : int, 0: Ok, 非0: error
    */
    int start(void);
    /* reset
     * In     :
     * Out    :
     * Return : int, 0: Ok, 非0: error
    */
    int reset(void);
    /* pause
     * In     :
     * Out    :
     * Return : int, 0: Ok, 非0: error
    */
    int pause(void);
    /* unInit
     * In     :
     * Out    :
     * Return : int, 0: Ok, 非0: error
    */
    int unInit(void);
    /* read
     * In     : int size, 请求读取的字节数
     * Out    : uint8_t* buf, 读取到的pcm数据
     * Return : int, 实际读取的字节数，< 0: error
    */
    int read(uint8_t* buf, int size);

private:
    sp<DeltaRecord> mRecord;
};

}

#endif
