
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <future>


enum class HttpMethod{
    HttpGet,
    HttpPost
};
enum class SENDPOLICY{
    DISCARD,
    APPEND
};

struct ReturnMsg {
    std::string _handle_str="";
    int  _handle_code;
    bool _handle_success;
};

class HttpClient{
public:
    /**
     * @brief : setSendPolicy: when meet error, whether go on send msg?
    */
    virtual bool setSendPolicy(SENDPOLICY policy) = 0;

    /**
     * @brief HTTP Get request
     * @param url : http url.
    */
    virtual std::shared_future<ReturnMsg> commitGet(const std::string& url)  = 0;

    /**
     * @brief HTTP Post request
     * @param url   : http url.
     * @param input : http post body.
    */
    virtual std::shared_future<ReturnMsg> commitPost(const std::string& url, const std::string& input) = 0;

    /**
     * @brief HTTP Post formdata, such as image,file...
     * @param url   : http url.
     * @param input : file path; (support).
     * @param return: Shared_future ReturnMsg object.
    */
    virtual std::shared_future<ReturnMsg> commitFormData(const std::string& url, const std::string& input) = 0;
    // virtual void set_callback(http_callback cb_ = nullptr) = 0;
};

std::shared_ptr<HttpClient> create_http_client();