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
	bool WriteBinary(const T* data, size_t size);

	template<typename T>
	vector<T> ReadBinary();

private:
	FILE* file_ptr = nullptr;
	string file_dirname;
	string seiral;
};

template<typename T>
inline bool FileTransfer::WriteBinary(const T* data, size_t size)
{
	if (file_ptr == nullptr)
		return false;

	fwrite(&size, sizeof(size_t), 1, file_ptr);
	size_t write_size = fwrite(data, sizeof(T), size, file_ptr);

	if (write_size <= 0)
		return false;

	return true;
}

template<typename T>
inline vector<T> FileTransfer::ReadBinary()
{
	vector<T> read_data;

	size_t size = 0;
	fread(&size, sizeof(size_t), 1, file_ptr);

	read_data.resize(size);

	size_t read_size = fread(read_data.data(), sizeof(T), size, file_ptr);

	return read_data;
}
