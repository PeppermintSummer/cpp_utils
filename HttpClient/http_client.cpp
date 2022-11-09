#include "http_client.h"

#include <cstring>
#include <functional>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <queue>
#include <memory>
#include <string>
#include <vector>
#include <future>
#include <iostream>

#include <curl/curl.h>

size_t write_data(void* buffer,size_t size,size_t nmemb,void *stream){
	((std::string*)stream)->append((char*)buffer, size * nmemb);
	return size*nmemb;
}

class HttpClientImpl : public HttpClient
{
    using task = std::function<void()>;
public:
    void startup(){
        m_worker_thread = std::thread([this]{ //async run
            while(true){
                task task_;
                {
                    std::unique_lock<std::mutex> lock_(this->m_mutex);
                    this->m_cv.wait(lock_,[this]{
                        return this->m_stop || !this->m_tasks.empty();
                    });
                    if(this->m_stop && this->m_tasks.empty()) return;
                    task_ = this->m_tasks.front();
                    this->m_tasks.pop();
                }
                task_();
            }
        });
        // return ;
    }
    ~HttpClientImpl(){
        {
            std::unique_lock<std::mutex> lock_(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();
        if(m_worker_thread.joinable())
            m_worker_thread.join();
        printf("safe quit()\n");
    }

    std::shared_future<ReturnMsg> commitGet(const std::string& url){
        auto func = [=]() -> ReturnMsg {
            ReturnMsg msg;
            CURL* curl;
            CURLcode res;
            struct curl_slist* header_list = nullptr;
            curl = curl_easy_init();
            std::string out;
            if(curl == nullptr){
                msg._handle_success = false;
                msg._handle_code = CURLE_FAILED_INIT;
                msg._handle_str = "CURLE_FAILED_INIT";
                return msg;
            }
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            header_list = curl_slist_append(header_list,"User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko");
            curl_easy_setopt(curl,CURLOPT_HTTPHEADER,header_list);
            curl_easy_setopt(curl,CURLOPT_HEADER,0); //不接收响应头数据0代表不接收 1代表接收
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
            curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data);
            curl_easy_setopt(curl,CURLOPT_WRITEDATA, &out);
            curl_easy_setopt(curl,CURLOPT_CONNECTTIMEOUT,6);
            curl_easy_setopt(curl,CURLOPT_TIMEOUT,6);
            res = curl_easy_perform(curl);
            if(res != CURLE_OK){
                msg._handle_success = false;
                msg._handle_code = res;
                msg._handle_str = curl_easy_strerror(res);
            }else{
                msg._handle_success = true;
                msg._handle_code = 0;
                msg._handle_str = out;
            }
            curl_slist_free_all(header_list);
            curl_easy_cleanup(curl);
            return msg;
        };
        auto task_specific = std::make_shared<std::packaged_task<ReturnMsg()>>(std::bind(func));
        std::shared_future<ReturnMsg> fut = task_specific->get_future();
        {
            std::unique_lock<std::mutex> lock_(m_mutex);
            if(m_stop) throw std::runtime_error("Commit on Stopped Thread");
            m_tasks.emplace([task_specific](){(*task_specific)();});
        }
        m_cv.notify_one();
        return fut;
    }

    std::shared_future<ReturnMsg> commitPost(const std::string& url, const std::string& data){
        auto func = [=](){
            ReturnMsg msg;
            CURL* curl;
            CURLcode res;
            curl_slist* http_headers = nullptr;
            curl = curl_easy_init();
            std::string out;
            if(curl == nullptr){
                msg._handle_success = false;
                msg._handle_code = CURLE_FAILED_INIT;
                msg._handle_str = "CURLE_FAILED_INIT";
                return msg;
            }
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            http_headers = curl_slist_append(http_headers, "Content-Type:application/json;charset=UTF-8");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, http_headers);
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
            res = curl_easy_perform(curl);
            if(res != CURLE_OK){
                msg._handle_success = false;
                msg._handle_code = res;
                msg._handle_str = curl_easy_strerror(res);
            }else{
                msg._handle_success = true;
                msg._handle_code = 0;
                msg._handle_str = out;
            }
            curl_slist_free_all(http_headers);
            curl_easy_cleanup(curl);
            return msg;
        };
        auto task_specific = std::make_shared<std::packaged_task<ReturnMsg()>>(std::bind(func));
        std::shared_future<ReturnMsg> fut = task_specific->get_future();
        {
            std::unique_lock<std::mutex> lock_(m_mutex);
            if(m_stop) throw std::runtime_error("Commit on Stopped Thread");
            m_tasks.emplace([task_specific](){(*task_specific)();});
        }
        m_cv.notify_one();
        return fut;
    }

    std::shared_future<ReturnMsg> commitFormData(const std::string& url, const std::string& info) override {
        auto split_string = [](const std::string& str, const std::string& spstr){
            std::vector<std::string> res;
            if (str.empty()) return res;
            // lambda must be specific type
            if (spstr.empty()) return std::vector<std::string>{str};
            auto p = str.find(spstr);
            if (p == std::string::npos) return std::vector<std::string>{ str };
            res.reserve(5);
            std::string::size_type prev = 0;
            int lent = spstr.length();
            const char* ptr = str.c_str();
            while (p not_eq std::string::npos) {
                int len = p - prev;
                if (len > 0) {
                    res.emplace_back(str.substr(prev, len));
                }
                prev = p + lent;
                p = str.find(spstr, prev);
            }
            int len = str.length() - prev;
            if (len > 0) {
                res.emplace_back(str.substr(prev, len));
            }
            return res;
        };
        auto func = [=](){
            ReturnMsg msg;
            CURL* curl;
            CURLcode res;
            curl = curl_easy_init();
            curl_slist* http_headers = nullptr;
            struct curl_httppost* post = nullptr;
            struct curl_httppost* last = nullptr;
            std::string out;
            if(curl == nullptr){
                msg._handle_success = false;
                msg._handle_code = CURLE_FAILED_INIT;
                msg._handle_str = "CURLE_FAILED_INIT";
                return msg;
            }
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            http_headers = curl_slist_append(http_headers, "Content-Type:multipart/form-data;charset=UTF-8");
            std::string push_info = split_string(info, "/").back();
            curl_formadd(&post, &last, CURLFORM_PTRNAME, "file", CURLFORM_FILE, info.c_str(), CURLFORM_FILENAME, push_info.c_str(), CURLFORM_END);
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
            res = curl_easy_perform(curl);
            if(res != CURLE_OK){
                msg._handle_success = false;
                msg._handle_code = res;
                msg._handle_str = curl_easy_strerror(res);
            }else{
                msg._handle_success = true;
                msg._handle_code = 0;
                msg._handle_str = out;
            }
            curl_slist_free_all(http_headers);
            curl_easy_cleanup(curl);
            return msg;
        };
        auto task_specific = std::make_shared<std::packaged_task<ReturnMsg()>>(std::bind(func));
        std::shared_future<ReturnMsg> fut = task_specific->get_future();
        {
            std::unique_lock<std::mutex> lock_(m_mutex);
            if(m_stop) throw std::runtime_error("Commit on Stopped Thread");
            m_tasks.emplace([task_specific](){(*task_specific)();});
        }
        m_cv.notify_one();
        return fut;
    }

    bool setSendPolicy(SENDPOLICY policy) override {
        this->m_policy = policy;
        return true;
    }

private:
    std::thread m_worker_thread;
    std::queue<task> m_tasks;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::atomic<bool> m_stop;
    SENDPOLICY m_policy = SENDPOLICY::DISCARD;
};


std::shared_ptr<HttpClient> create_http_client() {
    std::shared_ptr<HttpClientImpl> instance(new HttpClientImpl());
    instance->startup();
    return instance;
}