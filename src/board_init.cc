#include <board.h>


void Board::init(const json::jsonptr & conf) {
	boardConf=json::getChildPtrOrThrow("Missing board definition.", conf, "board");
	auto addrStr=json::getStringOrThrow("Missing board address.", boardConf, "i2c", "address");
	auto force=json::getBool(false, boardConf, "i2c", "force");
	int addr;
	std::stringstream ss;
	ss<<std::hex<<addrStr;
	if (!ss.good()) throw std::runtime_error(std::string("Invalid board address: ")+addrStr);
	ss>>addr;
	auto devStr=json::getStringOrThrow("Missing board device.", boardConf, "i2c", "device");
	i2cFd=::open(devStr,O_RDWR);
	if(!i2cFd) utils::errno_exception(std::string("unable to open ")+devStr);
	int ior=::ioctl((int)i2cFd,force?I2C_SLAVE_FORCE:I2C_SLAVE,addr);
	if (ior!=0) utils::errno_exception(std::string("unable to set ")+devStr+std::string(" I2C_SLAVE address to ")+addrStr+std::string(" (")+std::to_string(addr)+")");

	conversionRegister=json::getLongOrThrow("Missing board conversion register.",boardConf,"registers","conversion");
	configRegister=json::getLongOrThrow("Missing board config register.",boardConf,"registers","config");

	delayMicroseconds=json::getLong(0, conf, "work", "delayMicroseconds");

	auto workPins=json::getChildOrThrow("Missing work pins definition.", conf, "work", "pins");
	for (auto jp : json::getJsonArrayElements(workPins)) {
		if (!json::getBool(true, jp,"enabled")) {
			continue;
		}
		std::shared_ptr<Pin> pin=std::make_shared<Pin>(this);
		pin->init(jp);
		pins.push_back(pin);
	}

}

std::shared_ptr<DataWriter> makeDataWriter(const json_t* pinConf);

void Pin::init(const json_t* pinConf) {
	auto pinName=json::getStringOrThrow("Missing pin name. ", pinConf, "pin");
	name=pinName;
	auto gainName=json::getStringOrThrow("Missing gain. ", pinConf, "gain");
	auto rateName=json::getStringOrThrow("Missing rate. ", pinConf, "rate");
	count=json::getLong(1,pinConf,"count");
	coefficient=json::getNumberOrThrow("Missing denominator. ",pinConf,"coefficient");
	offset=json::getNumber(0,pinConf,"offset");
	timeoutMicroseconds=json::getLongOrThrow("",pinConf,"timeoutMicroseconds");
	retryMicroseconds=json::getLongOrThrow("",pinConf,"retryMicroseconds");

	auto pinDef=json::getChildOrThrow("No pin defined for the board. ", board->boardConf, "pins", pinName);

	id=json::getLongOrThrow("Missing pin id for "+std::string(pinName)+". ",pinDef,"id");

	rateId=json::getLongOrThrow("No rate id defined. ", board->boardConf, "rates", rateName,"id");
	frequency=json::getLongOrThrow("No rate id defined. ", board->boardConf, "rates", rateName,"frequency");

	gainId=json::getLongOrThrow("No gain id defined. ", board->boardConf, "gains", gainName,"id");
	gainVoltage=json::getNumberOrThrow("No gain voltage defined.", board->boardConf, "gains", gainName,"voltage");

	setupToReadMcDelay=1000000 /frequency; //1000* 1000 / rate-per-sec
	setup[0]=board->configRegister;
	setup[1]=(id << 4) | gainId << 1 | 0x81;
	setup[2]=(rateId << 5) | 3;
	dataWriter=makeDataWriter(pinConf);
}



