
FROM ubuntu:20.04

WORKDIR /home

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
                                               ca-certificates
RUN apt-get install -y g++
RUN apt-get install -y autotools-dev

# Library environment variable
ENV LD_LIBRARY_PATH="/usr/local/lib"


# Change login shell to zsh
RUN chsh -s /bin/zsh $(whoami)

COPY . /home

# Runs configurations for library. Git clones and sets up if necessary
RUN ./run_config.sh

# Set up certificates
COPY ./certs/myCA.pem /usr/local/share/ca-certificates/myCA.crt
RUN update-ca-certificates

