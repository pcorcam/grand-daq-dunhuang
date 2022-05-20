#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <memory>
#include <stdexcept>

namespace grand {

class XEvent { // No signal when init
public:
    void wait(){
        std::unique_lock<std::mutex> lock(m_mtx);
        cv_.wait(lock);
    }
 
    bool wait(int millSec){
        std::unique_lock<std::mutex> lock(m_mtx);
        std::chrono::milliseconds mills(millSec);
        auto ret = cv_.wait_for(lock, mills);
        return (ret != std::cv_status::timeout);
    }
 
    void notify(){
        cv_.notify_one();
    }
    void notifyAll(){
        cv_.notify_all();
    }
 
private:
    std::mutex m_mtx;
    std::condition_variable cv_;
};

class XSemaphore    {
public :
    XSemaphore(int value=0) : wakeups { value }    { }
 
    bool wait (int millSec) {
        std::chrono::milliseconds mills(millSec);
        std::unique_lock <std::mutex> lock{mutex};
        bool ret = cv_.wait_for(lock, mills, [&]()->bool{ return wakeups>0; }) ;    // suspend and wait ...
        if(ret) {
            --wakeups;     // ok, me wakeup !
            return true;
        }
        else {
            return false;
        }
    }

    void notify() {
        std::lock_guard <std::mutex> lock { mutex } ;
        ++wakeups ;
        cv_.notify_one();
    }
 
private :
    int wakeups ;
    std::mutex mutex ;
    std::condition_variable cv_;
};

class XClock {
public:
    inline static uint64_t nowMilliSeconds() {
        std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
    }
};

template<typename ... Args>
std::string stringFormat( const std::string& format, Args ... args )
{
    int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

}

#include <internal/thread_pool.h>
#include <internal/easylogging++.h>
#include <internal/buffer_pool.h>


