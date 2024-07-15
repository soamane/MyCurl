# MyCurl - Simple wrapper for the CURL library
This library (MyCURL) is useful for simplifying HTTP(S) request handling in your project. It provides a convenient interface for sending requests, supports proxy setup, and allows asynchronous request execution. Using such a library significantly reduces the amount of code required for network requests and enhances overall reliability and security when working with the HTTP protocol.

# Usage
This is a static library. To integrate it into your project, you need to use standard methods for linking external libraries, just like with any other library. The repository already contains pre-built binaries for the latest commit of MyCURL for x64/x86 in both Debug and Release configurations, so you don't need to build anything unless you want to make changes.

### Usage example
```
	MyCurl myCurl;
	myCurl.UseProxyServer("http://185.77.139.34:8000", "FVwg1b", "7qGrM");
	std::cout << myCurl.PerformGetRequest(ProtocolType::HTTPS, "https://httpbin.org/get", nullptr);

	myCurl.DisableProxyServer();
	std::cout << myCurl.PerformGetRequest(ProtocolType::HTTPS, "https://httpbin.org/get", nullptr);
```

# Why did I create it?
IT. IS. AN. INCREDIBLY. CONVENIENT. TOOL.
And I've gained experience working with static libraries, LOL
