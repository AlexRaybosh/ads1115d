#include <board.h>


extern json::jsonptr readEmbededConfig();
int usage();
const char* defaultConfPath="/etc/ads1115d.json";



int main(int argc, char ** argv) {

	std::string confFileName;
	if (argc>1) {
		confFileName=argv[1];
		if (argc>2 || confFileName.size()==0 || confFileName.at(0)=='-') return usage();
	} else {
		// assume default
		confFileName=defaultConfPath;
	}
	json::jsonptr conf;
	if (!utils::isFileSystemObject(confFileName)) {
		// lets try to init it
		try {
			conf=readEmbededConfig();
			utils::dumpToFile(confFileName,json::pretty(conf));
		} catch (const std::exception& ex) {
			std::cerr<<ex.what()<<std::endl;
			return 1;
		}
	} else {
		conf=json::parse(utils::slurpTextFile(confFileName));
	}
	//std::cout<<json::pretty(conf)<<std::endl;

	Board board;
	try {
		board.init(conf);
		board.loop();
	} catch (const std::exception& e) {
		std::cerr<<"Failed with "<<e.what()<<std::endl;
		return 1;
	}
	return 0;
}
int usage() {
	std::cerr<<"run <<"<<utils::getArgs()[0]<<" either with no arguments, then "<<defaultConfPath<<" will be created and used, or pass a name of desired json conf file"<<std::endl;
	return 1;
}


















