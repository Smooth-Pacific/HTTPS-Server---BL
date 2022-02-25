
FROM ubuntu:20.04

WORKDIR /home

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Installs 
RUN apt-get update -y
RUN apt-get install -y autoconf
RUN apt-get install -y --no-install-recommends ssh \
                                               sudo \
                                               zsh \
                                               zsh-autosuggestions \
                                               git \
                                               curl \
                                               wget \
                                               vim \
                                               tree \
                                               zip \
                                               unzip \
                                               pkg-config \
                                               libtool \
                                               m4 \
                                               gnutls-bin \
                                               automake \
                                               libmicrohttpd-dev \
                                               make \
                                               build-essential \
                                               clang \
                                               gdb \
                                               cscope \
                                               python3-dev \
                                               htop \
                                               iftop \
                                               iotop \
                                               openssl \
                                               ca-certificates \
                                               cmake \
                                               libgtest-dev \
                                               bison \
                                               flex
RUN apt-get install -y g++
RUN apt-get install -y autotools-dev


# Library environment variable
ENV LD_LIBRARY_PATH="/usr/local/lib"


# Change login shell to zsh
RUN chsh -s /bin/zsh $(whoami)


# Runs configurations for library and certifications. Git clones and sets up if necessary
COPY run_config.sh /home
RUN ./run_config.sh

