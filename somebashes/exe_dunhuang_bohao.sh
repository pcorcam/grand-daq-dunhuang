runtime=$1
mv ./cfgs/DU-readable-conf_bohao.yaml ./cfgs/DU-readable-conf.yaml && mv ./cfgs/sysconfig_bohao.yaml ./cfgs/sysconfig.yaml
source ./env.sh
csdaq_run $runtime
mv ./cfgs/DU-readable-conf.yaml ./cfgs/DU-readable-conf_bohao.yaml && mv ./cfgs/sysconfig.yaml ./cfgs/sysconfig_bohao.yaml
