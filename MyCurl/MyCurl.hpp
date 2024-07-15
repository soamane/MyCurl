#pragma once
#ifndef MY_CURL_HPP
#define MY_CURL_HPP

/*
   Internal libraries
*/
#include <future>
#include <string_view>
#include <initializer_list>

/*
   External libraries
*/
#include <curl/curl.h>

// Enum defining HTTP and HTTPS protocols
enum ProtocolType {
    HTTP,
    HTTPS
};

// Class for handling HTTP requests using libcurl
class MyCurl {
public:
    // Constructor: initializes libcurl handle
    MyCurl();

    // Destructor: cleans up libcurl resources
    ~MyCurl();
    
    // Use custom curl pointer for object
    void UseYourCurl(CURL* curl);

    /*
        Sets a proxy server without authentication
        Usage: "127.0.0.1:123"
    */ 
    void UseProxyServer(std::string_view proxy);

    // Sets a proxy server with authentication
    void UseProxyServer(std::string_view proxy, std::string_view username, std::string_view password);

    // Disables the use of proxy server
    void DisableProxyServer();

    // Adds headers to the HTTP request
    const curl_slist* AddHeaders(const std::initializer_list<std::string>& headers) const;

    // Performs a GET request
    std::string PerformGetRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers) const;

    // Performs a POST request
    std::string PerformPostRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers, std::string_view postfields) const;

    // Performs a async GET request
    std::future<std::string> PerformAsyncGetRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers);

    // Performs a async POST request
    std::future<std::string> PerformAsyncPostRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers, std::string_view postfields);

    // Cleanup the CURL's object
    void Cleanup();

    // Cleanup custom CURL's object
    void CleanupObject(CURL* curl);

private:
    // Callback function to handle data received in a HTTP response
    static const std::size_t WriteCallback(void* contents, std::size_t size, std::size_t nmemb, std::string* buffer);

private:
    CURL* m_curl;         // Handle for libcurl
    std::string m_proxy;  // Proxy server address
};

#endif // !MY_CURL_HPP
