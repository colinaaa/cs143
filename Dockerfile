FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

RUN sed -i 's/archive.ubuntu.com/mirrors.tuna.tsinghua.edu.cn/g' \
    /etc/apt/sources.list

# Update system
RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get upgrade -y

# Install essentials
RUN apt-get install -y git flex bison build-essential wget vim libc6:i386

# Install coolc compiler
WORKDIR /usr/class/cs143/cool
COPY --chown=root:root . .
RUN echo "export PATH=$PATH:/usr/class/cs143/cool/bin" >> /root/.bashrc
RUN make install

# Cleanup
RUN apt-get autoremove && apt-get autoclean
RUN rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

WORKDIR /root/cs143

ENTRYPOINT ["/bin/bash"]
