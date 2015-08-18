#include "CGI/MultipartRequest.h"

namespace NSTU {
namespace CGI {

bool
MultipartRequest::_readField(char * & data, long & length, long & index, char * end)
{
	bool ret = true;
	size_t sz;
	char * line;
	char * temp;
	char * last;
	size_t lastSz;
	typedef list< char * > PointerListType;
	PointerListType content;

	last = 0;
	lastSz = 0;
   while(true) {
		line = _readLine(sz);
		if(sz == 0) {
			delete [] line;
			break;
		}
		temp = _dup(line);
		_trim(temp);
		if(strcmp(temp, _boundary) == 0 || strcmp(temp, end) == 0) {
			if(strcmp(temp, end) == 0)
				_atEnd = true;
			index += sz;
			delete [] temp;
			break;
		}
		else {
			if(last) {
				content.push_back(last);
				length += lastSz;
				index += lastSz;
			}
			last = line;
			lastSz = sz;
		}
		delete [] temp;
	}
	if(last) {
		if(lastSz >= 2) {
			line = new char[lastSz-1];
			memset(line, 0, lastSz-1);
			copy(last, last+(lastSz-2), line);
			delete [] last;
			content.push_back(line);
			length += lastSz-2;
			index += lastSz;
		}
	}

	data = new char[length+1];
	memset(data, 0, length+1);
	char * p = data;
	sz = 0;
	for(PointerListType::iterator ptr = content.begin(); ptr != content.end(); ptr++) {
		for(last = (*ptr); *last && sz < (size_t)length; p++, sz++, last++) {
			*p = *last;
		}
		delete [] (*ptr);
	}

	return ret;
}

void
MultipartRequest::_writeBytes(FILE * f, char * p, size_t sz)
{
   for(char * ptr = p; sz > 0; ptr++, sz--) {
	   fwrite(ptr, 1, 1, f);
	   fflush(f);
   }
}

bool
MultipartRequest::_badChar(char ch)
{
	switch(ch) {
		case '\\':
		case '/':
		case ':':
		case '&':
		case ';':
		case '|':
		case '>':
		case '<':
		case '=':
		case '?':
		case ',':
		case '{':
		case '}':
		case ']':
		case '[':
		case '%':
		case '~':
			return true;
	}
	return false;
}

char *
MultipartRequest::_computeLocalFileName(char * filename)
{
	size_t fnsz = strlen(filename);
	char * name = filename + (fnsz-1);
	while(name != filename && !_badChar(*name))
		name--;
	if(_badChar(*name) && name == (filename + (fnsz-1)))
		return 0;
	if(_badChar(*name))
		name++;
	size_t sz = strlen(name);
	if(sz == 0)
		return 0;
	char * p = name;
	while(*p) {
		if(*p == ' ')
			*p = '_';
		p++;
	}
	size_t dirLen = strlen(_dir);
	size_t retLen = dirLen+sz+1;
	char * ret = new char[retLen];
	memset(ret, 0, retLen);
	memcpy(ret, _dir, dirLen);
	memcpy(ret+dirLen, name, sz);
	return ret;
}

long
MultipartRequest::_writeFile(char * & filename, char * end, long & index)
{
	FILE * fout;
	long ret = 0;
	size_t sz;
	char * line;
	char * temp;
	char * last;
	size_t lastSz;
	filename = _computeLocalFileName(filename);

	if(!filename) {
		_errorCode = IO_ERROR;
		return -1;
	}

	fout = fopen(filename, "wb");
	if(!fout) {
		_errorCode = IO_ERROR;
		return -1;
	}

	last = 0;
	lastSz = 0;
   while(true) {
      line = _readLine(sz);
		if(sz == 0) {
			delete [] line;
			break;
		}
		temp = _dup(line);
		_trim(temp);
		if(strcmp(temp, _boundary) == 0 || strcmp(temp, end) == 0) {
			if(strcmp(temp, end) == 0)
				_atEnd = true;
			index += sz;
			delete [] temp;
			break;
		}
		else {
			if(last) {
				_writeBytes(fout, last, lastSz);
				ret += lastSz;
            if(ret > _max) {
					_errorCode = MAX_UPLOAD;
					delete [] temp;
					delete [] last;
					return -1;
				}
				index += lastSz;
				delete [] last;
			}
			last = line;
			lastSz = sz;
		}
		delete [] temp;
	}
	if(last) {
		if(lastSz >= 2) {
			_writeBytes(fout, last, lastSz-2);
			ret += lastSz-2;
			index += lastSz;
		}
		delete [] last;
	}

	fclose(fout);

	return ret;
}

char *
MultipartRequest::_dup(char * p)
{
	size_t sz = strlen(p);
	char * ret = new char[sz+1];
	memset(ret, 0, sz+1);
	copy(p, p+sz, ret);
	return ret;
}

char *
MultipartRequest::_extractContentType(char * p)
{
	while(*p && *p == ' ')
		p++;
	size_t sz = strlen(p);
	char * ret = new char[sz+1];
	memset(ret, 0, sz+1);
	copy(p, p+sz, ret);
	return ret;
}


char *
MultipartRequest::_processParam(char * p)
{
	list< char > buffer;
	if(*p == '\"')
		p++;
	while(*p != '\"' && *p) {
		buffer.push_back(*p);
		p++;
	}
	char * ret = new char[buffer.size()+1];
	memset(ret, 0, buffer.size()+1);
	copy(buffer.begin(), buffer.end(), ret);
	return ret;
}

void
MultipartRequest::_trim(char * s)
{
	size_t sz = strlen(s);
	if(sz > 0) {
		char * p = s + (sz - 1);
		size_t i = sz - 1;
		while(i >= 0 && (*p == '\n' || *p == '\r')) {
			*p = '\0';
			p--;
			i--;
		}
	}
}

char *
MultipartRequest::_readLine(size_t & sz)
{
	list< char > buffer;
	char ch;
   while(fread(&ch, sizeof(char), 1, stdin) == 1) {
      buffer.push_back(ch);
		if(ch == '\n')
			break;
	}
	char * ret = new char[buffer.size()+1];
	sz = buffer.size();
	memset(ret, 0, buffer.size()+1);
	copy(buffer.begin(), buffer.end(), ret);
	return ret;
}

char *
MultipartRequest::_readLine()
{
	list< char > buffer;
	char ch;
   while(fread(&ch, sizeof(char), 1, stdin) == 1) {
		buffer.push_back(ch);
		if(ch == '\n')
			break;
	}
	char * ret = new char[buffer.size()+1];
	memset(ret, 0, buffer.size()+1);
	copy(buffer.begin(), buffer.end(), ret);
	return ret;
}

void
MultipartRequest::_parseContent()
{
	char * line;
	char * line2;
	char * name;
	char * filename;
	char * localFileName;
	char * end;
	char * type;
	char * data;
	long length;
	long index = 0;
	FormElement * pElement;
	size_t bsz = strlen(_boundary);
	bool skipRead = false;

	end = new char[bsz+3];
	ArrayDeleter endPtr(end);
	memset(end, 0, bsz+3);
	sprintf(end, "%s--", _boundary);

	// Iterate through the content
	while(index < _contentLength && !_atEnd) {
		// Read a line
		if(!skipRead) {
			skipRead = false;
			line = _readLine();
			index += strlen(line);
			// Trim white space off the end of the line
			_trim(line);
		}
		else
			line = _dup(_boundary);

		ArrayDeleter pLine(line);

		// If we found the boundary process the current part
		if(strcmp(line, _boundary) == 0) {
			// Read Content-Disposition
			line2 = _readLine();
			ArrayDeleter pLine2(line2);
			index += strlen(line2);
			_trim(line2);

			// Get a pointer to the current parts name
			name = strstr(line2, "name=");
			if(!name) {
				_errorCode = PARSE_ERROR;
				return;
			}

			// Extract the name
			name = _processParam(name+5);
			ArrayDeleter pName(name);

			// Look for a file name for a file upload part
			filename = strstr(line2, "filename=");

			// If we got a filename write the file
			if(filename) {
				// Extract the filename
				filename = _processParam(filename+9);
				ArrayDeleter pFilename(filename);

				// Read the Content-Type:
				line2 = _readLine();
				pLine2.reset(line2);
				index += strlen(line2);
				_trim(line2);

            if(strstr(line2, "Content-Type:") != line2) {
					_errorCode = PARSE_ERROR;
					return;
				}

				type = _extractContentType(line2+13);
				ArrayDeleter pType(type);
				// Skip \r\n
				line2 = _readLine();
				pLine2.reset(line2);
				index += strlen(line2);
				ArrayDeleter pLocalFilename;

				if(strlen(filename) != 0) {
					localFileName = filename;
					length = _writeFile(localFileName, end, index);
					if(localFileName)
						pLocalFilename.reset(localFileName);
				}
				else {
					localFileName = _dup(filename);
					pLocalFilename.reset(localFileName);
					length = 0;
					if(!_readField(data, length, index, end)) {
						_errorCode = PARSE_ERROR;
						delete [] data;
						return;
					}
					delete [] data;
				}

				if(length == -1)
					return;

				pType.release();
				pFilename.release();
				pLocalFilename.release();
				pName.release();
				pElement = new FormElement();
				pElement->_name = name;
				pElement->_fileName = filename;
				pElement->_localFileName = localFileName;
				pElement->_type = type;
				pElement->_length = length;
				_elementVec.insert(_elementVec.end(), pElement);
				if(_atEnd)
					break;
				skipRead = true;
			}
			// Else handle the regular field
			else {
				line2 = _readLine();
				pLine2.reset(line2);
				index += strlen(line2);

				length = 0;
				if(!_readField(data, length, index, end)) {
					_errorCode = PARSE_ERROR;
					delete [] data;
					return;
				}

				pName.release();
				pElement = new FormElement();
				pElement->_name = name;
				pElement->_length = length;
				pElement->_data = data;
				_elementVec.insert(_elementVec.end(), pElement);
				if(_atEnd)
					break;
				skipRead = true;
			}
		}
		else
			index = _contentLength;
	} // end while

	for(int i = 0; i < (int)_elementVec.size(); i++) {
		pElement = _elementVec[i];
		_map[pElement->_name] = pElement;
	}
}

MultipartRequest::MultipartRequest(char * dir, long max)
:_dir(dir), _errorCode(MultipartRequest::MPR_NO_ERROR), _atEnd(false), _max(max)
{
	char * requestMethod = getenv("REQUEST_METHOD");
	if(!requestMethod) {
		_errorCode = CGI_ERROR;
		return;
	}
	if(strcmp(requestMethod, "POST") != 0) {
		_errorCode = CGI_ERROR;
		return;
	}
	char * contentType = getenv("CONTENT_TYPE");
	if(!contentType) {
		_errorCode = CGI_ERROR;
		return;
	}
	const char BOUNDARY[] = "boundary=";
	_boundary = strstr(contentType, BOUNDARY);
	if(_boundary == NULL) {
		_errorCode = CGI_ERROR;
		return;
	}
	_boundary += (sizeof(BOUNDARY) - 1);
	size_t sz = strlen(_boundary);
	char * temp = new char[sz+3];
	memset(temp, 0, sz+3);
	sprintf(temp, "--%s", _boundary);
	_boundary = temp;
	char * scontentLength = getenv("CONTENT_LENGTH");
	if(!scontentLength) {
		_errorCode = CGI_ERROR;
		return;
	}
	_contentLength = atol(scontentLength);
	if(_contentLength < 0) {
		_errorCode = CGI_ERROR;
		return;
	}
	if(_contentLength > _max) {
		_errorCode = MAX_UPLOAD;
		return;
	}
#ifdef MPR_WIN32
   if(_setmode(0, _O_BINARY) <= 0) {
      _errorCode = IO_ERROR;
      return;
   }
#endif

	_parseContent();
}

MultipartRequest::~MultipartRequest()
{
	if(_boundary)
		delete [] _boundary;
	for(int i = 0; i < (int)_elementVec.size(); i++)
		delete _elementVec[i];
}

} // namespace CGI
} // namespace NSTU