/*
 * Copyright (c) 2021, Nippon Seiki Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef	AGLCLUSTER_SERVICE_PROTOCOL_H
#define	AGLCLUSTER_SERVICE_PROTOCOL_H

#include <stdint.h>

struct __attribute__((packed)) S_AGLCLUSTER_SERVICE_PACKETHEADER_V1 {
	uint32_t magic;
	uint32_t version;
	uint64_t seqnum;
};

struct __attribute__((packed)) S_AGLCLUSTER_SERVICE_DATA_V1 {
	// Telltale
	int32_t  turnR;
	int32_t  turnL;
	int32_t  brake;
	int32_t  seatbelt;
	int32_t  frontRightSeatbelt;
	int32_t  frontCenterSeatbelt;
	int32_t  frontLeftSeatbelt;
	int32_t  mid1RightSeatbelt;
	int32_t  mid1CenterSeatbelt;
	int32_t  mid1LeftSeatbelt;
	int32_t  mid2RightSeatbelt;
	int32_t  mid2CenterSeatbelt;
	int32_t  mid2LeftSeatbelt;
	int32_t  rearRightSeatbelt;
	int32_t  rearCenterSeatbelt;
	int32_t  rearLeftSeatbelt;
	int32_t  highbeam;
	int32_t  door;
	int32_t  frontRightDoor;
	int32_t  frontLeftDoor;
	int32_t  rearRightDoor;
	int32_t  rearLeftDoor;
	int32_t  trunkDoor;
	int32_t  hoodDoor;
	int32_t  eps;
	int32_t  srsAirbag;
	int32_t  abs;
	int32_t  lowBattery;
	int32_t  oilPress;
	int32_t  engine;
	int32_t  fuel;
	int32_t  immobi;
	int32_t  tmFail;
	int32_t  espAct;
	int32_t  espOff;
	int32_t  adaptingLighting;
	int32_t  autoStop;
	int32_t  autoStopFail;
	int32_t  parkingLights;
	int32_t  frontFog;
	int32_t  exteriorLightFault;
	int32_t  accFail;
	int32_t  ldwOff;
	int32_t  hillDescent;
	int32_t  autoHiBeamGreen;
	int32_t  autoHiBeamAmber;
	int32_t  sportsMode;
	int32_t  ldwOperate;
	int32_t  generalWarn;
	int32_t  drivingPowerMode;
	int32_t  hotTemp;
	int32_t  lowTemp;

	// ShiftPosition
	int32_t  gearAtVal;
	int32_t  gearMtVal;

	// Speed
	uint32_t spAnalogVal;
	int32_t  spAnaDigUnitVal;

	// Tacho
	uint32_t taAnalogVal;

	// TripComputer
	uint32_t trcomTripAVal;
	uint32_t trcomTripBVal;
	uint32_t trcomOdoVal;
	int32_t  trcomUnitVal;
	uint16_t avgSpeedAVal;
	uint16_t avgSpeedBVal;
	uint16_t hourAVal;
	uint16_t hourBVal;
	uint8_t  minuteAVal;
	uint8_t  minuteBVal;
	uint8_t  secondAVal;
	uint8_t  secondBVal;
	int16_t  oTempVal;
	int32_t  oTempUnitVal;
	uint16_t cruRangeVal;
	uint16_t avgFuelAVal;
	uint16_t avgFuelBVal;
	uint16_t insFuelAVal;
	uint16_t insFuelBVal;
	int32_t  fuelEconomyUnitVal;
};

struct __attribute__((packed)) S_AGLCLUSTER_SERVICE_PACKET_V1 {
	struct S_AGLCLUSTER_SERVICE_PACKETHEADER_V1 header;
	struct S_AGLCLUSTER_SERVICE_DATA_V1 data;
};

typedef struct S_AGLCLUSTER_SERVICE_PACKET_V1 AGLCLUSTER_SERVICE_PACKET_V1;
typedef struct S_AGLCLUSTER_SERVICE_PACKET_V1 AGLCLUSTER_SERVICE_PACKET;

#endif //AGLCLUSTER_SERVICE_PROTOCOL_H
