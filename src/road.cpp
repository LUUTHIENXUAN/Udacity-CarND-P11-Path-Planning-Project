#include <iostream>
#include <math.h>
#include <map>
#include <string>
#include <iterator>
#include "Behavior_planning/road.h"
#include "Behavior_planning/vehicle.h"


/**
 * Initializes Road
 */
Road::Road(int speed_limit, vector<int> lane_speeds) {

    this->num_lanes     = lane_speeds.size();
    this->lane_speeds   = lane_speeds;
    this->speed_limit   = speed_limit;

}

Road::~Road() {}

Vehicle Road::get_ego() {

	return this->vehicles.find(this->ego_key)->second;
}

void Road::ego_localization(double s){

  Vehicle &ego = this->vehicles.find(this->ego_key)->second;

  ego.s = (int) s;

}

void Road::behavior_planning() {

  // generate predictions for surrounding vehicles in horizon
  map<int ,vector<Vehicle>> predictions;

	map<int, Vehicle>::iterator it = this->vehicles.begin();

  while(it != this->vehicles.end()) {

    int v_id              = it->first;

    vector<Vehicle> preds = it->second.generate_predictions();

    predictions[v_id]     = preds;

    it++;
  }

  //Update Ego
  it = this->vehicles.begin();

  while(it != this->vehicles.end()) {

    int v_id = it->first;

    if(v_id == ego_key)
    {

      vector<Vehicle> trajectory
      = it->second.choose_next_state(predictions);

      it->second.realize_next_state(trajectory);


    }
    it++;

  }

}

void Road::add_ego(int lane_num, int s, double vel, vector<int> config_data) {

  //Vehicle ego = Vehicle(lane_num, s, this->lane_speeds[lane_num], 0);

  Vehicle ego = Vehicle(lane_num, s, vel, 0);

  ego.configure(config_data);
  ego.state = "KL";

  this->vehicles.insert(std::pair<int,Vehicle>(ego_key,ego));

}

void Road::add_vehicles_surrounding(vector<vector<double>> sensor_fusion, int prev_size) {

  Vehicle ego;

  map<int, Vehicle>::iterator it = this->vehicles.begin();

  while(it != this->vehicles.end()) {

    // delete all fused vehicles except ego
    if (it->first != ego_key) it = this->vehicles.erase(it);
    else {

      ego = it->second;
    }

    it++;
  }
  this->vehicles.clear();

  this->vehicles.insert(std::pair<int,Vehicle>(ego_key,ego));

  for (auto &fused_info: sensor_fusion){

    if (fused_info[6] < 0) continue;
    auto l = (int) fused_info[6] / 4; //lane is 4 meter

    double vx = fused_info[3];
    double vy = fused_info[4];
    auto v    = sqrt( vx*vx + vy*vy);

    auto s = fused_info[5];
    // if using previous points can project x_value
    s += (double)prev_size * .02 * v;

    Vehicle vehicle = Vehicle(l,s,v,0);
    vehicle.state   = "CS";

    this->vehicles_added = fused_info[0]; // ID number
    this->vehicles.insert(std::pair<int,Vehicle>(vehicles_added,vehicle));
  }

}

/*
void Road::advance() {

	map<int ,vector<Vehicle>> predictions;

	map<int, Vehicle>::iterator it = this->vehicles.begin();

  while(it != this->vehicles.end()) {

    int v_id              = it->first;

    vector<Vehicle> preds = it->second.generate_predictions();

    predictions[v_id]     = preds;

    it++;
  }

  it = this->vehicles.begin();

  while(it != this->vehicles.end()) {

    int v_id = it->first;
    if(v_id == ego_key){

      vector<Vehicle> trajectory = it->second.choose_next_state(predictions);

      it->second.realize_next_state(trajectory);
    }
    else {

      it->second.increment(1);
    }

      it++;
  }

}
*/

/*
void Road::populate_traffic() {

	int start_s = max(this->camera_center - (this->update_width/2), 0);

  for (int l = 0; l < this->num_lanes; l++) {

		int lane_speed = this->lane_speeds[l];
		bool vehicle_just_added = false;

		for(int s = start_s; s < start_s+this->update_width; s++) {

			if(vehicle_just_added) {

				vehicle_just_added = false;

			}

			if(((double) rand() / (RAND_MAX)) < this->density) {

				Vehicle vehicle = Vehicle(l,s,lane_speed,0);
				vehicle.state   = "CS";

        this->vehicles_added += 1;
				this->vehicles.insert(std::pair<int,Vehicle>(vehicles_added,vehicle));

        vehicle_just_added = true;
			}
		}
	}

}
*/

/*
void Road::display(int timestep) {

    Vehicle ego = this->vehicles.find(this->ego_key)->second;
    int s = ego.s;
    string state = ego.state;

    this->camera_center = max(s, this->update_width/2);

    int s_min = max(this->camera_center - this->update_width/2, 0);
    int s_max = s_min + this->update_width;

    vector<vector<string> > road;

    for(int i = 0; i < this->update_width; i++) {

        vector<string> road_lane;
        for(int ln = 0; ln < this->num_lanes; ln++) {

            road_lane.push_back("     ");
        }
        road.push_back(road_lane);

    }

    map<int, Vehicle>::iterator it = this->vehicles.begin();

    while(it != this->vehicles.end()){

        int v_id = it->first;
        Vehicle v = it->second;

        if(s_min <= v.s && v.s < s_max) {

            string marker = "";
            if(v_id == this->ego_key) {

                marker = this->ego_rep;
            }
            else {

              stringstream oss;
              stringstream buffer;
              buffer << " ";
              oss << v_id;
              for(int buffer_i = oss.str().length(); buffer_i < 3; buffer_i++) {
                  buffer << "0";

              }
              buffer << oss.str() << " ";
              marker = buffer.str();
            }
            road[int(v.s - s_min)][int(v.lane)] = marker;
        }

        it++;
    }

    ostringstream oss;
    oss << "+Meters ======================+ step: " << timestep << endl;
    int i = s_min;

    for(int lj = 0; lj < road.size(); lj++) {

        if(i%20 ==0){

            stringstream buffer;
            stringstream dis;
            dis << i;

            for(int buffer_i = dis.str().length(); buffer_i < 3; buffer_i++) {
                 buffer << "0";
            }

            oss << buffer.str() << dis.str() << " - ";
        }

        else {
            oss << "      ";
        }

        i++;
        for(int li = 0; li < road[0].size(); li++) {
            oss << "|" << road[lj][li];
        }
        oss << "|";
        oss << "\n";
    }

    cout << oss.str();

}
*/