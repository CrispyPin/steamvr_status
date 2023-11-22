# SteamVR-status
A very simple program that prints the battery levels of connected VR devices to `/tmp/steamvr_battery_status` if SteamVR is running.

You can set it to auto-launch with SteamVR by adding it to the launch options in Steam: `%command% & sleep 4 && ~/bin/steamvr-status/vr_status`.
