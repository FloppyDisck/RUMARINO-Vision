FROM nvidia/cuda:10.2-cudnn8-devel-ubuntu18.04 AS base 

#TODO: USE VARIABLES TO SUBSTITUTE VERSIONS 
ARG CUDA_MAJOR=10
ARG CUDA_MINOR=2
ARG UBUNTU_RELEASE_YEAR=18

ARG ZED_SDK_MAJOR=3
ARG ZED_SDK_MINOR=4
ARG OPENCV_VERSION=4.2.0
ARG OPENCV_INSTALL_PATH=/usr/local


#Testing
ENV NVIDIA_DRIVER_CAPABILITIES \
    ${NVIDIA_DRIVER_CAPABILITIES:+$NVIDIA_DRIVER_CAPABILITIES,}compute,video,utility

WORKDIR /
#Essentials. 
RUN \
	apt-get update -y && \
	apt-get autoremove -y && \ 
	apt-get install -y --no-install-recommends\
	build-essential \
	cmake \ 
	git \ 
	wget \ 
	unzip \
	yasm \ 
	pkg-config \ 
	checkinstall \ 
	gcc-7 g++-7 g++-8 \
	libjpeg-dev \
	libpng-dev libtiff-dev \ 
	ffmpeg x264 libx264-dev \
	libavcodec-dev libavformat-dev libswscale-dev \ 
	libavresample-dev \
	libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
	libxvidcore-dev x264 libx264-dev libfaac-dev\
	libmp3lame-dev libtheora-dev \
	libfaac-dev libmp3lame-dev libvorbis-dev \
	#OpenCore Speech Codec 
	libopencore-amrnb-dev libopencore-amrwb-dev \ 
	#Camera programming interface libs
	libdc1394-22 libdc1394-22-dev libxine2-dev libv4l-dev v4l-utils \
	############################TODO SYMBOLIC LINK
	&& cd /usr/include/linux \
	&& ln -s -f ../libvl1-videodev.h videodev.h && \
	cd && \
	############################
	# GTK lib for GUI functionalities
	apt-get install -y libgtk-3-dev \
	python3-dev python3-pip python3-testresources \ 
	#C++ parallelism
	libtbb-dev \ 
	#Optimization libs for OpenCV 
	libatlas-base-dev gfortran \ 
	#Install optional libs
	libprotobuf-dev protobuf-compiler \ 
	libgoogle-glog-dev libgflags-dev \
	libgphoto2-dev libeigen3-dev libhdf5-dev doxygen && \
	rm -rf /var/lib/apt/lists/*

#update-alternatives
RUN \
	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 7 && \
	update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 7 && \
	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 8 && \
	update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 8 && \
	rm -rf /var/lib/apt/lists/*

#OpenCV Download and Installation
#TODO: USE ADD COMMAND FOR DOWNLOADING OPENCV
# ADD https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip .
# RUN unzip ${OPENCV_VERSION}.zip && rm ${OPENCV_VERSION}.zip
# ADD https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip .
# RUN unzip ${OPENCV_VERSION}.zip && rm ${OPENCV_VERSION}.zip

RUN \
	mkdir -p ${OPENCV_INSTALL_PATH} ; exit 0 

WORKDIR / 

RUN \
	echo "Downloading OpenCV" && \
	cd && \
	cd && \
	wget -O opencv.zip https://github.com/opencv/opencv/archive/${OPENCV_VERSION}.zip && \
	#rm -r opencv-4.2.0 && \
	unzip -o opencv.zip && \
	rm opencv.zip && \
	wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/${OPENCV_VERSION}.zip && \ 
	#rm -r opencv_contrib-4.2.0 && \
	unzip -o opencv_contrib.zip && \ 
	rm opencv_contrib.zip && \
	echo "Installing OpenCV" && \
	cd opencv-${OPENCV_VERSION} && \ 
	mkdir build && \ 
	cd build && \ 
	cmake -D CMAKE_BUILD_TYPE=RELEASE \
	-D CMAKE_INSTALL_PREFIX=$OPENCV_INSTALL_PATH \
	-D INSTALL_PYTHON_EXAMPLES=ON \
	-D INSTALL_C_EXAMPLES=ON \
	-D WITH_TBB=ON \
	-D BUILD_opencv_cudacodec=OFF \
	-D ENABLE_FAST_MATH=1 \
	-D CUDA_FAST_MATH=1 \
	-D WITH_CUDA=ON \
	-D WITH_CUBLAS=1 \
	-D WITH_V4L=ON \
	-D WITH_QT=OFF \
	-D WITH_OPENGL=ON \
	-D WITH_GSTREAMER=ON \
	-D OPENCV_GENERATE_PKGCONFIG=ON \
	-D OPENCV_PC_FILE_NAME=opencv.pc \
	-D OPENCV_ENABLE_NONFREE=ON \
	-D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib-${OPENCV_VERSION}/modules \
	-D BUILD_EXAMPLES=ON \
	-D WITH_CUDNN=ON \
	-D OPENCV_DNN_CUDA=ON \
	-D CUDA_ARCH_BIN=7.5 .. && \
	make -j4 && \ 
	make install && \ 
	ldconfig


WORKDIR / 
# ADD https://download.stereolabs.com/zedsdk/3.4/cu110/ubuntu18 . 
# Setup the ZED SDK
RUN apt-get update -y && apt-get install --no-install-recommends lsb-release wget less udev sudo  build-essential cmake -y && \
    wget -q -O ZED_SDK_Linux_Ubuntu${UBUNTU_RELEASE_YEAR}.run https://download.stereolabs.com/zedsdk/${ZED_SDK_MAJOR}.${ZED_SDK_MINOR}/cu${CUDA_MAJOR}${CUDA_MINOR}/ubuntu${UBUNTU_RELEASE_YEAR} && \
    chmod +x ZED_SDK_Linux_Ubuntu${UBUNTU_RELEASE_YEAR}.run ; ./ZED_SDK_Linux_Ubuntu${UBUNTU_RELEASE_YEAR}.run -- silent skip_tools && \
    rm ZED_SDK_Linux_Ubuntu${UBUNTU_RELEASE_YEAR}.run && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update -y && apt-get install --no-install-recommends python3 python3-pip -y && \ 
    wget download.stereolabs.com/zedsdk/pyzed -O /usr/local/zed/get_python_api.py && \
    python3 /usr/local/zed/get_python_api.py && \
    python3 -m pip install numpy opencv-python *.whl && \
    rm *.whl ; rm -rf /var/lib/apt/lists/*

#Build Cmake from source; required for newest version 
ARG CMAKE_MAJOR=3
ARG CMAKE_MINOR=16.5
RUN apt-get update -y && apt remove --purge cmake -y \
	&& hash -r \
	&& apt-get -y install --no-install-recommends build-essential libssl-dev\ 
	&& wget https://github.com/Kitware/CMake/releases/download/v3.16.5/cmake-${CMAKE_MAJOR}.${CMAKE_MINOR}.tar.gz \ 
	&& tar -zxvf cmake-${CMAKE_MAJOR}.${CMAKE_MINOR}.tar.gz \
	&& cd cmake-${CMAKE_MAJOR}.${CMAKE_MINOR} \ 
	&& bash bootstrap \ 
	&& make \ 
	&& make install 

#DARKNET
COPY setup_darknet.sh .
# RUN setup_darknet.sh
#  RUN git clone https://github.com/FloppyDisck/darknet.git \
#  	&& cd darknet \ 
#  	&& make -j5 \ 
#  	&& sudo cp libdarknet.so /usr/local/lib \ 
#  	&& sudo cp include/yolo_v2_class.hpp /usr/local/include 

#API DEPENDENCIES
#cpp-httplib 
COPY setup_httplib.sh .
# RUN setup_httplib.sh  
#  RUN git clone https://github.com/yhirose/cpp-httplib.git \ 
#  	&& cd cpp-httplib \ 
#  	&& mkdir -p build && cd build \ 
#  	&& cmake -DCMAKE_BUILD_TYPE=Release -DHTTPLIB_COMPILE=on -DBUILD_SHARED_LIBS=on .. \
#  	&& cmake --build . --target install 

#json 
#  RUN git clone https://github.com/nlohmann/json.git \ 
#  	&& cd json \ 
#  	&& mkdir -p build && cd build \ 
#  	&& cmake -DCMAKE_BUILD_TYPE=Release .. \
#  	&& make install -j5 

EXPOSE 8080:8080

WORKDIR /RUMARINO 
