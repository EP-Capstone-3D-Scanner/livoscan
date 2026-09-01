FROM nvcr.io/nvidia/l4t-jetpack:r36.4.0

ARG DEBIAN_FRONTEND=noninteractive

ARG ROS_DISTRO=humble

# ROS2 installation
RUN apt-get update && apt-get install -y --no-install-recommends \
    software-properties-common \
    curl \
    && add-apt-repository -y universe \
    && export ROS_APT_SOURCE_VERSION=$(curl -s https://api.github.com/repos/ros-infrastructure/ros-apt-source/releases/latest | grep -F "tag_name" | awk -F'"' '{print $4}') \
    && curl -L -o /tmp/ros2-apt-source.deb "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.$(. /etc/os-release && echo ${UBUNTU_CODENAME:-${VERSION_CODENAME}})_all.deb" \
    && dpkg -i /tmp/ros2-apt-source.deb \
    && rm -f /tmp/ros2-apt-source.deb \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends \
    ros-${ROS_DISTRO}-desktop \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends \
    ros-dev-tools \
    build-essential \
    libjemalloc2 \
    && rm -rf /var/lib/apt/lists/*

ENV LD_PRELOAD=/usr/lib/aarch64-linux-gnu/libjemalloc.so.2

# ZED SDK installation
RUN apt-get update && apt-get install -y --no-install-recommends \
    wget \
    zstd \
    && rm -rf /var/lib/apt/lists/*

RUN wget -q -O ZED_SDK_Linux.run "https://download.stereolabs.com/zedsdk/5.4/l4t36.4/jetsons" \
    && chmod +x ZED_SDK_Linux.run \
    && ./ZED_SDK_Linux.run silent skip_cuda skip_od_module skip_python skip_hub \
    && rm ZED_SDK_Linux.run

# Install package dependencies using rosdep
RUN apt-get update && apt-get install -y --no-install-recommends \
    python3-rosdep \
    && rm -rf /var/lib/apt/lists/* \
    && rosdep init || true \
    && rosdep update

RUN mkdir -p /tmp/dummy_ws/src \
    && cd /tmp/dummy_ws/src \
    && git clone --recursive https://github.com/stereolabs/zed-ros2-wrapper.git \
    && cd /tmp/dummy_ws \
    && apt-get update \
    && rosdep install --from-paths src --ignore-src -r -y --rosdistro ${ROS_DISTRO} \
    && rm -rf /tmp/dummy_ws \
    && rm -rf /var/lib/apt/lists/*

# Install apt-get packages that are dependencies for install files
RUN apt-get update && apt-get install -y --no-install-recommends \
    # For Livox-SDK
    ros-${ROS_DISTRO}-pcl-ros \
    \
    && rm -rf /var/lib/apt/lists/*

# Clone files for installation
RUN mkdir -p /home/install_files/source
RUN mkdir -p /home/install_files/build
COPY ./docker_build_files /home/install_files/source

# Install Livox-SDK
RUN set -ex; \
    cd /home/install_files/build; \
    mkdir Livox-SDK; \
    cd Livox-SDK; \
    cmake /home/install_files/source/Livox-SDK -DCMAKE_POSITION_INDEPENDENT_CODE=ON; \
    make -j$(nproc); \
    make install


# Install apt-get packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    # For V4l2 Camera
    # ros-${ROS_DISTRO}-v4l2-camera \
    # For usb_cam
    ros-${ROS_DISTRO}-usb-cam \
    ros-${ROS_DISTRO}-image-pipeline \
    # For Fast-LIVO2
    ros-${ROS_DISTRO}-cv-bridge \
    ros-${ROS_DISTRO}-image-transport \
    ros-${ROS_DISTRO}-sophus \
    # For faster ROS middleware
    ros-${ROS_DISTRO}-rmw-cyclonedds-cpp \
    # For livoscan
    ros-${ROS_DISTRO}-rviz2 \
    ros-${ROS_DISTRO}-rqt* \
    \
    && rm -rf /var/lib/apt/lists/*

# User setup
# ARG USERNAME=ros2user
# ARG USER_UID=1000
# ARG USER_GID=${USER_UID}

# # Create the user
# RUN groupadd --gid ${USER_GID} ${USERNAME}
# RUN useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME}

# # Add sudo support for user
# RUN apt-get update && apt-get install -y --no-install-recommends \
#     sudo \
#     && rm -rf /var/lib/apt/lists/*

# RUN echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME}
# RUN chmod 0440 /etc/sudoers.d/${USERNAME}

# USER ${USERNAME}
RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc
RUN echo "source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash" >> ~/.bashrc
ENV RMW_IMPLEMENTATION=rmw_cyclonedds_cpp
ENV SHELL /bin/bash
