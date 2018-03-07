
#! /bin/bash

# We use the official maven Docker image for building our project. This way, we
# don't have to have Java installed on the build machine.
docker run -it --rm --name maven-build -v "$PWD":/usr/src/app -v "$PWD"/.m2:/root/.m2 -w /usr/src/app maven:3.5.2-jdk-8 mvn package

# Now, build the actual Docker image
docker build -t  wp3_services .
