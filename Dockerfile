ARG ROS_DISTRO=humble

FROM ros:${ROS_DISTRO}-ros-base

ARG USERNAME=ros2user
ARG USER_UID=1000
ARG USER_GID=${USER_UID}

# Create the user
RUN groupadd --gid ${USER_GID} ${USERNAME}
RUN useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME}

# Add sudo support for user
RUN apt-get -y update
RUN apt-get install -y sudo
RUN echo ${USERNAME} ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/${USERNAME}
RUN chmod 0440 /etc/sudoers.d/${USERNAME}

# Install apt-get packages
RUN apt-get update && apt-get install -y --no-install-recommends \
    # For Livox-SDK
    ros-${ROS_DISTRO}-pcl-ros \
    # For Fast-LIVO2
    ros-${ROS_DISTRO}-cv-bridge \
    ros-${ROS_DISTRO}-image-transport \
    ros-${ROS_DISTRO}-sophus \
    # For faster ROS middleware
    ros-${ROS_DISTRO}-rmw-cyclonedds-cpp \
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
    sudo make install

ENV RMW_IMPLEMENTATION=rmw_cyclonedds_cpp

USER ${USERNAME}
RUN echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> ~/.bashrc
RUN echo "source /usr/share/colcon_argcomplete/hook/colcon-argcomplete.bash" >> ~/.bashrc
ENV SHELL /bin/bash
