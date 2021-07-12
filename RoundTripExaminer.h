#pragma once
class RoundTripExaminer {
	private:
		float collectedTimes[1000]; //the time data collected over the course of a single succesful processTimeSync() call
		//NOTE: 3 timers are neccisary as these need to all run concurrently
		float timer; //generic timer, used for any small time collection
		float timerReceiveACK; //timer for how long it takes to receive ACK AFTER sending the REQUEST_DATA packet NOTE: shared for "Total readUDP Time" and "Total loop Time"
		float timerTotalTime; //timer for whole processTimeSync() call
		int _READUDP_TIME_POSITION = 1; //for modiying READUDP_TIME_POSITION inside the class only
	public:
		//positions for placing time data in collectedTimes, all are read only
		static const int SEND_READ_DATA_TIME_POSITION = 0;
		const int& READUDP_TIME_POSITION = _READUDP_TIME_POSITION; //readUDP time data can be any position from 1-995
		static const int TOTAL_READUDP_TIME_POSITION = 996;
		static const int CREATE_PACKET_TIME_POSITION = 997;
		static const int RECEIVE_ACK_TIME_POSITION = 998;
		static const int TOTAL_TIME_POSITION = 999;


		RoundTripExaminer() { //gurrantee no junk data is in collectedTimes
			for (int i = 0; i < 1000; i++) {
				collectedTimes[i] = -1;
			}
		}
		float startTimer() {
			timer = millis();
			return timer;
		}
		float startTimerReceiveACK() {
			timerReceiveACK = millis();
			return timerReceiveACK;
		}
		float startTimerTotalTime() {
			timerTotalTime = millis();
			return timerTotalTime;
		}
		void recordTimer(int pos, float timerVal) //takes in the value from and "startTimer<>" call and the position to be placed in collectedTimes
		{
			collectedTimes[pos] = millis() - timerVal;
			if (pos != SEND_READ_DATA_TIME_POSITION && pos != TOTAL_READUDP_TIME_POSITION && pos != CREATE_PACKET_TIME_POSITION && pos != RECEIVE_ACK_TIME_POSITION && pos != TOTAL_TIME_POSITION) {
				_READUDP_TIME_POSITION++;
			}
		}
		void printTrip(){ //prints values in collectedTimes in human readable format to Serial
			for (int i = 0; i < 1000; i++) {
				if (i == SEND_READ_DATA_TIME_POSITION) {
					Serial.print("Send Request Data Time: ");
					Serial.print(collectedTimes[i]);
					Serial.println("");
				}
				//only print valid readUDP attempts (!=-1) and non-zero ms attempts
				if (collectedTimes[i] != -1 && collectedTimes[i] != 0 && i != SEND_READ_DATA_TIME_POSITION && i!= TOTAL_READUDP_TIME_POSITION && i != CREATE_PACKET_TIME_POSITION && i != RECEIVE_ACK_TIME_POSITION && i != TOTAL_TIME_POSITION) {
					Serial.print("readUDP Attempt ");
					Serial.print(i);
					Serial.print(" Time: ");
					Serial.print(collectedTimes[i]);
					Serial.println("");
				}
				if (i == TOTAL_READUDP_TIME_POSITION) {
					Serial.print("Total readUDP Time: ");
					Serial.print(collectedTimes[i]);
					Serial.println("");
				}
				if (i == CREATE_PACKET_TIME_POSITION) {
					Serial.print("Create Packet Time: ");
					Serial.print(collectedTimes[i]);
					Serial.println("");
				}
				if (i == RECEIVE_ACK_TIME_POSITION) {
					Serial.print("Total loop Time: ");
					Serial.print(collectedTimes[i]);
					Serial.println("");
				}
				if (i == TOTAL_TIME_POSITION) {
					Serial.print("Total Time: ");
					Serial.print(collectedTimes[i]);
					Serial.println("");
				}
			}
			_READUDP_TIME_POSITION = 1;
		}
};