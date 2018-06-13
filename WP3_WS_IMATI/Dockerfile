# Base this Docker image on one with a pre-configured glassfish server
# FROM python:2.7.9
# FROM oracle/glassfish:5.0

FROM ubuntu:latest

# Install python
RUN apt-get update
RUN apt-get -y install python2.7
RUN apt-get -y install python-pip
RUN apt-get -y install python-suds

# Install glassfish server
RUN apt-get -y install software-properties-common
RUN apt-get -y install python-software-properties
RUN add-apt-repository -y ppa:webupd8team/java
RUN apt-get update
RUN echo debconf shared/accepted-oracle-license-v1-1 select true | debconf-set-selections
RUN echo debconf shared/accepted-oracle-license-v1-1 seen true | debconf-set-selections
RUN apt-get -y install oracle-java8-installer
RUN java -version

RUN wget download.java.net/glassfish/5.0/release/glassfish-5.0.zip
RUN apt-get -y install unzip
RUN unzip glassfish-5.0.zip -d /opt

ENV PATH /opt/glassfish5/bin:/opt/app/bin:$PATH

# Copy entrypoint script and compiled source package
COPY docker-entrypoint.sh /entrypoint.sh
COPY target/WP3_WS_IMATI-1.0.war /app.war

# Copy scripts to access gss
RUN mkdir /root/infrastructureClients/
COPY infrastructureClients/ /root/infrastructureClients/

# Copy service executables (cpp)
RUN mkdir /root/CaxMan/
COPY cpp/ /root/CaxMan/


# Copy libzip (dinamically linked)
RUN mkdir -p /usr/
RUN mkdir -p /usr/local/

COPY libs/ /usr/local/

ENV LD_LIBRARY_PATH /usr/local/lib:/usr/lib:/lib

# Copy scripts for async services
RUN mkdir -p /usr/local/bin
COPY scripts/. /usr/local/bin/


# Copy third_party libs
RUN mkdir -p /root/third_party_libs
COPY third_party_libs/third_party_libs/ /root/third_party_libs/

# Crete output directory
RUN mkdir /root/CAxManIO/

# We want to have port 8080 visible from outside
EXPOSE 8080

ENTRYPOINT ["/entrypoint.sh"]

CMD ["asadmin", "start-domain", "-v"]
