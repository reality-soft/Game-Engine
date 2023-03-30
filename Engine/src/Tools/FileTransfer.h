#pragma once
namespace reality
{
	enum
	{
		READ, WRITE
	} typedef ReadOrWrite;

	class DLL_API TextFileTransfer
	{
	public:
		TextFileTransfer(string dirname, ReadOrWrite mode);
		~TextFileTransfer();

	public:
		bool WriteText(string tag, string text);
		string ReadText(string tag);

	public:
		void Close();

	private:
		ofstream out_stream;
		ifstream in_stream;
		string file_dirname;
	};

	class DLL_API FileTransfer
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
			Close();
		}
	public:
		template<typename T>
		bool WriteBinary(T* data, size_t size);

		template<typename T>
		void ReadBinary(vector<T>& buffer);

		template<typename T>
		void ReadBinary(T& single);

		template<typename T>
		bool WriteBinaryWithoutSize(T* data, size_t size);

		template<typename T>
		vector<T> ReadBinaryWithoutSize(size_t size);

		void Close()
		{
			if (file_ptr)
				fclose(file_ptr);
		}

	private:
		FILE* file_ptr = nullptr;
		string file_dirname;
		string seiral;
	};

	template<typename T>
	inline bool FileTransfer::WriteBinaryWithoutSize(T* data, size_t size)
	{
		if (file_ptr == nullptr)
			return false;

		size_t write_size = fwrite(data, sizeof(T), size, file_ptr);

		if (write_size <= 0)
			return false;

		return true;
	}

	template<typename T>
	inline vector<T> FileTransfer::ReadBinaryWithoutSize(size_t size)
	{
		vector<T> read_data;

		read_data.resize(size);

		size_t read_size = fread(read_data.data(), sizeof(T), size, file_ptr);

		return read_data;
	}

	template<typename T>
	inline void FileTransfer::ReadBinary(vector<T>& buffer)
	{
		size_t size = 0;
		fread(&size, sizeof(size_t), 1, file_ptr);
		buffer.resize(size);

		size_t read_size = fread(buffer.data(), sizeof(T), size, file_ptr);
	}

	template<typename T>
	inline void FileTransfer::ReadBinary(T& single)
	{
		size_t size = 0;
		fread(&size, sizeof(size_t), 1, file_ptr);
		if (size == 1)
		{
			size_t read_size = fread(&single, sizeof(T), 1, file_ptr);
		}
	}

	template<typename T>
	inline bool FileTransfer::WriteBinary(T* data, size_t size)
	{
		if (file_ptr == nullptr)
			return false;

		fwrite(&size, sizeof(size_t), 1, file_ptr);
		size_t write_size = fwrite(data, sizeof(T), size, file_ptr);

		if (write_size <= 0)
			return false;

		return true;
	}
}
