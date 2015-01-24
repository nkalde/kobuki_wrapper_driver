#ifndef KOBUKI_PLAYER_H
#define KOBUKI_PLAYER_H

#include <string>
#include <math.h>
#include <ecl/sigslots.hpp>
#include <kobuki_driver/kobuki.hpp>
#include <libplayercore/playercore.h>
//#include "diagnostics.hpp"


inline float DegToRad(float x){return x / 180 * M_PI;}

namespace kobuki
{
	class KobukiPlayer{

		public:
			KobukiPlayer(std::string& namespace_sigslot);
			~KobukiPlayer();
			bool init();
			bool update();	
			void print();

			bool is_safe();
			void emergency_stop();
			void end();
			
			//player callback
			//power
			void data_state_power(player_power_data_t* d);
			void req_set_charging_policy_power(player_power_chargepolicy_config_t* c);
			
			//bumper
			void data_state_bumper(player_bumper_data_t* d);
			void data_geom_bumper(player_bumper_geom_t* g);	
			void req_get_geom_bumper(player_bumper_geom_t* g);

			//position
			void req_get_geom_position2d(player_position2d_geom_t* g);
			void req_motor_power_position2d(player_position2d_power_config_t* c);
			void req_reset_odom_position2d(player_position2d_set_odom_req_t* r);
			void data_state_position2d(player_position2d_data_t* d);
			void cmd_vel_position2d(player_position2d_cmd_vel_t* v);

			//led
			void cmd_power_blinkenlight(player_blinkenlight_cmd_power_t* p);

			//imu
			/*
			   void data_state_imu(player_imu_data_state_t* s);
			   void data_calib_imu(player_imu_data_calib_t* c);
			   void data_quat_imu(player_imu_data_quat_t* q);
			   void data_euler_imu(player_imu_data_euler_t* e);
			   void data_fullstate_imu(player_imu_data_fullstate_t* f);
			   void req_set_datatype_imu(player_imu_datatype_config_t* c);
			   void req_reset_orientation_imu(player_imu_reset_orientation_config_t* c);
			 */

			//dio
			//void data_values_dio(player_dio_data_t* d);
			//void cmd_values_dio(player_dio_cmd_t* c);

		private:
			//variables
			std::string name;
			Kobuki* kobuki;
			bool cmd_vel_timed_out_;
			bool serial_timed_out_;
			ecl::Pose2D<double> pose;
			ecl::linear_algebra::Vector3d pose_rates;
			
			//sigslots
			ecl::Slot<> slot_stream_data;

			//callbacks
			void processStreamData();
	};
}//namespace kobuki


#endif
