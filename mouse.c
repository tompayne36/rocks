#include "wt.h"
#include "rocks.h"

void mouse_myupdate(WTsensor *sensor)
{
	WTmouse_rawdata *raw;
	WTp3 p;
	WTq q, rq;
	float speed;
	WTwindow *w;
	int x,y,width,height;
	float x1,y1;
	float roll,yaw,pitch;

	WTmouse_rawupdate(sensor);

	w = WTmouse_whichwindow(sensor);

	/* zero sensor readings if not in a valid window */
	if (!w)
	{
		WTp3_init(p);
		WTq_init(q);
		WTsensor_setrecord(sensor,p,q);
		return;
	}

	/* get window height and width */
	WTwindow_getposition(w, &x, &y, &width, &height);

	/* get raw x and y mouse values in screen coordinates */
	raw = (WTmouse_rawdata *)WTsensor_getrawdata(sensor);

	speed = WTsensor_getsensitivity(sensor);

	WTp3_init(p);

	x1 = 1.0f - (2.0f * ((raw->pos[X] - (float) x) / (float) width ));
	y1 = 1.0f - (2.0f * ((raw->pos[Y] - (float) y) / (float) height));



	if (ThreeDVersion)
		pitch =    y1 * speed * sens * deltaT();
	else
		pitch = 0.0f;

	yaw   =   -x1 * speed * sens * deltaT();
	roll  =  0.0f;


	WTq_init(q);
	WTeuler_2q(pitch, 0.0f, 0.0f, rq);
	WTq_mult(q, rq, q);
	WTeuler_2q(0.0f, yaw, 0.0f, rq);
	WTq_mult(q, rq, q);

	WTsensor_setrecord(sensor, p, q);

}
