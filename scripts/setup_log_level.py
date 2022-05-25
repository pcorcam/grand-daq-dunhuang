#!/usr/bin/env python3

import os
import sys

#* GLOBAL:
#ENABLED                 =   true
#TO_STANDARD_OUTPUT      =   true
#TO_FILE                 =   false
#PERFORMANCE_TRACKING    =   false
#* DEBUG:
#ENABLED                 =   true
#TO_STANDARD_OUTPUT      =   true
#TO_FILE                 =   false
#* WARNING:
#ENABLED                 =   true
#TO_STANDARD_OUTPUT      =   true
#TO_FILE                 =   false
#* TRACE:
#ENABLED                 =   true
#TO_STANDARD_OUTPUT      =   true
#TO_FILE                 =   false
#* VERBOSE:
#ENABLED                 =   true
#TO_STANDARD_OUTPUT      =   true
#TO_FILE                 =   false
#* ERROR:
#ENABLED                 =   true
#TO_STANDARD_OUTPUT      =   true
#TO_FILE                 =   false
#* FATAL:
#ENABLED                 =   true
#TO_STANDARD_OUTPUT      =   true
#TO_FILE                 =   false

log_cfg_dir = os.environ.get("GRAND_DAQ_CONFIG_LOG")

if not log_cfg_dir: 
    print("GRAND_DAQ_CONFIG_LOG is not set")
    sys.exit(-1)

cfg_levels = {
    "data": "INFO",
    "default": "INFO",
    "elec": "INFO",
    "network": "INFO"
}

level_map = {
    "GLOBAL":  0,
    "FATAL":   1,
    "ERROR":   2,
    "WARNING": 3,
    "INFO":    4,
    "DEBUG":   5,
    "TRACE":   6,
}

levels = [ "GLOBAL", "FATAL", "ERROR", "WARNING", "INFO", "DEBUG", "TRACE" ]

def create_cfg(level, is_enable):
    if is_enable:
        en = "true"
    else:
        en = "false"

    return "* %s:\n  ENABLED = %s\n  TO_STANDARD_OUTPUT = true\n  TO_FILE = false\n" % (level, en)

for cfg in cfg_levels:
    cfg_file = os.path.join(log_cfg_dir, cfg + ".cfg")
    file_data = ""
    for l in levels:
        enabled = False
        if level_map[l] <= level_map[cfg_levels[cfg]]:
            enabled = True
        cfg_data = create_cfg(l, enabled)
        file_data += cfg_data
    with open(cfg_file, 'w') as f:
        f.write(file_data);

