#include "MyCurl.hpp"

#include <stdexcept>

MyCurl::MyCurl() : m_curl(curl_easy_init()), m_proxy("")
{
    if (!this->m_curl) {
        throw std::runtime_error("Failed to initialize curl");
    }
}

MyCurl::~MyCurl()
{
    curl_easy_cleanup(this->m_curl);
}

void MyCurl::UseProxyServer(std::string_view proxy)
{
    // Set the proxy server address
    this->m_proxy = std::move(proxy);
    curl_easy_setopt(this->m_curl, CURLOPT_PROXY, m_proxy.c_str());
}

void MyCurl::UseProxyServer(std::string_view proxy, std::string_view username, std::string_view password)
{
    // Set the proxy server address
    this->UseProxyServer(proxy);

    // Set proxy authentication details
    curl_easy_setopt(this->m_curl, CURLOPT_PROXYUSERNAME, username.data());
    curl_easy_setopt(this->m_curl, CURLOPT_PROXYPASSWORD, password.data());
}

void MyCurl::DisableProxyServer()
{
    // Clear proxy settings
    this->m_proxy.clear();

    // Disable proxy settings in libcurl
    curl_easy_setopt(this->m_curl, CURLOPT_PROXY, nullptr);
    curl_easy_setopt(this->m_curl, CURLOPT_PROXYUSERNAME, nullptr);
    curl_easy_setopt(this->m_curl, CURLOPT_PROXYPASSWORD, nullptr);
}

const curl_slist* MyCurl::AddHeaders(const std::initializer_list<std::string> headers) const
{
    curl_slist* headerList = nullptr;
    for (const auto& header : headers) {
        // Append each header to the list
        headerList = curl_slist_append(headerList, header.c_str());
    }
    return headerList;
}

const std::string MyCurl::PerformGetRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers) const
{
    // Duplicate the curl handle to ensure thread-safety
    CURL* curl = curl_easy_duphandle(this->m_curl);
    if (!curl) {
        throw std::runtime_error("Failed to duplicate curl handle");
    }

    std::string response;

    // Set URL for the GET request
    curl_easy_setopt(curl, CURLOPT_URL, url.data());

    // Set custom headers if provided
    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Set callback function for writing response data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the GET request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        response = "Failed to perform GET request: " + std::string(curl_easy_strerror(res));
    }

    // Cleanup and release resources
    curl_easy_cleanup(curl);
    return response;
}

const std::string MyCurl::PerformPostRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers, std::string_view postfields) const
{
    // Check for invalid request arguments
    if (headers == nullptr && postfields.empty()) {
        throw std::runtime_error("Invalid request arguments");
    }

    // Duplicate the curl handle to ensure thread-safety
    CURL* curl = curl_easy_duphandle(this->m_curl);
    if (!curl) {
        throw std::runtime_error("Failed to duplicate curl handle");
    }

    std::string response;

    // Set URL for the POST request
    curl_easy_setopt(curl, CURLOPT_URL, url.data());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);  // Set POST method

    // Set POST fields
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.data());

    // Set custom headers if provided
    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Set callback function for writing response data
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the POST request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        response = "Failed to perform POST request: " + std::string(curl_easy_strerror(res));
    }

    // Cleanup and release resources
    curl_easy_cleanup(curl);
    return response;
}

const std::size_t MyCurl::WriteCallback(void* contents, std::size_t size, std::size_t nmemb, std::string* buffer)
{
    // Append received data to the buffer
    buffer->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;  // Return the number of bytes received
}
