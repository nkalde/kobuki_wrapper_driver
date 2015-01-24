#if !defined (WIN32)
#include <unistd.h>
#endif
#include "kobuki_player.h"
//#include <string>
//#include <libplayercore/playercore.h>
////////////////////////////////////////////////////////////////////////////////
// The class for the driver
class KobukiWrapperDriver : public ThreadedDriver
{
	public:    
		// Constructor; need that
		KobukiWrapperDriver(ConfigFile* cf, int section);

		// This method will be invoked on each incoming message
		virtual int ProcessMessage(QueuePointer &resp_queue, player_msghdr * hdr, void * data);

	private:
		// Main function for device thread.
		virtual void Main();
		virtual int MainSetup();
		virtual void MainQuit();

		//Device addresses
		player_devaddr_t blinkenlight_devaddr;
		player_devaddr_t power_devaddr;
		player_devaddr_t position2d_devaddr;
		player_devaddr_t bumper_devaddr;
		player_devaddr_t dio_devaddr;
		player_devaddr_t imu_devaddr;
	
		kobuki::KobukiPlayer* kobuki_player;
		
		// Config File Options
		std::string serial_port;

		player_bumper_geom_t bumper_geom;
		player_bumper_data_t bumper_data;
		player_position2d_data_t pos2d_data;
		player_position2d_geom_t pos2d_geom;
		player_power_data_t  power_config;
		player_position2d_cmd_vel_t pos2d_cmd_vel;
		player_position2d_power_config_t pos2d_power_config;		
		player_blinkenlight_cmd_power_t blinkenlight_cmd_power;

		bool safe_mode;
		bool locked;
};

Driver* KobukiWrapperDriver_Init(ConfigFile* cf, int section)
{
	return((Driver*)(new KobukiWrapperDriver(cf, section)));
}

void KobukiWrapperDriver_Register(DriverTable* table)
{
	table->AddDriver("kobukiwrapperdriver", KobukiWrapperDriver_Init);
}
