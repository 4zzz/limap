# this is our first build stage, it will not persist in the final image
FROM ubuntu as intermediate

# install git
RUN apt-get update && \
    apt-get install -y git

# add credentials on build
#ARG SSH_PRIVATE_KEY
RUN mkdir /root/.ssh/
#RUN echo "${SSH_PRIVATE_KEY}" > /root/.ssh/id_ed25519
#RUN chmod 600 /root/.ssh/id_ed25519

# make sure your domain is accepted
RUN touch /root/.ssh/known_hosts
RUN ssh-keyscan github.com >> /root/.ssh/known_hosts

RUN git clone --recursive https://github.com/cvg/limap

# From here, final image
FROM nvidia/cuda:11.6.1-devel-ubuntu20.04

ARG COLMAP_VERSION=3.8
# ARG CUDA_ARCHITECTURES=86
ARG CUDA_ARCHITECTURES=all

# Prevent stop building ubuntu at time zone selection.  
ENV DEBIAN_FRONTEND=noninteractive

# Prepare and empty machine for building.
RUN apt-get update && apt-get install -y \
    ca-certificates \
    gpg \
    git \
    ninja-build \
    build-essential \
    libboost-program-options-dev \
    libboost-filesystem-dev \
    libboost-graph-dev \
    libboost-system-dev \
    libboost-test-dev \
    libflann-dev \
    libfreeimage-dev \
    libmetis-dev \
    libgoogle-glog-dev \
    libgflags-dev \
    libsqlite3-dev \
    libglew-dev \
    qtbase5-dev \
    libqt5opengl5-dev \
    libcgal-dev \
    libceres-dev \
    wget \
    software-properties-common \
    lsb-core

# CMake Install (Default version does not meet the LIMAP requirement)
#RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | \
#    gpg --dearmor - | \
#    tee /etc/apt/trusted.gpg.d/kitware.gpg > \
#    /dev/null

RUN test -f /usr/share/doc/kitware-archive-keyring/copyright || \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

RUN echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ focal main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null
RUN apt-get update

RUN test -f /usr/share/doc/kitware-archive-keyring/copyright || \
    rm /usr/share/keyrings/kitware-archive-keyring.gpg

RUN apt-get install kitware-archive-keyring

#RUN apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" && \
#    apt update && \
#    apt install kitware-archive-keyring && \
#    rm /etc/apt/trusted.gpg.d/kitware.gpg && \
#    apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 42D5A192B819C5DA

RUN apt update && \
    apt install -y cmake

RUN wget https://answers.launchpad.net/ubuntu/+archive/primary/+sourcefiles/google-glog/0.4.0-4/google-glog_0.4.0.orig.tar.gz && \
    tar xzf google-glog_0.4.0.orig.tar.gz && \
    cd glog-0.4.0 && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make && \
    make install

# Build and install COLMAP.
RUN git clone https://github.com/colmap/colmap.git
RUN cd colmap && \
    git reset --hard ${COLMAP_VERSION} && \
    mkdir build && \
    cd build && \
    cmake .. -GNinja -DCMAKE_CUDA_ARCHITECTURES=${CUDA_ARCHITECTURES} && \
    ninja && \
    ninja install && \
    cd .. && rm -rf colmap

# Enable GUI
RUN apt-get update \
  && apt-get install -y -qq --no-install-recommends \
    libglvnd0 \
    libgl1 \
    libglx0 \
    libegl1 \
    libxext6 \
    libx11-6 \
  && rm -rf /var/lib/apt/lists/*

# Env vars for the nvidia-container-runtime.
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute

# PoseLib Dependency for LIMAP
RUN git clone --recursive https://github.com/vlarsson/PoseLib.git && \
    cd PoseLib && \
    mkdir build && cd build && \
    cmake .. && \
    make install -j8

RUN apt-get update && \
    apt-get install -y \
    libhdf5-dev

RUN wget http://ftp.debian.org/debian/pool/main/e/eigen3/libeigen3-dev_3.4.0-4_all.deb
RUN dpkg -i libeigen3-dev_3.4.0-4_all.deb

RUN apt-get install -y libsuitesparse-dev libatlas-base-dev
RUN git clone https://github.com/ceres-solver/ceres-solver
RUN cd ceres-solver && \
    git checkout 2.1.0 && \
    mkdir build && \
    cd build \
    && cmake .. \
    && make -j8 \
    && make install

# Only Python 3.9 seems to satisfy all dependencies
RUN add-apt-repository ppa:deadsnakes/ppa
RUN apt-get update && \
    apt-get install -y --fix-missing\
    python3.9-dev \
    python3.9-venv

ENV VIRTUAL_ENV=/opt/venv
RUN /usr/bin/python3.9 -m venv $VIRTUAL_ENV
ENV PATH="$VIRTUAL_ENV/bin:$PATH"

# Copy the repository from the first image
COPY --from=intermediate /limap /limap
RUN python -m pip install torch==1.12.0 torchvision==0.13.0 --index-url https://download.pytorch.org/whl/cu116
RUN python -m pip install --upgrade pip setuptools && \
    cd limap && \
    python --version && \
    pip --version && \
    python -m pip install -r requirements.txt && \
    python -m pip install -Ive .

