#ifdef _WIN32
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _WIN32_WINNT 0x0A00
#endif

#include "TCPServer.h"
#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include "CallbackQueue.h"

namespace {
	const std::string delimiter = "\r\n";
}

class TCPServerImpl {

public:
	TCPServerImpl(unsigned int portNr, const std::function<void(const std::string&)>& receiveCallback, CallbackQueue& txQueue) :
		m_TxQueue(txQueue),
		m_IOService(),
		m_Acceptor(m_IOService, ::boost::asio::ip::tcp::endpoint(::boost::asio::ip::tcp::v4(), portNr)),
		m_Socket(m_IOService),
		m_RxBuffer(),
		m_TxBuffer(),
		m_ReceiveCallback(receiveCallback)
	{
		m_TxQueue.InstallCallback(std::bind(&TCPServerImpl::SendMsg, this));

		m_Acceptor.listen();
		m_Acceptor.async_accept(m_Socket, std::bind(&TCPServerImpl::AcceptHandler, this, std::placeholders::_1));
		m_IOService.run();
	}

	~TCPServerImpl()
	{
		m_Socket.close();
		m_IOService.stop();
	}


	void SendMsg()
	{
		if (m_Socket.is_open())
		{
			if (auto msgToSend = this->m_TxQueue.Pop())
			{
				m_TxBuffer.consume(m_TxBuffer.size());
				std::ostream output(&m_TxBuffer);
				output << *msgToSend << delimiter;

				boost::asio::async_write(m_Socket, m_TxBuffer, std::bind(&TCPServerImpl::WriteHandler, this, std::placeholders::_1, std::placeholders::_2));
		
			}
		}
	}

private:


	void WriteHandler(const boost::system::error_code &ec, std::size_t bytes_transferred)
	{
		if (!ec)
		{
			//std::this_thread::sleep_for(std::chrono::seconds(1));
			//successfully transferd -> maybe still messages to send? just run send again
			SendMsg();
		}
	}

	void AcceptHandler(const boost::system::error_code &ec)
	{
		if (!ec)
		{
			boost::asio::async_read_until(m_Socket, m_RxBuffer, delimiter, std::bind(&TCPServerImpl::ReadHandler, this, std::placeholders::_1, std::placeholders::_2));
		}
	}

	void ReadHandler(const boost::system::error_code& ec, size_t bytes_transferred)
	{
		if (!ec)
		{
			std::string message;
			std::copy_n(boost::asio::buffers_begin(m_RxBuffer.data()), bytes_transferred, back_inserter(message));

			m_ReceiveCallback(message);

			//prepare fo next "line/message"
			m_RxBuffer.consume(bytes_transferred);
			boost::asio::async_read_until(m_Socket, m_RxBuffer, delimiter, std::bind(&TCPServerImpl::ReadHandler, this, std::placeholders::_1, std::placeholders::_2));
		}
	}

	CallbackQueue&	                        m_TxQueue;
	boost::asio::io_service                 m_IOService;
	boost::asio::ip::tcp::acceptor          m_Acceptor;
	boost::asio::ip::tcp::socket            m_Socket;
	boost::asio::streambuf                  m_RxBuffer;
	boost::asio::streambuf                  m_TxBuffer;
	std::function<void(const std::string&)> m_ReceiveCallback;
};



TCPServer::TCPServer(unsigned int portNr, const std::function<void(const std::string&)>& receiveCallback, class CallbackQueue& txQueue) :
	m_Stop(false),
	m_SessionHandler([this, portNr, receiveCallback, &txQueue]() {
		while (!m_Stop)
		{
			try {
				TCPServerImpl tcpServer(portNr, receiveCallback, txQueue);
			}
			catch (...) {
				;
			}
		}
	})
{
}

TCPServer::~TCPServer()
{
	m_Stop = true;
	if (m_SessionHandler.joinable())
		m_SessionHandler.join();

}


