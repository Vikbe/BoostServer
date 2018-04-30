#pragma once

#include <boost/asio.hpp> 
#include <boost/bind.hpp>
#include <cstdint> 
#include <iostream>
#include <list>
#include <memory> 
#include "Repository.h" 
#include "DataModel.h"

namespace srv {

	struct Connection
	{
		boost::asio::ip::tcp::socket socket;
		boost::asio::streambuf read_buffer; 
		size_t header;

		Connection(boost::asio::io_service & io_service) : socket(io_service), read_buffer() { }

	};

	class Server
	{
		// Create repository and table 
		repo::Repository repo;
		
		boost::asio::io_service m_ioservice;
		boost::asio::ip::tcp::acceptor m_acceptor;
		std::list<Connection> m_connections; 

		// Buffers for serializing and sending
		std::vector<boost::asio::const_buffer> buffers; 
		boost::asio::streambuf databuf; 
		size_t header;

		using con_handle_t = std::list<Connection>::iterator;

	public:

		Server();

			void handle_read(con_handle_t con_handle, boost::system::error_code const & err, size_t bytes_transfered);

			void do_read(con_handle_t con_handle);

			void do_write(con_handle_t con_handle, QueryData result);
			
			void handle_write(con_handle_t con_handle, boost::system::error_code const & err);

			void handle_accept(con_handle_t con_handle, boost::system::error_code const & err);

			void start_accept();

			void listen(uint16_t port);

			void run();
		
	};
}