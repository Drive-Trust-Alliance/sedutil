FROM ubuntu:16.04

RUN dpkg --add-architecture i386 && apt update && apt install -y \
        bc \
        build-essential \
        cpio \
        dosfstools \
        g++-multilib \
        gdisk \
        git-core \
        libncurses5-dev \
        libncurses5-dev:i386 \
        python \
        squashfs-tools \
        sudo \
        unzip \
        wget \
        locales \
    && rm -rf /var/lib/apt/lists/*

RUN locale-gen en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US:en
ENV LC_ALL en_US.UTF-8

WORKDIR "/sedutil"

CMD "/sedutil/images/autobuild.sh"
