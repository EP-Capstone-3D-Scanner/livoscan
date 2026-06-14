FROM ros:humble-ros-base

ARG USERNAME=ros2user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME
RUN useradd --uid $USER_UID --gid $USER_GID -m $USERNAME

# Add sudo support for user
RUN apt-get -y update
RUN apt-get install -y sudo
RUN echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME
RUN chmod 0440 /etc/sudoers.d/$USERNAME
RUN apt-get update && apt-get upgrade -y

RUN echo "source /opt/ros/humble/setup.bash" >> /home/$USERNAME/.bashrc
ENV SHELL /bin/bash
