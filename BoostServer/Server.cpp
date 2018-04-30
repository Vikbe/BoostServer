#include "Server.h"
#include "Repository.h" 


namespace srv {

	Server::Server()
		: m_ioservice(), m_acceptor(m_ioservice), m_connections()
	{}

	void Server::handle_read(con_handle_t con_handle, boost::system::error_code const & err, size_t bytes_transfered)
	{

		if (bytes_transfered > 0 && !err)
		{
			// Read body
			size_t rc = boost::asio::read(con_handle->socket, con_handle->read_buffer.prepare(con_handle->header));
			con_handle->read_buffer.commit(con_handle->header);

			// Deserialize
			std::istream is(&con_handle->read_buffer);
			boost::archive::text_iarchive ar(is);
			SearchModel search;
			ar & search;

			QueryData result = repo.Search(search);
			do_write(con_handle, result);
		}
		else
		{
			std::cerr << "We had an error: " << err.message() << std::endl;
			m_connections.erase(con_handle);
		}
	}

	void Server::do_read(con_handle_t con_handle)
	{
		auto handler = boost::bind(&Server::handle_read, this, con_handle, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);

		boost::asio::async_read(con_handle->socket, boost::asio::buffer(&con_handle->header, sizeof(con_handle->header)), handler);

		std::cout << "Incoming Body is " << con_handle->header << " bytes " << std::endl;
	}

	void Server::do_write(con_handle_t con_handle, QueryData result)
	{
		// Serialize
		std::ostream os(&databuf);
		boost::archive::text_oarchive ar(os);
		ar & result;

		header = databuf.size();
		std::cout << "buffer size " << header << " bytes" << std::endl;

		// Send data using scatter

		buffers.push_back(boost::asio::buffer(&header, sizeof(header)));
		buffers.push_back(databuf.data());

		boost::asio::async_write(con_handle->socket, buffers,
			boost::bind(&Server::handle_write, this, con_handle, boost::asio::placeholders::error));
	}

	void Server::handle_write(con_handle_t con_handle, boost::system::error_code const & err)
	{
		if (!err)
		{
			std::cout << "Finished sending data\n";
			if (con_handle->socket.is_open()) {

				databuf.consume(header);
				buffers.clear();
				// Write completed successfully and connection is open
			}
		}
		else
		{
			std::cerr << "We had an error: " << err.message() << std::endl;
			m_connections.erase(con_handle);
		}
	}

	void Server::handle_accept(con_handle_t con_handle, boost::system::error_code const & err)
	{
		if (!err)
		{
			std::cout << "Connection from: " << con_handle->socket.remote_endpoint() << "\n";

			do_read(con_handle);
		}
		else
		{
			std::cerr << "We had an error: " << err.message() << std::endl;
			m_connections.erase(con_handle);
		}

		start_accept();
	}

	void Server::start_accept()
	{
		auto con_handle = m_connections.emplace(m_connections.begin(), m_ioservice);

		auto handler = boost::bind(&Server::handle_accept, this, con_handle, boost::asio::placeholders::error);

		m_acceptor.async_accept(con_handle->socket, handler);
	}

	void Server::listen(uint16_t port)
	{
		auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
		m_acceptor.open(endpoint.protocol());
		m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		m_acceptor.bind(endpoint);
		m_acceptor.listen();

		start_accept();
	}

	void srv::Server::run()
	{
		m_ioservice.run();
	}

}