#ifndef HTTP_WIN_SERVER_H
#define HTTP_WIN_SERVER_H

#include"userlib.h"
#include<boost/beast/core.hpp>
#include<boost/beast/http.hpp>
#include<boost/asio/ip/tcp.hpp>
#include<boost/config.hpp>
#include<string>
#include<iostream>
#include<thread>
#include<memory>
#include<cstdlib>
#include<exception>

namespace rhttp
{
	constexpr const char* _BOOST_BEAST_VERSION_STRING = "Boost.Beast\0";
	namespace beast = boost::beast;         
	namespace http = beast::http;           
	namespace net = boost::asio;            
	using tcp = boost::asio::ip::tcp;       

	class server : private userlib::helperclass::NonCopyable
	{
	public:
		server();
		~server();
		int start(const net::ip::address addr, const unsigned short port,const std::string& rootdir);
	private:
		template<class Body, class Allocator>
		http::message_generator	generate_response
		(
					beast::string_view rootdir,
					http::request<Body, http::basic_fields<Allocator>>&& req
		);
		beast::string_view mime_type(beast::string_view);

		std::string path_cat(beast::string_view base, beast::string_view path);
		void do_session(tcp::socket& sock, std::shared_ptr<const std::string>& rootdir);
		void fail(beast::error_code ec, const char* what);
		net::ip::address m_addr;
		unsigned short m_port;
		std::shared_ptr<const std::string> m_rootdir;
		net::io_context ioc;
	};

}


#endif