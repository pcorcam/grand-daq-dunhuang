import yaml
import pandas as pd
import shutil
import copy
from copy import deepcopy
from collections import OrderedDict
import ruamel.yaml

# 定义DU数据列表的模版
# 定义一个 DU 的模板数据结构，包含所有必要的默认值
du_template = {
    "Global": {
        "Configuration": {
            "Auto Reboot": 0,
            "Filter 1": 1,
            "Filter 2": 0,
            "Filter 3": 1,
            "Filter 4": 1,
            "Fake ADC": 0,
            "1PPS": 1,
            "Enable DAQ": 1
        },
        "Trigger": {
            "Channel 1": 0,
            "Channel 2": 0,
            "Channel 3": 1,
            "Channel 4": 0,
            "Ch1 AND Ch2": 0,
            "(not Ch1)AND Ch2": 0,
            "Ch1 AND Ch2, Ch2>Ch1": 0,
            "Ch3 AND Ch4": 0,
            "20Hz": 0,
            "10 sec": 1,
            "Internal": 0
        },
        "Internal Trigger Rate": {
            "(1000000,124)Hz": "0x76"
        },
        "TriggerOverlap": {
            "Time(ns)": 64
        },
        "Battery Voltages(off,on)V": {
            "BatLow": 2570,
            "BatHigh": 3111
        },
        "TriggerBlock": {
            "FIFO count": 10880
        }
    },
    "Input": {
        "Ch1": {"Off": 0, "ADC": 7},
        "Ch2": {"Off": 0, "ADC": 2},
        "Ch3": {"Off": 0, "ADC": 3},
        "Ch4": {"Off": 0, "ADC": 4}
    },
    "Channels": {
        "Channel 1": {
            "Pre Trigger": 1856,
            "Post Trigger": 128,
            "Additional Gain[-14,23.5](dB)": 20,
            "Integration time": 0,
            "Min.Baseline(ADC)": 6144,
            "Max.Baseline(ADC)": 10240,
            "Signal Treshold(ADC)": 30,
            "Noise Treshold(ADC)": 10,
            "Quiet Time before Sig treshold(ns)": 52,
            "Time after Sig threshold(ns)": 0,
            "Max Time between threshold crossings(ns)": 500,
            "Min number of threshold crossings(ns)": 0,
            "Max number of threshold crossings(ns)": 255,
            "Min charge": 0,
            "Max charge": 255,
            "Filter1": [59.0, 0.99],
            "Filter2": [118.8, 0.99],
            "Filter3": [137.8, 0.99],
            "Filter4": [189.0, 0.99]
        },
        "Channel 2": {
            "Pre Trigger": 1856,
            "Post Trigger": 128,
            "Additional Gain[-14,23.5](dB)": 20,
            "Integration time": 0,
            "Min.Baseline(ADC)": 6144,
            "Max.Baseline(ADC)": 10240,
            "Signal Treshold(ADC)": 30,
            "Noise Treshold(ADC)": 10,
            "Quiet Time before Sig treshold(ns)": 52,
            "Time after Sig threshold(ns)": 0,
            "Max Time between threshold crossings(ns)": 500,
            "Min number of threshold crossings(ns)": 0,
            "Max number of threshold crossings(ns)": 255,
            "Min charge": 0,
            "Max charge": 255,
            "Filter1": [59.0, 0.99],
            "Filter2": [118.8, 0.99],
            "Filter3": [137.8, 0.99],
            "Filter4": [189.0, 0.99]
        },
        "Channel 3": {
            "Pre Trigger": 1856,
            "Post Trigger": 128,
            "Additional Gain[-14,23.5](dB)": 20,
            "Integration time": 0,
            "Min.Baseline(ADC)": 6144,
            "Max.Baseline(ADC)": 10240,
            "Signal Treshold(ADC)": 30,
            "Noise Treshold(ADC)": 10,
            "Quiet Time before Sig treshold(ns)": 52,
            "Time after Sig threshold(ns)": 0,
            "Max Time between threshold crossings(ns)": 500,
            "Min number of threshold crossings(ns)": 0,
            "Max number of threshold crossings(ns)": 255,
            "Min charge": 0,
            "Max charge": 255,
            "Filter1": [59.0, 0.99],
            "Filter2": [118.8, 0.99],
            "Filter3": [137.8, 0.99],
            "Filter4": [189.0, 0.99]
        },
        "Channel 4": {
            "Pre Trigger": 1856,
            "Post Trigger": 128,
            "Additional Gain[-14,23.5](dB)": 20,
            "Integration time": 0,
            "Min.Baseline(ADC)": 6144,
            "Max.Baseline(ADC)": 10240,
            "Signal Treshold(ADC)": 30,
            "Noise Treshold(ADC)": 10,
            "Quiet Time before Sig treshold(ns)": 52,
            "Time after Sig threshold(ns)": 0,
            "Max Time between threshold crossings(ns)": 500,
            "Min number of threshold crossings(ns)": 0,
            "Max number of threshold crossings(ns)": 255,
            "Min charge": 0,
            "Max charge": 255,
            "Filter1": [59.0, 0.99],
            "Filter2": [118.8, 0.99],
            "Filter3": [137.8, 0.99],
            "Filter4": [189.0, 0.99]
        },
        "Station Rate(Simulation)": 1000
    },
    
}

# 自定义Loader以防止布尔值自动转换并保持顺序
class NoBooleanLoader(yaml.SafeLoader):
    pass

# 自定义Dumper以保留字典顺序
class OrderedDumper(yaml.SafeDumper):
    pass

# 定义字典的顺序表示方法
def dict_representer(dumper, data):
    return dumper.represent_dict(data.items())

# 定义布尔值（如 'on', 'off'）作为字符串处理
def bool_representer(dumper, data):
    return dumper.represent_scalar('tag:yaml.org,2002:str', data)

# 注册自定义字典和布尔处理器
yaml.add_representer(OrderedDict, dict_representer, Dumper=OrderedDumper)
yaml.add_representer(str, bool_representer, Dumper=OrderedDumper)

# 禁止布尔转换，将 'on', 'off' 等保持为字符串
for bool_value in ['yes', 'no', 'true', 'false', 'on', 'off']:
    NoBooleanLoader.add_constructor(
        'tag:yaml.org,2002:bool',
        lambda loader, node: loader.construct_scalar(node)
    )

# 读取 YAML 文件，使用自定义的 NoBooleanLoader，保持顺序
def load_yaml(file_path):
    yaml = ruamel.yaml.YAML()
    yaml.loader = NoBooleanLoader  
    try:
        with open(file_path, 'r') as file:
            yaml_data = yaml.load(file)
            if yaml_data is None:
                raise ValueError(f"YAML file '{file_path}' is empty or could not be parsed.")
            return yaml_data
    except Exception as e:
        print(f"Error loading YAML file '{file_path}': {e}")
        return None  # 处理错误时返回 None

# 保存修改后的 YAML 文件，保持顺序
def save_yaml(yaml_data, file_path):
    yaml = ruamel.yaml.YAML()
    with open(file_path, 'w') as file:
        yaml.dump(yaml_data, file)  # 不需要 Dumper 参数

# 读取 Excel 文件
def load_excel(file_path):
    df = pd.read_excel(file_path)
    return df

# 更新 YAML 中的 CommonConfig 参数
def update_common_config_from_excel(yaml_data, excel_data):
    filter_values = {}
    for index, row in excel_data.iterrows():
        parameter = row['Parameter']  # Excel 中的参数名
        new_value = row['Value']  # Excel 中的值

        # 跳过 NaN 值
        if pd.isna(new_value):
            print(f"Warning: New value for {parameter} is NaN. Skipping...")
            continue

        # 将参数路径分解为键
        keys = parameter.split(';')
        temp_data = yaml_data

        # 遍历键路径直到最后一个键
        for key in keys[:-1]:
            if key in temp_data:
                temp_data = temp_data[key]
            else:
                print(f"Warning: Key '{key}' not found in YAML data.")
                break
        else:
            last_key = keys[-1]
            
            # 获取当前 Channel 的名称
            if keys[1] == 'Channels':
                channel_name = keys[2]  # 假设 Channel 名称在第三个键
            else:
                channel_name = None

            # 处理 Channels 下的 Filter 参数
            if keys[:2] == ['CommonConfig', 'Channels'] and last_key.startswith('Filter'):
                filter_key = last_key
                print("filter_key: ", filter_key)

                # 初始化当前 Channel 的 Filter 值字典
                if channel_name not in filter_values:
                    filter_values[channel_name] = {}

                # 初始化当前 Filter 的值列表
                if filter_key not in filter_values[channel_name]:
                    filter_values[channel_name][filter_key] = [None, None]  # 初始化为两个 None 值
                    print(f"Initialized filter_values[{channel_name}][{filter_key}] = {filter_values[channel_name][filter_key]}")

                # 根据 Excel 数据的顺序替换第一个或第二个值
                if filter_values[channel_name][filter_key][0] is None:
                    filter_values[channel_name][filter_key][0] = new_value  # 替换第一个值
                    print(f"Set first value of filter_values[{channel_name}][filter_key] = {new_value}")
                else:
                    filter_values[channel_name][filter_key][1] = new_value  # 替换第二个值
                    print(f"Set second value of filter_values[{channel_name}][{filter_key}] = {new_value}")

                # 使用完整参数路径更新 YAML 数据
                try:
                    # 将更新后的值写入 YAML
                    temp_data[last_key] = filter_values[channel_name][filter_key]
                    print(f"Updated {parameter} in YAML to: {temp_data[last_key]}")
                except Exception as e:
                    print(f"Error updating {parameter} in YAML: {e}")

            else:
                # 处理其他普通参数
                if last_key in temp_data:
                    temp_data[last_key] = new_value
                else:
                    print(f"Warning: Key '{last_key}' not found in YAML data.")

    return yaml_data

# 更新 YAML 中的 SpecialConfig 参数
def update_special_config_from_excel(yaml_data, excel_data):
    print("******** SpecialConfig ********")
    du_ids = excel_data.columns[3:]  # 获取DU列，从第四列开始

    # 存储每个DU的Filter值
    filter_values = {}
    
    # 确保 SpecialConfig 节点存在于 yaml_data 中
    if 'SpecialConfig' not in yaml_data:
        yaml_data['SpecialConfig'] = {}

    # 删除不在 Excel 中的所有 DU
    existing_du_ids = set(yaml_data['SpecialConfig'].keys())
    excel_du_ids = set(map(str, du_ids))  # 转换为字符串集合
    for du_id in existing_du_ids - excel_du_ids:
        del yaml_data['SpecialConfig'][du_id]
        print(f"Removed DU '{du_id}' from SpecialConfig as it is not in Excel data.")

    # 遍历 Excel 数据中的每个 DU
    for du_id in du_ids:
        du_values = excel_data[du_id]  # 获取该DU的所有值
        du_id_str = str(du_id)  # 确保 DU ID 是字符串类型
        filter_values = {}  # 初始化当前DU的filter字典

        # 如果 yaml_data 中没有该 DU，则使用模板初始化
        if du_id_str not in yaml_data['SpecialConfig']:
            yaml_data['SpecialConfig'][du_id_str] = copy.deepcopy(du_template)  # 使用深拷贝确保模板内容完整
            print(f"Added DU '{du_id_str}' to SpecialConfig using the template.")

        # 遍历 Excel 的每一行数据，更新对应的 YAML 参数
        for index, new_value in du_values.items():
            du_specific_param_path = excel_data.iloc[index, 2]  # 第三列是DU的特定参数路径

            # 跳过 NaN 值
            if pd.isna(new_value):
                print(f"Warning: New value for {du_specific_param_path} under {du_id_str} is NaN. Skipping...")
                continue
            
            # 处理 ifopen 的情况
            if du_specific_param_path == 'SpecialConfig;ifopen':
                yaml_data['SpecialConfig']['ifopen'] = new_value  # 更新 ifopen 值
                print(f"Updated 'ifopen' for SpecialConfig to: {new_value}")
                continue  # 跳过后续处理

            # 使用DU-specific路径（第三列）更新该DU的特殊配置
            du_specific_keys = du_specific_param_path.split(';')
            
            # 插入 DU ID 到路径中
            if len(du_specific_keys) > 1 and du_specific_keys[0] == 'SpecialConfig' and du_specific_keys[1] != 'ifopen':
                du_specific_keys.insert(1, str(du_id))  # 将 du_id 转换为字符串类型
                
            # print("du_specific_keys: ", du_specific_keys)
            # print("du_specific_keys[1:-1]: ", du_specific_keys[1:-1])    

            # 开始遍历路径并更新数据
            temp_data = yaml_data  # 获取该 DU 的配置字典
            
            for key in du_specific_keys[:-1]:  # 不包含最后一个键
                if key in temp_data:
                    temp_data = temp_data[key]
                else:
                    print(f"Warning: SpecialConfig Key '{key}' not found in YAML data. Skipping...")
                    break
            else:
                last_key = str(du_specific_keys[-1])
                
                print("du_specific_keys: ", du_specific_keys)
                print("du_specific_keys[:2]: ", du_specific_keys[:2] )

                # 处理 SpecialConfig 下的 Channels 中的 Filter 参数
                
                if du_specific_keys[:3] == ['SpecialConfig', str(du_id), 'Channels'] and last_key.startswith('Filter'):
                    channel_name = du_specific_keys[3]  # 获取 Channel 名称
                    filter_key = last_key

                   # 初始化当前 Channel 的 Filter 值字典
                    if channel_name not in filter_values:
                        filter_values[channel_name] = {}

                    # 收集 Filter 的值，保持列表的两个值
                    if filter_key not in filter_values[channel_name]:
                        filter_values[channel_name][filter_key] = [None, None]  # 初始化为两个 None 值
                        print(f"Initialized filter_values[{filter_key}] = {filter_values[channel_name][filter_key]}")

                    # 根据 Excel 数据的顺序替换第一个或第二个值
                    if filter_values[channel_name][filter_key][0] is None:
                        filter_values[channel_name][filter_key][0] = new_value  # 替换第一个值
                        print(f"Set first value of filter_values[{channel_name}][{filter_key}] = {new_value}")
                    else:
                        filter_values[channel_name][filter_key][1] = new_value  # 替换第二个值
                        print(f"Set second value of filter_values[{channel_name}][{filter_key}] = {new_value}")

                    # 使用完整参数路径更新 YAML 数据
                    try:
                        # 将更新后的值写入 YAML
                        temp_data[last_key] = filter_values[channel_name][filter_key]
                        print(f"Updated {du_specific_param_path} in YAML to: {temp_data[last_key]}")
                    except Exception as e:
                        print(f"Error updating {du_specific_param_path} in YAML: {e}")

                    print("filter_key: ", filter_key)
                    print("du_specific_keys: ", du_specific_keys[:])
                    print(last_key, ": ", filter_values[channel_name][filter_key][0], ", ", filter_values[channel_name][filter_key][1])
                    # 将更新后的值写入 YAML，避免锚点
                    temp_data[last_key] = [filter_values[channel_name][filter_key][0], filter_values[channel_name][filter_key][1]]

                else:
                    # 更新普通参数
                    if isinstance(temp_data, dict) and last_key in temp_data:
                        temp_data[last_key] = new_value  # 更新值
                        print(f"SpecialConfig: Updated parameter '{du_specific_param_path}' for {du_id_str} to {new_value}")
                    else:
                        print(f"Warning: Last Key '{last_key}' not found in YAML data.")

    return yaml_data

# 强制 `ifopen` 顺序函数
def enforce_ifopen_order(yaml_data):
    # 确保 CommonConfig 中的 ifopen 排在最前面
    if 'CommonConfig' in yaml_data:
        common_config = yaml_data['CommonConfig']
        if 'ifopen' in common_config:
            ifopen_value = common_config.pop('ifopen')  # 移除并获取 ifopen
            common_config = {'ifopen': ifopen_value, **common_config}  # 将 ifopen 放到最前面
            yaml_data['CommonConfig'] = common_config

    # 确保 SpecialConfig 中的 ifopen 也排在最前面
    if 'SpecialConfig' in yaml_data:
        special_config = yaml_data['SpecialConfig']
        if 'ifopen' in special_config:
            ifopen_value = special_config.pop('ifopen')  # 移除并获取 ifopen
            special_config = {'ifopen': ifopen_value, **special_config}  # 将 ifopen 放到最前面
            yaml_data['SpecialConfig'] = special_config

    return yaml_data

# 自动添加或删除 DU 节点
def sync_du_nodes(yaml_data, excel_data):
    du_ids_in_excel = set(excel_data.columns[3:])  # Excel中的DU列集合，从第四列开始
    special_config_data = yaml_data.get('SpecialConfig', {})

    # 删除 YAML 中不存在于 Excel 的 DU 节点
    for du_id in list(special_config_data.keys()):
        if str(du_id) not in du_ids_in_excel:
            print(f"Removing DU {du_id} from YAML as it's not in Excel.")
            del special_config_data[du_id]

    # 添加 Excel 中存在但 YAML 中不存在的 DU 节点
    for du_id in du_ids_in_excel:
        if str(du_id) not in special_config_data:
            print(f"Adding DU {du_id} to YAML as it was found in Excel.")
            # 初始节点，不设置 ifopen
            special_config_data[str(du_id)] = copy.deepcopy(du_template)  # 使用深拷贝模板

    # 更新 YAML 数据
    yaml_data['SpecialConfig'] = special_config_data
    return yaml_data

# 主函数
def modify_yaml(yaml_file, excel_file, output_yaml_file):
    # 加载 YAML 和 Excel 文件
    yaml_data = load_yaml(yaml_file)
    excel_data = load_excel(excel_file)

    # 确保 SpecialConfig 节点存在
    if 'SpecialConfig' not in yaml_data:
        yaml_data['SpecialConfig'] = {}
        print("Created missing 'SpecialConfig' node in YAML data.")

    # 自动同步 YAML 中的 DU 节点
    yaml_data = sync_du_nodes(yaml_data, excel_data)

    # 更新 YAML 中的 CommonConfig 和 SpecialConfig 参数
    updated_yaml_data = update_common_config_from_excel(yaml_data, excel_data)
    updated_yaml_data = update_special_config_from_excel(updated_yaml_data, excel_data)

    # 强制确保 ifopen 的顺序
    updated_yaml_data = enforce_ifopen_order(updated_yaml_data)
    
    # 保存修改后的 YAML，保留顺序
    save_yaml(updated_yaml_data, output_yaml_file)


# 调用主函数
yaml_file = './DU-readable-conf.yaml'  # YAML 文件路径
excel_file = './multipleDUsConfigurationTest3.xlsx'  # Excel 文件路径
# output_yaml_file = 'updated_config4.yaml'  # 输出修改后的 YAML 文件路径
output_yaml_file = './DU-readable-conf.yaml' 

shutil.copyfile(yaml_file, 'backup_DU-readable-conf.yaml')

modify_yaml(yaml_file, excel_file, output_yaml_file)

# 创建 Excel 备份
shutil.copyfile(excel_file, 'backup_multipleDUsConfigurationTest.xlsx')
# shutil.copyfile(yaml_file, 'backup_DU-readable-conf.yaml')
