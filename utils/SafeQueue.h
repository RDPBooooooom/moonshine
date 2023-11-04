#ifndef MOONSHINENETWORKING_SAFEQUEUE_H
#define MOONSHINENETWORKING_SAFEQUEUE_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <deque>

namespace moonshine {

    template<typename T>
    class SafeQueue {

    protected:
        boost::mutex m_muxQueue;
        std::deque<T> m_deqQueue;
        boost::condition_variable m_cvBlocking;
        boost::mutex m_muxBlocking;
        
        bool notifiedToStop = false;

    public:
        SafeQueue() = default;

        SafeQueue(const SafeQueue<T> &) = delete;

        virtual ~SafeQueue() { clear(); }

    public:
        const T &front() {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            return m_deqQueue.front();
        }

        const T &back() {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            return m_deqQueue.back();
        }

        T pop_front() {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            auto item = std::move(m_deqQueue.front());
            m_deqQueue.pop_front();
            return item;
        }

        T pop_back() {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            auto item = std::move(m_deqQueue.back());
            m_deqQueue.pop_back();
            return item;
        }

        void push_front(const T &item) {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            m_deqQueue.emplace_front(std::move(item));
            
            boost::unique_lock<boost::mutex> uniqueLock(m_muxBlocking);
            m_cvBlocking.notify_one();
        }

        void push_back(const T &item) {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            m_deqQueue.emplace_back(std::move(item));

            boost::unique_lock<boost::mutex> uniqueLock(m_muxBlocking);
            m_cvBlocking.notify_one();
        }

        bool is_empty() {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            return m_deqQueue.empty();
        }

        size_t count() {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            return m_deqQueue.size();
        }

        void clear() {
            boost::mutex::scoped_lock scopedLock(m_muxQueue);
            m_deqQueue.clear();
        }
        
        void notify(){
            boost::unique_lock<boost::mutex> uniqueLock(m_muxBlocking);
            m_cvBlocking.notify_one();
        }

        void notifyToStop(){
            boost::unique_lock<boost::mutex> uniqueLock(m_muxBlocking);
            notifiedToStop = true;
            m_cvBlocking.notify_one();
        }

        void wait() {
            while (is_empty() && !notifiedToStop) {
                boost::unique_lock<boost::mutex> uniqueLock(m_muxBlocking);
                m_cvBlocking.wait(uniqueLock);
            }
            notifiedToStop = false;
        }
    };
}
#endif //MOONSHINENETWORKING_SAFEQUEUE_H
