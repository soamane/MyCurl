#include "MyCurl.hpp"
#include <stdexcept>

MyCurl::MyCurl() : m_curl(curl_easy_init()), m_proxy("") {
    if (!this->m_curl) {
        throw std::runtime_error("Failed to initialize curl");
    }
}

MyCurl::~MyCurl() {
    this->Cleanup();
}

void MyCurl::UseYourCurl(CURL* curl) {
    if (this->m_curl) {
        curl_easy_cleanup(this->m_curl);
    }
    this->m_curl = curl;
}

void MyCurl::UseProxyServer(std::string_view proxy) {
    this->m_proxy = proxy;
    curl_easy_setopt(this->m_curl, CURLOPT_PROXY, m_proxy.c_str());
}

void MyCurl::UseProxyServer(std::string_view proxy, std::string_view username, std::string_view password) {
    this->UseProxyServer(proxy);
    curl_easy_setopt(this->m_curl, CURLOPT_PROXYUSERNAME, username.data());
    curl_easy_setopt(this->m_curl, CURLOPT_PROXYPASSWORD, password.data());
}

void MyCurl::DisableProxyServer() {
    this->m_proxy.clear();
    curl_easy_setopt(this->m_curl, CURLOPT_PROXY, nullptr);
    curl_easy_setopt(this->m_curl, CURLOPT_PROXYUSERNAME, nullptr);
    curl_easy_setopt(this->m_curl, CURLOPT_PROXYPASSWORD, nullptr);
}

const curl_slist* MyCurl::AddHeaders(const std::vector<std::string>& headers) const {
    curl_slist* headerList = nullptr;
    for (const auto& header : headers) {
        headerList = curl_slist_append(headerList, header.c_str());
    }
    return headerList;
}

std::string MyCurl::PerformGetRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers) const {
    if (!this->m_curl) {
        throw std::runtime_error("CURL handle is not initialized");
    }

    CURL* curl = curl_easy_duphandle(this->m_curl);
    if (!curl) {
        throw std::runtime_error("Failed to duplicate curl handle");
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.data());

    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        response = "Failed to perform GET request: " + std::string(curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    return response;
}

std::string MyCurl::PerformPostRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers, std::string_view postfields) const {
    if (!this->m_curl) {
        throw std::runtime_error("CURL handle is not initialized");
    }

    if (headers == nullptr && postfields.empty()) {
        throw std::runtime_error("Invalid request arguments");
    }

    CURL* curl = curl_easy_duphandle(this->m_curl);
    if (!curl) {
        throw std::runtime_error("Failed to duplicate curl handle");
    }

    std::string response;
    curl_easy_setopt(curl, CURLOPT_URL, url.data());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.data());

    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        response = "Failed to perform POST request: " + std::string(curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
    return response;
}

std::future<std::string> MyCurl::PerformAsyncGetRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers)
{
    return std::async(std::launch::async, &MyCurl::PerformGetRequest, this, protocol, url, headers);
}

std::future<std::string> MyCurl::PerformAsyncPostRequest(const ProtocolType& protocol, std::string_view url, const curl_slist* headers, std::string_view postfields)
{
    return std::async(std::launch::async, &MyCurl::PerformPostRequest, this, protocol, url, headers, postfields);
}

void MyCurl::Cleanup() {
    if (this->m_curl) {
        curl_easy_cleanup(this->m_curl);
        this->m_curl = nullptr;
    }
}

void MyCurl::CleanupObject(CURL* curl) {
    if (curl) {
        curl_easy_cleanup(curl);
        curl = nullptr;
    }
}
const std::size_t MyCurl::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
    buffer->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}
