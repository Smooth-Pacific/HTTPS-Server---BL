
FROM ubuntu:20.04

WORKDIR /home

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Installs 
RUN apt-get update -y
RUN apt-get install -y autoconf
RUN apt-get install -y ssh
RUN apt-get install -y sudo
RUN apt-get install -y zsh
RUN apt-get install -y zsh-autosuggestions
RUN apt-get install -y git
RUN apt-get install -y curl
RUN apt-get install -y wget
RUN apt-get install -y vim
RUN apt-get install -y tree
RUN apt-get install -y zip
RUN apt-get install -y unzip
RUN apt-get install -y pkg-config
RUN apt-get install -y libtool
RUN apt-get install -y m4
RUN apt-get install -y gnutls-bin
RUN apt-get install -y automake
RUN apt-get install -y libmicrohttpd-dev
RUN apt-get install -y make
RUN apt-get install -y build-essential
RUN apt-get install -y clang
RUN apt-get install -y gdb
RUN apt-get install -y cscope
RUN apt-get install -y htop
RUN apt-get install -y python3-dev
RUN apt-get install -y iftop
RUN apt-get install -y iotop
RUN apt-get install -y openssl
RUN apt-get install -y ca-certificates
RUN apt-get install -y cmake
RUN apt-get install -y libgtest-dev
RUN apt-get install -y bison
RUN apt-get install -y flex
RUN apt-get install -y g++
RUN apt-get install -y autotools-dev
RUN apt-get install -y valgrind
RUN apt-get install -y sysstat
RUN apt-get install -y fio
RUN apt-get install -y ioping
RUN apt-get install -y libcurl4-openssl-dev
RUN apt-get install -y libssl-dev
RUN apt-get install -y libxml2-dev
RUN apt-get install -y libxmlsec1-dev
RUN apt-get install -y libsqlite3-dev
RUN apt-get install -y libtbb-dev
 

# Library environment variable
ENV LD_LIBRARY_PATH="/usr/local/lib"


# Change login shell to zsh
RUN chsh -s /bin/zsh $(whoami)


# Runs configurations for library and certifications. Git clones and sets up if necessary
COPY . /home

RUN ./run_config.sh
RUN ./create_certs.sh

# Set user
RUN adduser http
user http
