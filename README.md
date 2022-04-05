# HTTPS-Server---BL

# 1. Setup Environment

## 1.1 Clone the server repo
Clone the repo with:
```
git clone https://github.com/Smooth-Pacific/HTTPS-Server---BL.git
```

## 1.2 Build Docker image
We can build the docker image with the following script:
```
sh dock.sh build
```

This script will build the docker image and run any set up scripts needed to set up the webserver, including configuring the libraries used.

## 1.3 Spinup Docker container
We can spinup the docker container once the image has been created.
```
sh dock.sh 
```

## 1.4 Attach to Docker container
We can attach to the container in a new terminal with:
```
sh dock.sh exec
```

# 2 Build and Run Server

## 2.1 Customizing Server Options
Before creating the docker image, you can adjust the server options in the file called env.list. There you can modify start up options as needed.

## 2.2 Run Server
To run the server, navigate to the server folder and run make to build the server executable. Then execute it to start the server.
```
cd server
make 
./server
```

## 2.3 cURL server
You can curl the server by opening a new terminal tab and attaching to the container and running `curl -v -XGET --digest --user myuser:mypass 'https://localhost:8080'`
```
# In new terminal 
sh dock.sh exec
curl -XGET --digest --user myuser:mypass 'https://localhost:8080'
```
This should output
```
Success, Hello World
```

# 3 Performance Monitoring

## 3.1 Logging
There are serveral way to monitor the performance of the server. One of which is through logging. While the server is running, a log.txt file will be 
created, logging relavant system information.

## 3.2 APM 
There is also a bash script to run while the server is up to get logs from various tools such as mpstat, top, iostat, and valgrind.
```
sh apmtool.sh
```
This will create an amplog folder with all of the logs separated.

## 3.3 Throughput
To check the read and write throughput of your host machine, use the throughput script.
```
sh throughput.sh
```
This will output a summary text file.
 
## 3.4 Latency, Request Throughput and Response Throughput
In the client folder, there are 3 files which can be used to monitor each of these aspects. Use the Makefile to compile and run these programs.
```
# creates executables
make lat
make req
make resp
# Runs executables and aggregates output into summmary text file
make test 
```

# 4 Security

## 4.1 Certificate Authorities
The server uses 2 tier hierarchy pki for the certificate authorities. When building the image, the create_certs.sh script will run and create a root
certificate authority and two subordinate certificates for document signing and server signing. The will be used for digital signatures and 
ensure integrity of the files sent as well as provide authenticity for the server. There is a config folder where one can adjust the configurations
for the certificates before they are generated. There are three, one for each root, server, and document certificate.

## 4.2 Digest Authentication
The server also uses digest authentication to verify authorized users. The username and password can be set in the env.list file. With this only 
requests with the correct username and password will be accepted. Any other requests will be rejected. 
```
# Accepted assuming username and password is correct
curl -XGET --digest --user myuser:mypass 'https://localhost:8080'

# Failed
curl -XGET 'https://localhost:8080'
```


#  Testing
* TODO