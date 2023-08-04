# Dockerfile for Carl-storm library
###################################
# The Docker image can be built by executing:
# docker build -t yourusername/carl .
# A different base image can be set from the commandline with:
# --build-arg BASE_IMAGE=<new_base_image>

# Set base image
ARG BASE_IMAGE=movesrwth/storm-basesystem:latest
FROM $BASE_IMAGE
MAINTAINER Matthias Volk <m.volk@utwente.nl>


# Configuration arguments
#########################
# The arguments can be set from the commandline with:
# --build-arg <arg_name>=<value>

# CMake build type
ARG build_type=Release
# Number of threads to use for parallel compilation
ARG no_threads=2


# Build Carl-storm
##################
RUN mkdir /opt/carl
WORKDIR /opt/carl

# Copy the content of the current local Carl repository into the Docker image
COPY . .

# Switch to build directory
RUN mkdir -p /opt/carl/build
WORKDIR /opt/carl/build

# Configure Carl
RUN cmake .. -DCMAKE_BUILD_TYPE=$build_type

# Build Carl library
RUN make lib_carl -j $no_threads


# Additional commands
# (This can be skipped or adapted depending on custom needs)
# Build and execute tests
#RUN make -j $no_threads
#RUN ctest test --output-on-failure
