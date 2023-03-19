#include "stdafx.h"
#include "FileTransfer.h"

using namespace reality;

TextFileTransfer::TextFileTransfer(string dirname, ReadOrWrite mode)
{
	switch (mode)
	{
	case READ:  in_stream.open(dirname); break;
	case WRITE: out_stream.open(dirname); break;
	}
}

TextFileTransfer::~TextFileTransfer()
{
	Close();
}

bool TextFileTransfer::WriteText(string tag, string text)
{
	if (!out_stream.is_open())
		return false;

	out_stream << string(tag + " : " + text) << endl;

	return true;
}

string TextFileTransfer::ReadText(string tag)
{
	if (!in_stream.is_open())
		return string();

	char buffer[128];
	while (in_stream.getline(buffer, sizeof(buffer)))
	{
		if (string(buffer).find(tag) != string::npos)
			break;
	}

	vector<string> splited = split(string(buffer), ' : ');

	return splited.back();
}

void TextFileTransfer::Close()
{
	if (in_stream.is_open())
		in_stream.close();

	if (out_stream.is_open())
		in_stream.close();
}
