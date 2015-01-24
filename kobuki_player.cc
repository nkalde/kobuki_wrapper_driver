#include "kobuki_player.h"
namespace kobuki{

	KobukiPlayer::KobukiPlayer(std::string& namespace_sigslot):
		name(namespace_sigslot), 
		cmd_vel_timed_out_(false),
		serial_timed_out_(false),
		slot_stream_data(&KobukiPlayer::processStreamData, *this)
	{
		this->kobuki = new Kobuki();
	}

	KobukiPlayer::~KobukiPlayer(){}

	bool KobukiPlayer::init(){
		//slots		
		slot_stream_data.connect(this->name+"/stream_data");	
		
		//parameters	
		Parameters parameters;
		parameters.device_port = "/dev"+this->name;
		parameters.sigslots_namespace = this->name;
		parameters.enable_acceleration_limiter = true;
		parameters.battery_capacity =  Battery::capacity;
		parameters.battery_low =  Battery::low;
		parameters.battery_dangerous = Battery::dangerous;

		//driver init	
		try {
			this->kobuki->init(parameters);
			usleep(100000);
			if ( !this->kobuki->isAlive()){
				std::cerr << "Kobuki no data stream, is kobuki turned on?"<< std::endl;
			}
		} catch (ecl::StandardException &e ) {
			std::cerr << e.what() << std::endl;
			return false;
		}

		return true;

	}

	bool KobukiPlayer::is_safe(){
		bool safe = !(this->kobuki->getCoreSensorData().bumper and (CoreSensors::Flags::LeftBumper or CoreSensors::Flags::RightBumper or CoreSensors::Flags::CenterBumper)) and !(this->kobuki->getCoreSensorData().wheel_drop and (CoreSensors::Flags::LeftWheel or CoreSensors::Flags::RightWheel));
		if (safe){
			this->kobuki->setLed(Led1,Green);
	                this->kobuki->setLed(Led2,Green);
		}
		return safe;
	}

	void KobukiPlayer::end(){
		this->kobuki->setBaseControl(0.0,0.0);
		this->kobuki->setLed(Led1,Black);
		this->kobuki->setLed(Led2,Black);
	}

	void KobukiPlayer::emergency_stop(){
		this->kobuki->setBaseControl(0.0,0.0);
		this->kobuki->setLed(Led1,Red);
		this->kobuki->setLed(Led2,Red);
	}
	
	bool KobukiPlayer::update(){}

	//bumper
	void KobukiPlayer::data_state_bumper(player_bumper_data_t* d){
		//count
		d->bumpers_count = 3;
		//bumpers	
		uint8_t status = this->kobuki->getCoreSensorData().bumper;
		d->bumpers = new uint8_t[d->bumpers_count];
		d->bumpers[0] = status & CoreSensors::Flags::LeftBumper?1:0;
		d->bumpers[1] = status & CoreSensors::Flags::CenterBumper?1:0;
		d->bumpers[2] = status & CoreSensors::Flags::RightBumper?1:0;
	}

	void KobukiPlayer::data_geom_bumper(player_bumper_geom_t* g){
		this->req_get_geom_bumper(g);	
	}	

	void KobukiPlayer::req_get_geom_bumper(player_bumper_geom_t* g){
		//count
		g->bumper_def_count = 3;
		
		//defines
		g->bumper_def = new player_bumper_define_t[g->bumper_def_count];
		
		//length		
		float length = 0.1;
		g->bumper_def[0].length = length;
		g->bumper_def[1].length = length;
		g->bumper_def[2].length = length;

		//radius
		float radius = 0;
		g->bumper_def[0].radius = radius;
		g->bumper_def[1].radius = radius;
		g->bumper_def[2].radius = radius;

		//center
		float angleDegree = 0.0;
		g->bumper_def[1].pose.px = (0.3515/2.0)*cos(DegToRad(angleDegree));
		g->bumper_def[1].pose.py = (0.3515/2.0)*sin(DegToRad(angleDegree));
		g->bumper_def[1].pose.pz = (0.1248/2.0);
		g->bumper_def[1].pose.proll = 0.0;
		g->bumper_def[1].pose.ppitch = 0.0;
		g->bumper_def[1].pose.pyaw = DegToRad(angleDegree);

		//left
		angleDegree = 45.0;
		g->bumper_def[0].pose.px = (0.3515/2.0)*cos(DegToRad(angleDegree));
		g->bumper_def[0].pose.py = (0.3515/2.0)*sin(DegToRad(angleDegree));
		g->bumper_def[0].pose.pz = (0.1248/2.0);
		g->bumper_def[0].pose.proll =0;
		g->bumper_def[0].pose.ppitch =0;
		g->bumper_def[0].pose.pyaw = DegToRad(angleDegree);

		//right
		angleDegree = -45.0;
		g->bumper_def[2].pose.px = (0.3515/2.0)*cos(DegToRad(angleDegree));
		g->bumper_def[2].pose.py = (0.3515/2.0)*sin(DegToRad(angleDegree));
		g->bumper_def[2].pose.pz = (0.1248/2.0);
		g->bumper_def[2].pose.proll =0.0;
		g->bumper_def[2].pose.ppitch =0.0;
		g->bumper_def[2].pose.pyaw = DegToRad(angleDegree);
	}


	//blinkenlight
	void KobukiPlayer::cmd_power_blinkenlight(player_blinkenlight_cmd_power_t* p){
		if (this->kobuki->isEnabled()){
			if (p->enable){
				if (p->id == 0)
					this->kobuki->setLed(Led1,Red);
				if (p->id == 1)
					this->kobuki->setLed(Led2,Red);
			}	
			else{
				if (p->id == 0)
					this->kobuki->setLed(Led1,Black);
				if (p->id == 1)
					this->kobuki->setLed(Led2,Black);
			}
		}
	}

	//power
	void KobukiPlayer::data_state_power(player_power_data_t* d){		
		this->kobuki->lockDataAccess();
		float current = 3.14;//A
		float nominalCapacity = 2.2;//Ah
		Battery bat = this->kobuki->batteryStatus();
		d->valid = PLAYER_POWER_MASK_VOLTS | PLAYER_POWER_MASK_WATTS | PLAYER_POWER_MASK_JOULES | PLAYER_POWER_MASK_PERCENT | PLAYER_POWER_MASK_CHARGING;
		d->volts = bat.voltage;
		d->percent = bat.percent();
		d->joules = d->volts * nominalCapacity;
		d->watts = d->volts * current;//d->percent *1;
		d->charging = (bat.charging_state == Battery::Charging)?1:0;
		this->kobuki->unlockDataAccess();
	}
	//void req_set_charging_policy_power(player_power_chargepolicy_config_t* c){}

	//position
	void KobukiPlayer::req_get_geom_position2d(player_position2d_geom_t* g){
		//static information 0
		memset(g,0,sizeof(player_position2d_geom_t));
		
		g->size.sw = 0.3515;
		g->size.sl = 0.3515;
		//g->size.sh = 0.1248;	
	}

	void KobukiPlayer::req_motor_power_position2d(player_position2d_power_config_t* c){
		if (c->state)
			this->kobuki->enable();
		else
			this->kobuki->disable();
	}

	void KobukiPlayer::req_reset_odom_position2d(player_position2d_set_odom_req_t* r){
		this->pose.setIdentity();
		this->kobuki->resetOdometry();
	}

	void KobukiPlayer::data_state_position2d(player_position2d_data_t* d){
		this->kobuki->lockDataAccess();
		//std::cerr << "update_rates" << this->pose_rates[0] << "," << this->pose_rates[1] << "," << this->pose_rates[2] << std::endl;
		d->pos.px = this->pose.x();
		d->pos.py = this->pose.y();
		d->pos.pa = this->pose.heading();
		d->vel.px = this->pose_rates[0];
		d->vel.py = this->pose_rates[1];
		d->vel.pa = this->pose_rates[2];
		//update with angular velocity from imu
		d->vel.pa = this->kobuki->getAngularVelocity();
		d->stall = this->kobuki->isEnabled()?1:0;
		this->kobuki->unlockDataAccess();
	}

	void KobukiPlayer::cmd_vel_position2d(player_position2d_cmd_vel_t* v){
		if (this->kobuki->isEnabled()){
			if (v->state){
				double lin_vel = v->vel.px;
				double ang_vel = v->vel.pa;
				this->kobuki->setBaseControl(lin_vel, ang_vel);
			}
		}
	}

	//callbacks
	void KobukiPlayer::processStreamData(){
		this->kobuki->lockDataAccess();
		CoreSensors::Data data = this->kobuki->getCoreSensorData();
		ecl::Pose2D<double> pose_update;
		ecl::linear_algebra::Vector3d pose_update_rates;
		this->kobuki->updateOdometry(pose_update,pose_update_rates);
		this->pose *= pose_update;
		memcpy(&this->pose_rates,&pose_update_rates,sizeof(pose_update_rates));
		//update with heading with imu
		this->pose.heading(this->kobuki->getHeading());	
		this->kobuki->unlockDataAccess();	
	}
	
	void KobukiPlayer::print(){	
		/*
		std::cerr << "BATTERY" << std::endl;
		std::cerr << "\tpercent: " << this->kobuki->batteryStatus().percent() << std::endl;	
		switch ( this->kobuki->batteryStatus().level() ){
			case ( Battery::Maximum ):
				std::cerr << "\tstatus: maximum" << std::endl;		
				break;
			case ( Battery::Healthy ):
				std::cerr << "\tstatus: healthy" << std::endl;
				break;
			case ( Battery::Low ):
				std::cerr << "\tstatus: low" << std::endl;
				break;
			case ( Battery::Dangerous ):
				std::cerr << "\tstatus: dangerous" << std::endl;
				break;	
		}
		switch ( this->kobuki->batteryStatus().charging_state ){
			case ( Battery::Charged ):
				std::cerr << "\tcharging: charged" << std::endl;
				break;
			case ( Battery::Charging ):
				std::cerr << "\tcharging: charging" << std::endl;
				break;
			case ( Battery::Discharging ):
				std::cerr << "\tcharging: discharging" << std::endl;
				break;
		}
		switch( this->kobuki->batteryStatus().charging_source ){
			case ( Battery::None ):
				std::cerr << "\tsource: none" << std::endl;
				break;
			case ( Battery::Adapter ):
				std::cerr << "\tsource: adapter" << std::endl;
				break;
			case ( Battery::Dock ):
				std::cerr << "\tsource: dock" << std::endl;
				break;
		}	
		std::cerr << "\tvoltage: " << this->kobuki->batteryStatus().voltage << std::endl;
		//this->kobuki->printSigSlotConnections();
		*/
	}
}
