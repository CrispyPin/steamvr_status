#include "../lib/openvr.h"
#include <cstdio>
#include <fstream>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <vector>

const char *OUT_PATH = "/tmp/steamvr_battery_status";

bool should_exit = false;
vr::IVRSystem *vr_sys;

void interrupted(int _sig) {
	should_exit = true;
}

void update();

int main() {
	vr::EVRInitError init_err;
	vr_sys = vr::VR_Init(&init_err, vr::VRApplication_Background);
	if (init_err == vr::VRInitError_Init_NoServerForBackgroundApp) {
		printf("SteamVR is not running.\n");
		vr::VR_Shutdown();
		return 0;
	} else if (init_err != 0) {
		printf("Could not initialize OpenVR session. Error code: %d\n", init_err);
		vr::VR_Shutdown();
		return 1;
	}

	signal(SIGINT, interrupted);

	while (!should_exit) {
		update();
		sleep(1);
	}

	std::remove(OUT_PATH);
	vr::VR_Shutdown();
	return 0;
}

void update() {
	auto di_left = vr_sys->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
	auto di_right = vr_sys->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

	std::vector<vr::TrackedDeviceIndex_t> trackers;
	{
		vr::TrackedDeviceIndex_t tracker_arr[64];
		int controller_count = vr_sys->GetSortedTrackedDeviceIndicesOfClass(vr::TrackedDeviceClass_Controller, tracker_arr, 64);

		trackers = std::vector<vr::TrackedDeviceIndex_t>(tracker_arr, tracker_arr + controller_count);
	}

	std::string output;

	for (int di = 0; di < vr::k_unMaxTrackedDeviceCount; di++) {
		if (!vr_sys->IsTrackedDeviceConnected(di))
			continue;

		vr::ETrackedPropertyError err;
		bool provides_battery = vr_sys->GetBoolTrackedDeviceProperty(di, vr::Prop_DeviceProvidesBatteryStatus_Bool, &err);
		if (!provides_battery || err != vr::TrackedProp_Success)
			continue;

		if (di == di_left) {
			output += "L";
		} else if (di == di_right) {
			output += "R";
		} else {
			output += "T";
			output += std::to_string(di);
		}
		output += ": ";
		float battery_level = vr_sys->GetFloatTrackedDeviceProperty(di, vr::Prop_DeviceBatteryPercentage_Float, &err);
		output += std::to_string((int)(battery_level * 100));
		output += "%";
		bool is_charging = vr_sys->GetBoolTrackedDeviceProperty(di, vr::Prop_DeviceIsCharging_Bool, &err);
		if (is_charging) {
			output += "+";
		}
		output += " ";
	}

	if (output.empty()) {
		output = "No tracked devices connected";
	}

	std::ofstream out_file(OUT_PATH);
	out_file.write(output.c_str(), output.size());
	out_file.close();
}