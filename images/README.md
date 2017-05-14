# Images

## Docker

Docker can be used as a repeatable build environment to build the images in an Ubuntu 16.04 container.  Steps to reproduce from the `images` directory:


### Step 1 - Build the Docker container

    docker build -t sedutil .

### Step 2 - Run the build from within the Docker container

Try the `autobuild.sh` script which runs automatically when no command is
passed:

    docker run --rm -it -v $PWD/../:/sedutil --privileged sedutil

Or, refer to `BUILDING` file and run manually in the Docker container by
starting `bash`:

    docker run --rm -it -v $PWD/../:/sedutil --privileged sedutil bash

