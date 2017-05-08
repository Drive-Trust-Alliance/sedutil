# Images

## Docker

Docker can be used as a repeatable build environment to build the images in an Ubuntu 16.04 container.  Steps to reproduce from the `images` directory:


### Step 1 - Build and start the Docker container

    docker build -t sedutil .
    docker run --rm -it -v $PWD/../:/sedutil --privileged sedutil

### Step 2 - Run the build from within the Docker container in the shell created by `docker run` above:

Refer to `BUILDING` file or try the audiobuild.sh script:

   cd /sedutil/images
   ./autobuild.sh
