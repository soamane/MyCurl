#pragma once
#ifndef MY_CURL_HPP
#define MY_CURL_HPP

/*
   Internal libraries
*/
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

    // Sets a proxy server without authentication
    void UseProxyServer(std::string_view proxy);

    // Sets a proxy server with authentication
    void UseProxyServer(std::string_view proxy, std::string_view username, std::string_view password);

    // Disables the use of proxy server
    void DisableProxyServer();

    // Adds headers to the HTTP request
    const curl_slist* AddHeaders(const std::initializer_list<std::string> headers) const;

    // Performs a GET request
    const std::string PerformGetRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers) const;

    // Performs a POST request
    const std::string PerformPostRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers, std::string_view postfields) const;

private:
    // Callback function to handle data received in a HTTP response
    static const std::size_t WriteCallback(void* contents, std::size_t size, std::size_t nmemb, std::string* buffer);

private:
    CURL* m_curl;         // Handle for libcurl
    std::string m_proxy;  // Proxy server address
};

#endif // !MY_CURL_HPP
