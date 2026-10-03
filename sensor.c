/***********************************/
/*	sensor.c					   */
/*								   */
/*	by Tom Payne				   */
/*	   Common Sense Engineering	   */
/*	   (c) 1996					   */
/***********************************/

#include "wt.h"
#include "rocks.h"

int sensor_type = 99;
int port = 99;
int baud = 99;
int unit = 99;

void setup_sensors(WTsensor **lsensor)
{
	if (sensor_type == 99) sensor_type=ST_MOUSE; /* default to mouse */

	WTmessage("Initializing %s\n", sensor_name[sensor_type]);
		switch (sensor_type) {
			case ST_BIRD:
				*lsensor = WTbird_new(com[port], (short)unit);baud=3; break;
			case ST_BOOM:
				*lsensor = WTboom_new(com[port]); baud=3; break;
			case ST_CRYSTALVR:
				*lsensor = WTcrystaleyesVR_new(com[port]);baud =0 ; break;
			case ST_FASTRAK:
				*lsensor = WTfastrak_new(com[port], (short)unit); baud = 4;break;
			case ST_FORMULA:
				*lsensor = WTformula_new((short)unit); baud = 99; break;
			case ST_GEOBALL:
				*lsensor = WTgeoball_new(com[port]); baud = 3; break;
			case ST_GLOVE5DT:
				*lsensor = WTglove5DT_new(com[port]); baud = 4;break;
			case ST_IGLASSES:
				*lsensor = WTiglasses_new(com[port]); baud = 4;break;
			case ST_INSIDETRAK:
				*lsensor = WTinsidetrak_new((short)unit); baud=99; break;
			case ST_ISOTRAK2:
				*lsensor = WTisotrak2_new(com[port], (short)unit);baud = 4; break;
			case ST_JOYSERIAL:
				*lsensor = WTjoyserial_new(com[port]); baud = 4;break;
			case ST_LOGITECH:
				*lsensor = WTlogitech_new(com[port]); baud = 0; break;
			case ST_POLHEMUS:
				*lsensor = WTpolhemus_new(com[port]); baud = 3;break;
			case ST_PRECNAVI:
				*lsensor = WTprecision_new(com[port]); baud = 5; break;
			case ST_REDBARON:
				*lsensor = WTbaron_new(com[port]); baud = 0;break;
			case ST_SCONTROL:
				*lsensor = WTspacecontrol_new(com[port]); baud = 3; break;
			case ST_SPACEBALL:
				*lsensor = WTspaceball_new(com[port]); baud = 3;break;
			case ST_MOUSE:
				*lsensor = WTmouse_new();
				WTsensor_setsensitivity(sensor, 1.0f);
				WTsensor_setupdatefn(*lsensor, mouse_myupdate);
				baud = 99;
				break;
		if (lsensor) 	/* Failed */
		{
			WTmessage("\nproblem opening sensor: %s.\n", sensor_name[sensor_type]);
		}
	}

}

void parse_sensor(char *arg)
{
	char *cp = arg;

	port = 0;
	/* most sensors require a serial point specified */
	switch (sensor_type) {
		case ST_BIRD:
		case ST_BOOM:
		case ST_CRYSTALVR:
		case ST_CYBERMAXX2:
		case ST_FASTRAK:
		case ST_GEOBALL:
		case ST_GLOVE5DT:
		case ST_IGLASSES:
		case ST_ISOTRAK2:
		case ST_JOYSERIAL:
		case ST_LOGITECH:
		case ST_PRECNAVI:
		case ST_POLHEMUS:
		case ST_REDBARON:
		case ST_SBALLSC:
		case ST_SCONTROL:
		case ST_SPACEBALL:
			port = *cp - 49;
			cp++;
			break;
	}
	/* some sensors need a unit number */
	switch (sensor_type) {
		case ST_BIRD:
			unit = 0;	/* default */
			/* for a bird, we might not have a unit number (not a flock) */
			if (!isdigit(*cp)) break;
		case ST_FASTRAK:
			unit = 0;	/* default */
		case ST_FORMULA:
			unit = 0;	/* default */
		case ST_INSIDETRAK:
			unit = 0;	/* default */
		case ST_ISOTRAK2:
			unit = 0;	/* default */
			unit = *cp - '0';
			cp++;
			break;
	}
}

