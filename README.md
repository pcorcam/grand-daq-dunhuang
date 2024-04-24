 This webpage provides the installation steps for setting up the GRAND environment on a new host. You can access the English version of the webpage through the following link:
 
https://blog.csdn.net/qq_43085476/article/details/134203475?spm=1001.2014.3001.5501

## 简要说明 A brief explanation
  - 为了方便未来统一使用软件，建议将每台主机的名称命名为“grand”。随着未来可能增加的主机数量，可以采用类似“grand1”的序列号命名。这里不详细展开说明，将在后续的使用说明中介绍。在实际运行数据采集（DAQ）时，除了软件负责人之外，其他成员将不会被授予权限。
 
 	For the sake of convenience in future uniform software usage, it is suggested to name each host as 'grand'. As the number of hosts may increase in the future, a naming convention similar to 'grand1' can be adopted for serialization. Detailed explanation is not provided here but will be covered in subsequent usage instructions. During the actual operation of data acquisition (DAQ), permissions will not be granted to any member other than the software administrator.
  - 软件根目录:
  	Software root directory:
    ```bash 
    ${GRAND_DAQ_DIRECTORY} = /home/grand/workarea/
    mkdir -p /home/grand/workarea/ # 创建软件根目录 create the root directory
    ```
  - 创建存储原始数据的目录raw_data以及存储t3触发后的数据目录t3_data，路径为:
  	Create a directory named 'raw_data' to store raw data and another directory named 't3_data' to store data triggered by T3. The paths are:
    ```bash 
    ${raw_data_file_path} = ${GRAND_DAQ_DIRECTORY}/raw_data
    ${t3_data_file_path} = ${GRAND_DAQ_DIRECTORY}/t3_data 
   - 创建外部软件库：
   	Create an external software library:
		```bash
		  	mkdir /home/grand/pc_installed
		  	mkdir /home/grand/arm_a_installed
		   ```
## 在gitee上下载grand-daq软件源码，请下载最新版本，这里以grand-daq_V1.2.0为例 Download software source code from Gitee, an example for version grand-daq_V1.2.0
```shell
cd ${GRAND_DAQ_DIRECTORY}
git clone -b grand-daq-V1.2.0 https://gitee.com/duanbh/grand-daq-dunhuang.git
mv grand-daq-dunhuang grand-daq
```
https://gitee.com/duanbh/grand-daq-dunhuang.git
## 新主机的环境配置过程 Environment configuration process for the new host
1. **安装make和cmake**       
	**Install make and Cmake** 
    ```shell
    sudo apt-get install make
    sudo apt-get install cmake
    ```
2. **安装C++环境**
	**Install the environment for C++**
	```bash
	sudo apt-get install g++ # g++ version should be 7.3 or higher
	```
3. **删除 'build-x86_64' 和 'build-arm' 目录中的 'CmakeCache.txt' 文件。如果这两个文件夹存在，则执行此操作**
**Delete the 'CmakeCache.txt' file in the 'build-x86_64' and 'build-arm' directories. Execute this operation if these two folders exist.**
	```shell
	rm /home/grand/workarea/grand-daq/build-x86_64/CMakeCache.txt # 该文件含有旧的编译信息
	rm /home/grand/workarea/grand-daq/build-arm/CMakeCache.txt
	```
4. **搭建zmq环境，需要下载ZeroMQ的安装包**
   **To set up the ZMQ environment, you need to download the installation package for ZeroMQ.**
   > 建议将下载的外部软件放在目录 /home/grand/externals(It is recommended to place downloaded external software in a directory **/home/grand/externals**)：
   > ```bash
   > mkdir -p /home/grand/externals

   - Zeromq版本对于Ubuntu 18.04以及Ubuntu20.04只要版本高于4.3.4均可，如果使用更新的Linux系统，为了保证正常使用，需要自行下载最新版的Zeromq，链接为：<font color=red> https://gitee.com/mirrors/libzmq.git</font>，这里以Ubuntu 18.04安装作为示例
   (For Ubuntu 18.04 and Ubuntu 20.04, any version higher than 4.3.4 of ZeroMQ should suffice. However, for updated Linux systems, it's recommended to manually download the latest version of ZeroMQ to ensure smooth operation, link follows: **https://gitee.com/mirrors/libzmq.git**, here's an example of installing ZeroMQ on Ubuntu 18.04.)
   
   - 安装步骤 (steps for install ZeroMQ)
   		- 64位linux主机安装 (for linux x86-64 machine)
   	 ```shell
   		cd /home/grand/externals
	    git clone https://gitee.com/mirrors/libzmq.git
	    tar -xzvf  libzmq-4.3.4.tar.gz . # 解压 extract
	    cd libzmq-4.3.4 # 进入目录 enter the directory 
	    ./autogen.sh #开始安装
	    ./configure --prefix=/home/grand/pc_installed --without-libsodium #使用prefix来指定安装目录：
	    make -j8
	    make install
    	```
   		- arm安装(for arm machine) 	
    	> 指定交叉编译工具 ：aarch64-linux-gnu-g++与aarch64-linux-gnu-gcc
    	Specify Cross-Compilation Tool: aarch64-linux-gnu-g++ and aarch64-linux-gnu-gcc
    	```shell
    	sudo apt-get install g++-aarch64-linux-gnu # 下载交叉编译工具 download the cross compile tool
		aarch64-linux-gnu-g++ --vresion # 查看版本 check the version 
		cd /home/grand/externals/libzmq-4.3.4 
		make clean
		./autogen.sh
		sudo find / -name aarch64-linux-gnu-g++ # 寻找交叉编译工具的路径，此处以/usr/bin/aarch64-linux-gnu-gcc为例 find the path of Cross-Compilation Tool，"/usr/bin/aarch64-linux-gnu-gcc" for example
		./configure --host=aarch64-linux CC=/usr/bin/aarch64-linux-gnu-gcc CXX=/usr/bin/aarch64-linux-gnu-g++   --prefix=/home/grand/arm_a_installed/ --without-libsodium
		make -j8
		make install
    	```
		-  解决问题的思路在于使用与物理机相匹配的编译工具进行编译
	Solution Approach: Compile with Compilation Tools Compatible with the Physical Machine
5. **搭建yaml环境，以yaml-cpp-yaml-cpp-0.7.0.tar.gz为例**
	**Setting up a YAML environment, using yaml-cpp-yaml-cpp-0.7.0.tar.gz as an example**
	> 同样，将yaml压缩包放在/home/grand/externals路径下
	> Similarly, place the YAML compressed file in the path /home/grand/externals.
	- 64位linux主机安装 (for linux x86-64 machine)
	> 下载链接: https://gitee.com/mirrors/yaml-cpp.git
	> download link: https://gitee.com/mirrors/yaml-cpp.git
	```shell
	cd /home/grand/externals
	git clone https://gitee.com/mirrors/yaml-cpp.git
	tar -xzvf yaml-cpp-yaml-cpp-0.7.0.tar.gz
	cd yaml-cpp-yaml-cpp-0.7.0
    mkdir build
	```
	修改 CMakeLists.txt (modify the CMakeLists.txt)
	```bash
	# 添加如下两行命令 
	add_definitions(-w) // 忽略dummy错误 ignore the dummy mistakes
	link_directories( /home/grand/pc_installed/lib ) // 指定外部链接库 Specify External Linking Libraries
	```
	进入build目录并指定安装目录 Enter the build directory and specify the installation directory
    ```shell
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=/home/grand/pc_installed -DBUILD_SHARED_LIBS=ON  ..
    # -DCMAKE_INSTALL_PREFIX=/path/to/where/you/want 指定安装目录 Specify the Installation Directory
    # 或者在CMake文件中添加 SET(CMAKE_INSTALL_PREFIX < install_path >), 要加在 PROJECT(< project_name>) 之后。 Alternatively, add SET(CMAKE_INSTALL_PREFIX <install_path>) in the CMake file, placing it after PROJECT(<project_name>)
    # -DBUILD_SHARED_LIBS=ON 开启动态链接库编译，默认关闭 Enable dynamic library compilation by default, disabled by default
    ```
    开始编译 To begin compilation:
    ```shell
    make -j8
    sudo make install
    sudo ldconfig
    ```
   	 - arm安装(for arm machine)
   	 在已有对 CMakeLists.txt修改的基础上，指定C++和C的编译器为交叉编译工具
	   	 ```shell
	   	 # 必须加在project(< project_name>)之前
	   	 set(CMAKE_SYSTEM Linux)
	   	 set(CMAKE_SYSTEM_PROCESSOR arm)
	   	 set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc) # 交叉编译工具路径与安装zeroMQ时保持一致
	   	 set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)
		# 同时修改link_directories的路径为/home/grand/arm_a_installed/lib
		link_directories( /home/grand/arm_a_installed/lib )
	   	 ```
	   	 编译
	   	 ```shell
	   	 cd build
	   	 make clean
	   	 cmake -DCMAKE_INSTALL_PREFIX=/home/grand/arm_a_installed -DBUILD_SHARED_LIBS=ON ..
	   	 make -j8
		sudo make install
		sudo ldconfig
	   	 ```
6. 编译 **grand-daq**  
compile **grand-daq**

- 执行脚本 execute the script
	```bash
		cd /home/grand/workarea/grand-daq
		sh duantest.sh
	```
- 可能遇到的问题 possible issues that may arise :
	- 如果遇到下述问题，则是库文件与系统环境不兼容的问题，简单来说是x86-64版本只适合64位的主机，而arm上需要的则是交叉编译后的库文件 
	If encountering the following issue, it is likely due to incompatibility between the library files and the system environment. Simply put, the x86-64 version is suitable for 64-bit hosts, while what's needed for ARM are the library files compiled through cross-compilation

		1. ERROR: skipping incompatible
		解决方法(solutions)
		首先查看链接库相关信息来确定猜测(First, examine the information related to the linked libraries to confirm the assumption.)
			```bash
			readelf -h libxxx.so
			```
			使用交叉编译工具对工具进行重新编译(ZeroMQ, yaml)，或者找到交叉编译工具内的库文件，笔者使用的交叉编译工具为(Recompile the tools using a cross-compilation tool (ZeroMQ, yaml), or find the library files within the cross-compilation tool. The cross-compilation tool I'm using is)
		 **/tools/Xilinx/SDK/2018.3/gnu/aarch64/lin/aarch64-linux/bin/aarch64-linux-gnu-g++**
		 但此工具需要事先下载Xilinx，并不通用(But this tool requires pre-downloading Xilinx and is not universal)
		-	下载aarch64-linux-gnu-g++ (download aarch64-linux-gnu-g++)
			```bash
			sudo apt-get install g++-aarch64-linux-gnu
			aarch64-linux-gnu-g++ --vresion(查看版本 check the version)
			```

		2. cannot open linker script file ****: Too many open files，解决办法同上(The solution is the same as above)
		3. 软件打包时可能遇到如图所示的情况，arm.tar.gz与x86_64.tar.gz只有几百K大小，理想情况应为6M左右
When packaging software, you may encounter a situation as shown in the image, where 'arm.tar.gz' and 'x86_64.tar.gz' are only a few hundred kilobytes in size, whereas ideally, they should be around 6 megabytes.
![在这里插入图片描述](https://img-blog.csdnimg.cn/8e55d5043edd45a4a319bc6fb32028e7.png#pic_center)
此时应仔细检查打包脚本中的库文件等在主机上是否存在，如下图所示 
Please carefully check whether the library files and other dependencies mentioned in the packaging script exist on the host machine, as shown in the image below.
![在这里插入图片描述](https://img-blog.csdnimg.cn/09e3b87b4fed4bcdb91b4e1f26886dd5.png#pic_center)
	修改 **/home/grand/workarea/grand-daq/package.sh** 的信息 modify the information in file **/home/grand/workarea/grand-daq/package.sh**
			```shell
			cd /home/grand/arm_a_installed/lib # 进入此lib目录，比对 /home/grand/workarea/grand-daq/package.sh 文件中EXTERNAL_LIB_ARM双引号包括的内容，例如：EXTERNAL_LIB_ARM="libstdc++.so.6.0.24 libstdc++.so.6 libstdc++.so libgcc_s.so.1 libgcc_s.so libzmq.so libzmq.so.5 libzmq.so.5.2.4" 找到相应的库文件并修改名称以匹配。这种情况通常是由于在不同系统中下载 ZeroMQ（zmq） 时出现了库版本不同的问题，因此在打包时未能将所需的库文件一并打包进去。
			# Go into this 'lib' directory and compare the content within the double quotes of '/home/grand/workarea/grand-daq/package.sh' under 'EXTERNAL_LIB_ARM,' for instance: 'EXTERNAL_LIB_ARM="libstdc++.so.6.0.24 libstdc++.so.6 libstdc++.so libgcc_s.so.1 libgcc_s.so libzmq.so libzmq.so.5 libzmq.so.5.2.4"'. Find the corresponding library files and modify their names to match. This scenario typically occurs due to different versions of ZeroMQ (zmq) downloaded on different systems, causing necessary library files not to be included in the packaging.
			cd /home/grand/pc_installed/lib # 同上 same as before
			```
7. 其他 other content
	- 更换主机后，若主机名称不是grand，则需要进行以下步骤
	After changing the host, if the hostname is not 'grand,' the following steps need to be taken
	> 注意：user需使用用户主机名称替换
	> Note: Replace 'user' with your host's username.
	```bash
	1. cd ${GRAND_DAQ_DIRECTORY}/grand-daq
	2. vi CMakeLists.txt 
	# 修改以下两处：
		# 1. 18行：set(EXTERNAL_DIR_ARM /home/grand/arm_a_installed) 与 	
		# 2. 19行：set(EXTERNAL_DIR_PC /home/grand/pc_installed) 中的grand替换至user，即/home/grand/arm_a_installed修改至/home/user/arm_a_installed
	4. vi ./package.sh 
	# 修改以下两处：
		# 1. 3行：EXTERNAL_DIR_PC=/home/grand/pc_installed 
		# 2. 4行：EXTERNAL_DIR_ARM=/home/grand/arm_a_installed 中的grand替换至user，即/home/grand/pc_installed修改至/home/user/pc_installed
	5. vi env.sh 
	# 将1,7,8,14行的/home/grand 修改为 /home/user，
	```
	- 如果使用yaml-cpp-release-0.5.0，那么可能会遇到如下问题：
	  If using yaml-cpp-release-0.5.0, you might encounter the following issues
	    - 找不到boost依赖库
	      Cannot find Boost dependencies.
      	- 首先安装boost：
      	  First, install Boost
	        ```bash
	        sudo apt-get install libboost-all-dev
	        ```
      - 安装完成后没有效果，则先查找boost_thread库，然后修改CMakeList文件，指定boost库所在的路径
      After installation without effect, first search for the 'boost_thread' library, then modify the 'CMakeLists.txt' file to specify the path where the Boost library is located
        ```bash
        locate boost_thread
        set(BOOST_LIBRARYDIR /usr/lib) # 假设boost所在位置为/usr/lib/x86_64-linux-gnu/libboost_thread.a
        find_package(Boost REQUIRED COMPONENTS thread)
        ```
      - boost库存在，但是在调用相应文件时显示：error:'next' is not a member of 'boost'
      Boost library exists, but when calling the respective file, it shows an error: 'next' is not a member of 'boost'.
      - 在找不到next的文件中添加头文件
      	Add header files to the file where 'next' cannot be found.  
	      ```bash
	      vi /home/xuxing/externalTools/DBH/yaml-cpp-release-0.5.0/include/yaml-cpp/node/detail/iterator.h
	      ```
	      ```cpp
	      #include <boost/next_prior.hpp> // boost所在头文件
	      ```
	 - 其他工具（非必要） other tools (unnecessary)
	 	1. **iperf**
		将arm可以使用的iperf工具拷贝在文件夹iperf_for_arm中备用
		Copy the iperf tool that can be used on ARM into the folder 'iperf_for_arm' for future use
		2. **配置arm上的C++环境，步骤如下：**
		Configuring the C++ environment on ARM follows these steps:
			```shell
			scp /tools/Xilinx/SDK/2018.3/gnu/aarch64/lin/aarch64-linux/aarch64-linux-gnu/lib64/libstdc++.so.6.0.24 root@192.168.10.2:/lib/ 
			scp /tools/Xilinx/SDK/2018.3/gnu/aarch64/lin/aarch64-linux/aarch64-linux-gnu/lib64/libgcc_s.so.1 root@192.168.10.2:/lib/
			
			ssh root@<arm-host>
			cd /lib
			ln -s libstdc++.so.6.0.24 libstdc++.so.6 # ln -s origin goal
			ln -s libstdc++.so.6 libstdc++.so
			ln -s libgcc_s.so.1 libgcc_s.so
			```
			详情见step_for_cpp.txt，内容如下
			See 'step_for_cpp.txt' for details, contents as follows
			```shell
			3. scp /tools/Xilinx/SDK/2018.3/gnu/aarch64/lin/aarch64-linux/aarch64-linux-gnu/lib64/libstdc++.so.6.0.24 root@192.168.61.xx:/lib/
			4. scp /tools/Xilinx/SDK/2018.3/gnu/aarch64/lin/aarch64-linux/aarch64-linux-gnu/lib64/libgcc_s.so.1 root@192.168.61.xx:/lib/
			5. ssh root@<arm-host>
			6. cd /lib/
			7. # 寻找c++库然后建立软链接 Search for the C++ library and then create a symbolic link.
			8. ln -s libstdc++.so.6.0.24 libstdc++.so.6
			9. ln -s libstdc++.so.6 libstdc++.so
			10. ln -s libgcc_s.so.1 libgcc_s.so
			```
	  - problems:
	  ```shell
	 	cp: cannot stat '/home/grand/arm_a_installed/lib/libstdc++.so.6.0.24': No such file or directory
		cp: cannot stat '/home/grand/arm_a_installed/lib/libstdc++.so.6': No such file or directory
		cp: cannot stat '/home/grand/arm_a_installed/lib/libstdc++.so': No such file or directory
		cp: cannot stat '/home/grand/arm_a_installed/lib/libgcc_s.so.1': No such file or directory
		cp: cannot stat '/home/grand/arm_a_installed/lib/libgcc_s.so': No such file or directory
		cp: cannot stat '/home/grand/arm_a_installed/lib/libzmq.so.5.2.4': No such file or directory
		cp: cannot stat '/home/grand/arm_a_installed/bin': No such file or directory
	  ```
8. **grand-daq** 软件使用说明 Instructions for using the **grand-daq** software
请访问此链接：**https://jupyter.ihep.ac.cn/NAqWt4YzQJKj3yQmjp1awA?view**
please enter this link: **https://jupyter.ihep.ac.cn/NAqWt4YzQJKj3yQmjp1awA?view*

如有其他问题，仍会继续更新。
Updates will continue.


