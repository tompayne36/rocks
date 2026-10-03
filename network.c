/********************************/
/* Network functions            */
/********************************/

#include "wt.h"
#include "rocks.h"

#define MASTER_TIMEOUT 25

typedef struct{
	int id;
	int live;
	int iff;
	WTp3 pos;
	WTq  ori;
	WTp3 dir;
	WTp3 angvel;
	float size;
} RockInfo;

typedef struct{
	WTp3 pos;
	WTq  ori;
	int iff;
} MissileInfo;

typedef struct{
	WTp3 pos;
	WTq  ori;
	int id;
} ShipInfo;

int tag;
int type;
int retlen;
int my_net_id = 0; 
WTp3 dummy_p3;

int my_deltree(WTnode *parent) /* deletes nodes BELOW parent */
{
	int x;
	WTnode *node;
	void *data;

	for (x=1; x<WTnode_numchildren(parent); x++)
	{
		node = WTnode_getchild(parent, x);
		data = WTnode_getdata(node);
		free(data);
		WTnode_delete(node);
		node = NULL;
	}

	return x;
}

WTnode *my_findnode(WTnode *parent, int id)
{
	FLAG found;
	int i,num;
	WTnode *node = NULL;
	RockData *data;

	found = FALSE;
	i = 0;
    num = WTnode_numchildren(parent);
	while ( (found == FALSE) && (i<num) )
	{
		node = WTnode_getchild(parent,i);
		data = (RockData *)WTnode_getdata(node);
		if (data->id == id) 
			found = TRUE;
		else
		{
			i++;
			node = WTnode_getchild(parent,i);
		}
    }
	return node;
}

FLAG MyNetOpen(void)
{
	if (WTnet_open("224.3.2.1", 4567, 1) == 0)
	{
		WTmessage("Error opening network\n");
		return FALSE;
	}
	else
	{
		WTmessage("Network opened successfully!\n");
		return TRUE;
	}

}



void MyNetSetInfo(int type, WTnode *node)
{
	RockData *data;
	MissileInfo missileinfo;
	RockInfo rockinfo;
	ShipInfo shipinfo;

	data = (RockData *)WTnode_getdata(node);
	tag = my_net_id;

	switch(type)
	{
		case ROCKINFO:
			rockinfo.id = data->id;
			rockinfo.size = data->mass;
			WTp3_copy(data->dir, rockinfo.dir);
			WTp3_copy(data->ang, rockinfo.angvel);
			WTnode_gettranslation (node, rockinfo.pos);
			WTnode_getorientation (node, rockinfo.ori);
			WTnet_additem(&rockinfo,    sizeof(rockinfo),   ROCKINFO, tag);
			break;
		case SHIPINFO:
			WTnode_gettranslation(node, shipinfo.pos);
			WTnode_getorientation(node, shipinfo.ori);
			shipinfo.id = my_net_id;
			WTnet_additem(&shipinfo,    sizeof(shipinfo),   SHIPINFO, tag);
			break;
		case NEWMISSILE:
			WTnode_gettranslation(node, missileinfo.pos);
			WTnode_getorientation(node, missileinfo.ori);
			data->iff = missileinfo.iff;
			WTnet_additem(&missileinfo, sizeof(missileinfo),NEWMISSILE,tag);
			break;
		case NEWROCK:
			rockinfo.id = data->id;
			tag = data->id;
			rockinfo.live = TRUE;
			WTnode_gettranslation (node, rockinfo.pos);
			WTnode_getorientation (node, rockinfo.ori);
			rockinfo.size = data->mass;
			WTnet_additem(&rockinfo,    sizeof(rockinfo),   NEWROCK, tag);
			break;
		case NEWSHIP:
			WTnode_gettranslation(node, shipinfo.pos);
			WTnode_getorientation(node, shipinfo.ori);
			shipinfo.id = my_net_id;
			WTnet_additem(&shipinfo,    sizeof(shipinfo),   NEWSHIP, tag);
			break;
		case DELSHIP:
			shipinfo.id = data->id;
			tag = data->id;
			WTnet_additem(&shipinfo,    sizeof(shipinfo),   DELSHIP, tag);
			break;
		case DELROCK:
			rockinfo.id = data->id;
			tag = data->id;
			WTnet_additem(&shipinfo,    sizeof(shipinfo),   DELROCK, tag);
			break;
		default:
			WTmessage("Unknown message type (send)\n");
			break;

	}

}


void MyNetActions(void)
{
	RockData *data;
	WTnode *node;
	MissileInfo missileinfo;
	RockInfo rockinfo;
	ShipInfo shipinfo;

	static FLAG first_time = TRUE;
	static FLAG net_valid  = FALSE;
	static int net_timer = 0;
	int other_id;

	if (first_time)
	{
		RockData *data;

//		net_master = 0; /* assume slave for now */
		my_net_id = (int)myrand(100.0f); 
		data = (RockData *)WTnode_getdata(myShip);
		data->id = my_net_id;
		WTmessage("my net id: %f\n", my_net_id);
		net_valid = MyNetOpen();
		first_time = FALSE;
		WTnet_additem(&my_net_id, sizeof(int), NETHELLO, my_net_id);
	}

	if ((net_master == -1) && (net_timer < MASTER_TIMEOUT))
	{
		WTmessage("waiting for master: %d\n", net_timer++);
	}

	if  ((net_master == -1) &&(net_timer == MASTER_TIMEOUT))
	{
		net_master = 1; /* no one else has claimed master status */
		WTmessage("I am master! No one else claimed the title\n");
	}

	while (( net_valid == TRUE) && (type = WTnet_next(&tag, &retlen)) > 0 )
	{
//		if (tag != my_net_id)
//		{
			switch (type)
			{
			case ROCKINFO:
				WTnet_removeitem(&rockinfo,    sizeof(rockinfo), &tag, &retlen);
				/* find this rock in list */
				node = my_findnode(RockRoot, rockinfo.id);
				if (node == NULL)
				{
					WTmessage("WARNING: info for unknown rock!\n");
					WTp3_init (dummy_p3);
					node = newrock(RockRoot,
								dummy_p3,
								dummy_p3,
								dummy_p3,
								rockinfo.size);
					data = (RockData *)WTnode_getdata(node);
					data->id = rockinfo.id;
					WTmessage("Adding rock %d...size: %f\n",data->id, rockinfo.size);
					WTp3_print(rockinfo.pos, "rock\n");
				}
				else
				{
					data = (RockData *)WTnode_getdata(node);
				}
				WTnode_settranslation(node, rockinfo.pos);
				WTnode_setorientation(node, rockinfo.ori);
				WTp3_copy(rockinfo.dir, data->dir);
				WTp3_copy(rockinfo.angvel, data->ang);
				break;

			case SHIPINFO:
				WTnet_removeitem(&shipinfo, sizeof(shipinfo), &tag, &retlen);
				node = my_findnode(ShipRoot, shipinfo.id);
				if (node == NULL)
				{
					RockData *data;
					/* add ship */
					node = makenewship(ShipRoot, shipinfo.pos);
					data = (RockData *) malloc(sizeof(RockData));
					WTnode_setdata(node, (void *)data);
					data->id = shipinfo.id;
				}
				WTnode_settranslation(node, shipinfo.pos);
				WTnode_setorientation(node, shipinfo.ori);
				break;

			case DELROCK:
				WTmessage("Got DELROCK message\n");
				WTnet_removeitem(&rockinfo, sizeof(rockinfo), &tag, &retlen);
				node = my_findnode(RockRoot, rockinfo.id);
				if (node == NULL)
				{
					WTmessage("WARNING: Asked to delete unknown rock\n");
				}
				else
				{
					makeexplosion(rockinfo.pos, FALSE);
					WTnode_delete(node);
					node = NULL;
				}

				break;

			case DELSHIP:
				WTmessage("Got DELSHIP message\n");
				WTnet_removeitem(&shipinfo, sizeof(shipinfo), &tag, &retlen);
				node = my_findnode(ShipRoot, shipinfo.id);
				if (node == NULL)
				{
					WTmessage("WARNING: Asked to delete unknown ship\n");
				}
				else
				{
					makeexplosion(shipinfo.pos, FALSE);
					WTnode_delete(node);
					node = NULL;
				}

				break;

			case NEWMISSILE:
				WTmessage("Got NEWMISSILE message\n");
				WTnet_removeitem(&missileinfo, sizeof(missileinfo), &tag, &retlen);
				if (tag != my_net_id)
				{
					fire_missile( missileinfo.pos,
							  missileinfo.ori,
							  missileinfo.iff);
				}
				break;

			case NEWROCK:
				WTmessage("Got NEWROCK message\n");
				WTnet_removeitem(&rockinfo,    sizeof(rockinfo),    &tag, &retlen);
				node = newrock(RockRoot,
							rockinfo.pos,
							rockinfo.ori,
							dummy_p3,
							rockinfo.size);
				data = (RockData *)WTnode_getdata(node);
				data->id = rockinfo.id;
				WTmessage("Adding rock %d...\n",data->id);
				break;
			case NEWSHIP:
				WTmessage("Got NEWSHIP message\n");
				WTnet_removeitem(&shipinfo,    sizeof(shipinfo),    &tag, &retlen);
				MYmessage("New Network Ship");
				makenewship(ShipRoot, shipinfo.pos);
				break;

			case NETHELLO:
				/* someone is saying "hi" */
				WTmessage("Got NETHELLO message\n");

				WTnet_removeitem(&other_id,    sizeof(int),    &tag, &retlen);
				if (net_master == 1) /* reply if net master */
				{			
					WTmessage("Repling to NETHELLO message\n");
					WTnet_additem(&my_net_id, sizeof(int), NETREPLY, my_net_id);
				}

			case NETREPLY:
				/* someone is saying "hi" */
				WTnet_removeitem(&other_id,    sizeof(int),    &tag, &retlen);
				if (net_master < 1 )
				{
					WTmessage("Got NETREPLY message\n");
					WTmessage("Someone else is master (%d)\n",other_id);

					/* net_master = -1 => no networking */
					/* net_master =  1 => master */
					/* net_master =  0 => slave */
					net_master = 0;

					/* remove all of local rocks (more will be added later) */
					my_deltree(RockRoot);
					/* remove any aliens */
					my_deltree(AlienRoot);
					/* remove all missiles */
					my_deltree(MissileRoot);
				}
				else
				{
					WTmessage("Got NETREPLY message...but ignored it\n");
				}
			default:
				WTmessage("Unknown message type (receive)\n");
				break;
			}
//		}
	}
}
