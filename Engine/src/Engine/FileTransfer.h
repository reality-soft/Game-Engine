#pragma once
enum
{
	READ, WRITE
} typedef ReadOrWrite;

class FileTransfer
{
public:
	FileTransfer(string dirname, ReadOrWrite mode)
	{
		file_dirname = dirname;
		if (mode == WRITE)
			errno_t error = fopen_s(&file_ptr, dirname.c_str(), "wb");
		else if (mode == READ)
			errno_t error = fopen_s(&file_ptr, dirname.c_str(), "rb");
	}
	~FileTransfer()
	{
		fclose(file_ptr);
	}
public:
	template<typename T>
	bool WriteBinary(T* data, size_t size);

	template<typename T>
	vector<T> ReadBinary(size_t size);

private:
	FILE* file_ptr = nullptr;
	string file_dirname;
	string seiral;
};

template<typename T>
inline bool FileTransfer::WriteBinary(T* data, size_t size)
{
	if (file_ptr == nullptr)
		return false;

	size_t write_size = fwrite(data, sizeof(T), size, file_ptr);

	if (write_size <= 0)
		return false;

	return true;
}

template<typename T>
inline vector<T> FileTransfer::ReadBinary(size_t size)
{
	vector<T> read_data;

	read_data.resize(size);

	size_t read_size = fread(read_data.data(), sizeof(T), size, file_ptr);

	return read_data;
}
