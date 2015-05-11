#pragma once
#include <string.h>
#include <math.h>
#include "imageprocessor/command.hpp"
#include "sensordata.hpp"

namespace Autodrive {

	namespace Maneuver{

		int slowSpeed = 1;
		int normalSpeed = 2;

		// measuring distance traveled
		bool measuringDistance = false;
		double startPos = 0;

		// measuring angle turned
		bool measuringAngle = false;
		int startAngle = 0;

		enum direction { right, left, front, back };

		// maneuvers
        enum Maneuver { NO_MANEUVER, FORWARDS, BACKWARDS, FORWARD_RIGHT, BACKWARDS_RIGHT, FORWARD_LEFT, BACKWARDS_LEFT, DONE };
        Maneuver currentManeuver;

		// stops the car
		command Stop() {
			command cmd;
			cmd.setSpeed(0);
			cmd.setAngle(0);
			return cmd;
		}

		// sets the car speed
		command SetSpeed(int speed) {
			command cmd;
			cmd.setSpeed(speed);
			return cmd;
		}

		// moves the car
		command Move(int speed) {
			command cmd;
			cmd.setSpeed(speed);
			cmd.setAngle(0);
			return cmd;
		}

		// turns the car
		command Turn(int angle) {
			command cmd;
			cmd.setSpeed(slowSpeed);
			cmd.setAngle(angle);
			return cmd;
		}

		command Turn(direction direction) {
			command cmd;
			cmd.setSpeed(slowSpeed);
			if (direction == right) {
				cmd.setAngle(25);
			} else {
				cmd.setAngle(-25);
			}
			return cmd;
		}

		namespace Status {

			// is the car stopped
			bool IsStoped(){
				if(SensorData::currentSpeed < 1 && SensorData::currentSpeed > -1){
					return true;
				}else{
					return false;
				}
			}

			// checks if the car has traveled a specific distance
			bool HasTraveledDistance(int distance){

				// initialize start point
				if(!measuringDistance){
					startPos = SensorData::encoderDistance();
					measuringDistance = true;
				}

				if((SensorData::encoderDistance() - startPos) > distance){
					measuringDistance = false;
					return true;
				}else{
					return false;
				}
			}

			// checks if the car has turned a specific angle
			bool HasTurnedAngle(int desiredAngle){

				float currentAngle = 0;

				// initialize start point
				if(!measuringAngle){
					startAngle = SensorData::currentAngle;
					measuringAngle = true;
				}

				// get the current angle from where the car was, to where it is now
				currentAngle = fmod((startAngle - SensorData::currentAngle),360);
				if(currentAngle > 180) currentAngle = 360 - currentAngle;

				if(fabs(currentAngle) > fabs(desiredAngle)) {
					measuringAngle = false;
					return true;
				}else{
					return false;
				}
			}

			// stops the car if it gets too close to something, provided a direction to pay attention too
			bool EmergencyStop(direction direction){
				if(SensorData::infrared[2] > -1 && SensorData::infrared[2] < 2.5 && direction == back){
					return true;
				}else if(((SensorData::ultrasound[0] > -1 && SensorData::ultrasound[0] < 2) || (SensorData::ultrasound[1] > -1 && SensorData::ultrasound[1] <2)) && direction == front){
					return true;
		        }else{
		        	return false;
		        }
			}
		}

		//namespace Parking{
			/* ------------------------------------------ */
			/* the parallel parking maneuver */
			command ParallelStandard(){
				
				command empty;

				switch(currentManeuver){

					case NO_MANEUVER:
						if(Status::IsStoped()){
							currentManeuver = BACKWARDS_RIGHT;
							return empty;
						}else{
							return Stop();
						}

					case BACKWARDS_RIGHT:
						SetSpeed(slowSpeed * -1);

						if(Status::EmergencyStop(back)){
							currentManeuver = FORWARD_RIGHT;
							return Stop();
						}else{
							if(Status::HasTurnedAngle(60)){
								currentManeuver = BACKWARDS_LEFT;
								return empty;
							}else{
								return Turn(right);
							}
						}

					case BACKWARDS_LEFT:
						SetSpeed(slowSpeed * -1);
						if(Status::EmergencyStop(back)){
							currentManeuver = FORWARD_RIGHT;
							return Stop();
						}else{
							if(Status::HasTurnedAngle(-60)){
								currentManeuver = DONE;
								return empty;
							}else{
								return Turn(left);
							}
						}

					case FORWARD_RIGHT:
						SetSpeed(slowSpeed);
						if(Status::EmergencyStop(back)){
							currentManeuver = BACKWARDS_LEFT;
							return Stop();
						}else{
							if(Status::HasTurnedAngle(60)){
								currentManeuver = DONE;
								return empty;
							}else{
								return Turn(right);
							}
						}

					default:
						return empty;
				}
			}
			/* ------------------------------------------ */
			/* the parallel parking maneuver on wide spot */
			/* command ParallelWide(){
				
				command empty;
				
				switch(currentManeuver){
					case NO_MANEUVER:
						cout << "parallel wide" << endl;
		                if(TryStop()){
		                    currentManeuver = FORWARD_RIGHT;
		                }
						return PARKING;
		
		            case FORWARD_RIGHT:
		                cout << "forward right" << endl;
		                SetSpeed(turningSpeed);
		
		                if(TryTurn(60)){
		                	currentManeuver = FORWARD_LEFT;
		                }
		                return PARKING;
		
		            case FORWARD_LEFT:
		                 cout << "forward left" << endl;
		                 SetSpeed(turningSpeed);
		
		                 if(TryTurn(-60)){
		                	 if(TryStop()){
		                		 return DONE;
		                	 }else{
		                		 return PARKING;
		                	 }
		                 }else{
		                	 return PARKING;
		                 }
		
					default:
						return empty;
				}
			} */
			/* ------------------------------------------ */
			// perpendicular parking maneuver
			command PerpendicularStandard(){
				command empty;
				switch(currentManeuver){
					
					case NO_MANEUVER:
						if(Status::IsStoped()){
							currentManeuver = BACKWARDS_RIGHT;
							return empty;
						}else{
							return Stop();
						}
		
					case BACKWARDS_RIGHT:
						SetSpeed(slowSpeed * -1);
						if(Status::EmergencyStop(back)){
							currentManeuver = FORWARD_RIGHT;
							return Stop();
						}else{
							if(Status::HasTurnedAngle(90)){
								currentManeuver = BACKWARDS_LEFT;
								return empty;
							}else{
								return Turn(right);
							}
						}
		
		            case BACKWARDS:
		                if(Status::HasTraveledDistance(1)){
							currentManeuver = DONE;
							return Stop();
		                }else{
							return Move(slowSpeed);
						}
		
					default:
						return empty;
				}
			}
			/* ------------------------------------------ */
		//} // Parking
	} // Maneuver
}

