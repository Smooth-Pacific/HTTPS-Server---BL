
FROM ubuntu:20.04

WORKDIR /home

# Installs 
RUN apt-get update -y
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
                                               g++ \
                                               autotools-dev \
                                               autoconf

# Library environment variable
ENV LD_LIBRARY_PATH="/usr/local/lib"


# Change login shell to zsh
RUN chsh -s /bin/zsh $(whoami)

COPY . /home

# Set up certificates
COPY ./certs/myCA.pem /usr/local/share/ca-certificates/myCA.crt
RUN update-ca-certificates

