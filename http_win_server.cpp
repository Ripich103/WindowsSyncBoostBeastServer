#include"http_win_server.h"
namespace rhttp {

	server::server() : m_addr(), m_port(), m_rootdir(nullptr), ioc()
	{
	}

	server::~server()
	{
	}

	template<class Body, class Allocator>
	http::message_generator server::generate_response
	(
			beast::string_view doc_root,
			http::request<Body, http::basic_fields<Allocator>>&& req
	)
	{
		auto const bad_request =
			[&req](beast::string_view why)
			{
				http::response<http::string_body> res{ http::status::bad_request, req.version() };
				res.set(http::field::server, _BOOST_BEAST_VERSION_STRING);
				res.set(http::field::content_type, "text/html");
				res.keep_alive(req.keep_alive());
				res.body() = std::string(why);
				res.prepare_payload();
				return res;
			};

		auto const not_found =
			[&req](beast::string_view target)
			{
				http::response<http::string_body> res{ http::status::not_found, req.version() };
				res.set(http::field::server, _BOOST_BEAST_VERSION_STRING);
				res.set(http::field::content_type, "text/html");
				res.keep_alive(req.keep_alive());
				res.body() = "The resource " + std::string(target) + " was not found.";
				res.prepare_payload();
				return res;
			};

		auto const server_error =
			[&req](beast::string_view what)
			{
				http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
				res.set(http::field::server, _BOOST_BEAST_VERSION_STRING);
				res.set(http::field::content_type, "text/html");
				res.keep_alive(req.keep_alive());
				res.body() = "An error occurred: '" + std::string(what) + "'";
				res.prepare_payload();
				return res;
			};

		if (req.method() != http::verb::get &&
			req.method() != http::verb::head)
			return bad_request("Unknown HTTP-method");

		if (req.target().empty() ||
			req.target()[0] != '/' ||
			req.target().find("..") != beast::string_view::npos)
			return bad_request("Illegal request-target");

		std::string path = path_cat(doc_root, req.target());
		if (req.target().back() == '/')
			path.append("index.html");

		beast::error_code ec;
		http::file_body::value_type body;
		body.open(path.c_str(), beast::file_mode::read, ec);
		if (ec == beast::errc::no_such_file_or_directory)
			return not_found(req.target());
		if (ec)
			return server_error(ec.message());

		const uint64_t size = body.size();
		if (req.method() == http::verb::head)
		{
			http::response<http::empty_body> res{ http::status::ok, req.version() };
			res.set(http::field::server, _BOOST_BEAST_VERSION_STRING);
			res.set(http::field::content_type, mime_type(path));
			res.content_length(size);
			res.keep_alive(req.keep_alive());
			return res;
		}

		http::response<http::file_body> res{
		std::piecewise_construct,
		std::make_tuple(std::move(body)),
		std::make_tuple(http::status::ok, req.version()) };
		res.set(http::field::server, _BOOST_BEAST_VERSION_STRING);
		res.set(http::field::content_type, mime_type(path));
		res.content_length(size);
		res.keep_alive(req.keep_alive());
		return res;
	}
	void server::fail(beast::error_code ec, const char* what)
	{
		std::cerr << what << " : " << ec.message() << '\n';
	}

	beast::string_view server::mime_type(beast::string_view path)
	{
		using beast::iequals;
		const auto ext = [&path]
			{
				const auto pos = path.rfind(".");
				if (pos == beast::string_view::npos)
					return beast::string_view{};
				return path.substr(pos);
			}();
		if (iequals(ext, ".htm"))  return "text/html";
		if (iequals(ext, ".html")) return "text/html";
		if (iequals(ext, ".php"))  return "text/html";
		if (iequals(ext, ".css"))  return "text/css";
		if (iequals(ext, ".txt"))  return "text/plain";
		if (iequals(ext, ".js"))   return "application/javascript";
		if (iequals(ext, ".json")) return "application/json";
		if (iequals(ext, ".xml"))  return "application/xml";
		if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
		if (iequals(ext, ".flv"))  return "video/x-flv";
		if (iequals(ext, ".png"))  return "image/png";
		if (iequals(ext, ".jpe"))  return "image/jpeg";
		if (iequals(ext, ".jpeg")) return "image/jpeg";
		if (iequals(ext, ".jpg"))  return "image/jpeg";
		if (iequals(ext, ".gif"))  return "image/gif";
		if (iequals(ext, ".bmp"))  return "image/bmp";
		if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
		if (iequals(ext, ".tiff")) return "image/tiff";
		if (iequals(ext, ".tif"))  return "image/tiff";
		if (iequals(ext, ".svg"))  return "image/svg+xml";
		if (iequals(ext, ".svgz")) return "image/svg+xml";
		return "application/text";
	}

	std::string server::path_cat(beast::string_view base, beast::string_view path)
	{
		if (base.empty())
			return std::string(path);
		std::string res(base);
		constexpr char path_sep = '\\';
		if (res.back() == path_sep)
			res.pop_back();
		res.append(path.data(), path.size());
		for (auto& c : res)
		{
			if (c == '/')
				c = path_sep;
		}
		return res;
	}

	void server::do_session(tcp::socket& sock, std::shared_ptr<const std::string>& rootdir)
	{
		beast::flat_buffer buff;
		beast::error_code ec;
		const auto remote_endpoint = sock.remote_endpoint();
		const std::string client_ip = remote_endpoint.address().to_string();
		const uint16_t client_port = remote_endpoint.port();

		std::cout << "Client connected: " << client_ip << ":" << client_port << "\n";
		for (;;)
		{
			http::request<http::string_body> req;
			http::read(sock, buff, req, ec);
			if (ec == http::error::end_of_stream)
				break;
			if (ec)
				return fail(ec, "read");

			http::message_generator msg = generate_response(*rootdir, std::move(req));
			bool keep_alive = msg.keep_alive();
			beast::write(sock, std::move(msg), ec);
			if (ec)
				return fail(ec, "write");
			if (!keep_alive)
				break;
		}
		std::cout << "disconnecting!\n";

		sock.shutdown(tcp::socket::shutdown_send, ec);
	}

	int server::start(const net::ip::address addr, const unsigned short port, const std::string& rootdir)
	{
		try {
			m_addr = addr;
			m_port = port;
			m_rootdir = std::make_shared<const std::string>(rootdir);
			tcp::acceptor acceptor{ ioc, {m_addr, m_port} };

			for (;;)
			{
				tcp::socket sock{ ioc };
				acceptor.accept(sock);

				std::thread tr{ std::bind(&server::do_session, this, std::move(sock), m_rootdir)};
				tr.detach();
			}
		}
		catch (const std::exception& ex)
		{
			std::cerr << "Error what : " << ex.what() << '\n';
			return EXIT_FAILURE;
		}
		return 0;
	}

}
