#define LOG_NDEBUG 0
#define LOG_TAG "TestDeltaRecord_S"
#include <utils/Log.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <binder/ProcessState.h>
#include <media/testdeltarecord.h>

#include <audio_file.h>

int32_t singintCnt = 0;

namespace android
{

}

static void usage(const char* me)
{
    fprintf(stderr,
            "usage:\n"
            "	%s  -o file.wav [-b -r rate -c ch -f frames]\n",
            me);
}

void sigint_handler(int sig)
{
    singintCnt = 1;
    fprintf(stderr, "sigint_handler singintCnt = 1 --> [%d]\n", singintCnt);
}


int main(int argc, char** argv)
{
    using namespace android;

    ProcessState::self()->startThreadPool();

    int logCnt = 0;

    int flushInsert;

    int res;
    int rate = 16000;
    int channels = 1;
    int frames = 800;
    int blockFlag = 0;
    //0 i2s
    //1 pcm
    int device = 0;

    int readBlock = 0;
    int readSize = 0;
    unsigned int resultWriteFrames = 0;

    struct wav_file* filesResult = NULL;
    uint8_t* readBuffer = NULL;
    const char* fileName = NULL;

    while ((res = getopt(argc, argv, "o:r:c:f:bd:h")) != -1) {
        switch (res) {
        case 'o': {
            fileName = optarg;
        }
        break;

        case 'r': {
            rate = atoi(optarg);
        }
        break;

        case 'c': {
            channels = atoi(optarg);
        }
        break;

        case 'd': {
            device = atoi(optarg);
        }
        break;

        case 'f': {
            frames = atoi(optarg);
        }
        break;

        case 'b': {
            blockFlag = 1;
        }
        break;

        case '?':
        case 'h':
        default: {
            usage(argv[0]);
            exit(1);
        }
        }
    }

    if (fileName == NULL) {
        fprintf(stderr, "fileName is NULL exit\n");
        exit(1);
    }
    fprintf(stderr, "paras: rate=%d, chs=%d, blockFlag=%d, frames=%d\n", rate, channels, blockFlag, frames);
    readSize = 2 * channels * frames;
    readBuffer = (uint8_t*)malloc(readSize + 4);

    if (!readBuffer) {
        fprintf(stderr, "Unable to malloc readBuffer \n");
        exit(1);
    } else {
        fprintf(stderr, "malloc readBuffer 0x%x\n", (unsigned int)(readBuffer));
        filesResult = create_wav_file(rate, AUDIO_FORMAT_PCM_16_BIT, channels, (char*)fileName);
        if (!filesResult) {
            fprintf(stderr, "Unable to create wav file '%s'\n", fileName);
            free(readBuffer);
            exit(1);
        } else {
            fprintf(stderr, "create wav file '%s' ok\n",  fileName);
        }
    }

    sp<TestDeltaRecord> record = new TestDeltaRecord(0);
    //default:
    //rate: 16000 ch: 1 readframes 256 framecnt 4 readFlag 0
    record->init(rate, channels, blockFlag, frames, 8);
    record->start();

    fprintf(stderr, "start test ......\n");
    signal(SIGINT, sigint_handler);

    flushInsert = 0;

    do {
        int ret;

        if (blockFlag) {
            ret = record->read(readBuffer, readSize);
            if (ret != readSize) {
                fprintf(stderr, "is error?? block read ret %d != readSize %d\n", ret, readSize);
            }
            write_wav_file(filesResult, readBuffer, ret / 2);
        } else {
            ret = record->read(readBuffer, readSize);
            if (ret > 0) {
                write_wav_file(filesResult, readBuffer, ret / 2);
            }
            if (ret != readSize) {
                usleep(((frames * 1000) / rate) * 1000);
            }
        }
    }while (!singintCnt);

    release_wav_file(filesResult);
    fprintf(stderr, "test exit\n");
    record->unInit();
    if (readBuffer) {
        fprintf(stderr, "free readBuffer ....\n");
        free(readBuffer);
    }

    return 0;
}
