// ****************************************************************************
// **** Author: Manash Kumar Mandal (commented by Brendon McBain)
// **** Source: https://blog.manash.me/serial-communication-with-an-arduino-using-c-on-windows-d08710186498
// ****************************************************************************

#include "stdafx.h"		// Visual studio header
#include "SerialPort.h"

SerialPort::SerialPort(char *portName)
{
	// We are not yet connected
	this->connected = false;

	// Try to connect to the given port throuh CreateFileA
	this->handler = CreateFileA(static_cast<LPCSTR>(portName),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Check if the connection was successful
	if (this->handler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			printf("ERROR: Handle was not attached. Reason: %s not available\n", portName);
		}
		else
		{
			printf("ERROR!\n");
		}
	}
	else {
		// If connected we set the comm parameters
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(this->handler, &dcbSerialParameters)) {
			printf("Failed to get current serial parameters\n");
		}
		else {
			// Define serial connection parameters for the Arduino board
			dcbSerialParameters.BaudRate = CBR_9600;
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;	// Setting the DTR to Control_Enable ensures that the Arduino is properly
																	// reset upon establishing a connection

			// Try to get the current comm parameters
			if (!SetCommState(handler, &dcbSerialParameters))
			{
				printf("ALERT: Could not set Serial port parameters\n");
			}
			else {
				// We are connected
				this->connected = true;
				PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);	// Flush any remaining characters in the buffers
				Sleep(ARDUINO_WAIT_TIME);									// We wait 2 seconds as the arduino board will be reseting
			}
		}
	}
}

SerialPort::~SerialPort()
{
	// Check if we are connected before trying to disconnect
	if (this->connected) {
		this->connected = false;
		CloseHandle(this->handler);
	}
}

int SerialPort::readSerialPort(char *buffer, unsigned int buf_size)
{
	DWORD bytesRead;		// Number of bytes we will read
	unsigned int toRead;	// Number of bytes we will try to read

	ClearCommError(this->handler, &this->errors, &this->status);		// Use the ClearCommError function to get status info on the Serial port

	// Check if there is something to read
	if (this->status.cbInQue > 0) {
		if (this->status.cbInQue > buf_size) {
			toRead = buf_size;		// buf_size is the max number of bytes to read
		}
		else toRead = this->status.cbInQue;
	}

	// Try to read the required number of chars, and return the number of read bytes on success
	if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;

	return 0;
}

bool SerialPort::writeSerialPort(char *buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	// Try to write the buffer on the Serial port
	if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

bool SerialPort::isConnected()
{
	return this->connected;
}