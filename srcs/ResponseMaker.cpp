#include "ResponseMaker.hpp"
#include "Socket.hpp"

// ResponseMaker::ResponseMaker() {}

ResponseMaker::ResponseMaker(const ResponseMaker &res)
: request(res.request), server(res.server), location(res.location),
	responseHeader(res.responseHeader), mimeType(res.mimeType)
{}

ResponseMaker::ResponseMaker(const Request &request, const Server &server, const Location &location)
: request(request), server(server), location(location)
{
	this->mimeType[".aac"] = "audio/aac";
	this->mimeType[".abw"] = "application/x-abiword";
	this->mimeType[".arc"] = "application/octet-stream";
	this->mimeType[".avi"] = "video/x-msvideo";
	this->mimeType[".azw"] = "application/vnd.amazon.ebook";
	this->mimeType[".bin"] = "application/octet-stream";
	this->mimeType[".bz"] = "application/x-bzip";
	this->mimeType[".bz2"] = "application/x-bzip2";
	this->mimeType[".csh"] = "application/x-csh";
	this->mimeType[".css"] = "text/css";
	this->mimeType[".csv"] = "text/csv";
	this->mimeType[".doc"] = "application/msword";
	this->mimeType[".epub"] = "application/epub+zip";
	this->mimeType[".gif"] = "image/gif";
	this->mimeType[".htm"] = "text/html";
	this->mimeType[".html"] = "text/html";
	this->mimeType[".ico"] = "image/x-icon";
	this->mimeType[".ics"] = "text/calendar";
	this->mimeType[".jar"] = "Temporary Redirect";
	this->mimeType[".jpeg"] = "image/jpeg";
	this->mimeType[".jpg"] = "image/jpeg";
	this->mimeType[".js"] = "application/js";
	this->mimeType[".json"] = "application/json";
	this->mimeType[".mid"] = "audio/midi";
	this->mimeType[".midi"] = "audio/midi";
	this->mimeType[".mpeg"] = "video/mpeg";
	this->mimeType[".mpkg"] = "application/vnd.apple.installer+xml";
	this->mimeType[".odp"] = "application/vnd.oasis.opendocument.presentation";
	this->mimeType[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	this->mimeType[".odt"] = "application/vnd.oasis.opendocument.text";
	this->mimeType[".oga"] = "audio/ogg";
	this->mimeType[".ogv"] = "video/ogg";
	this->mimeType[".ogx"] = "application/ogg";
	this->mimeType[".pdf"] = "application/pdf";
	this->mimeType[".ppt"] = "application/vnd.ms-powerpoint";
	this->mimeType[".rar"] = "application/x-rar-compressed";
	this->mimeType[".rtf"] = "application/rtf";
	this->mimeType[".sh"] = "application/x-sh";
	this->mimeType[".svg"] = "image/svg+xml";
	this->mimeType[".swf"] = "application/x-shockwave-flash";
	this->mimeType[".tar"] = "application/x-tar";
	this->mimeType[".tif"] = "image/tiff";
	this->mimeType[".tiff"] = "image/tiff";
	this->mimeType[".ttf"] = "application/x-font-ttf";
	this->mimeType[".vsd"] = " application/vnd.visio";
	this->mimeType[".wav"] = "audio/x-wav";
	this->mimeType[".weba"] = "audio/webm";
	this->mimeType[".webm"] = "video/webm";
	this->mimeType[".webp"] = "image/webp";
	this->mimeType[".woff"] = "application/x-font-woff";
	this->mimeType[".xhtml"] = "application/xhtml+xml";
	this->mimeType[".xls"] = "application/vnd.ms-excel";
	this->mimeType[".xml"] = "application/xml";
	this->mimeType[".xul"] = "application/vnd.mozilla.xul+xml";
	this->mimeType[".zip"] = "application/zip";
	this->mimeType[".3gp"] = "video/3gpp audio/3gpp";
	this->mimeType[".3g2"] = "video/3gpp2 audio/3gpp2";
	this->mimeType[".7z"] = "application/x-7z-compressed";
};

ResponseMaker &ResponseMaker::operator=(const ResponseMaker &res)
{
	if (this != &res)
	{
		this->request = res.request;
		this->server = res.server;
		this->location = res.location;
		this->responseHeader = res.responseHeader;
		this->mimeType = res.mimeType;
	}
	return *this;
}

ResponseMaker::~ResponseMaker()
{}

/*
* ?????? ????????? ?????? ?????? ?????? ????????? ???????????????.
* @param ????????? ????????? ????????? ???, ????????? ??????
*/
void ResponseMaker::addResponseHeader(std::string key, std::string value)
{
	this->responseHeader[key] = value;
}

/*
* ???????????? ????????? ?????? ???????????????.
* @param ????????? tm ?????????
* @return ????????? char *
*/
std::string ResponseMaker::getFormatTime(const struct tm* timeinfo)
{
	char buffer[80];

	strftime(buffer, 80, "%a, %d, %b %Y %X GMT", timeinfo);
	return (std::string(buffer));
}

/*
* Date ????????? ???????????????.
*/
void ResponseMaker::addDateHeader(void)
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	addResponseHeader("Date", getFormatTime(timeinfo));
}

/*
* ?????? ????????? ???????????????. 'FDB'
*/
void ResponseMaker::addServerHeader(void)
{
	addResponseHeader("Server", "FDB");
}

/*
* ????????? ???????????? ???????????? mime-type??? content-type ????????? ??????????????? ??????
* @param ????????? ?????? ????????? ??????
*/
void ResponseMaker::addContentTypeHeader(std::string extension)
{
	if (mimeType.count(extension) == 0)
		addResponseHeader("Content-Type", "text/plain");
	else
		addResponseHeader("Content-Type", mimeType[extension]);
}

/*
* contentLength ????????? ??????????????????.
* @param ?????? value??? ??? ???
*/
void ResponseMaker::addContentLengthHeader(int length)
{
	char *lenStr = ft_itoa(length);
	addResponseHeader("Content-Length", static_cast<std::string>(lenStr));
	free(lenStr);
}

/*
* HTML ???????????? ????????????.
* @param HTML ?????? ????????? string
* @return HTML ?????? string
*/
std::string ResponseMaker::makeHTMLPage(std::string str)
{
	std::string body;

	body += "<!DOCTYPE html>\n";
	body += "<html>\n";
	body += "<head>\n";
	body += "</head>\n";
	body += "<body>\n";
	body += "<h1>";
	body += str;
	body += "<h1>\n";
	body += "</body>\n";
	body += "</html>";
	return (body);
}

/*
* ?????? Response??? ????????? ???????????????.
* @param http ????????????
* @param http ?????? (default HTTP 1.1)
* @return ?????? Response ?????????
*/
Response ResponseMaker::makeErrorResponse(int httpStatus, std::string version)
{
	char	*strTmp = ft_itoa(httpStatus);
	std::string body = makeHTMLPage(strTmp);
	free(strTmp);
	addDateHeader();
	addServerHeader();
	addContentTypeHeader(".html");
	addContentLengthHeader((int)body.length());
	switch (httpStatus)
	{
		case NOT_FOUND:
			return Response(404, responseHeader, body, version);
		case SERVER_ERR:
			return Response(500, responseHeader, body, version);
		case FORBIDDEN:
			return Response(403, responseHeader, body, version);
		case METHOD_NOT_ALLOWED:
			return Response(405, responseHeader, body, version);
		case 413:
			return Response(413, responseHeader, body, version);
		default:
			return Response(httpStatus, responseHeader, body, version);
	}
}


// ====================== Response ====================

/*
 * ????????? ??????????????? / ???????????? / ???????????? ???????????????.
 * @param ????????? ??????
 * @return ????????? ???????????? ?????? ?????? 0 ??????, ????????? ?????? 1 ??????, ????????????????????? 2 ??????
 */
int ResponseMaker::checkPath(std::string path)
{
	struct stat buffer;

	int exist = stat(path.c_str(), &buffer);
	if (exist == 0)
	{
		if (S_ISREG(buffer.st_mode))
			return (ISFILE);
		else if (S_ISDIR(buffer.st_mode))
			return (ISDIR);
	}
	return (NOT_FOUND);
}

void	ResponseMaker::updateErrorStatus(int clientFd, int statusCode)
{
	Socket::getInstance()->getPool()[clientFd]->setStatus(PROCESSING_ERROR);
	(*dynamic_cast<Client *>(Socket::getInstance()->getPool()[clientFd])).getResponse().setStatusCode(statusCode);
	Socket::getInstance()->updateFds(clientFd, FD_WRITE);
	return ;
}
