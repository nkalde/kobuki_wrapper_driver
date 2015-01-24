#include "kobuki_wrapper_driver.h"


KobukiWrapperDriver::KobukiWrapperDriver(ConfigFile* cf, int section) : ThreadedDriver(cf, section, false, PLAYER_MSGQUEUE_DEFAULT_MAXLEN),
	kobuki_player(NULL)
{
	memset(&this->blinkenlight_devaddr,0,sizeof(player_devaddr_t));
	memset(&this->power_devaddr,0,sizeof(player_devaddr_t));
	memset(&this->position2d_devaddr,0,sizeof(player_devaddr_t));
	memset(&this->bumper_devaddr,0,sizeof(player_devaddr_t));
	memset(&this->dio_devaddr,0,sizeof(player_devaddr_t));
	memset(&this->imu_devaddr,0,sizeof(player_devaddr_t));

	//reading config file provide
	//BLINKENLIGHT
	if (cf->ReadDeviceAddr(&this->blinkenlight_devaddr, section, "provides", PLAYER_BLINKENLIGHT_CODE, -1, NULL) == 0)
	{
		if(AddInterface(this->blinkenlight_devaddr) != 0){
			PLAYER_ERROR("Error adding Blinkenlight interface\n");
			SetError(-1);
			return;
		}
	}

	//POWER
	if (cf->ReadDeviceAddr(&power_devaddr, section, "provides", PLAYER_POWER_CODE, -1, NULL) == 0)
	{
		if(AddInterface(power_devaddr) != 0){
			PLAYER_ERROR("Error adding Power interface\n");
			SetError(-1);
			return;
		}
	}

	//POSITION2D
	if (cf->ReadDeviceAddr(&position2d_devaddr, section, "provides", PLAYER_POSITION2D_CODE, -1, NULL) == 0)
	{
		if(AddInterface(position2d_devaddr) != 0){
			PLAYER_ERROR("Error adding Position2d interface\n");
			SetError(-1);
			return;
		}
	}

	//BUMPER
	if (cf->ReadDeviceAddr(&bumper_devaddr, section, "provides", PLAYER_BUMPER_CODE, -1, NULL) == 0)
	{
		if(AddInterface(bumper_devaddr) != 0){
			PLAYER_ERROR("Error adding Bumper interface\n");
			SetError(-1);
			return;
		}
	}

	//DIO
	if (cf->ReadDeviceAddr(&dio_devaddr, section, "provides", PLAYER_DIO_CODE, -1, NULL) == 0)
	{
		if(AddInterface(dio_devaddr) != 0){
			PLAYER_ERROR("Error adding Dio interface\n");
			SetError(-1);
			return;
		}
	}

	//IMU
	if (cf->ReadDeviceAddr(&imu_devaddr, section, "provides", PLAYER_IMU_CODE, -1, NULL) == 0)
	{
		if(AddInterface(imu_devaddr) != 0){
			PLAYER_ERROR("Error adding Imu interface\n");
			SetError(-1);
			return;

		}

	}

	//reading config file options
	this->serial_port = cf->ReadString(section, "port", "/dev/kobuki");
	std::string ns = cf->ReadString(section, "namespace", "/kobuki");
	this->kobuki_player = new kobuki::KobukiPlayer(ns);
	this->safe_mode = cf->ReadInt(section, "safe", 1);
	this->locked = 0;
}

int KobukiWrapperDriver::MainSetup()
{   
	puts("Kobuki Wrapper driver initialising");
	this->kobuki_player->init();
	puts("Kobuki Wrapper driver ready");

	return(0);
}


////////////////////////////////////////////////////////////////////////////////
// Shutdown the device
void KobukiWrapperDriver::MainQuit(){
	puts("Shutting Kobuki Wrapper driver down");
	this->kobuki_player->end();
	puts("Kobuki Wrapper driver has been shutdown");
}


////////////////////////////////////////////////
////////////////////////////////////////////////
int KobukiWrapperDriver::ProcessMessage(QueuePointer & resp_queue,player_msghdr * hdr,void * data){
	puts("Process Message");

	//BUMPER
	/*
	   if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_DATA,PLAYER_BUMPER_DATA_STATE,this->bumper_devaddr)){
	   puts("MESSAGE DATA STATE BUMPER");
	   player_bumper_data_t* d = new player_bumper_data_t;	
	   this->kobuki_player->data_state_bumper(d);
	   this->Publish(this->bumper_devaddr,resp_queue,PLAYER_MSGTYPE_DATA,PLAYER_BUMPER_DATA_STATE,(void*)d,sizeof(*d),NULL);
	   delete[] d->bumpers;
	   d->bumpers = NULL;
	   delete d;
	   return(0);
	   }
	 */

	if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_REQ,PLAYER_BUMPER_REQ_GET_GEOM,this->bumper_devaddr) | Message::MatchMessage(hdr,PLAYER_MSGTYPE_DATA,PLAYER_BUMPER_DATA_GEOM,this->bumper_devaddr)){
		puts("MESSAGE GET GEOM or DATA GEOM BUMPER");
		memset(&bumper_geom,0,sizeof(bumper_geom));
		this->kobuki_player->req_get_geom_bumper(&bumper_geom);
		this->Publish(this->bumper_devaddr,resp_queue,PLAYER_MSGTYPE_RESP_ACK,PLAYER_BUMPER_REQ_GET_GEOM,(void*)&bumper_geom,sizeof(bumper_geom),NULL);
		delete[] bumper_geom.bumper_def;
		bumper_geom.bumper_def = NULL;
		return(0);
	}

	//BLINKENLIGHT
	if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_CMD,PLAYER_BLINKENLIGHT_CMD_POWER,this->blinkenlight_devaddr)){
		puts("MESSAGE CMD BLINKENLIGHT POWER");
		memcpy(&blinkenlight_cmd_power,data,sizeof(blinkenlight_cmd_power));
		this->kobuki_player->cmd_power_blinkenlight(&blinkenlight_cmd_power);
		return(0);
	}

	//POWER
	/*
	   if(Message::MatchMessage(hdr,PLAYER_MSGTYPE_DATA,PLAYER_POWER_DATA_STATE,this->power_devaddr)){
	   puts("MESSAGE DATA STATE POWER");
	   player_power_data_t * d = new player_power_data_t;
	   this->kobuki_player->data_state_power(d);
	   this->Publish(this->power_devaddr, resp_queue,PLAYER_MSGTYPE_DATA,PLAYER_POWER_DATA_STATE,(void*)d,sizeof(*d),NULL);
	   delete d;
	   return(0);
	   }
	 */

	//POSITION
	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ, PLAYER_POSITION2D_REQ_MOTOR_POWER,this->position2d_devaddr)){
		puts("MESSAGE REQ POSITION2D MOTOR POWER");
		memcpy(&pos2d_power_config,data,sizeof(pos2d_power_config));
		this->kobuki_player->req_motor_power_position2d(&pos2d_power_config);
		this->Publish(this->position2d_devaddr,resp_queue,PLAYER_MSGTYPE_RESP_ACK,PLAYER_POSITION2D_REQ_MOTOR_POWER,NULL,0,NULL);
		return(0);
	}

	/*
	   if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_DATA,PLAYER_POSITION2D_DATA_STATE,this->position2d_devaddr)){
	   puts("MESSAGE DATA STATE POSITION2D");
	   player_position2d_data_t* d = new player_position2d_data_t;
	   this->kobuki_player->data_state_position2d(d);
	   this->Publish(this->position2d_devaddr,resp_queue,PLAYER_MSGTYPE_DATA,PLAYER_POSITION2D_DATA_STATE,(void*)d,sizeof(*d),NULL);
	   delete d;
	   return(0);
	   }
	 */
	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_CMD,PLAYER_POSITION2D_CMD_VEL,this->position2d_devaddr)){
		puts("MESSAGE CMD VEL POSITION2D");
		if (this->locked)
			memset(&pos2d_cmd_vel,0,sizeof(pos2d_cmd_vel));
		else
			memcpy(&pos2d_cmd_vel,data,sizeof(pos2d_cmd_vel));
		this->kobuki_player->cmd_vel_position2d(&pos2d_cmd_vel);
		//this->Publish(this->position2d_devaddr,resp_queue,PLAYER_MSGTYPE_RESP_ACK,PLAYER_POSITION2D_CMD_VEL,NULL,0,NULL);//UNCLAIMED ACK
		return(0);
	}

	if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ,PLAYER_POSITION2D_REQ_GET_GEOM,this->position2d_devaddr)){
		puts("MESSAGE REQ GET GEOM POSITION2D");
		memset(&pos2d_geom,0,sizeof(pos2d_geom));
		this->kobuki_player->req_get_geom_position2d(&pos2d_geom);
		this->Publish(this->position2d_devaddr,resp_queue,PLAYER_MSGTYPE_RESP_ACK,PLAYER_POSITION2D_REQ_GET_GEOM,(void*)&pos2d_geom,sizeof(pos2d_geom),NULL);
		return(0);
	}

	return(-1);
}

////////////////////////////////////////////////////////////////////////////////
// Main function for device thread
void KobukiWrapperDriver::Main() 
{
	// The main loop; interact with the device here
	for(;;)
	{
		//puts("Ranger map driver main");
		pthread_testcancel();
		this->ProcessMessages();

		if (this->safe_mode){
			this->locked = !this->kobuki_player->is_safe();
			if (this->locked){
				this->kobuki_player->emergency_stop();
				PLAYER_ERROR("Emergency Stop kobuki");
			}
		}

		//position data
		player_position2d_data_t pos_d;
		memset(&pos_d,0,sizeof(pos_d));
		this->kobuki_player->data_state_position2d(&pos_d);
		this->Publish(this->position2d_devaddr,PLAYER_MSGTYPE_DATA,PLAYER_POSITION2D_DATA_STATE,(void*)&pos_d,sizeof(pos_d),NULL);

		//power data
		player_power_data_t pow_d;
		memset(&pow_d,0,sizeof(pow_d));
		this->kobuki_player->data_state_power(&pow_d);
		this->Publish(this->power_devaddr,PLAYER_MSGTYPE_DATA,PLAYER_POWER_DATA_STATE,(void*)&pow_d,sizeof(pow_d),NULL);

		//bumper data
		player_bumper_data_t bum_d;
		memset(&bum_d,0,sizeof(bum_d));	
		this->kobuki_player->data_state_bumper(&bum_d);
		this->Publish(this->bumper_devaddr,PLAYER_MSGTYPE_DATA,PLAYER_BUMPER_DATA_STATE,(void*)&bum_d,sizeof(bum_d),NULL);
		delete[] bum_d.bumpers;

		usleep(100000);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Extra stuff for building a shared object.

/* need the extern to avoid C++ name-mangling  */
extern "C" {
	int player_driver_init(DriverTable* table)
	{
		puts("Kobuki Wrapper driver initializing");
		KobukiWrapperDriver_Register(table);
		puts("Kobuki Wrapper driver done");
		return(0);
	}
}

