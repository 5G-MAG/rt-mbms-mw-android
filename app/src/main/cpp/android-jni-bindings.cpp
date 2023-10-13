#include <jni.h>
#include <string>
#include "Receiver.h"
#include "Middleware.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/android_sink.h"
#include <boost/asio.hpp>
#include <thread>
#include <chrono>
#include <pplx/threadpool.h>

static std::unique_ptr<LibFlute::Receiver> receiver;
static std::unique_ptr<MBMS_RT::Middleware> mw;
static std::unique_ptr<boost::asio::io_service> ios;
static std::unique_ptr<std::thread> io_thread;
static signed char * cbuffer;

extern "C" jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }

    cpprest_init(vm);
    crossplat::threadpool::initialize_with_threads(100);
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT bool JNICALL
Java_com_nakolos_ossmw_NakolosMwService_startNativeMiddleware(JNIEnv* env,
jobject thiz, jstring device_name, jstring japi_key) {
    std::string tag = "nakolos-mw";
    try {
        auto android_logger = spdlog::android_logger_mt("nakolos-mw", tag);
        spdlog::set_default_logger(android_logger);
        spdlog::info("Launching NAKOLOS native middleware");
        spdlog::set_level(spdlog::level::debug);
    } catch (...) {}

    cbuffer = (signed char*)malloc(2048);

    const char *dev_name = env->GetStringUTFChars(device_name, NULL);
    const char *api_key = env->GetStringUTFChars(japi_key, NULL);

    ios = std::make_unique<boost::asio::io_service>();
    mw = std::make_unique<MBMS_RT::Middleware>(*(ios.get()), "http://0.0.0.0:3020/",
                                               dev_name, api_key);

    io_thread = std::make_unique<std::thread>([&]{
        ios->run();
        spdlog::info("ASIO io thread has ended");
    });

    env->ReleaseStringUTFChars(device_name, dev_name);
    env->ReleaseStringUTFChars(japi_key, api_key);
    //receiver = std::make_unique<LibFlute::Receiver>( 0 );
    return true;
}

extern "C" JNIEXPORT bool JNICALL
Java_com_nakolos_ossmw_UdpReceiver_handlePacket(JNIEnv* env,
                                                     jobject thiz,
                                                     jbyteArray buffer,
                                                     jint size) {
    assert(size <= 2048);
    env->GetByteArrayRegion(buffer, 0, size, reinterpret_cast<jbyte *>(cbuffer));
    //const jbyte* raw_buffer = env->GetByteArrayElements(buffer, NULL);
   // spdlog::info("Passing buffer with {} bytes at {} to FluteReceiver", size, (void*)cbuffer);
    mw->handle_received_data(cbuffer, size);
    //env->ReleaseByteArrayElements(buffer, const_cast<jbyte *>(cbuffer), JNI_ABORT);
    return true;
}
extern "C" JNIEXPORT bool JNICALL
Java_com_nakolos_ossmw_NakolosMwService_setLocalServiceAnnouncement(JNIEnv* env,
                                                    jobject thiz,
                                                    jstring sa) {
    const char *csa = env->GetStringUTFChars(sa, NULL);
    mw->set_local_service_announcement(csa);
    env->ReleaseStringUTFChars(sa,csa);
    return true;
}

extern "C" JNIEXPORT bool JNICALL
Java_com_nakolos_ossmw_NakolosMwService_stopNativeMiddleware(JNIEnv* env,
                                                     jobject thiz) {
    using namespace std::chrono_literals;
    spdlog::info("Stopping io_service tasks");
    if (mw) {
        mw->stop();
    }
    if (ios && !ios->stopped()) {
        while (!ios->stopped()) {
            std::this_thread::sleep_for(100ms);
        }
    }
    spdlog::info("Destroying mw");
    mw.reset();
    spdlog::info("Destroying ios");
    ios.reset();

    if (cbuffer) {
        spdlog::info("Freeing packet buffers");
        free(cbuffer);
        cbuffer = nullptr;
    }
    return true;
}
/*
extern "C" JNIEXPORT jboolean JNICALL
Java_com_bitstem_libflute_MainActivity_startReceiver(
        JNIEnv* env,
        jobject  this ) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}*/