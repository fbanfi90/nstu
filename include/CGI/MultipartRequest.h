#ifndef _MPR_H
#define _MPR_H

#define MPR_UNIX
//#define MPR_WIN32

#ifdef MPR_UNIX
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#define _read(a, b, c) read(a, b, c)
#endif
#ifdef MPR_WIN32
#include <io.h>
#include <fcntl.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <list>
#include <map>

using namespace std;

//
// This class is a CGI based file upload system.  This 
// software is free.  If you like this class plase send 
// an email to matthewcoan@hotmail.com
//
// Author:		Matthew W. Coan (matthewcoan@hotmail.com)
// Start Date:	April 13, 2002
// Version:		1.0.1
//
class MultipartRequest {
public:
	class FormElement {
	friend class MultipartRequest;
		char * _name;
		char * _fileName;
		char * _localFileName;
		char * _type;
		char * _data;
		long _length;
		
	public:
		FormElement()
		:_name(0), _fileName(0), 
		 _type(0), _data(0), 
		 _length(0), _localFileName(0)
		{}
		
		~FormElement()
		{
			if(_name)
				delete [] _name;
			if(_fileName)
				delete [] _fileName;
			if(_type)
				delete [] _type;
			if(_data)
				delete [] _data;
		}
		
		char * name() { return _name; }
		char * fileName() { return _fileName; }
		char * localFileName() { return _localFileName; }
		char * type() { return _type; }
		char * data() { return _data; }
		long length() { return _length; }
	};

	struct StringLess {
		bool operator()(const char* s1, const char* s2) const
		{
			return strcmp(s1, s2) < 0;
		}
	};

	class ArrayDeleter {
		char * _array;
	public:
		ArrayDeleter(char * p = 0) 
		: _array(p) 
		{ }
		
		~ArrayDeleter() {
			if(_array)
				delete [] _array;
		}
		
		void release() {
			_array = 0;
		}
		
		void reset(char * p = 0) {
			if(_array)
				delete [] _array;
			_array = p;
		}
	};

	typedef vector< FormElement * > FormElementVectorType;
	typedef map< char *, FormElement *, StringLess > MapType;
	enum { MAX_LINE = 1024, MAX_FILE = 1024 * 1024 * 1024};
	enum ErrorCode { MPR_NO_ERROR, IO_ERROR, PARSE_ERROR, MAX_UPLOAD, CGI_ERROR };
private:
	ErrorCode _errorCode;
	FormElementVectorType _elementVec;
	char * _boundary;
	char * _dir;
	bool _atEnd;
	MapType _map;
	long _max;
	long _contentLength;
	
	void _parseContent();
	char * _readLine();
	char * _readLine(size_t & sz);
	void _trim(char * s);
	char * _processParam(char * p);
	char * _extractContentType(char * ct);
	char * _dup(char * p);
	long _writeFile(char * & filename, char * end, long & index);
	char * _computeLocalFileName(char * filename);
	void _writeBytes(FILE * f, char * p, size_t sz);
	bool _readField(char * & data, long & sz, long & index, char * end);
	bool _badChar(char ch);
public:
	//
	// Construct a MultipartRequest from a directory to dump the files and
	// the maximum size of the file upload.
	//
	// dir - the directory to place the uploaded files (must end with a /)
	// max - the maximum upload
	//
	MultipartRequest(char * dir, long max = MAX_FILE);
	
	//
	// Free memeory
	//
	~MultipartRequest();
	
	//
	// Look up a form element
	//
	FormElement * operator[](char * name) { return _map[name]; }	
	
	//
	// Access the vector of elements
	//
	FormElement * operator[](size_t index) { return _elementVec[index]; }	

	//
	// Return the number of form elements
	//
	size_t size() { return _elementVec.size(); }

	//
	// Return the error code
	//
	ErrorCode errorCode() { return _errorCode; }
	
	//
	// Content length
	//
	long contentLength() { return _contentLength; }
	
	//
	// Max upload
	//
	long maxUpload() { return _max; }
};

#endif /* _MPR_H */

