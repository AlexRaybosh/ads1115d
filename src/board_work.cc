#include <board.h>


void Board::loop() {
	for (;;) {
		for (auto & pin : pins) {
			pin->work();
		}
		if (delayMicroseconds>0) std::this_thread::sleep_for(std::chrono::microseconds(delayMicroseconds));
	}
}

double Pin::measure() {
	double avg=0;
	for (int i=0;i<count;++i) {
		int adc=read();
		double v=adc/32767.0;
		avg+= v;
	}
	avg/=count;
	if (avg<offset) avg=offset;
	return coefficient*gainVoltage*avg+offset;
}
void Pin::work() {
	double m=measure();
	dataWriter->write(m);
}

int Pin::read() {
	//uint64_t startMs = utils::currentTimeMilliseconds();
	auto startClock = utils::clock();
	uint8_t hl[2];
	ssize_t rs, ws;
	uint8_t readyMask;
	int adc;

	START:
	for (;;) {
		ws=::write((int)board->i2cFd, setup, 3);
		if (ws==-1) {
			if (errno==EINTR) {
				if (utils::microseconds(startClock)>timeoutMicroseconds) utils::errno_exception("i2c timed out");
				continue;
			}
			utils::errno_exception("i2c conversion register flip failed");
		}
		if (ws != 3) throw std::runtime_error("failed on writing i2c setup bytes");
		break;
	}

	std::this_thread::sleep_for(std::chrono::microseconds(setupToReadMcDelay)); /// 1000* 1000 / rate-per-sec

	for (;;) {
		rs=::read((int)board->i2cFd, &readyMask, 1);
		if (rs==-1) {
			if (errno==EINTR) {
				if (utils::microseconds(startClock)>timeoutMicroseconds) throw std::runtime_error("i2c timed out");
				goto START;
			}
			utils::errno_exception("i2c ready read failed");
		}
		if (rs != 1) throw std::runtime_error("failed reading 1 byte reply mask from i2c");

		if (readyMask & 0x80) {
			ws=::write((int)board->i2cFd, &board->conversionRegister, 1);
			if (ws==-1) {
				if (errno==EINTR) {
					if (utils::microseconds(startClock)>timeoutMicroseconds) throw std::runtime_error("i2c timed out");
					goto START;
				}
				utils::errno_exception("i2c conversion register flip failed");
			}
			if (ws != 1) throw std::runtime_error("failed flipping i2c conversion register");


			rs=::read((int)board->i2cFd, hl, 2);
			if (rs==-1) {
				if (errno==EINTR) {
					if (utils::microseconds(startClock)>timeoutMicroseconds) throw std::runtime_error("i2c timed out");
					goto START;
				}
				utils::errno_exception("i2c data read failed");
			}
			if (rs != 2) throw std::runtime_error("failed reading ADC i2c reply values");

			int h=hl[0];
			int l=hl[1];
			adc=(h<<8)+l;
			if (adc> 32767) adc-=65535;

			return adc;
		} else {
			auto passed=utils::microseconds(startClock);
			//std::cout<<"not ready in "<<passed<<std::endl;
			if (passed>timeoutMicroseconds) throw std::runtime_error("timed out on i2c");
			if (passed>retryMicroseconds) goto START;
			auto easy=setupToReadMcDelay/5; // 20% more time
			std::this_thread::sleep_for(std::chrono::microseconds(easy>10?easy:10));
		}
	}
	return -1;
}



