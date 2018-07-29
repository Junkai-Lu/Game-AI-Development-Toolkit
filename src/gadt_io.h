#include "gadtlib.h"

#pragma once

namespace gadt
{
	namespace io
	{
		template<typename PodData, typename std::enable_if<std::is_pod<PodData>::value, int>::type = 0>
		class FileLoader
		{
		private:

			FILE * _fp;
			bool _eof;
			bool _closed;

		public:

			//constructor.
			FileLoader(std::string file_path) :
				_fp(),
				_eof(false),
				_closed(false)
			{
#ifdef __GADT_MSVC
				if (fopen_s(&_fp, file_path.c_str(), "rb") != 0)
#else
				if ((_fp = fopen(file_path.c_str(), "rb")) == 0)
#endif
				{
					_eof = true;
					_closed = true;
				}
			}

			//load next data.
			PodData load_next()
			{
				PodData item = PodData();
				if (_eof || _closed)
					return item;
#ifdef __GADT_MSVC
				auto res = fread_s(&item, sizeof(PodData), sizeof(PodData), 1, _fp);
#else
				auto res = fread(&item, sizeof(PodData), 1, _fp);

#endif
				if (res != 1)
					_eof = true;
				return item;
			}

			//try to load multiple 
			std::vector<PodData> load(size_t size)
			{
				std::vector<PodData> vec(size);
				if (size == 0 || _closed == true)
					return vec;
#ifdef __GADT_MSVC
				size_t load_size = fread_s(&vec.front(), sizeof(PodData) * size, sizeof(PodData), size, _fp);
#else
				size_t load_size = fread(&vec.front(), sizeof(PodData), size, _fp);
#endif
				if (load_size != size)
				{
					vec.resize(load_size);
					_eof = true;
				}
				return vec;
			}

			//return true if file is eof.
			bool is_eof() const
			{
				return _eof;
			}

			inline void close()
			{
				if (_closed == false)
				{
					fclose(_fp);
					_closed = true;
				}
			}

			//deconstructor, which would close file.
			~FileLoader()
			{
				close();
			}
		};

		template<typename PodData, typename std::enable_if<std::is_pod<PodData>::value, int>::type = 0>
		class FileWriter
		{
		private:

			FILE * _fp;
			bool _exist;
			
		public:

			FileWriter(std::string file_path, bool overwrite = true) :
				_fp(),
				_exist(true)
			{
#ifdef __GADT_MSVC
				if (fopen_s(&_fp, file_path.c_str(), (overwrite? "wb": "a")) != 0)
#else
				if ((_fp = fopen(file_path.c_str(), (overwrite ? "wb" : "a"))) == 0)
#endif
				{
					_exist = false;
				}
			}

			//save item into the end of the file, return true if save success.
			bool save_single(const PodData& item)
			{
				if (_exist)
				{
					bool res = fwrite(&item, sizeof(PodData), 1, _fp);
					if (res != 1)
						_exist = false;
				}
				return _exist;
			}

			//save multi data, return true if save success.
			bool save(PodData* first, size_t size)
			{
				if (_exist)
				{
					bool res = fwrite(first, sizeof(PodData), size, _fp);
					if (res != 1)
						_exist = false;
				}
				return _exist;
			}

			//close file.
			inline void close()
			{
				if (_exist)
				{
					fclose(_fp);
					_exist = false;
				}
			}

			~FileWriter()
			{
				close();
			}
		};
	}
}
