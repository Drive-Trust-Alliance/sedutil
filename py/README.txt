# Fidelity Height LLC
# Date : 8/24/2021
# Sub : How to compile OpalLock python code into a single executable on centos 7
# Author : jerry hwang
# 
#
#
# use this command systax to build standalone OpalLock.exe on centos 7
# assume Nuitka is cloned at /home2/TC12/

/home2/TC12/Nuitka/bin/nuitka --recurese-all OpalLock.py

##########################################################
# make sure nuitka is 0.5.9
# get the right version 0.5.9 of nuitka on linux
# V0.5.9 work
# V0.6.3 or newer does not work
##########################################################

cd /home2/TC12

git clone https://github.com/Nuitka/Nuitka

cd Nuitka/

git checkout master

git tag

( find 0.5.9 )

git fetch --all --tags --prune


git checkout tags/0.5.9 -b V0.5.9

git branch
* V0.5.9
  develop
  master


##########################################################
# get the right version of python , i.e 2.7.18 and gcc 8 compiler
#
# 
# How to Install Python 2.7.18 on CentOS/RHEL 7/6 and Fedora 32/31
#
#                #################
# VIP : must use "make altinstall" otherwise it break centos 7
#                #################
#
##########################################################

cd /usr/src
wget https://www.python.org/ftp/python/2.7.18/Python-2.7.18.tgz

tar xzf Python-2.7.18.tgz

cd Python-2.7.18

./configure --prefix=/usr/local --enable-unicode=ucs4 

sudo make altinstall

/usr/local/bin/python --version


##########################################################
# create  python virture env, need to install pip before virturenv 
##########################################################

curl -O https://bootstrap.pypa.io/pip/2.7/get-pip.py

python get-pip.py

python -m pip install --upgrade "pip < 21.0"

##########################################################
# Install virtualenv for Python 2.7 and create a sandbox called my27project:
##########################################################

pip2.7 install virtualenv

virtualenv my27project

##########################################################
# Use the built-in functionality in Python 3.6 to create a sandbox called my36project:
##########################################################

python3.6 -m venv my36project

# Check the system Python interpreter version:
python --version

##########################################################
# Activate the my27project sandbox:
##########################################################

source my27project/bin/activate

##########################################################
# Check the Python version in the sandbox (it should be Python 2.7.14):
##########################################################

python --version

##########################################################
# Deactivate the sandbox:
##########################################################

deactivate

##########################################################
# Activate the my36project sandbox:
##########################################################

source my36project/bin/activate

##########################################################
# Check the Python version in the sandbox (it should be Python 3.6.3):
##########################################################

python --version

##########################################################
# Deactivate the sandbox:
##########################################################

deactivate



##########################################################
# 
# fatal error: Python.h: No such file or directory
# #include <Python.h>
# https://appdividend.com/2021/05/01/how-to-solve-python-h-no-such-file-or-directory/
##########################################################

For CentOS, RHE
sudo yum install python-devel    # for python2.x installs
sudo yum install python3-devel   # for python3.x installs


##########################################################
# How to install GNU GCC 8 on CentOS 7
#
# https://ahelpme.com/linux/centos7/how-to-install-gnu-gcc-8-on-centos-7/
##########################################################

yum update -y
yum -y install centos-release-scl

yum install devtoolset-8
scl enable devtoolset-8 bash


===  if yum list available complain google-chrome repo  , use following option =====

yum --disablerepo=google-chrome



