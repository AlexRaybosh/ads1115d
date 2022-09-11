#include <board.h>
#include <sys/mman.h>

class RenameWriter: public DataWriter {
	std::string file;
	std::string fileTmp;
	bool textFormat;
public:
	inline RenameWriter(const std::string& f, bool writeText): file(f), textFormat(writeText) {
		fileTmp=file+".tmp";
	}
	void write(double value);
};

class OverWriter: public DataWriter {
	std::string file;
	utils::FD fd;
	bool textFormat;
public:
	OverWriter(const std::string& f, bool writeText);
	void write(double value);
};


class MmapTextWriter: public DataWriter {
	std::string file;
	utils::FD fd;
	void* map;
	size_t size{9};
	std::string format{"%08.3lf"};
public:
	MmapTextWriter(const std::string& f) : file(f) {
		//lets dump 0 first
		char* buf=(char*)alloca(size);
		double z=0;
		snprintf(buf,size,format.c_str(),z);
		utils::dumpToFile(f,buf,size);
		fd=::open(file.c_str(), O_RDWR | O_CREAT, (mode_t)600);
		if (!fd) utils::errno_exception("Failed to open "+file);
		map = ::mmap(0, sizeof(buf), PROT_READ | PROT_WRITE, MAP_SHARED, (int)fd, 0);
		if (map==MAP_FAILED) utils::errno_exception("Failed to mmap "+file);
	}
	~MmapTextWriter() {
		::munmap(map, size);
	}
	void write(double value) {
		::snprintf((char*)map,size,format.c_str(),value);
		//auto startClock=utils::clock();
		::msync(map, size, MS_SYNC);
		//auto passed=utils::microseconds(startClock);
		//std::cout<<"mmap-ed in: "<<passed<<" mi"<<std::endl;
	}
};

class MmapNativeWriter: public DataWriter {
	std::string file;
	utils::FD fd;
	void* map;
public:
	MmapNativeWriter(const std::string& f) : file(f) {
		//lets dump 0 first
		double z=0;
		utils::dumpToFile(f,&z,sizeof(z));
		fd=::open(file.c_str(), O_RDWR | O_CREAT, (mode_t)600);
		if (!fd) utils::errno_exception("Failed to open "+file);
		map = ::mmap(0, sizeof(z), PROT_READ | PROT_WRITE, MAP_SHARED, (int)fd, 0);
		if (map==MAP_FAILED) utils::errno_exception("Failed to mmap "+file);
	}
	~MmapNativeWriter() {
		::munmap(map, sizeof(double));
	}
	void write(double value) {
		*((double*)map)=value;
		::msync(map, sizeof(double), MS_SYNC);
	}
};



std::shared_ptr<DataWriter> makeDataWriter(const json_t* pinConf) {
	std::string fileName=json::getStringOrThrow("Missing writeFile. ", pinConf, "writeFile");
	std::string method=json::getStringOrThrow("Missing writeMethod. ", pinConf, "writeMethod");
	std::string format=json::getStringOrThrow("Missing writeFormat. ", pinConf, "writeFormat");
	if (fileName.length()==0 || fileName.at(fileName.length()-1)=='/') throw std::runtime_error("Invalid writeFile in "+json::pretty(pinConf));
	if (utils::isFileSystemObject(fileName) && !utils::isRegularFile(fileName)) throw std::runtime_error("writeFile is not a file, defined in "+json::pretty(pinConf));

	std::string dir, base;
	utils::splitDirBasename(fileName, dir, base);
	utils::mkdir_p(dir);
	if (method=="rename") return std::make_shared<RenameWriter>(fileName,format=="text");
	else if (method=="write") return std::make_shared<OverWriter>(fileName,format=="text");
	else if (method=="mmap")  {
		if (format=="text") return std::make_shared<MmapTextWriter>(fileName);
		else if (format=="native") return std::make_shared<MmapNativeWriter>(fileName);
		else throw std::runtime_error("Only text/native writeFormat is currently supported, defined in "+json::pretty(pinConf));

	} else throw std::runtime_error("Only rename/write/mmap writeMethod values are supported "+json::pretty(pinConf));

}



void RenameWriter::write(double value) {
	if (textFormat) {
		char buf[50];
		snprintf(buf, sizeof(buf),"%.5lf",value);
		utils::dumpToFile(fileTmp.c_str(), buf, strlen(buf));
	} else {
		utils::dumpToFile(fileTmp.c_str(), &value, sizeof(value));
	}
	//std::chrono::time_point<std::chrono::system_clock> startClock = std::chrono::system_clock::now();
	int r=::rename(fileTmp.c_str(), file.c_str());
	if (r==-1) utils::errno_exception("Failed to flip "+fileTmp+" to "+file);
	//auto passed=microseconds(startClock);
	//std::cout<<"renamed in: "<<passed<<" mi"<<std::endl;
}



OverWriter::OverWriter(const std::string& f, bool writeText): file(f), textFormat(writeText) {
	fd=::creat(file.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (!fd) utils::errno_exception("Failed to open "+file);
}
void OverWriter::write(double value) {
	char buf[50];
	size_t left;
	off_t len;
	char* ptr;
	if (textFormat) {
		snprintf(buf, sizeof(buf),"%.3lf",value);
		len=left=strlen(buf);
		ptr=(char*)(buf);
	} else {
		ptr=(char*)&value;
		len=left=sizeof(value);
	}
	//std::chrono::time_point<std::chrono::system_clock> startClock = std::chrono::system_clock::now();
	off_t off=::lseek ((int)fd, 0, SEEK_SET);
	if (off==-1) utils::errno_exception("failed to lseek "+file);
	while (left>0) {
		auto w=::write((int)fd,ptr,left);
		if (w<0) {
			if (errno==EINTR) continue;
			utils::errno_exception("failed on writing to "+file);
		} else {
			left-=w;
			ptr+=w;
		}
	}
	ftruncate((int)fd,len);
	fsync((int)fd);

	//auto passed=microseconds(startClock);
	//std::cout<<"did it in: "<<passed<<" mi"<<std::endl;
}
