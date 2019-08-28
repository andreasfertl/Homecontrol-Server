#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING
#endif

#include "TCPHandler.h"
#include <memory>
#include <list>
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/bind.hpp>
#include "iNetworkReceive.h"
#include "seriealizeFunctions.h"
#include "queue.h"


namespace {
	const std::string delimiter = "\r\n";
	typedef unsigned int connectionHandle;
	const unsigned int   connectionHandleMAXValue(UINT_MAX - 1);
}

class connection {

	struct sendData
	{
		std::list<std::future<void>>   m_SendOperations;
		seriealizeFunctions            m_SendHandler;
	};

public:
	connection(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::yield_context& yield, boost::asio::io_service& ioService, const iNetworkReceive& receiveCallback, const std::function<void()>& disconnectCallback) :
		m_Socket(ioService),
		m_RxBuffer(),
		m_ReceiveCallback(receiveCallback),
		m_DisconnectCallback(disconnectCallback)
	{
		acceptor.async_accept(m_Socket, yield);
		if (m_Socket.is_open())
			boost::asio::async_read_until(m_Socket, m_RxBuffer, delimiter, std::bind(&connection::readHandler, this, std::placeholders::_1, std::placeholders::_2));
	}

	connection(const connection&) = delete;
	connection& operator = (const connection&) = delete;
	connection(connection&&) = default;
	connection& operator = (connection&&) = default;

	~connection() {
		//wait for writes to be finished, destructor of m_SendHandler ensures that all posted operations are done
		m_SendData.m_SendHandler.run([this]() {
			//ensure that previous operations are already handled
			while (m_SendData.m_SendOperations.size() != 0) {
				auto& previousOpertion = m_SendData.m_SendOperations.front();
				previousOpertion.wait();
				m_SendData.m_SendOperations.pop_front();
			}
			});
	}

private:
	void sendMessage(const std::string& message) {
		//reducing to just one copy with a shared ptr
		auto messageToSend = std::make_shared<std::string>(message + delimiter);

		//sending asynchronos with my own infrastructure with private data, to avoid a shared TXBuffer as a class member (as RX is using)
		m_SendData.m_SendHandler.run([this, messageToSend]() {
			//add the next sending operation asynchronosly
			m_SendData.m_SendOperations.emplace_back(std::async(std::launch::async, [this, messageToSend] {
				if (m_Socket.is_open())
					boost::asio::write(m_Socket, boost::asio::buffer(*messageToSend));
				}));

			//ensure that previous operations are already handled
			while (!m_SendData.m_SendOperations.empty()) {
				auto& previousOpertion = m_SendData.m_SendOperations.front();
				if (previousOpertion.valid() && previousOpertion.wait_for(std::chrono::microseconds(1)) == std::future_status::ready) //ok ready to remove it
					m_SendData.m_SendOperations.pop_front();
				else //not ready yet, take it next time
					break;
			}

			});
	}

	void readHandler(const boost::system::error_code & ec, size_t bytes_transferred) {
		if (!ec) {
			std::string message;
			std::copy_n(boost::asio::buffers_begin(m_RxBuffer.data()), bytes_transferred, back_inserter(message));

			m_ReceiveCallback.receive({ message, std::bind(&connection::sendMessage, this, std::placeholders::_1) });

			//prepare fo next "line/message"
			m_RxBuffer.consume(bytes_transferred);
			if (m_Socket.is_open())
				boost::asio::async_read_until(m_Socket, m_RxBuffer, delimiter, std::bind(&connection::readHandler, this, std::placeholders::_1, std::placeholders::_2));
		}
		else if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset) {
			m_DisconnectCallback();
		}
	}

	boost::asio::ip::tcp::socket   m_Socket;
	boost::asio::streambuf         m_RxBuffer;
	const iNetworkReceive& m_ReceiveCallback;
	sendData                       m_SendData;
	const std::function<void()>    m_DisconnectCallback;
};

class TCPHandlerImpl {

public:
	TCPHandlerImpl(unsigned int portNr, const iNetworkReceive& receiveCallback) :
		m_StopFlag(false),
		m_IOService(),
		m_Acceptor(m_IOService, ::boost::asio::ip::tcp::endpoint(::boost::asio::ip::tcp::v4(), portNr)),
		m_ReceiveCallback(receiveCallback),
		m_Connections(),
		m_Handler([this]() { this->m_IOService.run(); })
	{
		m_Acceptor.listen();
		spawn(m_IOService, boost::bind(&TCPHandlerImpl::accept, this, boost::placeholders::_1));
	}

	~TCPHandlerImpl()
	{
		m_StopFlag = true;
		m_IOService.stop();
		if (m_Handler.joinable())
			m_Handler.join();
	}

	void disconnect(connectionHandle handle) {
		m_DisconnectedConnections.Push(handle);
	}

	void cleanupDisconnectedSessions() {
		while (auto disconnectedSession = m_DisconnectedConnections.Pop())
			if (m_Connections.count(*disconnectedSession) > 0)
				m_Connections.erase(*disconnectedSession);
	}

	void accept(boost::asio::yield_context yield) {
		connectionHandle handle(1);

		while (!m_StopFlag) {
			bool inserted(false);

			cleanupDisconnectedSessions();

			//do we handle already to many sessions?
			while (m_Connections.size() >= connectionHandleMAXValue) {
				m_IOService.post(yield);
				cleanupDisconnectedSessions(); //kind of wait for next slot
			}

			//did we manage to insert/instanciate our next client?
			while (!inserted && m_Connections.size() < connectionHandleMAXValue)
			{
				//instance one client connection and wait for next
				inserted = m_Connections.try_emplace(handle, m_Acceptor, yield, m_IOService, m_ReceiveCallback, std::bind(&TCPHandlerImpl::disconnect, this, handle)).second;

				if (handle >= connectionHandleMAXValue)
					handle = 1; //restart to find next free handle
				else
					handle++;
			}

		}
	}

private:
	std::atomic_bool				m_StopFlag;
	boost::asio::io_service         m_IOService;
	boost::asio::ip::tcp::acceptor  m_Acceptor;
	const iNetworkReceive& m_ReceiveCallback;
	std::map<connectionHandle, connection>  m_Connections;
	Queue<connectionHandle>         m_DisconnectedConnections;
	std::thread						m_Handler;
};



TCPHandler::TCPHandler(unsigned int portNr, const iNetworkReceive & receiveCallback) :
	m_TCPHandlerImpl(std::make_unique<TCPHandlerImpl>(portNr, receiveCallback))
{
}

TCPHandler::~TCPHandler()
{
}


