/*
 * node.cpp
 *
 *  Created on: Aug 3, 2013
 *      Author: agu
 */

#include "node.h"

static const uint8_t Node::_uuid[] = "b66dce621d5d11e38e9ba77460cc2a25";

Node::Node(HardwareSerial & usart, uint8_t address, uint8_t de, uint8_t re) :
		SlaveRtu(usart, address, de, re) {
	this->initBitInputs(1);
	this->initCoils(1);
	this->initShortInputs(1);
	this->initHoldings(16);

	_coil_pins = (DigitalPin **) malloc(_coil_length * sizeof(DigitalPin *));
	_coil_pins[0] = new DigitalPin(13, OUTPUT);

	_bit_input_pins = (DigitalPin **) malloc(
			_bit_input_length * sizeof(DigitalPin *));
	_bit_input_pins[0] = new DigitalPin(12, INPUT_PULLUP);

	_short_input_pins = (AdcPin **) malloc(
			_short_input_length * sizeof(AdcPin *));
	_short_input_pins[0] = new AdcPin(A0);

	_tmp = new Ds18b20(10);
}

Node::~Node() {

	for (uint8_t i = 0; i < _coil_length; i++)
		delete _coil_pins[i];
	free(_coil_pins);

	for (uint8_t i = 0; i < _bit_input_length; i++)
		delete _bit_input_pins[i];
	free(_bit_input_pins);

	for (uint8_t i = 0; i < _short_input_length; i++)
		delete _short_input_pins[i];
	free(_short_input_pins);

	free(_tmp);
}

void Node::init() {
	this->SlaveRtu::init();
	_tmp->init();
}

uint8_t Node::updateCoils(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		_coil_pins[index + i]->set(this->getCoil(index + i));
	return 0;
}

uint8_t Node::updateBitInputs(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++)
		this->setBitInput(index + i, _bit_input_pins[index + i]->read());
	return 0;
}

uint8_t Node::updateShortInputs(uint16_t index, uint16_t length) {
	for (uint16_t i = 0; i < length; i++) {
		uint16_t j = index + i;
		if (j == 0) {
			this->setShortInput(0, _tmp->getValue());
			_tmp->convert();
		} else
			this->setShortInput(j, _short_input_pins[j]->read());
	}
	return 0;
}
