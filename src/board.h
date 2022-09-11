/*
 * board.h
 *
 *  Created on: Sep 11, 2022
 *      Author: alex
 */

#ifndef BOARD_H_
#define BOARD_H_
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "utils.h"
#include "jsonutils.h"
#include <map>
#include <vector>
#include <set>
#include <sstream>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>


class DataWriter {
public:
	virtual ~DataWriter() = default;
	virtual void write(double value) = 0;
};

class Board;

struct Pin {
	Board* board;
	std::string name;
	int id;
	int count;
	double coefficient;
	double offset;
	int rateId;
	int frequency;
	int gainId;
	double gainVoltage;
	int64_t setupToReadMcDelay;
	uint8_t setup[3];
	std::shared_ptr<DataWriter> dataWriter;
	uint64_t timeoutMicroseconds;
	uint64_t retryMicroseconds;


	Pin(Board* bp) : board(bp) {}
	void init (const json_t* pinConf);
	int read();
	double measure();
	void work();
};

struct Board {
	json::jsonptr boardConf;
	utils::FD i2cFd;
	std::vector<std::shared_ptr<Pin>> pins;
	uint8_t conversionRegister;
	uint8_t configRegister;
	uint64_t delayMicroseconds;

	void init(const json::jsonptr & conf);
	void loop();
};





#endif /* BOARD_H_ */
