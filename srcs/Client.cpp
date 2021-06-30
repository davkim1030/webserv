#include "webserv.h"
#include "Client.hpp"
#include "Server.hpp"
#include "Socket.hpp"

/*
 * 클라이언트 클래스 기본 생성자
 * fd 값들을 -1로 지정하여 초기화 하지 않으면 사용 못 하게 함
 */
Client::Client() : IoObject(CLIENT), serverSocketFd(-1),
		request(""), response(200, std::map<std::string, std::string>(), ""), pos(0),
		bodyLen(0), chunkedFlag(LEN)
{
	lastReqMs = ft_get_time();
}

/*
 * 복사 생성자
 * @param const Client &other: 복사할 객체
 */
Client::Client(const Client &other)
	: IoObject(other),
	serverSocketFd(other.serverSocketFd), request(other.request),
	response(other.response), pos(other.pos),
	lastReqMs(other.lastReqMs), bodyLen(other.bodyLen),
	chunkedFlag(other.chunkedFlag)
{
}

/*
 * 멤버 값들을 명시적으로 지정해주는 생성자
 * @param: int serverSocketFd: 클라이언트가 연결된 서버의 소켓 fd
 * @param: int socketFd: 클라이언트에 할당된 소켓 fd
 */
Client::Client(int serverSocketFd, int fd)
	: IoObject(fd, "", REQUEST_RECEIVING_HEADER, CLIENT),
	serverSocketFd(serverSocketFd),
	request(""), response(200, std::map<std::string, std::string>(), ""), pos(0),
	bodyLen(0), chunkedFlag(LEN)
{
	lastReqMs = ft_get_time();
}

Client::~Client()
{
}

/*
 * 할당 연산자 오버로딩
 */
Client &Client::operator=(const Client &other)
{
	if (this != &other)
	{
		status = other.status;
		serverSocketFd = other.serverSocketFd;
		fd = other.fd;
		readBuffer = other.readBuffer;
		status = other.status;
		request = other.request;
		response = other.response;
		pos = other.pos;
		lastReqMs = other.lastReqMs;
		bodyLen = other.bodyLen;
		chunkedFlag = other.chunkedFlag;
	}
	return *this;
}

void	Client::setServerSocketFd(int serverSocketFd)
{
	this->serverSocketFd = serverSocketFd;
}

void	Client::setStatus(Status status)
{
	this->status = status;
}

void	Client::setPos(size_t pos)
{
	this->pos = pos;
}

void	Client::setLastReqMs(unsigned long lastReqMs)
{
	this->lastReqMs = lastReqMs;
}

void	Client::setResponse(const Response &response)
{
	this->response = response;
}

void	Client::setBodyLen(size_t chunkedIndex)
{
	this->bodyLen = chunkedIndex;
}

void	Client::setChunkedFlag(ChunkedFlag chunkedFlag)
{
	this->chunkedFlag = chunkedFlag;
}

// getter

Response	&Client::getResponse()
{
	return (response);
}

Request		&Client::getRequest()
{
	return (request);
}

Status	Client::getStatus()
{
	return (status);
}

size_t	Client::getPos()
{
	return (pos);
}

unsigned long	Client::getLastReqMs()
{
	return (lastReqMs);
}

int		Client::getServerSocketFd()
{
	return (serverSocketFd);
}

size_t		Client::getBodyLen()
{
	return (bodyLen);
}

ChunkedFlag	Client::getChunkedFlag()
{
	return (chunkedFlag);
}


/*
 * 현재 buffer 데이터로 HTTP Request Message의 header 부분이 추출 가능한지 확인
 * @return bool: 헤더가 다 들어왔는지
 */
bool	Client::headerParsable()
{
	return (readBuffer.find("\r\n\r\n") != std::string::npos);
}
IoObject *Client::clone()
{
	IoObject *tmp = new Client(*this);
	return tmp;
}

Server Client::getServer()
{
	return *dynamic_cast<Server *>(Socket::getInstance()->getPool()[serverSocketFd]);
}

Status		Client::readRequestMessage(std::vector<IoObject *>	&pool, int currentFd)
{
	char buf[IO_BUFFER_SIZE];
	ft_memset(buf, '\0', IO_BUFFER_SIZE + 1);
	int readLen = read(currentFd, buf, IO_BUFFER_SIZE);
	if (readLen <= 0)
		{
			if (readLen == 0)
			{
				std::cout << "test" << std::endl;
				return DISCONNECTED;
			}
			else
			{
				std::cerr << "Read failed " << this->fd << " client!!" << std::endl;
			}
		}
		buf[readLen] = '\0';

		// 1 이상이면 읽기에 성공한 것이니 읽어온 버퍼 저장
		this->readBuffer = this->readBuffer + buf;
		
		// header && body 파싱 가능한지 보고 파싱해주기
		if (this->status == REQUEST_RECEIVING_HEADER && this->headerParsable())
		{
			this->readBuffer = this->request.parseFirstLine(this->readBuffer);
			std::map<std::string, std::string> requestHeader = Request::parseHeader(this->readBuffer);
			this->request.setHeader(requestHeader);
			Location location = findLocation(*dynamic_cast<Server *>(pool[this->serverSocketFd]),
								this->request.getUri());
			this->request.setLocation(location);
			this->readBuffer = this->readBuffer.substr(this->readBuffer.find("\r\n\r\n") + 4);
			this->status = REQUEST_RECEIVING_BODY;
		}

		if (this->status == REQUEST_RECEIVING_BODY)
		{
			if (this->request.getHeader().count("Transfer-Encoding") == 1 &&
				this->request.getHeader()["Transfer-Encoding"] == "chunked")
			{
				size_t carrageIdx;
				carrageIdx = this->readBuffer.find("\r\n");
				while (true)
				{
					if (carrageIdx == std::string::npos)
						return REQUEST_RECEIVING_BODY;					
					if (this->chunkedFlag == LEN)
					{
						int chunkedLen = ft_hex_atoi(this->readBuffer.substr(0, carrageIdx));
						this->bodyLen = chunkedLen;
						if (chunkedLen == 0)
						{
							if (this->readBuffer.length() >= carrageIdx + 4)
								this->readBuffer = this->readBuffer.substr(carrageIdx + 4);
							else
								this->readBuffer.clear();
							return RESPONSE_READY;
						}
						if (this->readBuffer.length() >= carrageIdx + 2)
						{
							this->readBuffer = this->readBuffer.substr(carrageIdx + 2);
							this->chunkedFlag = BODY;
						}
					}
					else if (this->chunkedFlag == BODY)
					{
						if (this->readBuffer.length() >= this->bodyLen + 2)
						{
							this->writeBuffer += this->readBuffer.substr(0, this->bodyLen);
							this->chunkedFlag = LEN;
							this->readBuffer = this->readBuffer.substr(this->bodyLen + 2);
						}
						else
							return REQUEST_RECEIVING_BODY;
					}
					carrageIdx = this->readBuffer.find("\r\n");
				}
			}
			else if (this->request.getHeader().count("Content-Length") == 1)
			{
				int contentLength = ft_atoi(this->request.getHeader()["Content-Length"].c_str());
				if (this->readBuffer.length() >= (size_t)contentLength)
				{
					this->writeBuffer = this->readBuffer.substr(0, contentLength);
					if (this->readBuffer.length() >= (size_t)contentLength + 4)
						this->readBuffer = this->readBuffer.substr(contentLength + 4);
					else
						this->readBuffer.clear();
					return RESPONSE_READY;
				}
				else
					return REQUEST_RECEIVING_BODY;
			}
			else if (this->request.getHeader().count("Content-Length") == 0)
			{
				this->request.getHeader()["Content-Length"] = "0";
				this->readBuffer.clear();
				return RESPONSE_READY;
			}
		}

	return this->status;

}


/*
 * request의 uri와 매칭되는 로케이션 반환
 */
Location Client::findLocation(Server server, std::string uri)
{
	std::vector<Location> ser = server.getLocationVector();
	Location res;

	for (std::vector<Location>::iterator it = ser.begin(); it != ser.end(); it++)
	{
		std::string path = it->getPath();
		std::string tempPath = path + '/';
		if (uri.compare(0, path.length(), path) == 0 || uri.compare(0, tempPath.length(), tempPath) == 0)
			res = *it;
	}
	return res;
}
